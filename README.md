# Communication between processes using named pipes in ANSI C
### Description
Program uses 

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
usr is username of a process you want to communicate with, command is of course just a bash command and fifo is the name of fifo
