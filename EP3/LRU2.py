"""
@author: João Gabriel Basi Nº USP: 9793801
@author: Juliano Garcia de Oliveira Nº USP: 9277086

MAC0422
20/11/17

This is the Least Recently Used version 2 object file.
"""

class LRU2(object):

    def __init__(self, size):
        self.MAX_VAL = (1 << size) - 1     # Generates a number with 'size' ones
        self.matrix = [0 for i in range(size)]
        # self.M = [False for i in range(size)]

    def get_new_page():
        """Apply the algorithm to find the best page to be given to a process"""
        page = 0
        bit_mask = self.matrix[0]
        for i in range(1, len(self.matrix)):
            if (self.matrix[i] < bit_mask):
                page = i
                bit_mask = self.matrix[i]
        # if (self.M[page]):
            # Copy to physical memory?
            # self.M[page] = False
        self.matrix[page] = self.MAX_VAL
        return page

    # TODO: Decide if the pid will be written at all the page or just at the
    # accessed position
    def use_page(page):
        """Updates the matrix as if an access to 'page' just happened"""
        num = self.MAX_VAL
        self.matrix[page] = num
        num ^= (1 << page)                # Turns off the 'page' bit
        for i in range(page):
            self.matrix[i] &= num
        # self.M[page] = True
        # TODO: Write at virtual memory

    def delete_page(page):
        """Delete all the information at a page.
           Used to delete a page from a finished process"""
        self.matrix[page] = 0
        self.M[page] = False
