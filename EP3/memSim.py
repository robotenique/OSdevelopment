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
        sim = Simulator(open(self.filename, "r"), self.fspc_id, self.pmem_id, int(interval))
        sim.loop()

    def do_sai(self, arg):
        """Exits the memory simulator interactive shell"""
        if (getattr(self, "file", None)):
            self.file.close()
        exit()


if __name__ == '__main__':
    MemoryShell().cmdloop()
