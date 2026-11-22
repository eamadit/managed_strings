# CMU CERT Managed Strings in 42 Coding Norm — Crash-Proof PID 1 (`musl` + QEMU)

A complete C11 implementation of the **CMU/SEI CERT Managed Strings specification** (`CMU/SEI-2010-TR-018` / `ESC-TR-2010-018`, *Specifications for Managed Strings, Second Edition*), engineered strictly under the **42 School Coding Norm (v1.8)**, statically linked against **musl libc**, and booted directly as **PID 1 (`/sbin/init`)** inside a minimal Linux `initramfs` in **QEMU**.

---

## Why PID 1?

In UNIX and Linux systems, **PID 1 (`/sbin/init`)** is the ancestor of every user-space process. Unlike a standard user application that terminates with a `Segmentation fault` on a buffer overflow, an unhandled memory violation or stack exhaustion in PID 1 causes an immediate **`Kernel panic - not syncing: Attempted to kill init!`**, halting the entire operating system.

Running an interactive shell (`msh`) and security test suite directly inside PID 1 demonstrates deterministic memory safety, runtime constraint handling, and `O(1)` stack-depth guarantees with zero safety net.

---

## Architecture & Standards

* **Specification:** CMU/SEI CERT Managed Strings (`CMU/SEI-2010-TR-018` / `ESC-TR-2010-018`)
  * Opaque, bounds-checked managed string objects (`t_string_mx`) tracking `len`, `maxlen`, `is_null`, `is_const`, `is_wide`, and character allow-lists (`charset`).
  * Automatic byte-to-wide (`char *` → `wchar_t *`) UTF-8 promotion.
  * Pluggable runtime-constraint handler (`set_constraint_handler_m`) that intercepts violations without heap allocation or terminating PID 1.
* **Coding Standard:** 42 School Coding Norm v1.8
  * Maximum 25 lines per function body, 5 functions per `.c` file, 4 parameters per function, and 80 columns per line.
  * Strict iterative design (`while` loops only, zero recursion, zero VLAs).
  * Compiled with `-Wall -Wextra -Werror -std=c11`.
* **C Runtime:** `musl libc` (`1.2.6`), producing a 100% self-contained static binary (`-static`) with zero `glibc` dependencies.
* **Boot Environment:** Linux Kernel + custom `cpio` `initramfs` running in `qemu-system-x86_64`.

---

## Live Security Audit (`run /test2.msh`)

Booting the `initramfs` launches `msh` as PID 1. Running `run /test2.msh` executes 9 consecutive exploit simulations and architectural stress tests without crashing PID 1:

1. **`[1/9]` Command Dispatch & Shell State:** Verifies bounded command matching (`cstrcmp_m`, `cstrncmp_m`).
2. **`[2/9]` 257-Byte Buffer Overflow Survival:** Attempts to copy a 257-byte payload into a 256-byte managed command buffer; caught cleanly by `pid1_fault_handler` (`strncpy_m: limit/set`).
3. **`[3/9]` Character Allow-List vs. Shell Injection:** Enforces `ABCDEFGHIJKLMNOPQRSTUVWXYZ_` via `setcharset_m` and blocks `;rm -rf /` (`strncat_m: limit/set`).
4. **`[4/9]` Immutability Shield (`const_strcreate_m`):** Protects a locked kernel string (`KERNEL_LOCKED`) from overwrite attempts (`strncpy_m: invalid arg`).
5. **`[5/9]` Iterative Path Tokenizer & UTF-8 Promotion:** Splits paths via `strtok_m` and promotes ASCII strings to wide UTF-8 (`wstrcat_m`).
6. **`[6/9]` PID 1 Stack & Heap Integrity:** Confirms clean recovery after consecutive runtime constraint violations.
7. **`[7/9]` 5,000-Level Nested Directory Traversal:** Walks a 5,000-level path iteratively in O(N) time with O(1) call-stack depth, eliminating recursive stack-overflow DoS.
8. **`[8/9]` 200-Level Nested Shell Expression Bomb (`$($(...))`):** Halts deeply nested expression parsing at depth 16 using a bounded iterative state machine.
9. **`[9/9]` Graph Database Schema Lock & Cypher Injection Shield:** Locks node labels via `const_strcreate_m`, sanitizes property keys with an identifier allow-list (blocking `'}) DETACH DELETE n //`), and verifies O(1) length retrieval (`strlen_m`).

---

## Prerequisites

* `gcc` & `make`
* `musl libc` (`musl-gcc` wrapper)
* `cpio` & `gzip`
* `qemu-system-x86_64` and a Linux kernel image (e.g., `/boot/vmlinuz`)

