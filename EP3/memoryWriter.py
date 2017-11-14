"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the MemoryWriter object file.
"""

MINUS_1 = 255
class MemoryWriter(object):
    def __init__(self, filename, page_size, ua_size, fsize):
        """Creates the instance and opens the file"""
        self.file = open(filename, "wb+")
        self.page_size = page_size
        self.write(MINUS_1, 0, fsize)


    def write(self, pid, pos, size):
        """Writes a pid 'size' times, beginning at 'pos'"""
        self.file.seek(pos)
        self.file.write(bytes((pid for i in range(size))))
        self.file.flush()

    def clean(self, page):
        """Cleans a page"""
        self.file.seek(page*self.page_size)
        self.file.write(bytes((MINUS_1 for i in range(self.page_size))))

    def close(self):
        """Closes the file"""
        self.file.close()
