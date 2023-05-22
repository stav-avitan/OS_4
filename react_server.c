#include "reactor.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(SERVER_PORT),
		.sin_addr.s_addr = htonl(INADDR_ANY)
	};

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	int reuse = 1;

	if (server_fd < 0) {
		printf("socket failed\n");
		return 1;
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("setsockopt failed\n");
		return 1;
	}

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("bind failed\n");
		return 1;
	}

	if (listen(server_fd, MAX_QUEUE) < 0) {
		printf("listen failed\n");
		return 1;
	}

	void *react = createReactor();

	if (react == NULL) {
		printf("createReactor failed\n");
		return 1;
	}

	printf("reactor created\n");

	addFd(react, server_fd, server_handler);

	printf("server added\n");

	startReactor(react);

	printf("reactor started\n");

	WaitFor(react);

	return 0;
}

int client_handler(int fd, void *react) {
	if (react == NULL) {
		printf("reactor is NULL\n");
		return 1;
	}

	char buf[MAX_BUFFER];
	int len = read(fd, buf, sizeof(buf));

	if (len < 0) {
		printf("read failed\n");
		return -1;
	}

	if (len == 0) {
		printf("client disconnected\n");
		return 0;
	}

	buf[len] = '\0';

	printf("client: %s\n", buf);

	return 1;
}

int server_handler(int fd, void *react) {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);

	if (client_fd < 0) {
		printf("accept failed\n");
		return -1;
	}

	printf("client connected\n");

	addFd(react, client_fd, client_handler);

	printf("client added\n");

	return 1;
}
