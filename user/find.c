#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *fileName)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
    
        char *name = path;
        for (char *t = path; *t; t++)
            if (*t == '/')
                name = t + 1;

        if (strcmp(name, fileName) == 0)
            printf("%s\n", path);

    case T_DIR:

        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("ls: path too long\n");
            close(fd);
            break;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;

            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue; 
                  
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            
            find(buf, fileName);
        }
        break;
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}