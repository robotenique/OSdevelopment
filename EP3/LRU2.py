"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the Least Recently Used version 2 object file.
"""

from math import ceil
from tables import FrameTable

class LRU2(object):

    def __init__(self, total_memory, page_size, pfile, ptable, ftable):
        self.MAX_VAL = (1 << size) - 1     # Generates a number with 'size' ones
        self.matrix = [0 for i in range(size)]
        self.page_size = page_size
        self.pfile = pfile
        self.pages_table = ptable
        self.frame_table = ftable

    def access(self, proc, pos):
        page = (proc.base + pos)//self.page_size
        frame = self.pages_frame.get_frame(page)
        if (frame == -1):
            out_page, new_frame = get_new_frame(page)
            if (out_page != -1):
                self.pages_frame.set_presence(out_page, False)
            self.pages_frame.set_presence(page, True)


    def get_new_frame(self, page):
        """Apply the algorithm to find the best page to be given to a process"""
        frame = 0
        bit_mask = self.matrix[0]
        for i in range(1, len(self.matrix)):
            if (self.matrix[i] < bit_mask):
                frame = i
                bit_mask = self.matrix[i]
                if bit_mask == 0:
                    break
        # if (self.pf_table[page].bit_m):
            # Copy to physical memory?
            # self.self.pf_table[page].bit_m = False
        self.matrix[frame] = self.MAX_VAL
        out_page = self.frame_table.get_page(frame)
        return out_page, frame

    # TODO: Decide if the pid will be written at all the page or just at the
    # accessed position
    def use_frame(frame):
        """Updates the matrix as if an access to 'page' just happened"""
        num = self.MAX_VAL
        self.matrix[frame] = num
        num ^= (1 << frame)                # Turns off the 'frame' bit
        for i in range(frame):
            self.matrix[i] &= num
        # self.frame_table[frame].bit_m = True

    def delete_frame(frame):
        """Delete all the information at a page.
           Used to delete a page from a finished process"""
        self.matrix[frame] = 0
        self.frame_table.reset_frame(frame)
        self.pfile.clean(frame)
        # self.M[frame] = False
