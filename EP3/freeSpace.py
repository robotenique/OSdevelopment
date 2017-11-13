"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

Implementation of the Free Space memory manager algorithms
"""

from abc import ABC, abstractmethod
from collections import deque
from memsimWrapper import doc_inherit, LinkedList, Node
import math


#TODO: Implement the FreeSpaceManagers algorithms

class FreeSpaceManager(ABC):
    def __init__(self, total_memory, alloc_unit, vfile):
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
        """Actually, we'll have to use a list... Can't remove something inside
        a deque, only in both ends... #TODO: Change to simple list, or make a linked list."""
        self.memory = LinkedList()
        # ('L' or 'P', position, quantity)
        self.memory.add_node(['L', 0, total_memory//alloc_unit])
        self.memory.print_nodes()

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
    def __init__(self, total_memory, alloc_unit, pfile):
        super().__init__(total_memory, alloc_unit, pfile)

    @doc_inherit
    def malloc(self, proc):
        mem_conv = lambda u: u*self.alloc_unit
        curr = self.memory.head
        ant = None
        bf_node = None
        bf_ant = None
        bf_value = math.inf
        print("")
        while curr:
            if curr.data[0] == 'L':
                #print("Looking node: ", curr.data)
                node_mem = mem_conv(curr.data[2])
                #print("Current node memory: ", node_mem)
                if proc.b <= node_mem and node_mem < bf_value:
                    # We found a best fit
                    bf_node = curr
                    bf_value = node_mem
                    bf_ant = ant
            ant = curr
            curr = curr.next_node()
            #print("")
        if bf_value == math.inf:
            print("No space left! Exiting simulator...")
            exit()
        #print("Best fit found!")
        #print(bf_node.data)
        #print("ANT: ",end="")
        #print(bf_ant)
        mem_slots = math.ceil(proc.b/self.alloc_unit)        
        memory = Node(['P', bf_node.data[1], mem_slots, proc])
        bf_node.data[1] += mem_slots
        bf_node.data[2] -= mem_slots
        if bf_node.data[2] == 0: # We used all space, so bf_node get's deleted
            memory.nextNode = bf_node.nextNode
        else:
            memory.nextNode = bf_node
        if bf_ant:
            bf_ant.nextNode = memory
        else:
            self.memory.head = memory
        self.memory.print_nodes()





    def free(self, process):
        print('Tope')
