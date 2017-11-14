"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

Implementation of the Free Space memory manager algorithms
"""

from abc import ABC, abstractmethod
from collections import deque
from memsimWrapper import doc_inherit
import math


#TODO: Implement the FreeSpaceManagers algorithms

class FreeSpaceManager(ABC):

    def __init__(self, total_memory, ua, page_size, ptable, ftable):
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
        self.memmap = [['L', 0, total_memory//ua]]
        self.pages_table = ptable
        self.frames_table = ftable
        debug_ptable(self.pages_table.table, self.pg_size)

    @abstractmethod
    def malloc(self, proc):
        """
        Allocates memory for a process in the virtual memory
        """
        pass

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
            self.pages_table.reset_page(base_page+i)
        debug_vmem(self.memmap)
        debug_ptable(self.pages_table.table, self.pg_size)


class BestFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        super().__init__(total_memory, ua, page_size, ptable, ftable)

    @doc_inherit
    def malloc(self, proc):
        mem_conv = lambda u: u*self.ua
        bf_val = math.inf
        bf_pos = -1
        # The REAL number of ua's used, to be written into the vfile
        real_ua_used = math.ceil(proc.b/self.ua)
        # Get number of pages a process will use, then convert to ua_used
        pg_to_ua = math.ceil(self.pg_size/self.ua)
        pgs_used = math.ceil(proc.original_sz/self.pg_size)
        ua_used = pgs_used*pg_to_ua
        for idx, curr in enumerate(list(self.memmap)):
            if curr[0] == 'L' and ua_used <= curr[2] and curr[2] < bf_val:
                bf_pos = idx
                bf_val = curr[2]
                if ua_used == curr[2]:
                    break

        if bf_pos == -1:
            print("No space left! Exiting simulator...")
            exit()

        new_entry = ['P', self.memmap[idx][1], ua_used]
        self.memmap[idx][1] += ua_used
        self.memmap[idx][2] -= ua_used
        self.memmap.insert(idx, new_entry)
        proc.base = new_entry[1]
        proc.size = ua_used
        if self.memmap[idx + 1][2] == 0:
            self.memmap.pop(idx + 1)
        inipos = self.memmap[idx][1]
        self.pages_table.palloc(proc.pid, inipos*self.ua, real_ua_used*self.ua)
        debug_vmem(self.memmap)
        debug_ptable(self.pages_table.table, self.pg_size)


    @doc_inherit
    def free(self, proc):
        super().free(proc)


class WorstFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        super().__init__(total_memory, ua, page_size, ptable, ftable)

    @doc_inherit
    def malloc(self, proc):
        mem_conv = lambda u: u*self.ua
        bf_val = -math.inf
        bf_pos = -1
        # The REAL number of ua's used, to be written into the vfile
        real_ua_used = math.ceil(proc.b/self.ua)
        # Get number of pages a process will use, then convert to ua_used
        pg_to_ua = math.ceil(self.pg_size/self.ua)
        pgs_used = math.ceil(proc.original_sz/self.pg_size)
        ua_used = pgs_used*pg_to_ua
        for idx, curr in enumerate(list(self.memmap)):
            if curr[0] == 'L' and ua_used <= curr[2] and curr[2] > bf_val:
                bf_pos = idx
                bf_val = curr[2]
                if ua_used == curr[2]:
                    break

        if bf_pos == -1:
            print("No space left! Exiting simulator...")
            exit()

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
        debug_vmem(self.memmap)
        debug_ptable(self.pages_table.table, self.pg_size)

    @doc_inherit
    def free(self, proc):
        super().free(proc)


class QuickFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        super().__init__(total_memory, ua, page_size, ptable, ftable)

    @doc_inherit
    def malloc(self, proc):
        print('Kek')

    @doc_inherit
    def free(self, proc):
        super().free(proc)

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
