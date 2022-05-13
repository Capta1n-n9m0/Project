# Operating Systems Architecture - Haiku project
This is the project for Operating Systems Architecture course of: 
- Abbas Aliyev
- Aziz Salimli
- Habil Abdulkhaligov
- Narmin Pashayeva
- Nijat Hamidov
 
## Introduction 
The goal of this project is to demonstrate our skills in using operating systems' tools 
and interfaces by building an application that utilises them. Application consists of two 
separate parts: client and a server. Server will perform an action: display a haiku the
user. Server is activated by request from a client: signal. We have 3 versions that show different OS tools.

### Version 1
Version 1 of our haiku application will work with signals. There should be two processes:
client and a server. Client sends one of two signals to a server: SIGINT of SIGKILL, which
correspond to the japanese or the western haiku. Server, depending on a signal, will print
corresponding haiku. To communicate, client needs to know process id of a server, for this
problem, our team has proposed two solutions, which resulted in two execution modes.
1. As single executable
2. As two executables

##### Single executable
When running as one executable, we are spawning daughter process using `fork()`, which
will be our server. Parent process, which is client, gets child's process id, and will
send signals to it. This way we start as one executable, but run 2 processes. This is
one way to pass server's process id to client.

##### Two executables
If we are running server and a client separately, we should have a way to run them first.
For this to happen, there should exist main function in server.c and client.c. But if we 
just create a `int main(){...}` in server.c and/or client.c, it would conflict with main 
in main.c and won't let us run project as single executable. To resolve this issue, we 
came up with creative solution: using macros and defining them as argument to gcc. In both
server.c and client.c we actually have main function, but it stays hidden until needed. In
server main hides under alias s_main and in client as c_main. That's how neither client, nor
server interfere with main.c's main function. But, we still have a way to run client and
server separately, we just need to replace s_main and c_main with main and compile sources
separately. This lines of code make this possible:
```c
// server.c
#ifdef STANDALONE
#define s_main main
#endif
```
```c
// client.c
#ifdef STANDALONE
#define c_main main
#endif
```
```makefile
# makefile
server: server.h server.c queue.h queue.c haiku.h haiku.c
	$(CC) $(CFLAGS) -DSTANDALONE server.c queue.c haiku.c -o server $(LDFLAGS)

client: client.c client.c queue.h queue.c haiku.h haiku.c
	$(CC) $(CFLAGS) -DSTANDALONE client.c queue.c haiku.c -o client $(LDFLAGS)
```
We compile server and client separately and define macro `STANDALONE` with flag -D to gcc, 
which enables replacement mechanism of main functions.