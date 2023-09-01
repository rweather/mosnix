
#include <unistd.h>

int getchar(void)
{
    unsigned char c;
    if (read(0, &c, 1) < 1)
        return -1;
    else
        return c;
}
