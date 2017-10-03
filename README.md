# Parallel-Palindromes
----------------------

### To run the project run the below command on you terminal :
```
$ ./master
```

### Executables :
```
master and palin
```

### Project Structure:

```
parallel-palindromes/
	|input.txt - Strings one each line
	|README.md
	|src/
		|master.c
		|palin.c
		|shm_header.h
	|Makefile
```

### Compiling:

```
$ make
```
### Running:
```
./master -c<child_process> -w<max_writes> -t<timeout>
```

### Help:

```
./master -h
-h: Prints this help message.
-c: Allows you to set the number of child process to run.
	The default value is 19. The max is 19.
-w: Allows you to set the number of times each child enters the critical section of code(writes to file).
	The default value is 5.
-t: Allows you set the wait time for the master process until it kills the slaves.
	The default value is 60.
```

### Clean the project:

```
$ make clean
```

### Clean the project only *.o:
```
$ make cleanobj
```

### Log:
```
debug.log - file shows the processID, index,c_s entered and time c_s exit.
Sample message:
Process (pid, processNumber) entered critical section at 15:10:42(1507061442).
```

***Parallel Palindromes*** : Project #2 as a part of CS4760. 