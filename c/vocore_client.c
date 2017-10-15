#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define HOSTNAME "192.168.1.108" 
//can also be "Hellbender", just not on vocore.
#define PORT "10"

int main(void)
{
	struct addrinfo hints, *res, *p;
	int status, sockfd, bytes_sent, length;
	char ipstr[INET6_ADDRSTRLEN];
	
	char buffer[88200]; //256
	int message, n;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	

	if ((status = getaddrinfo(HOSTNAME, PORT, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 2;
	}

	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		//-> operator accesses member of struct that's referenced by a pointer
		perror("Socket: error");
	}

	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		close(sockfd);
		perror("connection");
	}

	if (res == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	

	printf("IP addresses for %s:\n", HOSTNAME);

	for (p = res; p != NULL; p = p->ai_next) {
		void *addr;
		char *ipver;

		if (p->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = (char *)"IPv4";
		} else {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = (char *)"IPv6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("%s: %s\n", ipver, ipstr);
	}


	bzero(buffer, 88200);
	while(1){
		message = read(sockfd, buffer, 88200); //255
			//read() returns number of bytes read, -1 if zero
			//writes bytes into buffer
		if(message > 0){ 
			for(n=0; n<message; n=n+4){
					printf("%c%c", buffer[n+1], buffer[n]);
					//printf("%i ",(buffer[n+2]<< 8)+buffer[n+1]);
			}
		}
	}

	close(sockfd);
	freeaddrinfo(res);

	return 0;
}

//./test | aplay -f S16_BE -r44100
