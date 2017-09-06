import subprocess as sb
from sys import argv
import numpy as np

NUM_FILES = 30
def get_runCommand(schedType, traceFile, outFile, optional=""):
    cmd = ['../simproc', str(schedType), str(traceFile), str(outFile)]
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
    print("Completed deadlines:")
    print(f"Avg = {avg[0]:.2f}  /  Var = {var[0]:.2f}  /  IC = [{CI[0][0]:.2f}, {CI[0][1]:.2f}]")
    print("Context changes:")
    print(f"Avg = {avg[1]:.2f}  /  Var = {var[1]:.2f}  /  IC = [{CI[1][0]:.2f}, {CI[1][1]:.2f}]")
    print("Wait time:")
    print(f"Avg = {avg[2]:.2f}  /  Var = {var[2]:.2f}  /  IC = [{CI[2][0]:.2f}, {CI[2][1]:.2f}]")


def main():
    if (len(argv) < 2):
        print("Wrong number of arguments!!")
        print("Usage: ./statistics_gen -i <file>")
        print("Or:    ./statistics_gen -s <scheduler> <folder> <outfile> <SIGMOIDPLZ / optional>")
        return
    if (argv[1] == "-i"):
        genConfidenceInterval(argv[2])
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

if __name__ == '__main__':
    main()
