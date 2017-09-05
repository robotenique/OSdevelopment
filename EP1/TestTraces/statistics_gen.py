import subprocess as sb
from sys import argv

def get_runCommand(schedType, traceFile, outFile, optional=""):
    cmd = ['../simproc', str(schedType), str(traceFile), str(outFile)]
    if optional:
        cmd.append(optional)
    return cmd

def main():
    if (len(argv) < 3):
        print("Wrong number of arguments!!")
        print("Usage: ./statistics_gen <scheduler> <folder> <outfile>")
        return
    sched = int(argv[1])
    with open(argv[3], "w+") as f:
        for i in range(30):
            name = "{0}/{0}trace{1}".format(argv[2], str(i).zfill(2))
            args = [sched, name, "saida.out"]
            cmd = get_runCommand(*args)
            splits = sb.check_output(cmd).split()[-18:]
            stats = [float(splits[0][:-1]), float(splits[5]), float(splits[11]), float(splits[-2])]
            f.write("{} {} {} {}\n".format(*stats))
            print(f"{name} completed!!")

if __name__ == '__main__':
    main()
