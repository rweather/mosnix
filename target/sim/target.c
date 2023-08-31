
/* Stripped down version of the sim platform from LLVM-MOS-SDK */

#include <stdint.h>
#include <sys/types.h>

struct _sim_reg {
  uint8_t clock[4];  // 0
  uint8_t unclaimed; // 4
  char getchar;      // 5
  char input_eof;    // 6
  uint8_t abort;     // 7
  int8_t exit;       // 8
  uint8_t putchar;   // 9
};

#define sim_reg_iface ((volatile struct _sim_reg *)0xfff0)

void __putchar(char c)
{
  sim_reg_iface->putchar = c;
}

int getchar()
{
  char c = sim_reg_iface->getchar;
  if (sim_reg_iface->input_eof)
    return -1;
  return c;
}

void _exit(int status)
{
  // Writing to this IO register causes the simulator to exit with the given
  // status.
  sim_reg_iface->exit = (uint8_t)status;

  // Prevent the compiler from considering this entire basic block unreachable.
  __attribute__((leaf)) asm volatile("");

  __builtin_unreachable();
}

__attribute__((alias("_exit"))) void _Exit(int status);

clock_t sys_clock(void)
{
  // reading first byte latches whole 32-bit value
  // so it needs to be read byte by byte in correct order
  unsigned long ticks;
  for (int i = 0; i < 4; i++) {
    ((char *)&ticks)[i] = sim_reg_iface->clock[i];
  }
  return ticks;
}
