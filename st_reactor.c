#include "reactor.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void *reactorRun(void *react) {
	if (react == NULL) {
		printf("reactor is NULL\n");
		return NULL;
	}

	struct reactor_object *reactor = (struct reactor_object *)react;

	while (reactor->running)
	{
		struct reactor_node *temp = reactor->head;
		int max_fd = 0;

		while (temp != NULL) {
			max_fd++;
			temp = temp->next;
		}

		struct pollfd fds[max_fd];

		temp = reactor->head;

		for (int i = 0; i < max_fd; i++) {
			fds[i].fd = temp->fd;
			fds[i].events = POLLIN;
			fds[i].revents = 0;
			temp = temp->next;
		}

		if (poll(fds, max_fd, -1) < 0) {
			printf("poll failed\n");
			return NULL;
		}

		for (int i = 0; i < max_fd; i++) {
			if (fds[i].revents & POLLIN) {
				int ret = 0;
				temp = reactor->head;

				while (temp != NULL) {
					if (temp->fd == fds[i].fd) {
						ret = temp->handler(temp->fd, reactor);
						break;
					}

					temp = temp->next;
				}

				if (ret == -1) {
					printf("handler failed\n");
					return NULL;
				}

				else if (ret == 0 && i > 0) {
					temp = reactor->head;

					if (temp->fd == fds[i].fd) {
						reactor->head = temp->next;
						free(temp);
					}

					else {
						while (temp->next != NULL) {
							if (temp->next->fd == fds[i].fd) {
								struct reactor_node *temp2 = temp->next;
								temp->next = temp->next->next;
								free(temp2);
								break;
							}

							temp = temp->next;
						}
					}
				}

			}
		}
	}

	return NULL;
}

void *createReactor() {
	struct reactor_object *reactor = (struct reactor_object *)malloc(sizeof(struct reactor_object));

	if (reactor == NULL) {
		printf("malloc failed\n");
		return NULL;
	}

	reactor->head = NULL;
	reactor->running = false;

	return reactor;
}

void startReactor(void *react) {
	if (react == NULL) {
		printf("reactor is NULL\n");
		return;
	}

	struct reactor_object *reactor = (struct reactor_object *)react;

	if (reactor->running == true) {
		printf("reactor is already running\n");
		return;
	}

	reactor->running = true;

	if (pthread_create(&reactor->thread, NULL, reactorRun, reactor) != 0) {
		printf("pthread_create failed\n");
		return;
	}

	printf("reactor started\n");
}

void stopReactor(void *react) {
	if (react == NULL) {
		printf("reactor is NULL\n");
		return;
	}

	struct reactor_object *reactor = (struct reactor_object *)react;

	if (reactor->running == false) {
		printf("reactor is not running\n");
		return;
	}

	reactor->running = false;

	if (pthread_join(reactor->thread, NULL) != 0) {
		printf("pthread_join failed\n");
		return;
	}

	if (pthread_cancel(reactor->thread) != 0) {
		printf("pthread_cancel failed\n");
		return;
	}

	printf("reactor stopped\n");
}

void addFd(void *react, int fd, handler_t handler) {
	if (react == NULL) {
		printf("reactor is NULL\n");
		return;
	}

	if (fd < 0) {
		printf("fd is negative\n");
		return;
	}

	if (handler == NULL) {
		printf("handler is NULL\n");
		return;
	}

	struct reactor_object *reactor = (struct reactor_object *)react;
	struct reactor_node *node = (struct reactor_node *)malloc(sizeof(struct reactor_node));

	if (node == NULL) {
		printf("malloc failed\n");
		return;
	}

	node->fd = fd;
	node->handler = handler;
	node->next = NULL;

	if (reactor->head == NULL) {
		reactor->head = node;
	} else {
		struct reactor_node *temp = reactor->head;

		while (temp->next != NULL) {
			temp = temp->next;
		}

		temp->next = node;
	}
}

void WaitFor(void *react) {
	if (react == NULL) {
		printf("reactor is NULL\n");
		return;
	}

	struct reactor_object *reactor = (struct reactor_object *)react;

	if (reactor->running == false) {
		printf("reactor is not running\n");
		return;
	}

	if (pthread_join(reactor->thread, NULL) != 0) {
		printf("pthread_join failed\n");
		return;
	}
}
