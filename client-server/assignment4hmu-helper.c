#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void send_serial_number(int socket_fd, int serial_number);
FILE *open_file_for_writing(char *user_name, int serial_number,
                            char *file_name);
int write_file(int socket_fd, FILE *file, int promised_file_size);

int main(int argc, char *argv[]) {
  if (argc != 6) {
    fprintf(stderr, "Argv is wrong:%s\n", argv[0]);
    return EXIT_FAILURE;
  }

  int socket_fd = atoi(argv[1]);
  char *user_name = argv[2];
  char *file_name = argv[3];
  int serial_number = atoi(argv[4]);
  int promised_file_size = atoi(argv[5]);

  send_serial_number(socket_fd, serial_number);

  FILE *file = open_file_for_writing(user_name, serial_number, file_name);
  if (file == NULL) {
    perror("fopen");
    close(socket_fd);
    return EXIT_FAILURE;
  }

  if (write_file(socket_fd, file, promised_file_size) != 0) {
    fclose(file);
    remove(file_name);
    close(socket_fd);
    return EXIT_FAILURE;
  }

  fclose(file);

  if (recv(socket_fd, NULL, 0, 0) < 0) {
    fprintf(stderr, "Can not received\n");
  }

  close(socket_fd);

  printf("File received and saved successfully\n");

  return EXIT_SUCCESS;
}

void send_serial_number(int socket_fd, int serial_number) {
  char serial_str[BUFFER_SIZE];
  snprintf(serial_str, sizeof(serial_str), "%d\n", serial_number);
  send(socket_fd, serial_str, strlen(serial_str), 0);
}

FILE *open_file_for_writing(char *user_name, int serial_number,
                            char *file_name) {
  char saved_file_name[BUFFER_SIZE];
  snprintf(saved_file_name, sizeof(saved_file_name), "%s-%d-%s", user_name,
           serial_number, file_name);
  return fopen(saved_file_name, "wb");
}

int write_file(int socket_fd, FILE *file, int promised_file_size) {
  char buffer[BUFFER_SIZE];
  ssize_t bytes_received;
  int total_bytes_received = 0;
  while ((bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0) {
    fwrite(buffer, 1, bytes_received, file);
    total_bytes_received += bytes_received;
  }

  if (total_bytes_received != promised_file_size) {
    fprintf(
        stderr,
        "Received bytes do not match the promised file size. Deleting file.\n");
    return -1;
  }
  return 0;
}