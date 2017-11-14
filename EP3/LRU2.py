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

    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        self.tot_pages = total_memory//page_size
        self.MAX_VAL = (1 << self.tot_pages) - 1     # Generates a number with 'tot_pages' ones
        self.matrix = [0 for i in range(self.tot_pages)]
        self.total_memory = total_memory
        self.page_size = page_size
        self.ua = ua
        self.pages_table = ptable
        self.frame_table = ftable

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

    def access(self, proc, pos):
        """Updates the matrix as if an access to 'page' just happened"""
        page = (proc.base*self.ua + pos)//self.page_size
        print(f"pid {proc.pid}, base {proc.base}, pos {pos}, page size {self.page_size}, page {page}")
        frame = self.pages_table.get_frame(page)
        if (frame == -1):
            out_page, frame = self.get_new_frame(page)
            if (out_page != -1):
                self.pages_table.set_presence(out_page, False)
            self.pages_table.set_presence(page, True)
            frame_pos = frame*self.page_size
            self.frame_table.write_stream(frame_pos, page, self.pages_table.read(page))
            self.pages_table.set_frame(page, frame)
        num = self.MAX_VAL
        self.matrix[frame] = num
        num ^= (1 << frame)                # Turns off the 'frame' bit
        for i in range(len(self.matrix)):
            self.matrix[i] &= num
        print(f"Process: {proc.pid}\nFrame accessed: {frame}")
        print(list(map(lambda a: format(a, f"#0{self.tot_pages}b"),self.matrix)))
        debug_ptable(self.frame_table.table, self.page_size)
        # self.frame_table[frame].bit_m = True

    def delete_frame(frame):
        """Delete all the information at a page.
           Used to delete a page from a finished process"""
        self.matrix[frame] = 0
        self.frame_table.reset_frame(frame)

def debug_ptable(ptable, page_size):
    print(f"== FRAMES TABLE == -> {page_size}")
    for p in ptable:
        print(p)
    print("")
