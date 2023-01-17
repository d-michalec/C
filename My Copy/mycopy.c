/*
Programming Assignment #1, Damian Michalec, CS431
Program that copies contents of one file into another file. It also counts a number of bytes copied from the source.
*/
#define _POSIX_SOURCE
#include <fcntl.h>
#include <unistd.h>
#undef _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
/*
    Main function of the program. Takes in no more or less than two arguments, which are the names of the files.
    Example: mycopy <sourcefile> <destinationfile> and copies contents of sourcefile to the destination file,
    as well as calculates the number of bytes copied.
*/
int main(int argc, char **argv)
{
    int ret, fd, bytes_written;
    char buf[1024];

    // checking whether the appropiate amount of arguments was passed on. (3)
    if (argc != 3)
    {
        fprintf(stderr, "mycopy: invalid number of arguments.\n usage:  mycopy <sourcefile> <destinationfile>\n");
        exit(1);
    }

    // opening and reading the file
    if ((fd = open(argv[1], O_RDONLY)) < 0)
    {
        fprintf(stderr, "could not open file %s: No such file or directory.\n", argv[1]);
        exit(1);
    }
    else
    {
        while ((ret = read(fd, buf, sizeof(buf) - 1)) > 0)
        {
            buf[ret] = 0x00;
        }
        close(fd);
    }

    // writing into a newly created file contents of the source file
    if ((fd = creat(argv[2], 0644)) < 0)
    {
        fprintf(stderr, "could not write to file %s: Permission Denied.\n", argv[2]);
        exit(1);
    }
    else
    {
        bytes_written = write(fd, buf, strlen(buf));
        close(fd);
    }

    printf("copied %d bytes from file %s to %s\n", bytes_written, argv[1], argv[2]);
    return 0;
}