# managed_strings


Specifications for Managed Strings, Second Edition:


https://apps.dtic.mil/sti/tr/pdf/ADA528573.pdf


Implementation attempt of Specifications for Managed Strings, Second Edition:


https://mantellapical.blogspot.com/2026/10/you-can-have-bootable-proof-of-concept.html


stricks libraries:


https://github.com/alcover/stricks


https://github.com/ILMTitan/stricks


https://github.com/EwoutH/stricks




# libstring_m — CMU CERT Managed Strings in 42 Coding Norm (`musl` + QEMU PID 1)

A complete C11 implementation of the **CMU/SEI CERT Managed Strings specification** (`CMU/SEI-2010-TR-018` / `ESC-TR-2010-018`, *Specifications for Managed Strings, Second Edition*), engineered under **La norme de 42 (v1.8)**, statically linked against **musl libc**, and validated both via a standalone unit test suite and as **PID 1 (`/sbin/init`)** inside a bootable Linux `initramfs` in **QEMU**.

---

## Why PID 1?

In UNIX and Linux systems, **PID 1 (`/sbin/init`)** is the ancestor of every user-space process. Unlike a standard user application that terminates with a `Segmentation fault` on a buffer overflow, an unhandled memory violation or stack exhaustion in PID 1 causes an immediate **`Kernel panic - not syncing: Attempted to kill init!`**, halting the entire operating system.

Running an interactive shell (`msh`) and security test suite directly inside PID 1 demonstrates deterministic memory safety, runtime constraint handling, and constant `O(1)` stack usage in an environment where any crash halts the kernel.

---

## Core Architecture & Standards

* **Specification:** CMU/SEI CERT Managed Strings (`CMU/SEI-2010-TR-018` / `ESC-TR-2010-018`)
  * Opaque, bounds-checked managed string objects (`t_string_mx`) tracking `len`, `maxlen`, `is_null`, `is_const`, `is_wide`, and character allow-lists (`charset`).
  * Automatic byte-to-wide (`char *` → `wchar_t *`) UTF-8 promotion.
  * Pluggable runtime-constraint handler (`set_constraint_handler_m`) that intercepts violations without heap allocation or terminating PID 1.
* **Zero-Recursion Security Model:** All string creation, slicing (`strslice_m`), tokenization (`strtok_m`), formatting (`sprintf_m`, `fprintf_m`, `printf_m`), and scanning (`sscanf_m`, `fscanf_m`, `scanf_m`) routines are strictly iterative (`while`-only), guaranteeing `O(1)` call-stack depth against deeply nested path or shell expression bombs.
* **Coding Standard:** 42 School Coding Norm v1.8
  * Standard 80-column 42 file headers and `<= 80`-column line width (4-space tab stops).
  * `<= 5` functions per `.c` file and `<= 25` lines per function body.
  * `<= 4` parameters per function and `<= 5` uninitialized variable declarations at the top of each block.
  * No `for`, `do...while`, `switch`, `case`, `goto`, or C++ `//` comments.
  * Wildcard-free `Makefile` with `$(NAME)`, `all`, `clean`, `fclean`, and `re` rules—validated by both `norminette_v18.sh` and `norminette_v18.py`.
* **C Runtime:** `musl libc` (`1.2.6`), producing a 100% self-contained static binary (`-static`) with zero `glibc` dependencies.
* **Boot Environment:** Linux Kernel + custom `cpio` `initramfs` running in `qemu-system-x86_64`.

---

## How `libstring_m.a` and `musl` Work Together

When compiled with `musl-gcc -static`, the linker bundles your application (`init`), your managed-string archive (`libstring_m.a`), and `musl`'s standard C library (`libc.a`) into a single self-contained binary with zero shared library (`/lib/*.so`) dependencies:

