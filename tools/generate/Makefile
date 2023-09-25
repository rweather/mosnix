
.PHONY: all

ERRNO_TXT = ../bits/errno.txt
SYSCALL_TXT = ../bits/syscall.txt
BITS_ERRNO = ../../include/bits/errno.h
BITS_SYSCALL = ../../include/bits/syscall.h
BITS_UNISTD = ../../include/bits/unistd.h
MOSNIX_SYSCALL = ../../include/mosnix/syscall.h
OS_STRERROR = ../../os/strerror.c
LIBC_UNISTD = ../../libc-mosnix/unistd.c
DISPATCH_SYSCALL = ../../os/dispatch.c

all: \
	$(BITS_ERRNO) \
	$(BITS_SYSCALL) \
	$(BITS_UNISTD) \
	$(MOSNIX_SYSCALL) \
	$(OS_STRERROR) \
	$(LIBC_UNISTD) \
	$(DISPATCH_SYSCALL)

$(BITS_ERRNO): $(ERRNO_TXT) generrno.py
	./generrno.py $< >$@

$(BITS_SYSCALL): $(SYSCALL_TXT) genbits.py
	./genbits.py $< >$@

$(BITS_UNISTD): $(SYSCALL_TXT) genunistdh.py
	./genunistdh.py $< >$@

$(MOSNIX_SYSCALL): $(SYSCALL_TXT) gendecl.py
	./gendecl.py $< >$@

$(OS_STRERROR): $(ERRNO_TXT) genstrerror.py
	./genstrerror.py $< >$@

$(LIBC_UNISTD): $(SYSCALL_TXT) genunistdc.py
	./genunistdc.py $< >$@

$(DISPATCH_SYSCALL): $(SYSCALL_TXT) gendispatch.py
	./gendispatch.py $< >$@
