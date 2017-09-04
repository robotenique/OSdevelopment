from sys import argv
from random import random


def main():
    if (len(argv) < 5):
        print("Wrong number of arguments!!")
        print("./trace_gen <lines> <max t0> <max dt> <max deadline/dt ratio>")
        return
    lines = int(argv[1])
    maxt0 = int(argv[2])
    maxdt = int(argv[3])
    maxr = int(argv[4])
    t0s = []
    for i in range(lines):
        t0s.append(maxt0*random())
    t0s.sort()
    for i in range(lines):
        dt = maxdt*random()
        dl = t0s[i] + maxr*maxdt*random()
        print("{:.0f} {:.1f} {:.1f} processo{}".format(t0s[i], dt, dl, i))

if __name__ == "__main__":
    main()
