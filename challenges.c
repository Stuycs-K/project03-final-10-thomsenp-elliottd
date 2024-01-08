#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int err() {
    printf("errno %d\n%s\n", errno, strerror(errno));
    exit(1);
}

int randyLit() {
    int randNum;
    int* pointer = &randNum;
    int recieve = open("/dev/random", O_RDONLY);

    if (recieve == -1)
        err();

    read(recieve, pointer, 4);

    return *pointer;
}


