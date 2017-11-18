import sys
from random import randint, sample
from math import ceil

INPUT = [
    #ua       page_size  procs   comps   tf      max_dt   max_size   max_access
    #----------------------------------------------------
    [4,       8,         10,     2,      20,     6,       10,        3],
    [4,      12,         64,     7,     200,    30,       48,       10]
]

class Proc:
    ID = 0
    def __init__(self, tf, max_dt, max_size, max_access):
        self.t0 = randint(0, tf-max_dt)
        self.tf = min(randint(1, max_size), tf - self.t0) + self.t0
        self.size = randint(1, max_size)
        self.id = Proc.ID
        Proc.ID += 1
        num_access = min(randint(0, max_access), self.tf - self.t0)
        access_times = sorted(sample(range(self.t0, self.tf), num_access))
        self.accesses = []
        for i in access_times:
            self.accesses.append(randint(0, self.size))
            self.accesses.append(i)

    def is_process(self):
        return True

    def __str__(self):
        return f"{self.t0} {self.tf} {self.size} proc{self.id} {vec2str(self.accesses)}\n"

class Compact:
    def __init__(self, tf, max_dt):
        self.t0 = randint(0, tf-max_dt)

    def is_process(self):
        return False

    def __str__(self):
        return f"{self.t0} COMPACTAR\n"

def vec2str(v):
    return v.__str__().strip("[]").replace(",", "")

def generate(ua, page_size, num_procs, num_comps, tf, max_dt, max_size, max_access):
    lines = []
    for i in range(num_procs):
        lines.append(Proc(tf, max_dt, max_size, max_access))
    for i in range(num_comps):
        lines.append(Compact(tf, max_dt))
    lines = sorted(lines, key=lambda i: i.t0)
    pmax = 0
    fmax = 0
    for i in range(tf):
        pages = 0
        frames = 0
        for j in range(len(lines)):
            if (lines[j].is_process() and lines[j].t0 <= i and lines[j].tf >= i):
                pages += ceil(lines[j].size/page_size)
                for k in range(1, len(lines[j].accesses), 2):
                    if (lines[j].accesses[k] == i):
                        frames += 1
        if (pages > pmax):
            pmax = pages
        if (frames > fmax):
            fmax = frames
    return pmax, fmax, lines


def main():
    f = open("input6.in", "w+")
    ua = INPUT[1][0]
    page_size = INPUT[1][1]
    pmax, fmax, lines = generate(*(INPUT[1]))
    vsize = ceil(pmax*1.5)*page_size
    psize = ceil(fmax*1.5)*page_size
    f.write(f"{psize} {vsize} {ua} {page_size}\n")
    for i in lines:
        f.write(i.__str__())
    f.close()

if __name__ == "__main__":
    main()
