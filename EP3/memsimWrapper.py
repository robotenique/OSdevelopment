from functools import wraps
from collections import deque
from math import ceil

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

class LinkedList(object):
    class Node(object):
        def __init__(self, status, base, qtd, next_n=None):
            self.status = status
            self.base = base
            self.qtd = qtd
            self.next = next_n

        def __eq__(self, other):
            return other != None and \
                   self.status == other.status \
                   and self.base == other.base \
                   and self.qtd == other.qtd \
                   and self.next == other.next
        def __repr__(self):
            return f"({self.status}, {self.base}, {self.qtd})"

    def __init__(self, head=None):
        self.head = head

    def add_node(self, status, base, qtd):
        new_node = self.Node(status, base, qtd, self.head)
        self.head = new_node

    def print_nodes(self):
        curr = self.head
        while curr:
            print(f"[{curr.status}, {curr.base}, {curr.qtd}]", end="")
            if curr.next != None:
                print(" -> ", end="")
            else:
                print("")
            curr = curr.next


class Process(object):
    next_pid = 0
    pidQueue = deque((i for i in range(128)))
    def __init__(self, vals, ua_size):
        self.name = vals.pop(3)
        self.mem_access = deque()
        #print(vals)
        vals = list(map(int, vals))
        self.t0 = vals[0]
        self.tf = vals[1]
        self.original_sz = vals[2] # Bytes
        self.b  = ceil(vals[2]/ua_size)*ua_size # Bytes
        if len(Process.pidQueue) == 0:
            print("Can't run more than 127 processes at the same time!")
            exit()
        self.pid = Process.pidQueue.pop()
        self.base = 0 # UA
        self.size = 0 # UA
        Process.next_pid += 1
        for i in range(3, len(vals) - 1, 2):
            self.mem_access.append((vals[i], vals[i + 1]))

    def __repr__(self):
        return f"<pid: {self.pid}>"

    def __str__(self):
        return f"{self.name} ([t0, tf] = ({self.t0}, {self.tf}), [size] : {self.b}, mem_acess : {self.mem_access}"

    def reset_pids():
        Process.pidQueue = deque((i for i in range(128)))
