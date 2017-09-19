# OSdevelopment
Development related to operational systems algorithms and techniques.

## 1. **schedulersCPU**
* **ep1sh.c**:
This is an implementation of a ***shell***, using the *fork()* *syscall* and a loop structure. It has the commands *date* and *chown :<group> <file>* built-in (using exactly the structure provided before). It also executes any command using the *execvp() syscall*.
  
 * **ep1.c**:
Implementation of three ***process schedulers algorithms*** (*Shortest Job First, Round Robin, Priority Scheduler*). They are all multithreaded (although you can see the monothreaded implementation too), and will simulated a list of processes provided in the trace file via command line arguments. If a 'd' flag is provided, it will print important events as they occur. The TestTraces folder contains scripts to automatically generate the trace files, and a script we used to generate the statistics for the .results files (this statistics version for the schedulers is only included in the v1.0 version of this repo!). 

    You can view the results we obtained with each scheduler is two different machines (one with 4 cores, and another with 32 cores) in the finalPresentation.pdf (in portuguese). >:)
