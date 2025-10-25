#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define START 2
#define END   280
__attribute__((noreturn)) void exec_pipe(int fd);
__attribute__((noreturn)) void exec_pipe(int fd) 
{
  int prime;

  if (read(fd, &prime, sizeof(int)) != sizeof(int)) 
  {
    close(fd);
    exit(0);
  }

  printf("prime %d\n", prime);

  int p[2];
  if (pipe(p) < 0) 
  {
    close(fd);
    exit(1);
  }

  int pid = fork();
  if (pid < 0) 
  {
    close(fd);
    close(p[0]);
    close(p[1]);
    exit(1);
  }

  if (pid == 0) 
  {
    close(p[1]);
    close(fd);
    exec_pipe(p[0]);
  } 
  else 
  {
    close(p[0]);
    int tmp;
    while (read(fd, &tmp, sizeof(int)) == sizeof(int)) 
    {
      if (tmp % prime != 0) {
        if (write(p[1], &tmp, sizeof(int)) != sizeof(int)) 
        {
          break;
        }
      }
    }

    close(fd);
    close(p[1]);
    wait(0);
    exit(0);
  }
}

int main(int argc, char *argv[]) 
{
  int p[2];
  if (pipe(p) < 0) 
  {
    fprintf(2, "pipe error!\n");
    exit(1);
  }

  int pid = fork();

  if (pid < 0) 
  {
    fprintf(2, "fork error!\n");
    close(p[0]);
    close(p[1]);
    exit(1);
  }

  if (pid == 0) 
  {
    close(p[1]);
    exec_pipe(p[0]);
  } else {
    close(p[0]);
    for (int i = START; i <= END; i++) 
    {
      if (write(p[1], &i, sizeof(int)) != sizeof(int)) 
      {
        fprintf(2, "fail to write!\n");
        break;
      }
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
}
