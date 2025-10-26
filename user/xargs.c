#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXBUF 512

// Read a single character and handle it
// Returns: 1 if we got a token, 0 if EOF, -1 if we should continue reading
int readline(char *buf, int maxlen)
{
    int i = 0;
    char c;
    
    while(i < maxlen - 1) {
        int n = read(0, &c, 1);
        if(n <= 0) {
            // EOF
            if(i > 0) {
                buf[i] = 0;
                return i;
            }
            return 0;
        }
        
        if(c == '\n') {
            buf[i] = 0;
            return i;
        }
        
        buf[i++] = c;
    }
    
    buf[i] = 0;
    return i;
}

int main(int argc, char *argv[])
{
    char buf[MAXBUF], *args[MAXARG];
    int i;
    
    if(argc < 2) {
        fprintf(2, "usage: xargs command [args...]\n");
        exit(1);
    }

    for(i = 1; i < argc; i++) {
        args[i-1] = argv[i];
    }
    
    while(readline(buf, MAXBUF) > 0) {
        char *p = buf;
        int arg_idx = argc - 1;
        
        while(*p == ' ' || *p == '\t') {
            p++;
        }

        while(*p != 0 && arg_idx < MAXARG - 1) {
            args[arg_idx++] = p;
            
            while(*p != 0 && *p != ' ' && *p != '\t') {
                p++;
            }
            
            if(*p != 0) {
                *p = 0;
                p++;

                while(*p == ' ' || *p == '\t') {
                    p++;
                }
            }
        }

        args[arg_idx] = 0;
        int pid = fork();
        if(pid == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        } else if(pid > 0) {
            // Parent process
            wait(0);
        } else {
            fprintf(2, "xargs: fork failed\n");
            exit(1);
        }
    }
    
    exit(0);
}