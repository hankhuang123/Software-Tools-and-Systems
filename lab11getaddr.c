#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

void print_inet4_addr(const struct sockaddr *a) {
  if (a->sa_family != AF_INET) {
    fprintf(stderr, "non-IPv4 address\n");
    exit(1);
  }
  const struct sockaddr_in *a4 = (const struct sockaddr_in *)a;
  char buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &a4->sin_addr, buf, INET_ADDRSTRLEN);
  printf("%s\n", buf);
}

int main(int argc, char *argv[]) {
  struct addrinfo hint;
  struct addrinfo *head;

  if (argc < 2) {
    fprintf(stderr, "Please provide a host name\n");
    exit(1);
  }
  
  memset(&hint, 0, sizeof(struct addrinfo));
  hint.ai_family = AF_INET;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_protocol = 0;
  hint.ai_flags = 0;

  const char* portNumber = "80";
  int res = getaddrinfo(argv[1], portNumber, &hint, &head);
  if (res != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(res));
    exit(1);
  }

  struct addrinfo *current;
  for (current = head; current != NULL; current = current->ai_next) {
    print_inet4_addr(current->ai_addr);
  }

  freeaddrinfo(head);

  return 0;
}
