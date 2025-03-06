1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_my shell ensures all child processes complete by having the parent process call waitpid() for each child process after setting up the pipeline through a for loop. if this is not done, it would create zombie processes and possibly create rare conditions where the shell might accept new user input before previous commands have finished running, leading to unexpected behavior._

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_We have to close unused ends because file descriptors are limited resource, and if they remain open, it is wasted resource. Also, if the pipes are left open it could lead to scenarios such as dead lock, where processes are kept waiting indefinitely for input or output that will never arrive._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_This is because if cd was executed in a child process, it would only affect the working directory of the child process. This would require additional state management system that would sync the working directory bewteen child and parent procresses._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_I can Use dynamic memory allocation (e.g., malloc() and realloc()) to allocate memory for the command list and pipe file descriptors as needed and replace the fixed-size array for commands with a dynamically resizable array or linked list. Possible trade-offs that needs to be considered are memory overhead needed for dynamic memory allocation, and performance as fixed-size array are typically faster._
