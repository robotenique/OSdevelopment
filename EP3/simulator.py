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

fspc_managers = [None, BestFit]

class Process(object):
    next_pid = 0
    def __init__(self, vals):
        self.name = vals.pop(3)
        self.mem_access = deque()
        print(vals)
        vals = list(map(int, vals))
        self.t0 = vals[0]
        self.tf = vals[1]
        self.b  = vals[2]
        self.pid = Process.next_pid
        Process.next_pid += 1
        for i in range(3, len(vals) - 1, 2):
            self.mem_access.append((vals[i], vals[i + 1]))

    def __repr__(self):
        return f"<pid: {self.pid}>"

    def __str__(self):
        return f"{self.name} ([t0, tf] = ({self.t0}, {self.tf}), [size] : {self.b}, mem_acess : {self.mem_access}"

    def reset_pids():
        Process.next_pid = 0

class Simulator(object):
    VMEMORY_PATH = "/tmp/ep3.mem"
    PMEMORY_PATH = "/tmp/ep3.vir"

    def __init__(self, input_file, fspc_id, pmem_id, dt):
        self.input_file = input_file
        self.interval = dt
        self.compact_list = deque()
        self.procs = deque()
        Process.reset_pids()
        self.init_dict = dict() # Initialization dictionary
        self.parse()
        for i in self.procs:
            print(i)
        # TODO: One of these should use the ua_size...
        self.pfile = MemoryWriter(self.PMEMORY_PATH, self.page_size)
        self.vfile = MemoryWriter(self.VMEMORY_PATH, self.page_size)
        self.fspc_manager = fspc_managers[fspc_id](self.virtual_memory, self.ua_size, self.vfile)
        #self.pmem_manager = pagination_managers[pmem_id]()

    def debug_loop(self):
        tf = max([x.tf for x in self.procs])
        for t in range(tf):
            self.init_procs(t)



    def init_procs(self, t):
        """Allocate memory for all processes which arrived at time t"""
        init_list = self.init_dict.get(t)
        if init_list:
            for p in init_list:
                self.fspc_manager.malloc(p)


    def parse(self):
        for num_line, line in enumerate(self.input_file):
            vals = line.split()
            if vals[0] == '#' or vals[0][0] == '#':
                continue
            if num_line == 0:
                vals = list(map(int, vals))
                self.total_memory = vals[0]
                self.virtual_memory = vals[1]
                self.ua_size = vals[2]
                self.page_size = vals[3]
            elif "COMPACTAR" in line:
                self.compact_list.append(int(vals[0]))
            else:
                proc = Process(vals)
                self.procs.append(proc)
                if self.init_dict.get(proc.t0):
                    self.init_dict[proc.t0].append(proc)
                else:
                    self.init_dict[proc.t0] = [proc]





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