1. **Application Layer (`init.c` / `msh` / Graph DB):** Calls only safe managed-string APIs (`strcreate_m`, `cstrcat_m`, `strtok_m`) and never touches raw `<string.h>` functions directly.
2. **Security Guardrails (`libstring_m.a`):** Validates `maxlen` bounds, character allow-lists (`charset`), and `is_const` immutability before any memory operation occurs, invoking `pid1_fault_handler` on violations.
3. **C Runtime & Syscall Bridge (`musl libc`):** Provides `mallocng` heap allocation (`malloc`/`free`), built-in UTF-8 conversion (`mbstowcs`/`wcstombs` without external locale files), and raw Linux kernel syscalls (`read`, `write`, `open`, `mount`).
4. **Linux Kernel (QEMU):** Executes system calls issued by `musl` inside the minimal RAM disk.

---

## How the Test Pipeline Works (`run /test2.msh`)

The test runs as a three-stage pipeline: `test2.msh` feeds commands into `msh` (**PID 1**), `msh` dispatches them to compiled C functions in `init.c` and `msh_extra_tests.c`, and `libstring_m.a` intercepts any unsafe operation via `pid1_fault_handler` before it can crash the OS.

### 1. Boot & Safety Net Setup (`main()` in `init.c`)
* When QEMU boots the Linux kernel with `initramfs.cpio.gz`, the kernel mounts the RAM disk as `/` and executes `/init` as **PID 1**.
* The first line of `main()` registers `set_constraint_handler_m(pid1_fault_handler)`. This overrides the default `abort()` behavior with a non-allocating handler that writes `[SHIELD BLOCKED] <reason>` to `stderr` (`fd 2`) and returns control safely instead of terminating PID 1.

### 2. Script Reading (`msh_run_script()` in `init.c`)
* Typing `run /test2.msh` prompts `msh_exec_line()` to open `/test2.msh` inside the RAM disk via `open(path, O_RDONLY)`.
* It reads the file one byte at a time into a 512-byte buffer (`line`) until a newline (`\n`) is reached, passing each line to `msh_exec_line(cmd, line)`.

### 3. Command Execution & Exploit Interception (`[1/9]` to `[9/9]`)
Each line in `test2.msh` triggers one of three execution paths in C:

* **`[1/9]` Command Dispatch & Shell State:** `cstrcpy_m(cmd, raw)` copies the input into the 256-byte managed string `cmd`, while `cstrcmp_m` and `cstrncmp_m` safely match commands (`status`, `help`, `echo ...`) and print output.
* **`[2/9]` 257-Byte Buffer Overflow Survival:** `test2.msh` sends a 257-character `echo AAAA...AB` line. `cstrcpy_m(cmd, raw)` detects `262 > 256` (`cmd->maxlen`), refuses to copy a single byte, triggers `pid1_fault_handler` (`[SHIELD BLOCKED] strncpy_m: limit/set`), and returns an error so `msh_exec_line` skips the line cleanly.
* **`[3/9]` Charset Injection Shield:** `strcreate_m` restricts `env` to `A-Z_`. When `cstrcat_m(env, ";rm -rf /")` executes, `sm_validate_cset_n` detects that `;` is outside the allow-list and blocks the append.
* **`[4/9]` Immutability Shield:** `const_strcreate_m` sets `ver->is_const = true`, immediately rejecting `cstrcpy_m` overwrite attempts on `ver` (`KERNEL_LOCKED`).
* **`[5/9]` Path Slicing & UTF-8 Promotion:** `strtok_m` splits `"usr/local/bin"` on `"/"` iteratively, and `wstrcat_m` promotes `env` from `char *` to wide `wchar_t *`.
* **`[6/9]` PID 1 Stack & Heap Integrity:** Confirms PID 1 memory remains uncorrupted after consecutive fault triggers.
* **`[7/9]` 5,000-Level Nested Path Bomb:** Constructs a 10,000-byte path (`"a/a/.../a"`) and counts all 5,000 levels in a single `while` loop using `cchar_m` (`O(1)` stack depth, zero recursion).
* **`[8/9]` 200-Level Nested Shell Bomb:** Scans 200 nested `$($(...` tokens in a flat `while` loop and halts immediately when `depth > 16`.
* **`[9/9]` Graph DB Schema Lock & Cypher Injection Shield:** Locks the node label `"Person"` with `const_strcreate_m`, restricts property `prop` (`"alice"`) to `[a-z0-9_]`, blocks `'}) DETACH DELETE n //`, and confirms `strlen_m` still reports `5` in `O(1)` time.

