
#include <stdio.h>
#include <sys/utsname.h>

static void print_banner(void)
{
    struct utsname uts;
    uname(&uts);
    printf("%s %s %s\n", uts.sysname, uts.release, uts.machine);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    print_banner();

    return 0;
}
