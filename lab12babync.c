#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void parse_inaddr(struct addrinfo *ai, const char *hostname, const char *port) {
  struct addrinfo hint;
  struct addrinfo *head;

  memset(&hint, 0, sizeof(struct addrinfo));
  hint.ai_family = AF_INET;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_protocol = 0;
  hint.ai_flags = AI_NUMERICSERV;

  int r = getaddrinfo(hostname, port, &hint, &head);
  if (r != 0) {
    if (r == EAI_SYSTEM) {
      perror("getaddrinfo");
    } else {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
    }
    exit(1);
  } else {
    *ai = *head;
    ai->ai_next = NULL;
    freeaddrinfo(head);
  }
}
int initialize_connection(const char *hostname, const char *port,
                          struct addrinfo *ai) {
  parse_inaddr(ai, hostname, port);
  int s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
  if (-1 == connect(s, ai->ai_addr, ai->ai_addrlen)) {
    fprintf(stderr, "connect: %s\n", strerror(errno));
    return -1;
  }
  return s;
}

void handle_communication(int s) {
  fd_set readfds;
  char buffer[1024];
  int stdin_eof = 0;
  int socket_eof = 0;

  while (1) {
    FD_ZERO(&readfds);
    if (!socket_eof) {
      FD_SET(s, &readfds);
    }

    if (!stdin_eof) {
      FD_SET(STDIN_FILENO, &readfds);
    }

    if (select(s + 1, &readfds, NULL, NULL, NULL) < 0) {
      fprintf(stderr, "Error in select: %s\n", strerror(errno));
      return;
    }

    if (FD_ISSET(s, &readfds)) {
      ssize_t n = read(s, buffer, sizeof(buffer));
      if (n < 0) {
        fprintf(stderr, "read: %s\n", strerror(errno));
        return;
      }
      if (n == 0) {
        socket_eof = 1;
      } else {
        write(STDOUT_FILENO, buffer, n);
      }
    }

    if (FD_ISSET(STDIN_FILENO, &readfds)) {
      ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer));
      if (n < 0) {
        fprintf(stderr, "read: %s\n", strerror(errno));
        return;
      }
      if (n == 0) {
        stdin_eof = 1;
      } else {
        fprintf(stderr, "Read %zd bytes from stdin.\n", n);
        write(s, buffer, n);
      }
    }

    fprintf(stderr, "stdin_eof is %d socket_eof is %d.\n", stdin_eof,
            socket_eof);

    if (stdin_eof && socket_eof) {
      fprintf(stderr, "Exiting program.\n");
      break;
    }
  }

  shutdown(s, SHUT_WR);
  close(s);
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Need IPv4 address and port.\n");
    return 1;
  }

  struct addrinfo ai;
  int s = initialize_connection(argv[1], argv[2], &ai);
  if (s < 0)
    return 1;

  handle_communication(s);

  return 0;
}