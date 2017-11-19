class Pai():


    def __init__(self):
        pass

    def coisinha(self):
        tope = 3
        gege = 4

class Filho(Pai):
    def __init__(self):
        pass
    def coisinha(self):
        super().coisinha()
        print(tope)


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
