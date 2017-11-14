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
    # TODO: At the end, set back to original file
    #VMEMORY_PATH = "/tmp/ep3.vir"
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
        """Constructor method"""
        self.table = [self.Page() for _ in range(ceil(total_memory/page_size))]
        self.page_size = page_size
        self.file = MemoryWriter(self.VMEMORY_PATH, page_size, total_memory)

    def read(self, page):
        """Reads a page from the file and returns it"""
        return self.file.read(page)

    def palloc(self, pid, pos, size):
        """Reserves the memory for a process"""
        self.file.write(pid, pos, size)
        base_page = pos//self.page_size
        pages = ceil(size/self.page_size)
        for i in range(base_page, base_page + pages):
            self.table[i].pid = pid

    def set_frame(self, page, frame):
        """A setter method to bind a frame to a page"""
        self.table[page].page_frame = frame

    def set_presence(self, page, val):
        """A setter method to set if a page is at the physical memory or not"""
        self.table[page].bit_p = val

    def get_frame(self, page):
        """A getter method that returns the frame where a page is"""
        if (not self.table[page].bit_p):
            return -1
        return self.table[page].page_frame

    def reset_page(self, page):
        """Resets a page"""
        self.table[page].pid = -1
        self.table[page].bit_p = False
        self.page_frame = -1
        self.file.clean(page)

    def close(self):
        """Closes the file"""
        self.file.close()

class FrameTable(object):
    # TODO: At the end, set back to original file
    #PMEMORY_PATH = "/tmp/ep3.mem"
    PMEMORY_PATH = "ep3.mem"
    class PageFrame(object):
        page_size = 0
        def __init__(self):
            self.page = -1
            self.label = ''
            self.lruC = 0

        def __str__(self):
            return f"[Page: {str(self.page).zfill(3)}]"

    def __init__(self, total_memory, page_size):
        self.table = [self.PageFrame() for _ in range(ceil(total_memory/page_size))]
        self.page_size = page_size
        self.file = MemoryWriter(self.PMEMORY_PATH, page_size, total_memory)

    def get_page(self, frame):
        """A getter method that returns the page which is at the frame"""
        return self.table[frame].page

    def write_stream(self, pos, page, stream):
        self.file.write_stream(pos, stream)
        frame = pos//self.page_size
        self.table[frame].page = page

    def reset_frame(self, frame):
        self.table[frame].page = -1
        self.file.clean(frame)
