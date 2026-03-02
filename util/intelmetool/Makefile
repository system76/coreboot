# SPDX-License-Identifier: GPL-2.0-or-later

PROGRAM = intelmetool

TOP      ?= $(abspath ../..)
CC       ?= gcc
INSTALL  ?= /usr/bin/env install
PREFIX   ?= /usr/local
CFLAGS   ?= -O0 -g -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function \
           -I $(TOP)/src/commonlib/bsd/include
LDFLAGS  += -lpci -lz
SRCDIR   ?= src
BUILDDIR ?= build

OBJS = intelmetool.o me.o me_status.o mmap.o rcba.o msr.o
OBJS_P = $(addprefix $(BUILDDIR)/, $(OBJS))

OS_ARCH	= $(shell uname)
ifeq ($(OS_ARCH), Darwin)
LDFLAGS += -framework DirectHW
endif
ifeq ($(OS_ARCH), FreeBSD)
CFLAGS += -I/usr/local/include
LDFLAGS += -L/usr/local/lib
LIBS = -lz
endif
ifeq ($(OS_ARCH), NetBSD)
CFLAGS += -I/usr/pkg/include
LDFLAGS += -L/usr/pkg/lib -Wl,-rpath-link,/usr/pkg/lib -lz -lpciutils -lpci -l$(shell uname -p)
endif

all: builddir pciutils dep $(PROGRAM)

oldarc: CFLAGS += -DOLDARC
oldarc: all

$(PROGRAM): $(OBJS_P)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$(PROGRAM) $(OBJS_P) $(LDFLAGS)

# Define build dir and src dirs here. Instead of overloading the default .o target, we're forced to prefix our targets with the build dir. This allows make to pick up on whether a target has been built in the past or not.
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR) *~ junit.xml

distclean: clean
	rm -f .dependencies

dep:
	@$(CC) $(CFLAGS) -MM $(SRCDIR)/*.c > .dependencies

# Make sure the build directory exists.
builddir:
	mkdir -p $(BUILDDIR)

define LIBPCI_TEST
/* Avoid a failing test due to libpci header symbol shadowing breakage */
#define index shadow_workaround_index
#ifdef __NetBSD__
#include <pciutils/pci.h>
#else
#include <pci/pci.h>
#endif
struct pci_access *pacc;
int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;
	pacc = pci_alloc();
	return 0;
}
endef
export LIBPCI_TEST

pciutils:
	@printf "\nChecking for development libraries: pci and zlib... "
	@echo "$$LIBPCI_TEST" > $(BUILDDIR)/.test.c
	@$(CC) $(CFLAGS) $(BUILDDIR)/.test.c -o $(BUILDDIR)/.test $(LDFLAGS) >/dev/null 2>&1 &&	  \
		printf "found.\n" || ( printf "not found.\n\n"; 	  \
		printf "For RPM based distributions like Fedora, please install pciutils-devel and zlib-devel.\n"; \
		printf "For DEB based distributions, please install libpci-dev and zlib1g-dev.\n"; \
		rm -f $(BUILDDIR)/.test.c $(BUILDDIR)/.test; exit 1)
	@rm -rf $(BUILDDIR)/.test.c $(BUILDDIR)/.test $(BUILDDIR)/.test.dSYM

install: $(PROGRAM)
	mkdir -p $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(BUILDDIR)/$(PROGRAM) $(DESTDIR)$(PREFIX)/sbin

.PHONY: all builddir clean distclean dep pciutils oldarc

-include .dependencies
