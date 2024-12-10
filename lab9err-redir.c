#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/* Basically cmd 2> file */

int main(int argc, char **argv)
{
  pid_t p;
  int fd_err;

  if (argc < 3) {
    fprintf(stderr, "Need at least filename and program.\n");
    return 1;
  }
  if ((fd_err = open(argv[1], O_WRONLY |O_TRUNC|O_CREAT, 0666)) == -1) {
    perror("cannot open");
    return 1;
  }
  if ((p = fork()) == -1) {
    perror("cannot fork");
    return 1;
  } else if (p != 0) {
    // TODO: Which FD should you close right away? (I will test for this when
    // marking. So can you: Use ./sample po to check.)
    close(fd_err);
    
    int ws;
    wait(&ws);
    printf("wait status: %.4x\n", (unsigned)ws); // 4-digit hexadecimal
    // TODO:
    // If the wait status indicates normal exit:
    //   printf("exit status: %d\n", the exit code);
    // If the wait status indicates killed-by-signal:
    //   printf("signal: %d\n", the signal number);
    // If neither, nothing to do. (Outside the scope of this lab.)
    // See lecture slide or man page for how to tell.

    if(WIFEXITED(ws)){
      printf("exit status: %d\n", WEXITSTATUS(ws));
    }
    else if(WIFSIGNALED(ws)){
      printf("signal: %d\n", WTERMSIG(ws));
    }
    return 0;
  } else {
    // TODO: Redirect stderr to fd_err
    //
    // And don't forget to close fd_err after dupping it to stderr.  (I will
    // test for it, so can you: Use check-open.c as the target program to report
    // if any FD other than 0, 1, 2 is still open.)
    //
    // But wait! There is more.
    //
    // You see, if exec fails (therefore it returns), I want perror() to print
    // the error message to the *original* stderr (the one before redirection).
    // So, how do you make a backup of now, so that you can restore later?
    //
    // But wait! There is more.
    //
    // You see, the program you exec will inherit the backup too. We do not want
    // that. (I will test for it, so can you: Use check-open.c as the target
    // program to report if any FD other than 0, 1, 2 is still open.) How do you
    // say: if exec is successful, close the backup, else don't?
    //
    // For that, read up on fcntl() (super long but just look for:) and its
    // F_GETFD, F_SETFD, FD_CLOEXEC.

    // TODO: Replace ARGS below by the correct thing.
    // Hint: argv[argc] is guaranteed to be NULL too. We wondow why. :)
    int last_stderr = dup(2);
    if(dup2(fd_err,2)== -1){
      perror("it is error when redirect stderr");
      return 1;
    }

    close(fd_err);

    fcntl(last_stderr,F_SETFD, FD_CLOEXEC);
    execvp(argv[2], &argv[2]);
    int exec_errno = errno;
    // TODO: Restore the original stderr.
    dup2(last_stderr,2);

    errno = exec_errno;
    perror("cannot exec");
    return exec_errno == ENOENT ? 127 : 126; // ENOENT = file not found
  }
}
