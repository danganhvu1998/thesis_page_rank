
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 8080

int main(int argc, char const* argv[])
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char* message = (char*)malloc(1000 * sizeof(char));
	sprintf(message, "message from client id %d", rand());
	printf("%s\n", message);
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
	printf("\nConnected. Ready to send \n");
	send(sock, message, strlen(message), 0);
	while (1) {
		scanf("%s", message);
		if (strlen(message) == 1) break;
		printf("message: %s\n", message);
		send(sock, message, strlen(message), 0);
		printf("message message sent\n");
		// valread = read( sock , buffer, 1024);
		// printf("%s %d\n",buffer,valread );
	}
	return 0;
}
