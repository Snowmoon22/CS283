1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  It is a good choice because the shell takes command line by line. By using fgets, the reading automatically stop when a newline or EOF is inputted

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  It allows us to be more flexible in how cmd_buff can be used. For example, Fixed-size array does not allow you to manipulate string by index, in other words, we wouldn't be able to remove trailing \n and would need to account for it when we use cmd_buff and the tokens we parse from it.


3. In `dshlib.c`, the function `build_cmd_list()` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Removing leading and trailing spaces is necessary in order to accurately determine which command is being asked to execute. For example, my implemenation of build_cmd_list() separates the tokenized cmd_line by exe and arguments by finding the first whitespace. However, if leading whitespace is not removed, this would mess with the implementation.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: 
     - `ls > output.txt` The shell must be able to handle if the file already exists and needs to be truncated, and ensure proper file descriptors.
     - `sort < input.txt` One challenge would be having the shell read files. For example, the shell must be able to handle checking if the file exist or if the file has permission to be read.
     - `echo "hi" >> file.txt` One challenge would be ensuring that the file is opened in append mode to avoid overwriting existing data.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection is primarily fore dealing with interactions between commands and files. Piping on the other hand is primarily for dealing with how commands interact with other commands, in other words, sending the output of one command as the input of another command.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It's important for allowing easy indentification and handling of error messages without potential of it being mixed and confused with the program's expected output. It's also important for pipeline so that only output from STDOUT is redirected, or if you are to log errors, only STDERR is redirected.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  We can use fork-exec to detect command failures. For instance, wait() can be used in parent procress to see if the child procress were to fail. If we are to provide a way to merge STDOUT and STDERR, the 2>&1 syntax can be used in our shell.