"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This file contains the Page and Frame tables classes
"""

from math import ceil
from memoryWriter import MemoryWriter


MINUS_1 = 255

class PageTable(object):
    VMEMORY_PATH = "ep3.vir"
    class Page(object):
        page_size = 0
        def __init__(self):
            self.pid = -1
            self.bit_p = False
            self.page_frame = 0
            self.label = ''
            self.lruC = 0

        def __str__(self):
            return f"[{str(self.pid).zfill(3)}  frame: {self.page_frame}  p: {self.bit_p}]"

    def __init__(self, total_memory, page_size):
        self.table = [self.Page() for _ in range(ceil(total_memory/page_size))]
        self.page_size = page_size
        self.file = MemoryWriter(self.VMEMORY_PATH, page_size, total_memory)

    def read(self, page):
        return self.file.read(page)

    def palloc(self, pid, pos, size):
        self.file.write(pid, pos, size)
        base_page = pos//self.page_size
        pages = ceil(size/self.page_size)
        for i in range(base_page, base_page + pages):
            self.table[i].pid = pid

    def set_frame(self, page, frame):
        self.table[page].page_frame = frame

    def set_presence(self, page, val):
        self.table[page].bit_p = val

    def get_frame(self, page):
        if (not self.table[page].bit_p):
            return -1
        return self.table[page].page_frame

    def reset_page(self, page):
        self.table[page].pid = -1
        self.table[page].bit_p = False
        self.page_frame = -1
        self.file.clean(page)

    def close(self):
        """Closes the file"""
        self.file.close()

class FrameTable(object):
    class PageFrame(object):
        page_size = 0
        def __init__(self):
            self.page = -1
            self.bit_m = False
            self.bit_r = False
            self.label = ''
            self.lruC = 0

        def __str__(self):
            return f"[{str(self.page).zfill(3)}  m: {self.bit_m}  r: {self.bit_r}]"

    def __init__(self, total_memory, page_size):
        self.table = [self.PageFrame() for _ in range(ceil(total_memory/page_size))]

    def get_page(self, frame):
        return self.table[frame].page

    def write_stream(self, pos, stream):
        self.seek(pos)
        self.file.write(stream)

    def reset_frame(self, frame):
        self.table[frame].page = -1
        self.table[frame].bit_m = False
        self.table[frame].bit_r = False
