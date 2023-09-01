
#include <unistd.h>

void __putchar(char c)
{
    write(1, &c, 1);
}
