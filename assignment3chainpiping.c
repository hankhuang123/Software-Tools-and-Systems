#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void exec_command(int in_fd, int out_fd, char *cmd[]) {
  if (fork() == 0) {
    if (in_fd != 0) {
      dup2(in_fd, 0);
      close(in_fd);
    }
    if (out_fd != 1) {
      dup2(out_fd, 1);
      close(out_fd);
    }
    if (execvp(cmd[0], cmd) < 0) {
      perror("execvp");
      exit(EXIT_FAILURE);
    }
  }
}
void initPipe(int pp[2]) {
  int isSuccess = pipe(pp); 

  if (isSuccess == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
}
void chain_piping(const command_list *commands) {
  int len = commands->n;
  int in_fd = 0; // Record input side
  int fd[2];
  for (int i = 0; i < len; i++) {
    initPipe(fd);   
    (i == commands->n - 1) ? exec_command(in_fd, 1, commands->cmd[i])
                           : exec_command(in_fd, fd[1], commands->cmd[i]);
    close(fd[1]);  
    in_fd = fd[0];  
  }
  // Wait for all child processes to end
  for (int i = 0; i < len; i++) {
    wait(NULL);
  }
}
