"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the Simulator object file.
"""
from collections import deque

class Process(object):

    def __init__(self, vals):
        self.name = vals.pop(3)
        self.mem_access = deque()
        vals = list(map(int, vals))
        self.t0 = vals[0]
        self.tf = vals[1]
        self.b  = vals[2]
        i = 4
        while i < len(vals) - 1:
            self.mem_access.append((vals[i], vals[i + 1]))
            i += 2

        # TODO: Why doesn't __repr__ and __str__ doesn't work?? D:
        def pdebug(self):
            return f"{self.name} ([t0, tf] = ({self.t0}, {self.tf}), [size] : {self.b}, mem_acess : {self.mem_access}"
        def __repr__(self):
            return f"{self.name} ([t0, tf] = ({self.t0}, {self.tf}), [size] : {self.b}, mem_acess : {self.mem_access}"
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
        #self.fspc_manager = freespace_managers[fspc_id]
        #self.pmem_manager = pagination_managers[pmem_id]
        self.interval = dt
        self.compact_list = deque()
        self.procs = deque()
        self.parse()
        for i in self.procs:
            print(i.pdebug())

        #self.ffile = MemoryWriter(ffile, p)
        #self.vfile = MemoryWriter(vfile, p)
        """        with open(pfile, "r+") as f:
            lines = f.readlines()
        (self.procs_stack, self.compacts) = parse(lines)
        """

    def loop():
        procs = []
        t = 0
        while (len(self.procs_stack) != 0 or len(procs) != 0):
            while (self.procs_stack[0].t0 == t):
                procs.push(self.procs_stack.pop(0))
            for p in procs:
                if (len(p.accesses) != 0 and p.accesses[0].t == t):
                    # Access the page that contains the position p.accesses[0].p
                    p.accesses.pop(0)
            for i in range(len(procs)-1, -1, -1):
                if (procs.tf == t):
                    procs.pop(i)
            if (len(sel.compacts) != 0 and self.compacts[0] == t):
                # Compacts physical and virtual memory
                self.compacts.pop(0)
            t += 1
