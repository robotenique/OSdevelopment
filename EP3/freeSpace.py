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
from memsimWrapper import doc_inherit, LinkedList


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
        self.id = 0

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

    def __ptable_alloc(self, proc, node, ua_used, real_ua_used):
        """Allocate a given set of pages in the pages table and correctly
        adds to the memory list representation"""
        print(node)
        if (not node):
            curr = self.memmap.head
            new_entry = LinkedList.Node('P', 0, ua_used, curr)
            self.memmap.head = new_entry
        else:
            curr = node.next
            new_entry = LinkedList.Node('P', curr.base, ua_used, curr)
            node.next = new_entry

        curr.base += ua_used
        curr.qtd -= ua_used
        if (curr.qtd == 0):
            new_entry.next = curr.next

        inipos = new_entry.base
        proc.base = inipos
        proc.size = ua_used
        self.pages_table.palloc(proc.pid, inipos*self.ua, real_ua_used*self.ua)

    def free(self, proc, pmem_manager):
        curr = self.memmap.head
        prev = None
        while (curr.base != proc.base):
            prev = curr
            curr = curr.next
        if (prev and prev.status == 'L'):
            prev.qtd += curr.qtd
            prev.next = curr.next
            if (curr.next and curr.next.status == 'L'):
                prev.qtd += curr.next.qtd
                prev.next = curr.next.next
        elif (curr.next and curr.next.status == 'L'):
            curr.next.qtd += curr.qtd
            curr.next.base -= curr.qtd
            if prev:
                prev.next = curr.next
            else:
                self.memmap.head = curr.next
        else:
            curr.status = 'L'
        pg_to_ua = math.ceil(self.pg_size/self.ua)
        base_page = proc.base//pg_to_ua
        num_pages = proc.size//pg_to_ua
        for i in range(num_pages):
            frame = self.pages_table.get_frame(base_page+i)
            if (frame != -1):
                pmem_manager.delete_frame(frame)
            self.pages_table.reset_page(base_page+i)
        #self.print_table()

    def print_table(self):
        #debug_vmem(self.memmap)
        #debug_ptable(self.pages_table.table, self.pg_size)
        pass

class BestFit(FreeSpaceManager):
    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable, memmap):
        super().__init__(total_memory, ua, page_size, ptable, ftable, memmap)
        self.id = 1

    @doc_inherit
    def malloc(self, proc):
        real_ua_used, pg_to_ua, pgs_used, ua_used = super()._FreeSpaceManager__calc_units(proc)

        mem_conv = lambda u: u*self.ua
        bf_val = math.inf
        bf_node = None
        bf_prev = None
        curr = self.memmap.head
        prev = None
        while curr:
            if curr.status == 'L' and ua_used <= curr.qtd and curr.qtd < bf_val:
                bf_node = curr
                bf_prev = prev
                bf_val = curr.qtd
                if ua_used == curr.qtd:
                    break
            prev = curr
            curr = curr.next

        if bf_node == None:
            print("No space left! Exiting simulator...")
            exit()

        super()._FreeSpaceManager__ptable_alloc(proc, bf_prev, ua_used, real_ua_used)
        #self.print_table()

    @doc_inherit
    def free(self, proc, pmem_manager):
        super().free(proc, pmem_manager)
        #pass

    @doc_inherit
    def print_table(self):
        super().print_table()

class WorstFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable, memmap):
        super().__init__(total_memory, ua, page_size, ptable, ftable, memmap)
        self.id = 2

    @doc_inherit
    def malloc(self, proc):
        real_ua_used, pg_to_ua, pgs_used, ua_used = super()._FreeSpaceManager__calc_units(proc)

        mem_conv = lambda u: u*self.ua
        wf_val = -math.inf
        wf_node = None
        wf_prev = None
        curr = self.memmap.head
        prev = None
        while curr:
            if curr.status == 'L' and ua_used <= curr.qtd and curr.qtd > wf_val:
                wf_node = curr
                wf_prev = prev
                wf_val = curr.qtd
            prev = curr
            curr = curr.next

        if wf_node == None:
            print("No space left! Exiting simulator...")
            exit()

        super()._FreeSpaceManager__ptable_alloc(proc, wf_prev, ua_used, real_ua_used)
        #self.print_table()

    @doc_inherit
    def free(self, proc, pmem_manager):
        super().free(proc, pmem_manager)

    @doc_inherit
    def print_table(self):
        super().print_table()

class QuickFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable, memmap):
        super().__init__(total_memory, ua, page_size, ptable, ftable, memmap)
        self.id = 3
    @doc_inherit
    def malloc(self, proc):
        real_ua_used, pg_to_ua, pgs_used, ua_used = super()._FreeSpaceManager__calc_units(proc)
        slist = self.fspc_sizes
        rlist = self.fspc_ref

        lkp = lambda s : bst.bisect_left(slist, s)
        checkEqual = lambda p, l, v : p < len(l) and l[p] == v
        pos_slist = lkp(ua_used)
        isFrequent = False
        found = False
        print("")
        print(f"Alocando {proc.name}, size = {ua_used}")
        print(f"{slist}")
        print(f"{rlist}")
        self.memmap.print_nodes()
        if checkEqual(pos_slist, slist, ua_used) and rlist[pos_slist] != []:
            # If it's a frequent size AND there's free size available
            node = rlist[pos_slist].pop()
            node.status = 'P'
            isFrequent = True
        else: # Not a frequent size or not available, so manually find in the mlist
            curr = self.memmap.head
            ant = None # Previous node
            while curr and not found:
                if curr.status == 'L':
                    fx_ant = ant # Previous of the start node
                    start_node = curr
                    end_node = curr
                    sz_found = curr.qtd
                    found = True if sz_found >= ua_used else False
                    curr = curr.next
                    while curr and curr.status == 'L' and not found:
                        sz_found += curr.qtd
                        if sz_found >= ua_used:
                            found = True
                            end_node = curr
                        else:
                            ant = curr
                            curr = curr.next
                if not found and curr:
                    ant = curr
                    curr = curr.next
        if not isFrequent:
            if not found:
                print("No memory available!")
                exit()
            temp = start_node
            while temp and temp != end_node.next:
                ptemp = lkp(temp.qtd)
                if checkEqual(ptemp, slist, temp.qtd) and temp in rlist[ptemp]:
                    rlist[ptemp].remove(temp)
                temp = temp.next
            if start_node != end_node: # Compress the multiple nodes into a big full node
                full_node = LinkedList.Node('L', start_node.base, sz_found)
                if fx_ant == None:
                    self.memmap.head = full_node
                else:
                    fx_ant.next = full_node
                full_node.next = end_node.next
                start_node = full_node # The new start_node is this compressed node
                self.memmap.print_nodes()
            # This is equivalent to __ptable_alloc ...
            new_node = LinkedList.Node('P', start_node.base, ua_used)
            inipos = start_node.base
            start_node.base += ua_used
            start_node.qtd -= ua_used
            new_node.next = start_node if start_node.qtd != 0 else start_node.next
            if fx_ant == None:
                self.memmap.head = new_node
            else:
                fx_ant.next = new_node
            if start_node.qtd != 0:
                snode_pos = lkp(start_node.qtd)
                if checkEqual(snode_pos, slist, start_node.qtd):
                    rlist[snode_pos].append(start_node)
        else:
            inipos = node.base
        proc.base = inipos
        proc.size = ua_used
        self.pages_table.palloc(proc.pid, inipos*self.ua, real_ua_used*self.ua)
        print(f"Base found = {proc.base}")
        self.memmap.print_nodes()
        print(f"{slist}")
        print(f"{rlist}")
        print("")
        #debug_ptable(self.pages_table.table, self.pg_size)


    @doc_inherit
    def free(self, proc, pmem_manager):
        curr = self.memmap.head
        prev = None
        slist = self.fspc_sizes
        rlist = self.fspc_ref
        lkp = lambda s : bst.bisect_left(slist, s)
        checkEqual = lambda p, l, v : p < len(l) and l[p] == v
        print("")
        print(f"{slist}")
        print(f"{rlist}")
        print(f"Liberando {proc.name}, de base = {proc.base}")
        self.memmap.print_nodes()

        while curr and curr.base != proc.base:
            prev = curr
            curr = curr.next
        pos_slist = lkp(curr.qtd)
        if checkEqual(pos_slist, slist, curr.qtd):
            curr.status = 'L'
            rlist[pos_slist].append(curr)
        else:
            if prev and prev.status == 'L' and \
               not checkEqual(lkp(prev.qtd), slist, prev.qtd):
                prev.qtd += curr.qtd
                prev.next = curr.next
                if curr.next and curr.next.status == 'L' and \
                   not checkEqual(lkp(curr.next.qtd), slist, curr.next.qtd):
                    prev.qtd += curr.next.qtd
                    prev.next = curr.next.next
            elif curr.next and curr.next.status == 'L' and \
                 not checkEqual(lkp(curr.next.qtd), slist, curr.next.qtd):
                curr.next.qtd += curr.qtd
                curr.next.base -= curr.qtd
                if prev:
                    prev.next = curr.next
                else:
                    self.memmap.head = curr.next
            else:
                curr.status = 'L'
        pg_to_ua = math.ceil(self.pg_size/self.ua)
        base_page = proc.base//pg_to_ua
        num_pages = proc.size//pg_to_ua
        for i in range(num_pages):
            frame = self.pages_table.get_frame(base_page+i)
            if (frame != -1):
                self.frames_table.reset_frame(frame)
            self.pages_table.reset_page(base_page+i)
        self.memmap.print_nodes()
        self.print_table()
        print(f"{slist}")
        print(f"{rlist}")


    def analyze_processes(self, proc_deque):
        """
        Analyze the list of processes to get the most frequent sizes requested,
        then created the base list that will be populated later with the references
        for the blocks
        """
        MAX_SIZE = 5
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
        i = bst.bisect(self.fspc_sizes, self.total_memory)
        if i < len(self.fspc_sizes) and self.fspc_sizes[i] == self.total_memory:
            self.fspc_ref[i].append(self.memmap.head)

    def reorder_references(self):
        lkp = lambda s : bst.bisect_left(slist, s)
        checkEqual = lambda p, l, v : p < len(l) and l[p] == v
        slist = self.fspc_sizes
        print("")
        print("MEMÓRIA FOI COMPACTADA!")
        self.memmap.print_nodes()
        print(f"{slist}")
        print(f"{self.fspc_ref}")
        qlist = [len(self.fspc_ref[i]) for i in range(len(slist))]
        self.fspc_ref = [[] for _ in self.fspc_ref]
        end_node = self.memmap.head
        ant_end = None # Previous node
        while end_node.next:
            ant_end = end_node
            end_node = end_node.next

        for pos, sz in enumerate(slist):
            qtd = qlist[pos]
            while qtd != 0:
                if end_node.qtd < sz:
                    break
                elif checkEqual(lkp(end_node.qtd), slist, end_node.qtd) and \
                end_node in self.fspc_ref[lkp(end_node.qtd)]:
                    break
                new_node = LinkedList.Node('L', end_node.base, sz)
                end_node.base += sz
                end_node.qtd -= sz
                if ant_end:
                    ant_end.next = new_node
                else:
                    self.memmap.head = new_node
                if end_node.qtd != 0:
                    new_node.next = end_node
                    ant_end = new_node
                else:
                    end_node = new_node
                self.fspc_ref[pos].append(new_node)

                qtd -= 1
        print("Após reorder_references")
        self.memmap.print_nodes()
        print(f"{slist}")
        print(f"{self.fspc_ref}\n")



    @doc_inherit
    def print_table(self):
        super().print_table()

# TODO: remove this at the end
def debug_vmem(mmem):
    mmem.print_nodes()

def debug_ptable(ptable, page_size):
    return
    print(f"== PAGES TABLE == -> {page_size}")
    for i, p in zip(range(len(ptable)), ptable):
        print(i, p)
    print("")
