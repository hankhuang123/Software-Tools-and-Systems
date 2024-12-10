#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define BACKLOG 5
#define BUFFER_SIZE 1024

void sigchld_handler(int s);
ssize_t readline(int sock, char *buffer, size_t max_len);
int setup_server(int port);
void handle_client(int client_fd, const char *helper_path, int serial_number);
void process_client(int server_fd, const char *helper_path, int *serial_number);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Argc is wrong: %s \n", argv[0]);
    return EXIT_FAILURE;
  }

  int port = atoi(argv[1]);
  char *helper_path = argv[2];

  int server_fd = setup_server(port);
  if (server_fd < 0)
    return EXIT_FAILURE;

  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa, NULL);

  int serial_number = 1;
  printf("Server is running on port %d\n", port);

  while (1) {
    process_client(server_fd, helper_path, &serial_number);
  }

  close(server_fd);
  return EXIT_SUCCESS;
}

void sigchld_handler(int s) {
  // Wait all dead processes
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
}

// Helper function to read a line from socket
ssize_t readline(int sock, char *buffer, size_t max_len) {
  ssize_t bytes_read = 0;
  while (bytes_read < max_len - 1) {
    char c;
    ssize_t res = recv(sock, &c, 1, 0);
    if (res <= 0)
      return res;
    if (c == '\n')
      break;
    buffer[bytes_read++] = c;
  }
  buffer[bytes_read] = '\0';
  return bytes_read;
}

int setup_server(int port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    fprintf(stderr, "Server can not create\n");
    return -1;
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    fprintf(stderr, "Server cannot bind the port: %d\n", port);
    return -1;
  }

  if (listen(server_fd, BACKLOG) < 0) {
    fprintf(stderr, "Server cannot listen the port: %d\n", port);
    return -1;
  }

  return server_fd;
}

void handle_client(int client_fd, const char *helper_path, int serial_number) {
  fflush(stdout);

  char username[BUFFER_SIZE];
  char filename[BUFFER_SIZE];
  char filesize_str[BUFFER_SIZE];

  readline(client_fd, username, sizeof(username));
  readline(client_fd, filename, sizeof(filename));
  readline(client_fd, filesize_str, sizeof(filesize_str));

  char serial_str[10];
  snprintf(serial_str, sizeof(serial_str), "%d", serial_number);
  char client_fd_str[10];
  snprintf(client_fd_str, sizeof(client_fd_str), "%d", client_fd);

  execl(helper_path, helper_path, client_fd_str, username, filename, serial_str,
        filesize_str, NULL);
  fprintf(stderr, "execl: %s\n", strerror(errno));
}

void process_client(int server_fd, const char *helper_path,
                    int *serial_number) {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd =
      accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd < 0) {
    fprintf(stderr, "Cannot connect with client: \n");
    return;
  }

  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
  printf("Client connected from %s:%d\n", client_ip,
         ntohs(client_addr.sin_port));

  pid_t pid = fork();
  if (pid < 0) {
    fprintf(stderr, "Can not fork\n");
    close(client_fd);
    return;
  }

  if (pid == 0) {
    close(server_fd);
    handle_client(client_fd, helper_path, *serial_number);
    close(client_fd);
    exit(EXIT_FAILURE);
  } else {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
      (*serial_number)++;
    }
    close(client_fd);
    printf("Client disconnected from %s:%d\n", client_ip,
           ntohs(client_addr.sin_port));
  }
}