### Installing `musl` Locally (No Root Required)

```bash
wget https://libc.org
tar -xzf musl-1.2.6.tar.gz
cd musl-1.2.6
./configure --prefix=/opt/musl --syslibdir=/opt/musl/lib
make -j$(nproc)
make install DESTDIR="$PWD/install"
sed -i "s|/opt/musl|$PWD/install/opt/musl|g" \
  "$PWD/install/opt/musl/bin/musl-gcc" \
  "$PWD/install/opt/musl/lib/musl-gcc.specs"
```

---

## Build & Boot Instructions

### 1. Build `libstring_m.a` with `musl-gcc`

```bash
make re CC=/path/to/musl-gcc
```

### 2. Verify 42 Coding Norm Compliance

```bash
python3 norminette_v18.py *.c *.h
```
bash norminette_v18.sh *.c *.h


### 3. Compile PID 1 (`init`) Statically & Pack `initramfs.cpio.gz`

```bash
/path/to/musl-gcc -Wall -Wextra -Werror -std=c11 -static \
  init.c msh_extra_tests.c -L. -lstring_m -o init

mkdir -p rootfs/proc rootfs/sys rootfs/dev
cp init rootfs/init
(cd rootfs && find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz)
```

### 4. Boot in QEMU

```bash
qemu-system-x86_64 \
  -kernel /boot/vmlinuz \
  -initrd initramfs.cpio.gz \
  -nographic \
  -append "console=ttyS0 quiet"
```

*(Note: On distributions where `/boot/vmlinuz` is readable only by root, run with `sudo` or copy the kernel image to a user-readable path. Press `Ctrl+A` followed by `X` to exit QEMU.)*

### 5. Run the Security Suite Inside PID 1

Once the `msh# ` prompt appears in QEMU:

```text
run /test2.msh
```

---

## Repository Structure

* `string_m.h` — Public API header (`CMU/SEI-2010-TR-018`)
* `string_m_internal.h` — Internal structures and hidden helper prototypes
* `sm_*.c` — 21 modular C11 source files implementing all 57 public functions
* `init.c` — Static PID 1 (`/sbin/init`) and bounded `msh` shell
* `msh_extra_tests.c` — Deep path traversal, nested shell bomb, and Graph DB injection tests
* `rootfs/test2.msh` — Automated 9-step runtime security audit script
* `norminette_v18.py` / `norminette_v18.sh` — 42 Coding Norm v1.8 static validators
* `Makefile` — Static archive build system (`libstring_m.a`)





------------------------------------------------------------------------------------

------------------------------------------------------------------------------------





# libstring_m — CMU CERT Managed Strings (42 Norm v1.8)


A C11 implementation of the **CMU CERT Managed Strings Library** (`CMU/SEI-2010-TR-018`), engineered from the ground up to comply strictly with **La norme de 42 (Version 1.8)** and non-recursive systems-security architecture.

---

## Key Features

- **Complete 46-Function API (`string_m.h`):** Supports narrow (`char`) and wide (`wchar_t`) managed strings (`t_string_mx`), dynamic length bounds (`maxlen`), character-set allowlists (`charset`), immutability flags (`is_const`), and automatic wide-character promotion.
- **Runtime Constraint Handling (`set_constraint_handler_m`):** Dispatches boundary, character-set, null-pointer, and allocation violations to a configurable `t_constraint_handler` (defaulting to safe diagnostic abort).
- **Zero-Recursion Security Architecture:** All string creation, slicing, tokenization (`strtok_m`), formatting (`sprintf_m`, `fprintf_m`, `printf_m`), and scanning (`sscanf_m`, `fscanf_m`, `scanf_m`) routines are strictly iterative (`while`-only) to prevent stack-exhaustion vulnerabilities on untrusted input.
- **Strict 42 Norm v1.8 Compliance:**
  - Standard 80-column 42 file headers and `<= 80`-column line width (4-space tab stops).
  - `<= 5` functions per `.c` file and `<= 25` lines per function body.
  - `<= 4` parameters per function and `<= 5` uninitialized variable declarations at the top of each block.
  - No `for`, `do...while`, `switch`, `case`, `goto`, or C++ `//` comments.
  - Wildcard-free `Makefile` with `$(NAME)`, `all`, `clean`, `fclean`, and `re` rules.

---

## Repository Structure

