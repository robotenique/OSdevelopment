"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the Least Recently Used version 2 object file.
"""

import math
from tables import FrameTable
from abc import ABC, abstractmethod
from memsimWrapper import doc_inherit
from collections import deque


class PaginationManager(ABC):

    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        self.tot_pages = total_memory//page_size
        self.total_memory = total_memory
        self.page_size = page_size
        self.ua = ua
        self.pages_table = ptable
        self.frames_table = ftable
        self.page_faults = 0

    @abstractmethod
    def update(self):
        """Updates the control table when needed"""
        pass

    @abstractmethod
    def get_new_frame(self):
        """Apply the algorithm to find the best page to be given to a process"""
        pass

    @abstractmethod
    def access(self, proc):
        """Updates the matrix as if an access to process 'proc' just happened"""
        pass

    @abstractmethod
    def swap_frames(self, frame1, frame2):
        """Lets the object know that two frames were swaped"""
        pass

    def delete_frame(self, frame):
        """Delete all the information at a page.
           Used to delete a page from a finished process"""
        self.frames_table.reset_frame(frame)

    @abstractmethod
    def print_table(self):
        pass



class Optimal(PaginationManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        super().__init__(total_memory, ua, page_size, ptable, ftable)
        self.next_access = [math.inf for i in range(self.tot_pages)]
        self.time = 0
        self.id = 1

    @doc_inherit
    def update(self):
        self.time += 1
        for i in range(len(self.next_access)):
            self.next_access[i] -= 1
            if (self.next_access[i] == -1):
                self.next_access[i] = math.inf

    @doc_inherit
    def get_new_frame(self):
        frame = 0
        mx = self.next_access[0]
        for i in range(1, len(self.next_access)):
            if (self.next_access[i] > mx):
                frame = i
                mx = self.next_access[i]
                if mx == math.inf:
                    break
        return frame

    @doc_inherit
    def access(self, proc):
        pos = proc.mem_access[0][0]
        page = (proc.base*self.ua + pos)//self.page_size
        frame = self.pages_table.get_frame(page)
        print(f"pid {proc.pid}, base {proc.base}, pos {pos}, page size {self.page_size}, page {page}")
        if (frame == -1):
            self.page_faults += 1
            frame = self.get_new_frame()
            out_page = self.frames_table.get_page(frame)
            if (out_page != -1):
                self.pages_table.set_presence(out_page, False)
            self.pages_table.set_presence(page, True)
            frame_pos = frame*self.page_size
            self.frames_table.write_stream(frame_pos, page, self.pages_table.read(page))
            self.pages_table.set_frame(page, frame)
        if (len(proc.mem_access) == 1):
            self.next_access[frame] = math.inf
        else:
            self.next_access[frame] = proc.mem_access[1][1] - self.time
        print(f"Process: {proc.pid}\nFrame accessed: {frame}")

    @doc_inherit
    def swap_frames(self, frame1, frame2):
        na1 = self.next_access[frame1]
        self.next_access[frame1] = self.next_access[frame2]
        self.next_access[frame2] = na1

    @doc_inherit
    def delete_frame(self, frame):
        self.next_access[frame] = math.inf
        super().delete_frame(frame)

    @doc_inherit
    def print_table(self):
        debug_ptable(self.frames_table.table, self.page_size, self.next_access)


class FIFO(PaginationManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        super().__init__(total_memory, ua, page_size, ptable, ftable)
        self.fifo = []
        self.num_frames = total_memory//page_size
        self.id = 2

    @doc_inherit
    def update(self):
        print(self.fifo)
        self.print_table()

    @doc_inherit
    def get_new_frame(self):
        if (len(self.fifo) == self.num_frames):
            return self.fifo.pop(0)
        for i in range(self.num_frames):
            if (self.frames_table.get_page(i) == -1):
                return i

    @doc_inherit
    def access(self, proc):
        pos = proc.mem_access[0][0]
        page = (proc.base*self.ua + pos)//self.page_size
        frame = self.pages_table.get_frame(page)
        print(f"pid {proc.pid}, base {proc.base}, pos {pos}, page size {self.page_size}, page {page}")
        if (frame == -1):
            self.page_faults += 1
            frame = self.get_new_frame()
            out_page = self.frames_table.get_page(frame)
            if (out_page != -1):
                self.pages_table.set_presence(out_page, False)
            self.pages_table.set_presence(page, True)
            frame_pos = frame*self.page_size
            self.frames_table.write_stream(frame_pos, page, self.pages_table.read(page))
            self.pages_table.set_frame(page, frame)
            self.fifo.append(frame)
        print(f"Process: {proc.pid}\nFrame accessed: {frame}")
        #self.print_table()

    @doc_inherit
    def swap_frames(self, frame1, frame2):
        def f(i):
            if (i == frame1):
                return frame2
            elif (i == frame2):
                return frame1
            return i
        self.fifo = list(map(f, self.fifo))


    @doc_inherit
    def delete_frame(self, frame):
        self.fifo.pop(self.fifo.index(frame))
        super().delete_frame(frame)

    @doc_inherit
    def print_table(self):
        debug_ptable(self.frames_table.table, self.page_size)



class LRU2(PaginationManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        super().__init__(total_memory, ua, page_size, ptable, ftable)
        self.MAX_VAL = (1 << self.tot_pages) - 1     # Generates a number with 'tot_pages' ones
        self.matrix = [0 for i in range(self.tot_pages)]
        self.id = 3

    @doc_inherit
    def update(self):
        pass

    @doc_inherit
    def get_new_frame(self):
        frame = 0
        bit_mask = self.matrix[0]
        for i in range(1, len(self.matrix)):
            if (self.matrix[i] < bit_mask):
                frame = i
                bit_mask = self.matrix[i]
                if bit_mask == 0:
                    break
        return frame

    @doc_inherit
    def access(self, proc):
        pos = proc.mem_access[0][0]
        page = (proc.base*self.ua + pos)//self.page_size
        frame = self.pages_table.get_frame(page)
        print(f"pid {proc.pid}, base {proc.base}, pos {pos}, page size {self.page_size}, page {page}")
        if (frame == -1):
            self.page_faults += 1
            frame = self.get_new_frame()
            out_page = self.frames_table.get_page(frame)
            if (out_page != -1):
                self.pages_table.set_presence(out_page, False)
            self.pages_table.set_presence(page, True)
            frame_pos = frame*self.page_size
            self.frames_table.write_stream(frame_pos, page, self.pages_table.read(page))
            self.pages_table.set_frame(page, frame)
        num = self.MAX_VAL
        self.matrix[frame] = num
        num ^= (1 << frame)                # Turns off the 'frame' bit
        for i in range(len(self.matrix)):
            self.matrix[i] &= num
        print(f"Process: {proc.pid}\nFrame accessed: {frame}")
        self.print_table()

    @doc_inherit
    def swap_frames(self, frame1, frame2):
        bm1 = self.matrix[frame1]
        self.matrix[frame1] = self.matrix[frame2]
        self.matrix[frame2] = bm1

    @doc_inherit
    def delete_frame(self, frame):
        self.matrix[frame] = 0
        super().delete_frame(frame)

    @doc_inherit
    def print_table(self):
        debug_ptable(self.frames_table.table, self.page_size, list(map(lambda a: format(a, f"#0{self.tot_pages}b"), self.matrix)))


class LRU4(PaginationManager):

    @doc_inherit
    def __init__(self, total_memory, ua, page_size, ptable, ftable):
        super().__init__(total_memory, ua, page_size, ptable, ftable)
        self.ADD_BIT = (1 << (self.tot_pages - 1))   # Generates a number with a one at 'tot_pages'
        self.timer = [0 for i in range(self.tot_pages)]
        self.bit_r = [False for i in range(self.tot_pages)]
        self.id = 4

    @doc_inherit
    def update(self):
        a = False
        for i in range(len(self.timer)):
            a |= self.bit_r[i]
        for i in range(len(self.timer)):
            if (a):
                self.timer[i] >>= 1
            self.timer[i] += self.ADD_BIT if (self.bit_r[i]) else 0
            self.bit_r[i] = False
        self.print_table()


    @doc_inherit
    def get_new_frame(self):
        frame = 0
        timer = self.timer[0]
        for i in range(1, len(self.timer)):
            if (self.timer[i] < timer):
                frame = i
                timer = self.timer[i]
        return frame

    @doc_inherit
    def access(self, proc):
        pos = proc.mem_access[0][0]
        page = (proc.base*self.ua + pos)//self.page_size
        frame = self.pages_table.get_frame(page)
        print(f"pid {proc.pid}, base {proc.base}, pos {pos}, page size {self.page_size}, page {page}")
        if (frame == -1):
            self.page_faults += 1
            frame = self.get_new_frame()
            out_page = self.frames_table.get_page(frame)
            if (out_page != -1):
                self.pages_table.set_presence(out_page, False)
            self.pages_table.set_presence(page, True)
            frame_pos = frame*self.page_size
            self.frames_table.write_stream(frame_pos, page, self.pages_table.read(page))
            self.pages_table.set_frame(page, frame)
        self.bit_r[frame] = True

    @doc_inherit
    def swap_frames(self, frame1, frame2):
        t1 = self.timer[frame1]
        self.timer[frame1] = self.timer[frame2]
        self.timer[frame2] = t1

    @doc_inherit
    def delete_frame(self, frame):
        self.timer[frame] = 0
        super().delete_frame(frame)

    @doc_inherit
    def print_table(self):
        debug_ptable(self.frames_table.table, self.page_size, list(map(lambda a: format(a, f"#0{self.tot_pages}b"), self.timer)))



def debug_ptable(ptable, page_size, aux_bits=None):
    if (not aux_bits):
        aux_bits = [None for i in range(len(ptable))]
    print(f"== FRAMES TABLE == -> {page_size}")
    for p, bm in zip(ptable, aux_bits):
        print("[", p, " aux:", bm, "]")
    print("")
