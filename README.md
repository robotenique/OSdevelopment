# OSdevelopment
Development related to operational systems algorithms and techniques.

## 1. **CPU-schedulers**
This app consists of two parts: 

An implementation of a **shell** (*ep1sh.c*), using the *fork()* *syscall* and a loop structure. It has the commands *date* and *chown :<group> <file>* built-in (using exactly the structure provided before). It also executes any command using the *execvp() syscall*.
  

And also, a multithreaded implementation of three **process scheduling algorithms for CPU's**, namely the *Shortest Job First*, *Round Robin* and *Priority Scheduler*. The program will simulate a list of processes providaded in a trace file via command line arguments. It also supports a debug flag 'd', to print the events as they occur.
<p align="center">
<img src="http://tutorials.jenkov.com/images/java-concurrency/java-concurrency-tutorial-introduction-1.png"/>
</p>

## 2. **multithreadedSync**
This is an implementation of a velodrome bike racing simulator, fully concurrent (each biker is a thread), and all the threads are synchronized using barriers, in our case, using the *pthread_barrier* from the *pthread* library. The race is simulated with strict rules for the scores of each biker, for details of the rules, please refer to the *assigment.pdf* file (or e-mail me :p). Each biker has some possible moves, and each one of them behave like an automata, and we synchronize the movements of the bikers using *mutexes*, namely *pthread_mutexes*. 

The simulator constructs a *dependency graph* each step, and calculates the *Strongly Connected Components* to detect cycles in the graph using *Tarjan's Algorithm*, to prevent *deadlocks* in the simulation. 
<p align="center">
<img src="https://raw.githubusercontent.com/robotenique/OSdevelopment/master/multithreadSync/overleaf/dependency.png"/>
</p>

## 3. **memory-simulator**
Simulator of virtual and physical memory allocation, using a trace file for the processes list (each process has a default size and make some memory access in a given time). The *memory-simulator* implements the following algorithms:

**Virtual memory allocation**:
1. Best Fit
2. Worst Fit
3. Quick Fit

**Pages substitution**(for page faults):
1.  Optimal
2.  FIFO
3.  LRU (bit matrix implementation)
4.  LRU (bit counter implementation)


<p align="center">
<img src="https://i.stack.imgur.com/uwgeO.png"/>
</p>
