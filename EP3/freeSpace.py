"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

Implementation of the Free Space memory manager algorithms
"""

import math
import bisect as bst
from abc import ABC, abstractmethod
from collections import deque, Counter
from memsimWrapper import doc_inherit


#TODO: Implement the FreeSpaceManagers algorithms

class FreeSpaceManager(ABC):

    def __init__(self, total_memory, ua, page_size, ptable, ftable, memmap):
        """Creates a new FreeSpaceManager.
           \ttotal_memory = Total physical memory
           \tua = The 'ua' of the memory
           \tvfile = MemoryWriter instance of physical memory file
        """
        self.pg_size = page_size
        self.ua = ua
        self.total_memory = total_memory
        self.free_memory = total_memory
        self.used_memory = 0
        # ('L' or 'P', position, quantity)
        self.memmap = memmap
        self.pages_table = ptable
        self.frames_table = ftable
        debug_ptable(self.pages_table.table, self.pg_size)

    @abstractmethod
    def malloc(self, proc):
        """
        Allocates memory for a process in the virtual memory
        """
        pass

    def __calc_units(self, proc):
        """Calculates the units of allocations required for the free space
        managers to allocate the memory corrrectly"""
        # The REAL number of ua's used, to be written into the vfile
        real_ua_used = math.ceil(proc.b/self.ua)
        # Get number of pages a process will use, then convert to ua_used
        pg_to_ua = math.ceil(self.pg_size/self.ua)
        pgs_used = math.ceil(proc.original_sz/self.pg_size)
        ua_used = pgs_used*pg_to_ua
        return real_ua_used, pg_to_ua, pgs_used, ua_used

    def __ptable_alloc(self, proc, idx, ua_used, real_ua_used):
       """Allocate a given set of pages in the pages table and correctly
       adds to the memory list representation"""
       new_entry = ['P', self.memmap[idx][1], ua_used]
       self.memmap[idx][1] += ua_used
       self.memmap[idx][2] -= ua_used
       self.memmap.insert(idx, new_entry)
       inipos = self.memmap[idx][1]
       proc.base = inipos
       proc.size = ua_used
       if self.memmap[idx + 1][2] == 0:
           self.memmap.pop(idx + 1)
       self.pages_table.palloc(proc.pid, inipos*self.ua, real_ua_used*self.ua)
       #self.print_table()

    def free(self, proc):
        idx = 0
        while (self.memmap[idx][1] != proc.base):
            idx += 1
        if (idx != 0 and self.memmap[idx-1][0] == 'L'):
            self.memmap[idx-1][2] += self.memmap[idx][2]
            self.memmap.pop(idx)
        elif (idx != len(self.memmap)-1 and self.memmap[idx+1][0] == 'L'):
            self.memmap[idx+1][2] += self.memmap[idx][2]
            self.memmap[idx+1][1] -= self.memmap[idx][2]
            self.memmap.pop(idx)
        else:
            self.memmap[idx][0] = 'L'
        pg_to_ua = math.ceil(self.pg_size/self.ua)
        base_page = proc.base//pg_to_ua
        num_pages = proc.size//pg_to_ua
        for i in range(num_pages):
            frame = self.pages_table.get_frame(base_page+i)
            if (frame != -1):
                self.frames_table.reset_frame(frame)
            self.pages_table.reset_page(base_page+i)
        #self.print_table()

    def print_table(self):
        debug_vmem(self.memmap)
        debug_ptable(self.pages_table.table, self.pg_size)



class BestFit(FreeSpaceManager):
    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable, memmap):
        super().__init__(total_memory, ua, page_size, ptable, ftable, memmap)

    @doc_inherit
    def malloc(self, proc):
        real_ua_used, pg_to_ua, pgs_used, ua_used = super()._FreeSpaceManager__calc_units(proc)

        mem_conv = lambda u: u*self.ua
        bf_val = math.inf
        bf_pos = -1
        for idx, curr in enumerate(list(self.memmap)):
            if curr[0] == 'L' and ua_used <= curr[2] and curr[2] < bf_val:
                bf_pos = idx
                bf_val = curr[2]
                if ua_used == curr[2]:
                    break
        if bf_pos == -1:
            print("No space left! Exiting simulator...")
            exit()

        super()._FreeSpaceManager__ptable_alloc(proc, bf_pos, ua_used, real_ua_used)

    @doc_inherit
    def free(self, proc):
        super().free(proc)

    @doc_inherit
    def print_table(self):
        super().print_table()



class WorstFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable, memmap):
        super().__init__(total_memory, ua, page_size, ptable, ftable, memmap)

    @doc_inherit
    def malloc(self, proc):
        real_ua_used, pg_to_ua, pgs_used, ua_used = super()._FreeSpaceManager__calc_units(proc)

        mem_conv = lambda u: u*self.ua
        bf_val = -math.inf
        bf_pos = -1
        for idx, curr in enumerate(list(self.memmap)):
            if curr[0] == 'L' and ua_used <= curr[2] and curr[2] > bf_val:
                bf_pos = idx
                bf_val = curr[2]
                if ua_used == curr[2]:
                    break

        if bf_pos == -1:
            print("No space left! Exiting simulator...")
            exit()

        super()._FreeSpaceManager__ptable_alloc(proc, bf_pos, ua_used, real_ua_used)

    @doc_inherit
    def free(self, proc):
        super().free(proc)

    @doc_inherit
    def print_table(self):
        super().print_table()



class QuickFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable, memmap):
        super().__init__(total_memory, ua, page_size, ptable, ftable, memmap)

    @doc_inherit
    def malloc(self, proc):
        real_ua_used, pg_to_ua, pgs_used, ua_used = super()._FreeSpaceManager__calc_units(proc)
        slist = self.fspc_sizes
        rlist = self.fspc_ref
        pos = bst.bisect_left(slist, ua_used)
        debug_vmem(self.memmap)

        while pos < len(rlist) and rlist[pos] == []:
            pos += 1
        if pos == len(rlist):
            # We didn't found a free space, so let's do a WorstFit
            mem_conv = lambda u: u*self.ua
            bf_val = -math.inf
            bf_pos = -1
            for idx, curr in enumerate(list(self.memmap)):
                if curr[0] == 'L' and ua_used <= curr[2] and curr[2] > bf_val:
                    bf_pos = idx
                    bf_val = curr[2]
                    if ua_used == curr[2]:
                        break

            if bf_pos == -1:
                print("No space left! Exiting simulator...")
                exit()
        else: # We found some free space
            bf_pos = rlist[pos].pop()
        self.__ptable_alloc(proc, bf_pos, ua_used, real_ua_used)
        debug_vmem(self.memmap)

    def __ptable_alloc(self, proc, idx, ua_used, real_ua_used):
       new_entry = ['P', self.memmap[idx][1], ua_used]
       self.memmap[idx][1] += ua_used
       self.memmap[idx][2] -= ua_used
       self.memmap.insert(idx, new_entry)
       inipos = self.memmap[idx][1]
       proc.base = inipos
       proc.size = ua_used
       if self.memmap[idx + 1][2] == 0:
           self.memmap.pop(idx + 1)
       else:
           b = bst.bisect_left(self.fspc_sizes, self.memmap[idx + 1][2])
           if b < len(self.fspc_sizes) and self.fspc_sizes[b] == self.memmap[idx + 1][2]:
               self.fspc_ref[b].append(idx + 1)
       self.pages_table.palloc(proc.pid, inipos*self.ua, real_ua_used*self.ua)

    @doc_inherit
    def free(self, proc):
        super().free(proc)

    def analize_processes(self, proc_deque):
        MAX_SIZE = 1
        plist = list(proc_deque)
        pg_to_ua = math.ceil(self.pg_size/self.ua)
        total_uas = lambda p: math.ceil(p.original_sz/self.pg_size)*pg_to_ua
        self.fspc_sizes = []
        self.fspc_ref = []
        for size, _ in Counter(map(total_uas, plist)).most_common(MAX_SIZE):
            pos = bst.bisect(self.fspc_sizes, size)
            self.fspc_sizes.insert(pos, size)
            self.fspc_ref.insert(pos, [])
        # If the most requested size is already the size of vmemory
        i = bst.bisect(self.fspc_sizes, self.memmap[0][2])
        if i < len(self.fspc_sizes) and self.fspc_sizes[i] == self.memmap[0][2]:
            self.fspc_ref[i].append(0)


        """
        print(fspc_sizes)
        print(fspc_ref)
        print(pcount)
        exit()
        """

    @doc_inherit
    def print_table(self):
        super().print_table()

# TODO: remove this at the end
def debug_vmem(mmem):
    for i in range(len(mmem) - 1):
        print(f"{mmem[i]} -> ", end="")
    print(mmem[-1])

def debug_ptable(ptable, page_size):
    print(f"== PAGES TABLE == -> {page_size}")
    for p in ptable:
        print(p)
    print("")
