
#include <stdio.h>
#include <sys/utsname.h>

static void print_banner(void)
{
    struct utsname uts;
    uname(&uts);
    printf("\n%s %s %s\n", uts.sysname, uts.release, uts.machine);
}

#if 1 /*defined(MOSNIX_TARGET_SIM)*/
/* The simulator echos within the host system as it doesn't have cbreak mode. */
#define echo_char(c) do { ; } while (0)
#else
#define echo_char(c) putchar((c))
#endif

static char buffer[128];

static int get_line(char *buf, size_t size)
{
    size_t posn = 0;
    int ch;
    for (;;) {
        ch = getchar();
        if (ch < 0) {
            return 0;
        } else if (ch == '\r' || ch == '\n') {
            echo_char('\n');
            break;
        } else if (ch == '\b' || ch == 0x7F) {
            if (posn > 0) {
                echo_char('\b');
                echo_char(' ');
                echo_char('\b');
                --posn;
            }
        } else if (ch == 0x15) { /* CTRL-U */
            while (posn > 0) {
                echo_char('\b');
                echo_char(' ');
                echo_char('\b');
                --posn;
            }
        } else if (ch >= ' ' && posn < (size - 1U)) {
            buf[posn++] = (char)ch;
            echo_char((char)ch);
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
        putchar('#');
        putchar(' ');
        if (!get_line(buffer, sizeof(buffer)))
            break;
        printf("ECHO: %s\n", buffer);
    }
    putchar('\n');

    return 0;
}
