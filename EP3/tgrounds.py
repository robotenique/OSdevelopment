#Testing grounds file, TODO: remove this thing after all
def coisa(func):
    print('ALO: ', end="")
    return func

class Teste(object):
    def __init__(self):
        pass

    #@coisa
    def testador(self, a):
        print(a)

"""g = Teste()
g.testador("HMMM")"""

from collections import deque
h = deque([('A', 0), ('B', 1), ('C', 2), ('D', 3), ('E', 4)])

print(h)
