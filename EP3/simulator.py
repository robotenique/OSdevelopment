"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the Simulator object file.
"""
from collections import deque
from memoryWriter import MemoryWriter
from freeSpace import BestFit, WorstFit, QuickFit
from paginators import Optimal, FIFO, LRU2, LRU4
from math import ceil
from tables import PageTable, FrameTable
from memsimWrapper import LinkedList, Process

fspc_managers = [None, BestFit, WorstFit, QuickFit]
pagination_managers = [None, Optimal, FIFO, LRU2, LRU4]

class Simulator(object):

    def __init__(self, input_file, fspc_id, pmem_id, dt):
        self.interval = dt
        self.compact_list = deque()
        self.procs = deque()
        Process.reset_pids()
        self.init_dict = dict() # Initialization dictionary
        self.parse(input_file)
        if sum((p.original_sz for p in self.procs)) > self.virt_memory:
            print("Wrong pattern in input file!")
        self.memmap = LinkedList()
        self.memmap.add_node("L", 0, self.virt_memory//self.ua_size)
        self.ptable = PageTable(self.virt_memory, self.page_size)
        self.ftable = FrameTable(self.phys_memory, self.page_size)
        self.fspc_manager = fspc_managers[fspc_id](self.virt_memory,
                            self.ua_size, self.page_size, self.ptable,
                            self.ftable, self.memmap)
        self.pmem_manager = pagination_managers[pmem_id](self.phys_memory,
                            self.ua_size, self.page_size, self.ptable, self.ftable)
        if fspc_id == 3: # Analysis of the processes if it's quick fit
            self.fspc_manager.analyze_processes(self.procs)

    def parse(self, input_file):
        """Parse the input file"""
        for num_line, line in enumerate(input_file):
            vals = line.split()
            if vals[0] == '#' or vals[0][0] == '#':
                continue
            if num_line == 0:
                vals = list(map(int, vals))
                self.phys_memory = vals[0]
                self.virt_memory = vals[1]
                self.ua_size = vals[2]
                self.page_size = vals[3]
            elif "COMPACTAR" in line:
                self.compact_list.append(int(vals[0]))
            else:
                proc = Process(vals, self.ua_size)
                self.procs.append(proc)
                if self.init_dict.get(proc.t0):
                    self.init_dict[proc.t0].append(proc)
                else:
                    self.init_dict[proc.t0] = [proc]

    def loop(self):
        """Main loop of the simulator"""
        act_procs = {}
        t = 0
        while (len(self.procs) != 0 or len(act_procs) != 0):
            while (len(self.procs) != 0 and self.procs[0].t0 == t):
                proc = self.procs.popleft()
                act_procs[proc.pid] = proc
                self.fspc_manager.malloc(proc)
            for p in act_procs.values():
                if (len(p.mem_access) != 0 and p.mem_access[0][1] == t):
                    # Access the page that contains the position p.accesses[0][0]
                    self.pmem_manager.access(p)
                    p.mem_access.popleft()
            for p in list(act_procs.values()):
                if (p.tf == t):
                    self.fspc_manager.free(act_procs.pop(p.pid), self.pmem_manager)
            if (len(self.compact_list) != 0 and self.compact_list[0] == t):
                # Compacts physical and virtual memory
                self.compact(act_procs)
                self.compact_list.popleft()
            self.pmem_manager.update()
            # Print the bitmap
            if self.interval and t%self.interval == 0:
                self.__print_bitmap(act_procs)

            t += 1

    def compact(self, procs):
        """Compacts the physical and virtual memories"""
        proc = 0
        top = 0
        new_memmap = LinkedList()
        total_pages = self.virt_memory//self.page_size
        total_frames = self.phys_memory//self.page_size
        pg_to_ua = self.page_size//self.ua_size
        # Compact virtual memory
        pid = -1
        last_pid = -1
        for i in range(total_pages):
            pid = self.ptable.get_pid(i)
            if (top == i and pid != -1):
                top += 1
            elif (pid != -1):
                self.ptable.swap_pages(top, i)
                if (pid != last_pid):
                    procs[pid].base = top*pg_to_ua
                top += 1
            last_pid = pid
        # Compact memory map
        page = 0
        if (self.ptable.get_pid(page) != -1):
            pid = self.ptable.get_pid(page)
            size = 0
            while (self.ptable.get_pid(page+size) == pid):
                size += 1
            new_memmap.head = LinkedList.Node("P", page*pg_to_ua, size*pg_to_ua)
            page += size
            curr = new_memmap.head
        while (page < total_pages and self.ptable.get_pid(page) != -1):
            pid = self.ptable.get_pid(page)
            size = 0
            while (self.ptable.get_pid(page+size) == pid):
                size += 1
            curr.next = LinkedList.Node("P", page*pg_to_ua, size*pg_to_ua)
            page += size
            curr = curr.next
        if (new_memmap.head == None):
            new_memmap.head = LinkedList.Node("L", page*pg_to_ua, (total_pages - page)*pg_to_ua)
        else:
            curr.next = LinkedList.Node("L", page*pg_to_ua, (total_pages - page)*pg_to_ua)
        self.memmap = new_memmap
        self.fspc_manager.memmap = self.memmap
        # Compact physical memory
        page = 0
        frame = 0
        while (page < total_pages and self.ptable.get_pid(page) != -1):
            pg_to_frame = self.ptable.get_frame(page)
            if (pg_to_frame != -1):
                while (frame < total_frames and self.ftable.get_page(frame) != -1):
                    frame += 1
                if (pg_to_frame > frame):
                    self.ftable.swap_frames(frame, pg_to_frame)
                    self.pmem_manager.swap_frames(frame, pg_to_frame)
                    self.ptable.set_frame(page, frame)
            pg_to_frame = self.ptable.get_frame(page)
            if (pg_to_frame != -1):
                self.ftable.set_page(pg_to_frame, page)
            page += 1
        # If it's quick fit, reset and re-add to the quick sizes list
        if self.fspc_manager.id == 3:
            self.fspc_manager.reorder_references()

    def __print_bitmap(self, act_procs):
        """
        Print the bitmap of the current memory state
        """
        curr = self.memmap.head
        ndict = {p.base : p.pid for p in act_procs.values()}
        while curr:
            if curr.status == 'L':
                print("-1 "*curr.qtd*self.ua_size, end="")
            else:
                print(f"{ndict[curr.base]} "*curr.qtd*self.ua_size, end="")
            curr = curr.next
        print("")
