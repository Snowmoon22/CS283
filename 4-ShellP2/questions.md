1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  calling execvp directly will overwrite the current memory footprint with elf file called from execvp. In other words, we would lose the shell program as it would get overwritten by the new program.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  fork() failure can be checked by the return value of fork(). If it is negative, that means something went wrong. And if it happens, a error message is sent to stderr and the exec_cmd function returns a ERR_EXEC_CMD value.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: When using execvp(), the command to execute is found by searching through the directories listed in the "PATH" environment variable. If the provided command path doesn't include a full path, it will check each directory in the PATH.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  By calling wait(), the parent process will not continue until the child process finishes. . If wait() wasn't called, the parent function will continue to run and even possibly terminating, and thereby making the child process into a zombie process. Furthermore, the parent will be able to obtain the child's exit status because it will only try to get the exit status once the child exists.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: WEXITSTATUS() gets the exit code/return value of the child process. The exit code indicates whether the child process executed successfully or encountered an error. This is important because it allows the shell to make decisions based on whether the executed command succeeded, failed, or encountered an error.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() has 2 pointers, one for the start and the end. It also has a flag that gets triggered when reaching a ". Without the flag, end will keep iterating till it reachs a whitespace. With the flag it will keep iterating till it eachs another ". This handling is necessary because it allows a way for the shell to take in arguments that contain spaces which are important to keep for the command it is executing.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  My previous parsing logic breaks the command parsed by the exe and arguments. This can be easily found by finding the first whitespace after trimming. However, the new parsing logic needs to include ways to deal with quotes, and tokenizing arguments depending on if its in quotes or not.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are  asynchronous, and lightweight mechanisms for event notification, aka notify when a specific event has occurred. Signals are simple notifications without data payloads, as such, they are not suitable for complex data exchange or communication where other IPC mechanisms are better.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: 
    
    - SIGKILL: Used for forceful termination when a process is unresponsive.

    - SIGTERM: Used for graceful termination, allowing the process to clean up.

    - SIGINT: Used for user-initiated interruption, typically via Ctrl+C.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:   When a process receives SIGSTOP, it is immediately paused by the OS. It cannot be caught, blocked, or ignored. This is because  SIGSTOP is intended for critical process control. This ensures that processes can always be paused by the system or administrator, even if they are misbehaving or have custom signal handlers.
