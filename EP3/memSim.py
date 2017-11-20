#!/usr/bin/env python
"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the main file, with the interactive shell of the memory simulator.
"""
import cmd, sys
from simulator import Simulator
import numpy as np
#import readline

class MemoryShell(cmd.Cmd):
    intro = "Welcome to the Memory Simulator! Type help or ? to list commands.\n"
    prompt = "[ep3]: "

    # EP3 shell methods
    def do_carrega(self, file):
        """Loads the file specified as argument for the simulation. Can be either relative or absolute path"""
        self.file = open(file, "r")
        self.filename = file
        print(f"File \'{file}\' loaded!")


    def do_espaco(self, num):
        """Informs the memory simulator the algorithm to run for free space management."""
        self.fspc_id = int(num)
        print(f"Free space manager loaded: \'{num}\'.")

    def do_substitui(self, num):
        """Informs the memory simulator the algorithm to run for pages substitution."""
        self.pmem_id = int(num)
        print(f"Page substitution manager loaded: \'{num}\'.")

    def do_executa(self, interval):
        """Runs the simulator and prints the memory state in 'interval' to 'interval' seconds, together with the bitmap content of the memory state"""
        sim = Simulator(open(self.filename, "r"), self.fspc_id, self.pmem_id, interval)
        sim.loop()

    def do_sai(self, arg):
        """Exits the memory simulator interactive shell"""
        if (getattr(self, "file", None)):
            self.file.close()
        exit()

    def do_fullinit(self, arg):
        self.file = open("input7.in", "r")
        sim = Simulator(self.file, 2, 4, 10)
        sim.loop()

    def do_qinit(self, arg):
        arg1, arg2 = map(int, arg.split())
        self.file = open("input7.in", "r")
        sim = Simulator(self.file, arg1, arg2, 10)
        sim.loop()

    def do_test(self, arg):
        self.file = open("input7.in", "r")
        alloc_times = []
        esp, sub = map(int, arg.split())
        for i in range(30):
            self.file.seek(0)
            sim = Simulator(self.file, esp, sub, 10)
            sim.loop()
            alloc_times.append(sim.fspc_manager.alloc_time*1000)
        print("Page faults:", sim.pmem_manager.page_faults)
        print("Alloc times:", alloc_times)
        alloc_times = np.array(alloc_times)
        print("Average alloc time:", alloc_times.mean(), "ms")
        print("alloc time standard dev:", alloc_times.std(), "ms")


if __name__ == '__main__':
    MemoryShell().cmdloop()
