#include <fcntl.h>
#include <unistd.h>
#include <err.h>

main()
{
        int fd, res;

        fd = open("/tmp/mist", O_RDWR|O_CREAT|O_TRUNC, 0644);
        res = lseek(fd, SEEK_SET, 100);
        if (res < 0)
                warn("lseek");
        res = write(fd, "x", 1);
        if (res < 0)
                warn("write");
}
