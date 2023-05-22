In this assignment we created a Reactor (Reactor Design Pattern) that handles multiple client file descriptors (TCP socket) and handles them accordingly, using an handler function for each file descriptor.

The Reactor library supports the following functions:

   - void *createReactor() – Create a reactor object - a linked list of file descriptors and their handlers.
   - void startReactor(void *react) – Start executing the reactor, in a new thread.
   - void stopReactor(void *react) – Stop the reactor - stop the reactor thread and free all the memory it allocated.
   - void addFd(void *react, int fd, handler_t handler) – Add a file descriptor to the reactor.
   - void WaitFor(void *react) – Joins the reactor thread to the calling thread and wait for the reactor to finish.
- int server_handler(int fd, void *react) - it does the accept operation and then takes the newly created socket and adds it to the linked list.

Requirements:

   - Linux machine (Ubuntu 22.04 LTS preferable)
   - GNU C Compiler
   - Make
   
   
   Running:
   ./react_server

