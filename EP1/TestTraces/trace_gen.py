from sys import argv
from random import random
# Hardcoded distributions to generate
trace_dist = [[[10, 13, 10, 2, 2],
           [10, 13, 10, 10, 0.2],
           [10, 13, 10, 7, 0.7]],
          [[10, 73, 20, 10, 0.8],
           [10, 73, 20, 8, 0.5],
           [10, 73, 20, 40, 0.3]],
          [[10, 157, 80, 20, 1],
           [10, 157, 80, 5, 0.8],
           [10, 157, 80, 10, 0.7]]
         ]
filename = ['small', 'med', 'long']
def genHC():
    tc = trace_dist
    for category in range(len(tc)):
        for dist in range(len(tc[category])):
            files, lines, maxt0, maxdt, maxr = tc[category][dist]
            for j in range(dist*10, dist*10 + 10):
                with open(f"{filename[category]}trace{j}", "w+") as f:
                    t0s = list()
                    for i in range(lines):
                        t0s.append(maxt0*random())
                        t0s.sort()
                    for i in range(lines):
                        dt = maxdt*random()
                        dl = t0s[i] + dt + dt*maxr*random()                
                        f.write("{:.0f} {:.1f} {:.1f} processo{}\n".format(t0s[i], dt, dl, i))


def main():
    if (len(argv) < 6):
        if(len(argv) == 2 and argv[1] == '--giveall'):
            genHC();
        else:
            print("Wrong number of arguments!!")
            print("./trace_gen <number of files> <lines> <max t0> <max dt> <max deadline/dt ratio>")
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