---

## Exercised API Functions (`run /test2.msh`)

**12 public functions** (plus 4 internal managed-string functions invoked under the hood) are exercised during `run /test2.msh`:

1. **`set_constraint_handler_m`** — Registers `pid1_fault_handler` at boot so constraint violations log `[SHIELD BLOCKED]` instead of crashing PID 1.
2. **`strcreate_m`** — Allocates managed strings with strict `maxlen` bounds and `charset` allow-lists (`cmd`, `env`, `tok`, `rem`, `path`, `expr`, `prop`).
3. **`const_strcreate_m`** — Creates immutable strings (`ver` in `[4/9]` and `label` in `[9/9]`) to block memory overwrites.
4. **`strdelete_m`** — Frees managed string structs and their internal buffers (`cmd`, `path`, `expr`, `label`, `prop`).
5. **`strlen_m`** — Retrieves string length in `O(1)` time without scanning for `\0` (`[7/9]`, `[8/9]`, `[9/9]`).
6. **`cstrcpy_m`** — Safely copies C strings into `cmd` and tests immutability protection on `ver` (`[1/9]`, `[2/9]`, `[4/9]`).
7. **`cstrcat_m`** — Safely appends C strings while enforcing `maxlen` and `charset` allow-lists (`[3/9]`, `[7/9]`, `[8/9]`, `[9/9]`).
8. **`wstrcat_m`** — Promotes a narrow `char *` string to a wide UTF-8 `wchar_t *` string (`[5/9]`).
9. **`cstrcmp_m`** — Compares commands (`help`, `status`, `audit`, `extra`) safely (`[1/9]`).
10. **`cstrncmp_m`** — Matches command prefixes (`echo `, `run `) safely (`[1/9]`, `[2/9]`).
11. **`strtok_m`** — Splits path segments iteratively (`[5/9]`).
12. **`cchar_m`** — Reads individual characters by index with bounds checking (`[7/9]` and `[8/9]`).





---

## Comparison & Synergy with Stricks (`stx_t`)

