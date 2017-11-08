"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the MemoryWriter object file.
"""

class MemoryWriter(object):
    def __init__(self, filename, page_size):
        """Creates the instance and opens the file"""
        this.file = open(filename, "rb+")
        this.page_size = page_size

    def write(pid, pos, size):
        """Writes a pid 'size' times, beginning at 'pos'"""
        this.file.seek(pos)
        this.file.write(bytes((pid for i in range(size))))

    def clean(page):
        """Cleans a page"""
        this.file.seek(page*this.page_size)
        this.file.write(bytes((254 for i in range(this.page_size))))

    def close():
        """Closes the file"""
        this.file.close()
