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
    class Page(object):
        def __init__(self):
            self.pid = -1
            self.page_frame = 0
            self.bit_p = False
            self.bit_r = False
            self.label = ''
            self.lruC = 0

    def __init__(self, total_memory, alloc_unit, vfile, page_size):
        """Creates a new FreeSpaceManager.
           \ttotal_memory = Total physical memory
           \talloc_unit = The 'ua' of the memory
           \tpfile = MemoryWriter instance of physical memory file
        """
        self.alloc_unit = alloc_unit
        self.total_memory = total_memory
        self.free_memory = total_memory
        self.vfile = vfile
        self.used_memory = 0
        # ('L' or 'P', position, quantity)
        self.memmap = [['L', 0, total_memory//alloc_unit]]
        self.pages_table = [self.Page() for _ in range(total_memory//page_size)]


    @abstractmethod
    def malloc(self, proc):
        """
        Allocates memory for a process in the virtual memory
        """
        pass

    @abstractmethod
    def free(self, proc):
        pass


class BestFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, alloc_unit, pfile, page_size):
        super().__init__(total_memory, alloc_unit, pfile, page_size)

    @doc_inherit
    def malloc(self, proc):
        mem_conv = lambda u: u*self.alloc_unit
        bf_val = math.inf
        bf_pos = -1
        for idx, curr in enumerate(list(self.memmap)):
            if curr[0] == 'L':
                curr_mem = mem_conv(curr[2])
                if proc.b <= curr_mem and curr_mem < bf_val:
                    bf_pos = idx
                    bf_val = curr_mem

        if bf_val == math.inf:
            print("No space left! Exiting simulator...")
            exit()

        ua_used = math.ceil(proc.b/self.alloc_unit)
        new_entry = ['P', self.memmap[idx][1], ua_used, proc]
        self.memmap[idx][1] += ua_used
        self.memmap[idx][2] -= ua_used
        self.memmap.insert(idx, new_entry)
        if self.memmap[idx + 1][2] == 0:
            self.memmap.pop(idx + 1)
        debug_vmem(self.memmap)


    def free(self, process):
        print('Tope')

# TODO: remove this at the end
def debug_vmem(mmem):
    for i in range(len(mmem) - 1):
        print(f"{mmem[i]} -> ", end="")
    print(mmem[-1])
