#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int find(char *path, char *fileName)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    int count = 0;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        return 0;
    }

    if (fstat(fd, &st) < 0)
    {
        close(fd);
        return 0;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
    {
        char *name = path;
        for (char *t = path; *t; t++)
            if (*t == '/')
                name = t + 1;

        if (strcmp(name, fileName) == 0)
        {
            printf("%s\n", path);
            count++;
        }
        
        break;
    }

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
            
            count += find(buf, fileName);
        }
        break;
    }
    close(fd);
    return count;
}

int
main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }
    
    int count = find(argv[1], argv[2]);
    
    if (count == 0)
    {
        printf("No files found matching '%s'\n", argv[2]);
    }
    
    exit(0);
}