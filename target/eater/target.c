
/* Stripped down version of the eater platform from LLVM-MOS-SDK */

#include <stdint.h>

int __chrin(void);
int __chrin_no_wait(void);
void __chrout(char c);

void __putchar(char c) {
  if (__builtin_expect(c == '\n', 0)) {
    __chrout('\r');
    __chrout('\n');
  } else {
    __chrout(c);
  }
}

int getchar(void) {
  int c = __chrin();
  if (c == '\r')
    c = '\n';
  return c;
}
