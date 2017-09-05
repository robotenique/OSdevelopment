import subprocess as sb
import sys

def get_runCommand(schedType, traceFile, outFile, optional=""):
    cmd = ['../simproc', str(schedType), str(traceFile), str(outFile)]
    if optional:
        cmd.append(optional)
    return cmd

def main():
    args = [1, "med/medtrace0", "saida.out"]
    cmd = get_runCommand(*args)
    print(sb.check_output(cmd))

if __name__ == '__main__':
    main()
