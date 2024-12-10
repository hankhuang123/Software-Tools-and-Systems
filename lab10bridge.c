#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void initPipe(int pp1[2], int pp2[2]);
void executeChildProcess(int pp1[2], int pp2[2], char *program);
int createChildProcess(int pp1[2], int pp2[2], char *program);
void closePipe(int pp1[2], int pp2[2]);

int main(void) {
  int pp1[2]; // foo write，bar read
  int pp2[2]; // foo read，bar write
  initPipe(pp1, pp2);

  int fooPid = createChildProcess(pp2, pp1, "./foo");
  int barPid = createChildProcess(pp1, pp2, "./bar");

  closePipe(pp1, pp2);
  waitpid(fooPid, NULL, 0);
  waitpid(barPid, NULL, 0);

  return EXIT_SUCCESS;
}

void initPipe(int pp1[2], int pp2[2]) {
  int isSuccessP1 = pipe(pp1); // initial pp1
  int isSuccessP2 = pipe(pp2); // inital pp2

  if (isSuccessP1 == -1 || isSuccessP2 == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
}

void executeChildProcess(int pp1[2], int pp2[2], char *program) {
  char in[4], out[4];
  snprintf(in, 4, "%d", pp1[0]);  //  int to str
  snprintf(out, 4, "%d", pp2[1]); //  int to str

  int result = execl(program, program, in, out, NULL);
  if (result == -1) { // run child process
    // if child process failed
    perror("execl");
    exit(EXIT_FAILURE);
  }
}

int createChildProcess(int pp1[2], int pp2[2], char *program) {
  int pid = fork(); // inital fork process pid

  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {  // if child process is successful
    close(pp1[1]); // close write
    close(pp2[0]); // close read

    executeChildProcess(pp1, pp2, program);
  }

  return pid;
}

void closePipe(int pp1[2], int pp2[2]) {
  close(pp1[0]);
  close(pp1[1]);
  close(pp2[0]);
  close(pp2[1]);
}