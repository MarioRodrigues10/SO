## Operating Systems | Sistemas Operativos
## Grade: */20 :star:

This project was developed during the second semester of the 2nd year.

The purpose was to develop a client program (tracer) to provide a user interface via the command line. A server program (monitor) should also be developed, which the client will interact with. The server should maintain relevant information in memory and in files to support the functionalities described in this prompt.

The standard output should be used by the client to present the necessary responses to the user, and by the server to display any necessary debug information.

Both the client and the server should be written in C and communicate via named pipes. In the implementation of this project, C library functions for file operations should not be used, except for printing to the standard output. Similarly, direct or indirect execution of commands through the command interpreter (e.g., bash or system()) is not allowed.

## Installing and running the project


#### Cloning the repository
```bash
$ git clone git@github.com:MarioRodrigues10/SO.git
```

#### Compiling
```bash
$ cd SO
$ make
```

#### Running
```bash
$ ./bin/monitor PIDS
$ ./bin/tracer execute -u "command"
$ ./bin/tracer execute -p "command1 | command2 | command3"
$ ./bin/tracer status   
$ ./bin/tracer stats-time PID 
$ ./bin/tracer stats-command PID
```

# Developed by:

- [Mário Rodrigues](https://github.com/RuiL1904)
- [Pedro](https://github.com/jkaxsh)
- [João Faria](https://github.com/joaofarys200)
