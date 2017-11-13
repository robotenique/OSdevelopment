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


#TODO: Implement the FreeSpaceManagers algorithms

class FreeSpaceManager(ABC):
    def __init__(self, total_memory, alloc_unit, pfile):
        """Creates a new FreeSpaceManager.
           \ttotal_memory = Total physical memory
           \talloc_unit = The 'ua' of the memory
           \tpfile = MemoryWriter instance of physical memory file
        """
        self.alloc_unit = alloc_unit
        self.total_memory = total_memory
        self.free_memory = total_memory
        self.pfile = pfile
        self.used_memory = 0
        """Actually, we'll have to use a list... Can't remove something inside
        a deque, only in both ends... #TODO: Change to simple list, or make a linked list."""
        self.memory = deque()
        self.memory.append(('L', 0, total_memory//alloc_unit))

    @abstractmethod
    def malloc(self, process):
        """
        HMMM
        """
        pass

    @abstractmethod
    def free(self, process):
        pass


class BestFit(FreeSpaceManager):

    @doc_inherit
    def __init__(self, total_memory, alloc_unit, pfile):
        super().__init__(total_memory, alloc_unit, pfile)

    def malloc(self, process):
        print('Alo')

    def free(self, process):
        print('Tope')
