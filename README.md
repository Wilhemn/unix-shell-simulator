Project 1: Simple Shell
==================

INSTRUCTIONS
============

In this project, you will explore and extend a simple Unix shell interpreter.
In doing so, you will learn the basics of system calls for creating and
managing processes.


STEP 1:  Compile the shell
==========================

    chmod +x b.sh
    make
    make test   # Use in Step 5 to test your changes to the project
    ./shell


STEP 2:  Try using the shell
============================

  Note: You need to specify the absolute paths of commands.

  Some commands to try:
  
    /bin/ls
    /bin/ls ..
    cd /
    /bin/pwd
    /bin/bash
    exit
    ./shell     (Note: You need to be in the project directory.)
    ./shell&    (Note: You need to be in the project directory.)
    ./b.sh      (Note: You need to be in the project directory.)
    /bin/kill -s KILL nnnn      (Where nnnn is a process ID.)

  "./" means the current directory


STEP 3:  Study the implementation of the shell
==============================================

  In preparation for the questions in Step 4, please explore the source code
  for the shell contained in 'shell.c'.  You needn't understand every detail
  of the implementation, but try to familiarize yourself with the structure
  of the code, what it's doing, and the various library functions involved.
  Please use the 'man' command to browse the Unix manual pages describing
  functions with which you are unfamiliar or use Google.


STEP 4:  Questions
==================

  1. Why is it necessary to implement a change directory 'cd' command in
     the shell?  Could it be implemented by an external program instead?
     (could you call 'exec' to run 'cd'?)

    Answer: It is necessary to implement a 'cd' in the shell because a user would not be able to change directories, which would
    greatly limit their options in both usefulness and navigation. The shell could not be implemented by an external program 
    instead because any changes made would only be limited to the child shell, not the parent.
    
  3. Explain in detail how our sample shell implements the change directory command.

    Answer: The sample shell implements the change directory command with the statement: else if (!strcmp(exec_argv[0], "cd") && exec_argc > 1). If there is a 'cd' followed by at least one path, then the program checks if the path inputted is valid, if it is then the chdir() function switches directories. 
     
  5. What would happen to the shell if this program did not use the fork function, but
     just used execv directly? Explain why that happens. (Try it!)

     Try temporarily changing the code 'pid_from_fork = fork();'
     to 'pid_from_fork = 0;'

    Answer: When pid_from_fork = fork() was changed to pid_from_fork = 0 and when shell.c was inputted, nothing happend. No child process was forked unlike when the code is the way it should be. So it would appear that removing the fork option just gets rid of any processes being made and just returns what you put in as a failure.
     
  7. Explain what the return value of fork() means and how this program
     uses it.

    Answer: The return value of fork() can be split into three posibilities. It will be a negative number, zero, or a positive number. In this program, if the result is a negative number, then the creation of the child process was unsuccessful. If the result is zero, then the creation of the child process was successful and it returns to it. If the result is a positive number, then the program returns to the parent/caller, and the ID of the new child process is also included. 
     
  9. What would happen if fork() were called prior to chdir(), and chdir()
     invoked within the forked child process?  (Try it!)
     Try temporarily changing the code for 'cd' to use fork:
     
```c
 if (fork() == 0) {
     if (chdir(exec_argv[1]))
         /* Error: change directory failed */
         fprintf(stderr, "cd: failed to chdir %s\n", exec_argv[1]);
     exit(EXIT_SUCCESS);
 }
```
    Answer:  No noticable changes were observed, but this change allows the child program to change the directory independently of the parent. It may seem nice for the parent and child to run concurrently, but if there is an error of some sort then it may not been seen immediately which would be an issue.
     
  11. Can you run multiple versions of ./b.sh in the background?
     What happens to their output? (You can run processes in the background
     by adding & at the end of the command, i.e. ./b.sh&)

     Answer: Yes you can run multiple versions of ./b.sh in the background, but there doesn't seem to be any noticable changes to the output. This is most likely because they run independently of eachother so changing one won't change the other.

  13. Can you execute a second instance of our shell from within our shell
     program (use './shell')?  Which shell receives your input?

     Answer: Yes you can execute a second instance of the shell by using ./shell. However, only the newly created shell will recieive the input and therefore any changes to the parent or the child.

  15. What happens if you type CTRL-C while the countdown script ./b.sh is
     running?  What if ./b.sh is running in the background?

     Answer: When CTRL-C was used when ./b.sh was running, the program just ended very abruptly since it didn't even have a termination message. When CTRL-C was used when ./b.sh was running in the background, it didn't end immediatly, as it took more CTRL-C presses to terminate it completely.

  17. Can a shell kill itself?  Can a shell within a shell kill the parent
     shell? (Run ps in the shell to see the process table)

