# Operating Systems Architecture - Haiku project
This is the project for Operating Systems Architecture course of: 
- Abbas Aliyev
- Aziz Salimli
- Habil ...
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
#### Execution modes
For version 1 we have two execution modes:
1. As one executable
2. As two executables

When running as one executable, we are spawning daughter process using `fork()`, which
will be our server. Parent process gets child's process id, and will send signals to it.
This way we start as one executable, run 