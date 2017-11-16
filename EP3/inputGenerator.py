import sys
from random import randint, sample

INPUT = [
    #procs   comps   tf      max_dt   max_size   max_access
    #----------------------------------------------------
    [10,     2,      20,     6,       10,        3]
]

MEM_INFOS = [50, 100, 4, 8]

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

    def process(self):
        return True

    def __str__(self):
        return f"{self.t0} {self.tf} {self.size} proc{self.id} {vec2str(self.accesses)}\n"

class Compact:
    def __init__(self, tf, max_dt):
        self.t0 = randint(0, tf-max_dt)

    def process(self):
        return False

    def __str__(self):
        return f"{self.t0} COMPACTAR\n"

def vec2str(v):
    return v.__str__().strip("[]").replace(",", "")

def generate(num_procs, num_comps, tf, max_dt, max_size, max_access):
    lines = []
    for i in range(num_procs):
        lines.append(Proc(tf, max_dt, max_size, max_access))
    for i in range(num_comps):
        lines.append(Compact(tf, max_dt))
    return sorted(lines, key=lambda i: i.t0)

def main():
    f = open("input3.in", "w+")
    f.write(vec2str(MEM_INFOS) + "\n")
    for i in generate(*(INPUT[0])):
        f.write(i.__str__())
    f.close()

if __name__ == "__main__":
    main()
