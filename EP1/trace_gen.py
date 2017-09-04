from sys import argv
from random import random

def main():
    if (len(argv) < 6):
        print("Wrong number of arguments!!")
        print("./trace_gen <number of files> <lines> <max t0> <max dt> <max deadline/dt ratio>")
        return
    files = int(argv[1])
    lines = int(argv[2])
    maxt0 = int(argv[3])
    maxdt = int(argv[4])
    maxr = int(argv[5])
    for j in range(files):
        with open(f"trace{j}.in", "w+") as f:
            t0s = []
            for i in range(lines):
                t0s.append(maxt0*random())
            t0s.sort()
            for i in range(lines):
                dt = maxdt*random()
                dl = t0s[i] + maxr*maxdt*random()
                f.write("{:.0f} {:.1f} {:.1f} processo{}\n".format(t0s[i], dt, dl, i))

if __name__ == "__main__":
    main()