```text
.
├── Makefile              # Static library build configuration (libstring_m.a)
├── string_m.h            # Public API header and type definitions
├── string_m_internal.h   # Internal structures and helper prototypes
├── sm_core.c             # Constraint handler and memory lifecycle hooks
├── sm_convert.c          # Charset validation and multibyte/wide conversion
├── sm_util_state.c       # Null, empty, narrow/wide, and length queries
├── sm_util_create.c      # Allocation, creation, const-creation, deletion
├── sm_util_extract.c     # Raw C/wide extraction and deep duplication
├── sm_copy.c             # Bounded and unbounded string copy functions
├── sm_concat.c           # Narrow and wide-promoted concatenation
├── sm_wconcat.c          # Wide C-string concatenation wrappers
├── sm_compare.c          # Lexicographical managed string comparison
├── sm_wcompare.c         # Narrow and wide C-string comparison wrappers
├── sm_search.c           # Tokenization (strtok_m) and character lookup
├── sm_span.c             # Prefix accept-span functions (strspn_m)
├── sm_cspan.c            # Prefix reject-span functions (strcspn_m)
├── sm_config.c           # Runtime charset and maxlen configuration
├── sm_slice.c            # Offset, left, and right substring slicing
├── sm_char.c             # Single narrow and wide character indexing
├── sm_printf_core.c      # Iterative format string parser
├── sm_printf_api.c       # sprintf_m, fprintf_m, printf_m entry points
├── sm_scanf_core.c       # Iterative input scanner engine
├── sm_scanf_api.c        # sscanf_m, fscanf_m, scanf_m entry points
├── sm_stdio_v.c          # va_list wrappers (vprintf_m, vscanf_m)
├── main.c                # Core unit test suite
├── test_scan.c           # Scanner, tokenizer, and charset unit tests
├── norminette_v18.sh     # Standalone Bash + awk Norminette v1.8 checker
└── norminette_v18.py     # Python 3 C-tokenizer Norminette v1.8 checker
```

---

## Build & Installation

### 1. Compile the Static Library (`libstring_m.a`)
```bash
make
```

### 2. Rebuild from Scratch
```bash
make re
```

### 3. Clean Object Files & Binary Artifacts
```bash
# Remove object files (.o)
make clean

# Remove object files and libstring_m.a
make fclean
```

---

## Running the Test Suite

### Compile and Execute `test_runner`
```bash
make re && gcc -Wall -Wextra -Werror -std=c11 main.c test_scan.c -L. -lstring_m -o test_runner && ./test_runner
```

Expected output:
```text
All 42-Norm Managed Strings tests passed!
```

### Troubleshooting `Permission denied` on Downloaded Scripts or Binaries
If executable bits were stripped on download or your working directory is mounted with `noexec`:

```bash
# Normalize CRLF line endings (if downloaded on Windows) and grant execute permissions
sed -i 's/\r$//' norminette_v18.sh norminette_v18.py
chmod +x norminette_v18.sh norminette_v18.py test_runner

# If your filesystem is mounted noexec, run test_runner from /tmp
cp test_runner /tmp/test_runner && chmod +x /tmp/test_runner && /tmp/test_runner
```

---

## 42 Norm v1.8 Validation

### Option A: Pure Bash + `awk` Checker
```bash
chmod +x norminette_v18.sh
./norminette_v18.sh *.c *.h
```
*(Or invoke directly with `bash norminette_v18.sh *.c *.h`.)*

### Option B: Python 3 C-Tokenizer Checker
```bash
chmod +x norminette_v18.py
python3 norminette_v18.py *.c *.h
```

### Full CI Check (Norminette + Build + Tests)
```bash
bash norminette_v18.sh *.c *.h && make re && gcc -Wall -Wextra -Werror -std=c11 main.c test_scan.c -L. -lstring_m -o test_runner && ./test_runner
```

---

## Linking Against Your Own Project

Include `"string_m.h"` in your C source files and link `libstring_m.a`:

```bash
gcc -Wall -Wextra -Werror -std=c11 your_app.c -I. -L. -lstring_m -o your_app
```

### Minimal Example

```c
#include "string_m.h"
#include <locale.h>

int	main(void)
{
	t_string_mx	*greeting;
	t_string_mx	*fmt;
	int			count;

	setlocale(LC_ALL, "");
	greeting = NULL;
	fmt = NULL;
	count = 0;
	strcreate_m(&greeting, "42 Network", 64, NULL);
	strcreate_m(&fmt, "Hello, %s!\n", 64, NULL);
	printf_m(fmt, &count, greeting);
	strdelete_m(&greeting);
	strdelete_m(&fmt);
	return (0);
}
```






qemu-system-x86_64 -kernel /boot/vmlinuz -initrd initramfs.cpio.gz -nographic -append "console=ttyS0 quiet"
