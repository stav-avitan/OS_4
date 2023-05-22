#ifndef _REACTOR_H
#define _REACTOR_H

#if !defined(_XOPEN_SOURCE) && !defined(_POSIX_C_SOURCE)
	#if __STDC_VERSION__ >= 199901L
		#define _XOPEN_SOURCE 600
	#else
		#define _XOPEN_SOURCE 500
	#endif
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <poll.h>

#define SERVER_PORT 9034

#define MAX_QUEUE 16384

#define MAX_BUFFER 1024

#define POLL_TIMEOUT -1

typedef int (*handler_t)(int, void *);

struct reactor_node
{
	int fd;
	handler_t handler;
	struct reactor_node *next;
};

struct reactor_object
{
	pthread_t thread;
	struct reactor_node *head;
	bool running;
};

void *createReactor();
void startReactor(void *react);
void stopReactor(void *react);
void addFd(void *react, int fd, handler_t handler);
void WaitFor(void *react);
int client_handler(int fd, void *react);
int server_handler(int fd, void *react);

#endif