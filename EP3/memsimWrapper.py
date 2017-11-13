from functools import wraps

class DocInherit(object):
    """
    Docstring inheriting method descriptor

    The class itself is also used as a decorator
    """

    def __init__(self, mthd):
        self.mthd = mthd
        self.name = mthd.__name__

    def __get__(self, obj, cls):
        if obj:
            return self.get_with_inst(obj, cls)
        else:
            return self.get_no_inst(cls)

    def get_with_inst(self, obj, cls):

        overridden = getattr(super(cls, obj), self.name, None)

        @wraps(self.mthd, assigned=('__name__','__module__'))
        def f(*args, **kwargs):
            return self.mthd(obj, *args, **kwargs)

        return self.use_parent_doc(f, overridden)

    def get_no_inst(self, cls):

        for parent in cls.__mro__[1:]:
            overridden = getattr(parent, self.name, None)
            if overridden: break

        @wraps(self.mthd, assigned=('__name__','__module__'))
        def f(*args, **kwargs):
            return self.mthd(*args, **kwargs)

        return self.use_parent_doc(f, overridden)

    def use_parent_doc(self, func, source):
        if source is None:
            raise NameError("Can't find '%s' in parents"%self.name)
        func.__doc__ = source.__doc__
        return func

doc_inherit = DocInherit

class Node:
   def __init__(self, data, nextNode=None):
       """Creates a new node with value 'data' and nextNode is the next node"""
       self.data = data
       self.nextNode = nextNode

   def next_node(self):
       """Gets the next node"""
       return self.nextNode

   def set_next_node(self, val):
       """Sets the next node of the current node"""
       self.nextNode = val

class LinkedList:
   def __init__(self, head = None):
       """ Creates a new LinkedList with head=head """
       self.head = head
       self.size = 0

   def add_node(self, data):
       """
       Add a new node in the list
       """
       new_node = Node(data, self.head)
       self.head = new_node
       self.size += 1
       return True

   def print_nodes(self):
       curr = self.head
       while curr:
           print(curr.data, end="")
           if curr.nextNode != None:
               print(" -> ", end="")
           else:
               print("")
           curr = curr.next_node()

"""myList = LinkedList()
print("Inserting")
print(myList.add_node(5))
print(myList.add_node(15))
print(myList.add_node(25))
print("Printing")
myList.print_node()
print("Size")
print(myList.size)
"""
