This project is an implementation of : https://brennan.io/2015/01/16/write-a-shell-in-c/


Future Improvements:
- Support for Piping
- implement more builtins

Notes: 

How Shells Start Processes
- 2 ways (really only one)
    - via the kernel, but this only happens for one process, Init
    - via the fork() system call
    - fork is then followed by exec() system call

- fork
    - when fork is called the current process duplicates itself, returning 0 to the "child" and the pid of the child the the "parent"
    - thus, the only way to start a process is to duplicate a currently running one

- exec
    - obviously, we dont want a complete copy of the parent process' program. We want a new program running on our child process
    - exec does this: it stops the current process, loads up the new program, and starts a new process in the old ones place
    - a process never returns from an exec call unless there is an error