```
 ./shell
 ./shell
 /bin/kill -s KILL NNN      (Where NNN is the the parent's PID.)
```
     Answer: Yes, by using /bin/kill -s KILL NNN in the shell, a shell will be able to kill itself. It would appear that a child shell is able to kill itself but is unable to kill it's parent with the /bin/kill -s KILL NNN.

  11. What happens to background processes when you exit from the shell?
      Do they continue to run?  Can you see them with the 'ps' command?
```
./shell
./b.sh&
exit
ps
```
     Answer: When you exit when a background process is running, the shell is exited out of but the process continues to run. When ps is used afterwards, they appear on the list that displays.

STEP 5:  Modify the Project
======================

  Please make the following modifications to the given file shell.c.  As in
  Project 0, we have included some built-in test cases, which are described along
  with the feature requests below.

  In addition to running the tests as listed individually, you can run
  "make test" to attempt all tests on your modified code.


  1. Modify this project so that you can use 'ls' instead of '/bin/ls'
     (i.e. the shell searches the path for the command to execute.) 
     Consider the following 3 cases:
     
     * The command includes an absolute path, such as /bin/ls
     * The command starts with a dot, such as ./shell
     * The command is somewhere in the ENVPATH, such as ls
     
     HINT: Use C getenv() function. http://www0.cs.ucl.ac.uk/staff/ucacbbl/getenv/
     Note: The use of any other execv function is forbidden, you must create a 
     function that will find any command in the system PATH if it exists.
```
Test: ./shell -test path
```
  2. Modify this project so that the command prompt includes a counter that
     increments for each command executed (starting with 1).  Your
     program should use the following prompt format:
       "Shell(pid=%1)%2> "  %1=process pid %2=counter
     (You will need to change this into a correct printf format)
     Do not increment the counter if no command is supplied to execute.
```
Test: ./shell -test counter
```
  3. Modify this project so that '!NN' re-executes the n'th command entered.
     You can assume that NN will only be tested with values 1 through 9,
     no more than 9 values will be entered. If it goes over 9, loop back to 1 (use a circular queue structure)
```
Shell(...)1> ls
Shell(...)2> !1     # re-executes ls
Shell(...)3> !2     # re-executes ls
Shell(...)4> !4     # prints "Not valid" to stderr
Shell(...)4> !5     # prints "Not valid" to stderr
```
```
Test: ./shell -test rerun
```
  4. Modify the project so that it uses waitpid instead of wait. 
  The parent process must wait for its direct child to finish executing before showing the next shell prompt.

  5. Create a new builtin command 'newshell' that forks the program to create
     a new subshell.  The parent shell should run the imtheparent()
     function just as if we were running an external command (like 'ls').
```
 ./shell
 Shell(.n1..)1> newshell
 Shell(.n2..)1> exit  # Exits sub shell
 Shell(.n1..)1> exit  # Exits back to 'real' shell
```
  6. Create a new global variable to prevent a subshell from invoking
     a subshell invoking a subshell (i.e., more than 3 levels deep):
```
./shell
Shell(.n1..)1> newshell
Shell(.n2..)1> newshell
Shell(.n3..)1> newshell   # prints "Too deep!" to stderr
```
```
Test: ./shell -test newshell
```
The project will be graded on correctness (60%), organization (10%), robustness (10%), formatting (10%), and documentation(10%). Don't forget to release memory stored in pointers, output to the correct streams, and close all files/streams before the last shell terminates. All work submitted must be yours and you must be ready to defend your program if asked to do so in class or in office. Do not copy solutions from the web, I've seen them all.
