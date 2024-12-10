#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int connect_to_server(const char *ip_address, int port_num);
int send_data(int client_socket, const char *username, const char *filename, FILE *file);
int receive_serial_number(int client_socket);

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "argv is wrong: %s\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *ip_address = argv[1];
  int port_num = atoi(argv[2]);
  char *username = argv[3];
  char *filename = argv[4];

  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "File can not open\n");
    return EXIT_FAILURE;
  }

  int client_socket = connect_to_server(ip_address, port_num);
  if (client_socket < 0) {
    fclose(file);
    return EXIT_FAILURE;
  }

  if (send_data(client_socket, username, filename, file) != 0) {
    fclose(file);
    close(client_socket);
    return EXIT_FAILURE;
  }

  fclose(file);

  if (receive_serial_number(client_socket) != 0) {
    close(client_socket);
    return EXIT_FAILURE;
  }

  close(client_socket);
  return EXIT_SUCCESS;
}

int connect_to_server(const char *ip_address, int port_num) {
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) {
    fprintf(stderr, "Can not connect to server");
    return -1;
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_num);
  inet_pton(AF_INET, ip_address, &server_address.sin_addr);

  // fprintf(stderr, "Connecting to server at %s:%d...\n", ip_address,
  // port_num);
  if (connect(client_socket, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    fprintf(stderr, "Can not connect to server\n");
    close(client_socket);
    return -1;
  }
  // fprintf(stderr, "Connected to server at %s:%d\n", ip_address, port_num);

  return client_socket;
}

int send_data(int client_socket, const char *username, const char *filename,
              FILE *file) {
  fseek(file, 0L, SEEK_END);
  long filesize = ftell(file);
  fseek(file, 0L, SEEK_SET);

  // Send username
  if (send(client_socket, username, strlen(username), 0) < 0 ||
      send(client_socket, "\n", 1, 0) < 0) {
    fprintf(stderr, "Can not send username\n");
    return -1;
  }
  // printf("Sending username: %s\n", username);

  // Send filename
  if (send(client_socket, filename, strlen(filename), 0) < 0 ||
      send(client_socket, "\n", 1, 0) < 0) {

    fprintf(stderr, "Can not send filename\n");
    return -1;
  }
  // printf("Sending filename: %s\n", filename);

  // Send file size
  char size_str[11];
  snprintf(size_str, sizeof(size_str), "%ld\n", filesize);
  if (send(client_socket, size_str, strlen(size_str), 0) < 0) {

    fprintf(stderr, "Can not send file size\n");
    return -1;
  }
  // printf("Sending file size: %ld\n", filesize);

  // printf("Sending file content...\n");
  char buffer[BUFFER_SIZE];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    if (send(client_socket, buffer, bytes_read, 0) < 0) {

      fprintf(stderr, "Can not send file content\n");
      return -1;
    }
  }
  // printf("File content sent successfully\n");

  return 0;
}

int receive_serial_number(int client_socket) {
  // printf("Receiving serial number from server...\n");
  char serial_number[12];
  int bytes_received =
      recv(client_socket, serial_number, sizeof(serial_number) - 1, 0);

  if (bytes_received <= 0 || bytes_received > 10 ||
      serial_number[bytes_received - 1] != '\n') {
    fprintf(stderr, "Server bug: Invalid serial number received\n");
    return -1;
  }
  // printf("Received serial number: %s\n", serial_number);

  serial_number[bytes_received - 1] = '\0';

  for (int i = 0; i < bytes_received - 1; i++) {
    if (!isdigit(serial_number[i])) {
      fprintf(stderr, "Server bug: Invalid serial number received\n");
      return -1;
    }
  }

  printf("%s\n", serial_number);

  return 0;
}
