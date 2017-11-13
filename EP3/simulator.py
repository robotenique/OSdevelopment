"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the Simulator object file.
"""
from collections import deque
from memoryWriter import MemoryWriter
from freeSpace import BestFit

freespace_managers = [None, BestFit]

class Process(object):

    def __init__(self, vals):
        self.name = vals.pop(3)
        self.mem_access = deque()
        print(vals)
        vals = list(map(int, vals))
        self.t0 = vals[0]
        self.tf = vals[1]
        self.b  = vals[2]
        for i in range(3, len(vals)-1, 2):
            self.mem_access.append((vals[i], vals[i + 1]))

    def __str__(self):
        return f"{self.name} ([t0, tf] = ({self.t0}, {self.tf}), [size] : {self.b}, mem_acess : {self.mem_access}"

class Simulator(object):
    VMEMORY_PATH = "/tmp/ep3.mem"
    PMEMORY_PATH = "/tmp/ep3.vir"

    def parse(self):
        for num_line, line in enumerate(self.input_file):
            vals = line.split()
            if num_line == 0:
                vals = list(map(int, vals))
                self.total_memory = vals[0]
                self.virtual_memory = vals[1]
                self.ua_size = vals[2]
                self.page_size = vals[3]
            elif "COMPACTAR" in line:
                self.compact_list.append(int(vals[0]))
            else:
                self.procs.append(Process(vals))


    def __init__(self, input_file, fspc_id, pmem_id, dt):
        self.input_file = input_file
        self.interval = dt
        self.compact_list = deque()
        self.procs = deque()
        self.parse()
        for i in self.procs:
            print(i)
        self.pfile = MemoryWriter(self.PMEMORY_PATH, self.page_size)
        self.fspc_manager = freespace_managers[fspc_id](self.total_memory, self.ua_size, self.pfile)
        #self.pmem_manager = pagination_managers[pmem_id]()
        #self.vfile = MemoryWriter(self.VMEMORY_PATH, self.page_size)

    def loop():
        act_procs = []
        t = 0
        while (len(self.procs) != 0 or len(act_procs) != 0):
            while (self.procs[0].t0 == t):
                act_procs.push(self.procs.pop(0))
            for p in act_procs:
                if (len(p.mem_access) != 0 and p.mem_access[0].t == t):
                    # Access the page that contains the position p.accesses[0].p
                    p.mem_access.pop(0)
            for i in range(len(act_procs)-1, -1, -1):
                if (act_procs.tf == t):
                    act_procs.pop(i)
            if (len(sel.compact_list) != 0 and self.compact_list[0] == t):
                # Compacts physical and virtual memory
                self.compact_list.pop(0)
            t += 1
