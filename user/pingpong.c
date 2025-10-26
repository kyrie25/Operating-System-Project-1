#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void child_process(int *p_to_c, int *c_to_p)
{
    char buf[1];

    close(p_to_c[1]); // Đóng write end
    close(c_to_p[0]); // Đóng read end

    if (read(p_to_c[0], buf, 1) != 1)
    {
        fprintf(2, "pingpong: child read failed\n");
        exit(1);
    }

    printf("%d: received ping\n", getpid());

    if (write(c_to_p[1], buf, 1) != 1)
    {
        fprintf(2, "pingpong: child write failed\n");
        exit(1);
    }

    close(p_to_c[0]);
    close(c_to_p[1]);
    exit(0);
}

void parent_process(int *p_to_c, int *c_to_p)
{
    char buf[1] = {'X'};

    close(p_to_c[0]); // Đóng read end
    close(c_to_p[1]); // Đóng write end

    if (write(p_to_c[1], buf, 1) != 1)
    {
        fprintf(2, "pingpong: parent write failed\n");
        exit(1);
    }

    if (read(c_to_p[0], buf, 1) != 1)
    {
        fprintf(2, "pingpong: parent read failed\n");
        exit(1);
    }

    printf("%d: received pong\n", getpid());

    close(p_to_c[1]);
    close(c_to_p[0]);
    wait(0);
    exit(0);
}

int main(int argc, char *argv[])
{
    int p_to_c[2]; // Pipe: parent -> child
    int c_to_p[2]; // Pipe: child -> parent

    if (pipe(p_to_c) < 0 || pipe(c_to_p) < 0)
    {
        fprintf(2, "pingpong: pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0)
    {
        fprintf(2, "pingpong: fork failed\n");
        exit(1);
    }

    if (pid == 0)
    {
        child_process(p_to_c, c_to_p);
    }
    else
    {
        parent_process(p_to_c, c_to_p);
    }

    exit(0);
}
