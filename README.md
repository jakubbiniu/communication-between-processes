# Communication between processes using named pipes in ANSI C
### Description
The program uses named pipes to enable a few processes of communication between them. In a file config.txt, every process/user has its fifo name. When you start the program, it finds a name of fifo in config.txt and crates a fifo. Then it creates two processes. The first process is waiting for commands, then reading them and sending them to a given process to let him execute the command. It also creates its temporary fifo to get back the output of the command. Next, it reads from the temporary fifo and displays the output on the terminal. The second process is waiting for another process to send some commands to it. If the process gets a command, executes it and sends back using a temporary pipe.

### How to run
To compile the file in linux terminal use command:
```
gcc projekt.c -o projekt
```
Then open at least two terminals and write that command:
```
./projekt usr
```
where usr is a username from config.txt (usr1, usr2 etc.)
Then you can type commands in a program in that way:
```
usr command fifo
```
usr is username of a process you want to communicate with, command is of course just a bash command and fifo is the name of fifo on which you want to receive the output of the command. If you want to delete the named pipe connected to a user just type "delete"

### Example of using the program
On terminal 1:
```
./projekt usr1
```
On terminal 2:
```
./projekt usr2
```
On terminal 1:
```
usr2 ls | tr a-z A-Z aaa
```
Usr1 wants usr2 to execute pipe "ls | tr a-z A-Z" and send the output back using fifo "aaa". Usr2 sends the output and usr1 can display it on its terminal




