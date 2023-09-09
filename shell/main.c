
#include "print.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>

static char buffer[128];

static void print_banner(void)
{
    struct utsname uts;
    uname(&uts);
    print_nl();
    print_string(uts.sysname);
    print_char(' ');
    print_string(uts.release);
    print_char(' ');
    print_string(uts.machine);
    print_nl();
    print_nl();
}

static int get_line(char *buf, size_t size)
{
    size_t posn = 0;
    int ch;
    for (;;) {
        ch = getchar();
        if (ch < 0) {
            return 0;
        } else if (ch == '\r' || ch == '\n') {
            break;
        } else if (ch == '\b' || ch == 0x7F) {
            if (posn > 0) {
                --posn;
            }
        } else if (ch == 0x15) { /* CTRL-U */
            posn = 0;
        } else if (ch >= ' ' && posn < (size - 1U)) {
            buf[posn++] = (char)ch;
        }
    }
    buf[posn] = '\0';
    return 1;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    print_banner();

    for (;;) {
        /* Print the current working directory and the prompt */
        getcwd(buffer, sizeof(buffer));
        print_string(buffer);
        print_string("# ");

        /* Get a line of input */
        if (!get_line(buffer, sizeof(buffer)))
            break;

        /* Process the input line */
        print_string("ECHO: ");
        print_string(buffer);
        print_nl();
    }
    print_nl();

    return 0;
}