**[Stricks](https://github.com)** (`stx_t`) is an open-source C dynamic string library modeled after Redis's Simple Dynamic Strings (SDS). Instead of wrapping a string inside an external `struct`, Stricks allocates a single contiguous memory block where a hidden prefix header (`capacity`, `length`, flags) sits immediately *before* the `char *` pointer returned to the caller.

### Architectural Comparison

* **Memory Layout & Pointer Compatibility:**
  * **`Stricks` (`stx_t`):** Returns a standard `char *` pointing directly to the character data (`s[-1]` accesses the hidden prefix header), allowing direct pass-through to standard C functions (`printf`, `open`) without getters.
  * **`libstring_m` (`t_string_mx *`):** Uses an opaque struct (`s->data.cstr` / `s->data.wstr`), requiring `cgetstr_m()` / `wgetstr_m()` to extract a raw pointer and preventing accidental mutation through raw C pointers.
* **Security Policy vs. Raw Throughput:**
  * **`Stricks`:** Optimized for maximum throughput and binary-safe buffers, auto-reallocating by default without `charset` allow-lists, `is_const` immutability locks, wide UTF-8 promotion, or CERT runtime constraint handlers.
  * **`libstring_m`:** Optimized for **high-assurance security** (hard `maxlen` caps, `charset` allow-lists, `const_strcreate_m` locks, and non-aborting PID 1 constraint handlers).

### How Stricks Can Complement This Stack

1. **Graph Database Storage Engine (`graphd`):** Use **`libstring_m`** at the untrusted query boundary (parsing incoming Cypher/DSL queries, enforcing `charset` allow-lists, and locking schema labels with `const_strcreate_m`), while adopting a **Stricks-style contiguous prefix-header layout** inside in-memory B+Tree / LSM storage pages where millions of validated properties are serialized—eliminating extra pointer dereferences and saving heap metadata overhead per property.
2. **Single-Allocation Optimization in `libstring_m`:** Borrowing Stricks' contiguous allocation layout (`malloc(sizeof(t_string_mx) + len + 1)`) for initial non-promoted narrow strings in `sm_util_create.c` cuts heap allocations per `strcreate_m` in half (1 `malloc` instead of 2) while preserving full `t_string_mx` security checks.

---

## Prerequisites & Local `musl` Setup

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

## Build, Validation & QEMU Boot Instructions

### 1. Validate 42 Coding Norm v1.8 Compliance

```bash
bash norminette_v18.sh *.c *.h
python3 norminette_v18.py *.c *.h
```

### 2. Build Static Library & Run Host Unit Tests

```bash
make re && gcc -Wall -Wextra -Werror -std=c11 main.c test_scan.c -L. -lstring_m -o test_runner && ./test_runner
```

Expected output:
```text
All 42-Norm Managed Strings tests passed!
```

### 3. Rebuild with `musl-gcc` & Pack `initramfs.cpio.gz`

```bash
make re CC=/path/to/musl-gcc

/path/to/musl-gcc -Wall -Wextra -Werror -std=c11 -static \
  init.c msh_extra_tests.c -L. -lstring_m -o init

mkdir -p rootfs/proc rootfs/sys rootfs/dev
cp init rootfs/init
(cd rootfs && find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz)
```

### 4. Boot in QEMU & Run the Security Suite

```bash
qemu-system-x86_64 \
  -kernel /boot/vmlinuz \
  -initrd initramfs.cpio.gz \
  -nographic \
  -append "console=ttyS0 quiet"
```

*(Note: On distributions where `/boot/vmlinuz` is readable only by root, run with `sudo` or copy the kernel image to a user-readable path. Press `Ctrl+A` followed by `X` to exit QEMU.)*

Once the `msh# ` prompt appears inside QEMU:

```text
run /test2.msh
```

### Troubleshooting `Permission denied` on Scripts or Binaries

If executable bits were stripped on download or your working directory is mounted with `noexec`:

```bash
sed -i 's/\r$//' norminette_v18.sh norminette_v18.py
chmod +x norminette_v18.sh norminette_v18.py test_runner

# If your filesystem is mounted noexec, run test_runner from /tmp
cp test_runner /tmp/test_runner && chmod +x /tmp/test_runner && /tmp/test_runner
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

---

## Repository Structure

```text
.
├── Makefile              # Static library build configuration (libstring_m.a)
├── string_m.h            # Public API header (CMU/SEI-2010-TR-018 / ESC-TR-2010-018)
├── string_m_internal.h   # Internal structures and hidden helper prototypes
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
├── init.c                # Static PID 1 (/sbin/init) and bounded msh shell
├── msh_extra_tests.c     # Deep path traversal, nested shell bomb, and Graph DB injection tests
├── rootfs/test2.msh      # Automated 9-step runtime security audit script
├── main.c                # Core unit test suite
├── test_scan.c           # Scanner, tokenizer, and charset unit tests
├── norminette_v18.sh     # Standalone Bash + awk Norminette v1.8 checker
└── norminette_v18.py     # Python 3 C-tokenizer Norminette v1.8 checker
```


> **Note:** That totals **12 public functions**—11 from Sections 3.1–3.9 of `CMU/SEI-2010-TR-018` plus `set_constraint_handler_m` from Section 2.2—which also automatically invoke `strncpy_m`, `strncat_m`, `strslice_m`, and `wstrcreate_m` internally.
