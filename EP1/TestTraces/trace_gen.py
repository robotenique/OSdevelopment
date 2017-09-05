from sys import argv
from random import random
# TODO: transform the random() into normal distribution
# Hardcoded distributions to generate
trace_dist = [[[10, 13, 10, 2, 1 , 2, 0],
           [10, 13, 10, 10, 1, 0.2, 0],
           [10, 13, 10, 1, 7, 0.7, 0]],
          [[10, 73, 20, 30, 5,  4, 1],
           [10, 73, 20, 80, 30, 8, 1],
           [10, 73, 20, 40, 20, 9, 1]],
          [[10, 157, 30, 50, 2, 1, 0],
           [10, 157, 30, 40, 1, 0.8, 0],
           [10, 157, 30, 80, 40, 0.7, 0]]
         ]
filename = ['small', 'med', 'long']
def genHC():
    tc = trace_dist
    for category in range(len(tc)):
        for dist in range(len(tc[category])):
            files, lines, maxt0, maxdt, mindt, maxr, minr = tc[category][dist]
            for j in range(dist*10, dist*10 + 10):
                with open(f"{filename[category]}trace{j}", "w+") as f:
                    t0s = list()
                    for i in range(lines):
                        t0s.append(maxt0*random())
                        t0s.sort()
                    for i in range(lines):
                        dt = maxdt*random()
                        dl = t0s[i] + dt + dt*maxr*random() + minr*dt
                        f.write("{:.0f} {:.1f} {:.1f} processo{}\n".format(t0s[i], dt, dl, i))


def main():
    if (len(argv) < 6):
        if(len(argv) == 2 and argv[1] == '--giveall'):
            genHC();
        else:
            print("Wrong number of arguments!!")
            print("Usage: ./trace_gen <number of files> <lines> <max t0> <max dt> <max deadline/dt ratio>")
        return
    files = int(argv[1])
    lines = int(argv[2])
    maxt0 = float(argv[3])
    maxdt = float(argv[4])
    maxr = float(argv[5])
    for j in range(files):
        with open(f"trace{j}", "w+") as f:
            t0s = []
            for i in range(lines):
                t0s.append(maxt0*random())
            t0s.sort()
            for i in range(lines):
                dt = maxdt*random()
                dl = t0s[i] + dt + dt*maxr*random()
                f.write("{:.0f} {:.1f} {:.1f} processo{}\n".format(t0s[i], dt, dl, i))

if __name__ == "__main__":
    main()
