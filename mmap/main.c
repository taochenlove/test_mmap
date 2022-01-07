
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    char *p_map;

    int fd;


    p_map = (char *)mmap(NULL, 100*1024*1024, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    printf("p_map:%p\n", p_map);

    fd = open("/dev/test_dev", O_RDWR | O_CLOEXEC);

    mmap(p_map, 100*1024*1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED , fd, 0x1ULL<<40);

    p_map[0] = 0xa;
    return 0;
}

