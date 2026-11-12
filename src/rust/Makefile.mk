# SPDX-License-Identifier: GPL-2.0-only

rustc_sysroot := $(shell $(RUSTC) --print sysroot)
RUST_LIB_SRC ?= $(rustc_sysroot)/lib/rustlib/src/rust/library

rust_core := $(obj)/rust/core.o
$(rust_core): $(RUST_LIB_SRC)/core/src/lib.rs
	@printf "    RUSTC      $(subst $(obj)/,,$(@))\n"
	$(RUSTC) $(RUSTFLAGS_common) $(RUSTFLAGS_x86_32) \
		--crate-type=rlib \
		-L $(obj)/rust \
		--crate-name=$(patsubst %.o,%,$(notdir $@)) \
		--out-dir=$(dir $@) \
		--emit=obj,metadata \
		--sysroot=/dev/null \
		$<

# FIXME: This doesn't actually work as a compiler-builtins lib.
rust_compiler_builtins := $(obj)/rust/compiler_builtins.o
$(rust_compiler_builtins): $(src)/rust/compiler_builtins.rs $(rust_core)
	@printf "    RUSTC      $(subst $(obj)/,,$(@))\n"
	$(RUSTC) $(RUSTFLAGS_common) $(RUSTFLAGS_x86_32) \
		--crate-type=rlib \
		-L $(obj)/rust \
		--crate-name=$(patsubst %.o,%,$(notdir $@)) \
		--out-dir=$(dir $@) \
		--emit=obj,metadata \
		--sysroot=/dev/null \
		$<

ramstage-rs-deps += $(rust_core) $(rust_compiler_builtins)
