#!/usr/bin/env python
"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the main file, with the interactive shell of the memory simulator.
"""
import cmd, sys

class MemoryShell(cmd.Cmd):
    intro = "Welcome to the Memory Simulator! Type help or ? to list commands.\n"
    prompt = "[ep3]: "

    # EP3 shell methods
    def do_carrega(self, file):
        """Loads the file specified as argument for the simulation. Can be either relative or absolute path"""
        pass

    def do_espaco(self, num):
        """Informs the memory simulator the algorithm to run for free space management."""
        pass

    def do_substitui(self, num):
        """Informs the memory simulator the algorithm to run for pages substitution."""
        pass

    def do_executa(self, interval):
        """Runs the simulator and prints the memory state in 'interval' to 'interval' seconds, together with the bitmap content of the memory state"""
        pass

    def do_sai(self, arg):
        """Exists the memory simulator interactive shell"""
        # TODO: close files then exit
        exit()

if __name__ == '__main__':
    MemoryShell().cmdloop()
