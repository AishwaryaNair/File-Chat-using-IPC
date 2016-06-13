# File-Chat-using-IPC

This is a file chat program using message queues which is a type of inter process communciation.

This project is a C application to share console messages with various processes. The program allows the user to type in a message and it should print out all messages received from other processes. This works no matter how many programs you start.

1. The program reads messages from standard input.
(a) The program will provide all messages entered by the user to other processes.
(b) The program will provide the sender process ID to the other processes.
(c) The program will terminate if the user types exit. This message is not sent. This is the only way the program terminates (besides help).

2. the program will print all information received from other processes. The program will not print messages typed into your process. It will print the sender process ID and the string message.

3. The  program executes two threads. It spawns an additional thread besides the main thread.

4. Your program supports unlimited connected processes.

5. The program cleans up all resources it uses. This means that after terminating the last program running, none of the queues are left around. Additionally, it correctly closes all files and clean up any threads or processes it creates.
