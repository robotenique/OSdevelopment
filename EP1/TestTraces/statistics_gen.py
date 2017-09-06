import subprocess as sb
from sys import argv
NUM_FILES = 30
#data = np.loadtxt('ex1data1.txt', delimiter=',')
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

def main():
    if (len(argv) < 4):
        print("Wrong number of arguments!!")
        print("Usage: ./statistics_gen <scheduler> <folder> <outfile> <SIGMOIDPLZ / optional>")
        return
    sched = int(argv[1])
    if(argv[3] == "saida.out"):
        print("PLEASE, A DIFFERENT FILENAME!")
        exit()
    optArg =  argv[len(argv) - 1] if argv[len(argv) - 1] == "SIGMOIDPLZ" else ""
    with open(argv[3], "w+") as f:
        for i in range(NUM_FILES):
            name = "{0}/{0}trace{1}".format(argv[2], str(i).zfill(2))
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
            f.write("{} {} {} {}\n".format(*stats))
            print(f"{name} completed!!")

if __name__ == '__main__':
    main()
