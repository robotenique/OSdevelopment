from sys import argv
from random import random, uniform
# Hardcoded distributions to generate >:)
trace_dist = [[
        #files   lines   maxt0   maxdt    mindt   maxr  minr
        #----------------------------------------------------
        #small
         [10,     7,      10,      8,    0.5,       2,   0.8], # Tempo folgado...
         [10,     7,      10,     10,      1,     0.2,   0.5], # Bem apertado...
         [10,     7,      10,      5,      2,     0.7,   0.6]],# Razoável...
        #medium
        [[10,     19,     15,     20,    0.5,       2,    1], # Grande variabilidade no dt.. deadline curta
         [10,     19,     15,     20,      5,       3,  1.5], # Dt's duram mais, porem deadline um pouco melhor
         [10,     19,     15,     40,      9,       4,    1]],# dts podem durar MUITO, deadline boa
        #long
        [[10,     47,     30,     15,    0.5,       3,   0.5], # Grande variabilidade de dt. deadline curta
         [10,     47,     20,     20,      5,       3,     1], # dt's duram mais. deadline continua curta
         [10,     47,     15,     30,     10,       5,     2]]# varios proc. em pouco tempo. dT grande, deadline boa.
         ]
filename = ['small', 'med', 'long']
def genHC():
    tc = trace_dist
    for category in range(len(tc)):
        for dist in range(len(tc[category])):
            files, lines, maxt0, maxdt, mindt, maxr, minr = tc[category][dist]
            for j in range(dist*10, dist*10 + 10):
                with open(f"{filename[category]}trace{str(j).zfill(2)}", "w+") as f:
                    t0s = list()
                    for i in range(lines):
                        t0s.append(maxt0*random())
                        t0s.sort()
                    for i in range(lines):
                        dt = uniform(mindt, maxdt)
                        dl = t0s[i] + dt + dt*maxr*random() + minr*dt
                        f.write("{:.1f} {:.1f} {:.1f} processo{}\n".format(t0s[i], dt, dl, i))


def main():
    if (len(argv) < 6):
        if(len(argv) == 2 and argv[1] == '--giveall'):
            genHC()
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
