import subprocess as sb
from sys import argv
import numpy as np
from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt

NUM_FILES = 30
NCORES = 32


def get_runCommand(schedType, traceFile, outFile, optional=""):
    cmd = ['../ep1', str(schedType), str(traceFile), str(outFile)]
    if optional:
        cmd.append(optional)
    return cmd

def printList(l):
    for i in l:
        print("[",end="")
        print(i)
        print("]",end="\n")
    print("")

def genConfidenceInterval(f):
    data = np.loadtxt(f)
    avg = [0.0, 0.0, 0.0]
    var = [0.0, 0.0, 0.0]
    CI = []
    n = len(data)
    for line in data:
        avg[0] += line[0]
        avg[1] += line[3]
        avg[2] += line[4]
        var[0] += line[0]**2
        var[1] += line[3]**2
        var[2] += line[4]**2
    for i in range(3):
        avg[i] /= n
        var[i] /= n-1
        var[i] -= n*(avg[i]**2)/(n-1)
        t = 2.042*np.sqrt(var[i]/n)
        CI.append([avg[i] - t, avg[i] + t])
    print(f"##===================== {f} ========================##")
    print("Completed deadlines:")
    print(f"Avg = {avg[0]:.2f}  /  Var = {var[0]:.2f}  /  IC = [{CI[0][0]:.2f}, {CI[0][1]:.2f}]")
    print("Context changes:")
    print(f"Avg = {avg[1]:.2f}  /  Var = {var[1]:.2f}  /  IC = [{CI[1][0]:.2f}, {CI[1][1]:.2f}]")
    print("Wait time:")
    print(f"Avg = {avg[2]:.2f}  /  Var = {var[2]:.2f}  /  IC = [{CI[2][0]:.2f}, {CI[2][1]:.2f}]")
    print("-"*len(f"##===================== {f} ========================##")+"\n")
    return (avg, var, CI)

def plotSpecific(titlePlot, yLabel, appFunction, stats, cat, schedL):
    schedNames = ["Shortest Job First","Round Robin", "Prioridade", "Prioridade (SIGMOID)"]
    colors = ["skyblue", "chartreuse", "gold", "tomato"]
    barWidth = 8 if len(stats) == 4 else 10
    tickLoc = np.linspace(10 ,50, 4 if len(stats) == 4 else 3)
    sjf, rr, prrstat, prrsigma = schedL
    fig, axarr = plt.subplots(figsize=(9, 7))
    # Deadline graph
    axarr.set_xlabel("Tipo de escalonador", size=14)
    axarr.set_ylabel(yLabel, size=14)
    axarr.set_title(f"{titlePlot} - {NCORES} core{'s' if NCORES == 1 else ''} ({cat})", size=18)
    y = [appFunction(sched) for sched in [sjf, rr, prrstat, prrsigma]]
    for l, h, i in zip(tickLoc, y, range(len(y))):
        axarr.bar(l, h, barWidth, bottom=0.001, align='center', color=colors[i], zorder=3)
    axarr.set_xticks(tickLoc)
    axarr.set_xticklabels(schedNames, size=12)
    plt.tight_layout()
    plt.show()

def plotCategory(cat, stats, truName):
    giveMeDEADLINE = lambda sched : sched[0][0][0]
    giveMeCONTEXT = lambda sched : sched[0][0][1]
    giveMeWAITTIME = lambda sched : sched[0][0][2]
    sjf = [x for x in stats if truName+"SJF" in x[len(x) - 1]]
    rr = [x for x in stats if truName+"RR" in x[len(x) - 1]]
    prrstat = [x for x in stats if truName+"PRR." in x[len(x) - 1]]
    prrsigma = [x for x in stats if truName+"PRRSIGMOD" in x[len(x) - 1]]
    plotSpecific("Cumprimento de deadline", "Deadline cumpridas (%)",
                  giveMeDEADLINE, stats, cat, [sjf, rr, prrstat, prrsigma])
    plotSpecific("Mudanças de contexto", "Mudanças de contexto",
                  giveMeCONTEXT, stats, cat, [sjf, rr, prrstat, prrsigma])
    plotSpecific("Tempo médio de espera", "Tempo médio (s)",
                  giveMeWAITTIME, stats, cat, [sjf, rr, prrstat, prrsigma])


def genGraphics(folderName):
    files = [f for f in listdir(folderName) if isfile(join(folderName, f))]
    stats_dict = {f:genConfidenceInterval(folderName+"/"+f) for f in files}
    stats_dict["S"] = [stat+(name,) for name, stat in stats_dict.items() if "small" in name]
    stats_dict["M"] = [stat+(name,) for name, stat in stats_dict.items() if "med" in name]
    stats_dict["L"] = [stat+(name,) for name, stat in stats_dict.items() if "long" in name]
    plotCategory('qtd baixa', sorted(stats_dict["S"]), "small")
    plotCategory('qtd média', sorted(stats_dict["M"]), "med")
    plotCategory('qtd alta', sorted(stats_dict["L"]), "long")


def printUsage():
    print("Wrong number of arguments!!")
    print("Usage: ./statistics_gen -i <file>       EXAMPLE: ./statistics_gen.py -graphs results32Cores  :)")
    print("Usage: ./statistics_gen -graphs <folder>")
    print("Or:    ./statistics_gen -s <scheduler> <folder> <outfile> <SIGMOIDPLZ / optional>")
    exit()

def main():
    if (len(argv) < 2):
        printUsage()
    if (argv[1] == "-i" and len(argv) == 3):
        genConfidenceInterval(argv[2])
    if(argv[1] == "-graphs" and len(argv) == 3):
        genGraphics(argv[2])
    elif (argv[1] == "-s"):
        if (len(argv) < 5):
            print("Wrong number of arguments!!")
            print("Usage: ./statistics_gen -s <scheduler> <folder> <outfile> <SIGMOIDPLZ / optional>")
            return
        sched = int(argv[2])
        if(argv[4] == "saida.out"):
            print("PLEASE, A DIFFERENT FILENAME!")
            exit()
        optArg =  argv[len(argv) - 1] if argv[len(argv) - 1] == "SIGMOIDPLZ" else ""
        with open(argv[4], "w+") as f:
            for i in range(NUM_FILES):
                name = "{0}/{0}trace{1}".format(argv[3], str(i).zfill(2))
                args = [sched, name, "saida.out"]
                cmd = get_runCommand(*args, optional=optArg)
                splits = sb.check_output(cmd).decode("utf-8")
                lines = list(filter(None, splits.split("%||")))
                lines = list(map(lambda s : s.strip(), lines))
                lines = list(map(lambda s : s.replace("||%",""), lines))
                procInfo = lines[0].split("\n")
                stats = lines[1].split("\n")
                procInfo = list(map(lambda k : repr(k.replace("\n","")), procInfo))
                stats = list(map(lambda k : repr(k.replace("\n","")), stats))
                stats = list(map(lambda sL : ''.join((i for i in sL if i.isdigit() or i=='.')), stats))
                stats = list(map(lambda sL : float(sL), list(filter(None, stats))))
                f.write("{} {} {} {} {}\n".format(*stats))
                print(f"{name} completed!!")
    else:
        printUsage()

if __name__ == '__main__':
    main()
