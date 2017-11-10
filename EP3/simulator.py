"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the Simulator object file.
"""

class Simulator(object):

    def __init__(self, fs, vs, s, p, fsm, psm, pfile):
        self.fs = fs
        self.vs = vs
        self.s = s
        self.p = p
        self.fsm = FreeSpaceMan[fsm]
        self.psm = PageSubstitutionMan[psm]
        self.ffile = MemoryWriter("/tmp/ep3.mem", p)
        self.vfile = MemoryWriter("/tmp/ep3.vir", p)
        with open(pfile, "r+") as f:
            lines = f.readlines()
        (self.procs_stack, self.compacts) = parse(lines)

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
