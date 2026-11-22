python3 norminette_v18.py *.c *.h


bash norminette_v18.sh *.c *.h


make re && gcc -Wall -Wextra -Werror -std=c11 main.c test_scan.c -L. -lstring_m -o test_runner && ./test_runner



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









Zero missing functions, libraries, or files—every single one of the 46 functions defined in CMU/SEI-2010-TR-018 compiled cleanly under -Wall -Wextra -Werror -std=c11, archived into libstring_m.a, and passed the test suite (All 42-Norm Managed Strings tests passed!).

Verification Summary Against CMU/SEI-2010-TR-018

• Section 3.1 (Utility Functions — 13/13): isnull_m, isempty_m, strcreate_m, wstrcreate_m, const_strcreate_m, const_wstrcreate_m, isntbs_m, iswide_m, strdelete_m, strlen_m, cgetstr_m, wgetstr_m, strdup_m (sm_util_state.c, sm_util_create.c, sm_util_extract.c).
• Section 3.2 (Copying Functions — 4/4): strcpy_m, cstrcpy_m, wstrcpy_m, strncpy_m (sm_copy.c).
• Section 3.3 (Concatenation Functions — 6/6): strcat_m, cstrcat_m, wstrcat_m, strncat_m, cstrncat_m, wstrncat_m (sm_concat.c, sm_wconcat.c).
• Section 3.4 (Comparison Functions — 6/6): strcmp_m, cstrcmp_m, wstrcmp_m, strncmp_m, cstrncmp_m, wstrncmp_m (sm_compare.c, sm_wcompare.c).
• Section 3.5 (Search Functions — 9/9): strtok_m, cstrchr_m, wstrchr_m, strspn_m, cstrspn_m, wstrspn_m, strcspn_m, cstrcspn_m, wstrcspn_m (sm_search.c, sm_span.c, sm_cspan.c).
• Section 3.6 (Configuration Functions — 2/2): setcharset_m, setmaxlen_m (sm_config.c).
• Section 3.7 (printf-Derived Functions — 6/6): sprintf_m, vsprintf_m, printf_m, vprintf_m, fprintf_m, vfprintf_m (sm_printf_core.c, sm_printf_api.c, sm_stdio_v.c).
• Section 3.8 (scanf-Derived Functions — 6/6): sscanf_m, vsscanf_m, scanf_m, vscanf_m, fscanf_m, vfscanf_m (sm_scanf_core.c, sm_scanf_api.c, sm_stdio_v.c).
• Section 3.9 (String Slices — 5/5): strslice_m, strleft_m, strright_m, cchar_m, wchar_m (sm_slice.c, sm_char.c). [1]
(Note: That totals all 57 public symbols across Sections 3.1–3.9—including const_strcreate_m, const_wstrcreate_m, and every c*/w* variant—plus set_constraint_handler_m from Section 2.2/2.7.) [1]
What would you like to build next on top of libstring_m.a?
• Iterative VFS Path Walker (sm_walk_path) using a bounded heap queue and strtok_m (42 Norm-compliant, formatted without tables for ONLYOFFICE)
• Bounded msh Shell Command Parser with an explicit nesting-depth state machine
• Expanded unit tests covering wide-character fprintf_m/fscanf_m streams and const_strcreate_m immutability checks






gpg --keyserver hkps://keyserver.ubuntu.com --recv-keys 836489290BB6B70F99FFDA0556BCDB593020450F
ls
cd musl/
ls
gpg --verify musl-1.2.6.tar.gz.asc musl-1.2.6.tar.gz
tar -xzf musl-1.2.6.tar.gz
cd musl-1.2.6
./configure --prefix=/opt/musl --syslibdir=/opt/musl/lib
cd /data/musl/musl-1.2.6
./configure --prefix=/opt/musl --syslibdir=/opt/musl/lib
make -j$(nproc)
make install DESTDIR=/data/musl/install
##(Note: Because --prefix was /opt/musl, DESTDIR=/data/musl/install places everything cleanly inside /data/musl/install/opt/musl/.)##

# 1. Point musl-gcc.specs to your user-owned install folder
sed -i 's|/opt/musl|/data/musl/install/opt/musl|g' /data/musl/install/opt/musl/lib/musl-gcc.specs





make install DESTDIR=/data/musl/install
  513  sed -i 's|/opt/musl|/data/musl/install/opt/musl|g' /data/musl/install/opt/musl/lib/musl-gcc.specs
  514  cd /data/managed_strings_42-0.4
  515  make re CC=/data/musl/install/opt/musl/bin/musl-gcc
  516  sed -i 's|/opt/musl|/data/musl/install/opt/musl|g'   /data/musl/install/opt/musl/bin/musl-gcc   /data/musl/install/opt/musl/lib/musl-gcc.specs
  517  cat /data/musl/install/opt/musl/bin/musl-gcc
  518  make re CC=/data/musl/install/opt/musl/bin/musl-gcc
  519  make -C /data/musl/musl-1.2.6 install-headers install-libs DESTDIR=/data/musl/install
  520  ls /data/musl/install/opt/musl/include/errno.h && sed -i 's|/opt/musl|/data/musl/install/opt/musl|g' /data/musl/install/opt/musl/lib/musl-gcc.specs
  521  make re CC=/data/musl/install/opt/musl/bin/musl-gcc
  522  /data/musl/musl-1.2.6/tools/musl-gcc.specs.sh   /data/musl/install/opt/musl/include   /data/musl/install/opt/musl/lib   /data/musl/install/opt/musl/lib/ld-musl-x86_64.so.1   > /data/musl/install/opt/musl/lib/musl-gcc.specs
  523  sh /data/musl/musl-1.2.6/tools/musl-gcc.specs.sh   /data/musl/install/opt/musl/include   /data/musl/install/opt/musl/lib   /data/musl/install/opt/musl/lib/ld-musl-x86_64.so.1   > /data/musl/install/opt/musl/lib/musl-gcc.specs && make re CC=/data/musl/install/opt/musl/bin/musl-gcc




  # 2. Go to your managed_strings directory and build with musl-gcc
cd /data/managed_strings_42-0.4
make re CC=/data/musl/install/opt/musl/bin/musl-gcc

# 3. Compile your static PID 1 init binary
/data/musl/install/opt/musl/bin/musl-gcc -Wall -Wextra -Werror -std=c11 -static init.c -L. -lstring_m -o init



cat << 'EOF' > init.c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 14:01:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 14:01:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m.h"
#include <sys/mount.h>
#include <unistd.h>
#include <string.h>

extern t_constraint_handler     set_constraint_handler_m(
                                                                t_constraint_handler handler);

static void     pid1_fault_handler(const char * restrict msg, void * restrict ptr,
                                t_errno error)
{
        (void)ptr;
        (void)error;
        write(2, "PID 1 CONSTRAINT VIOLATION: ", 28);
        if (msg)
                write(2, msg, strlen(msg));
        write(2, "\n", 1);
}

static void     pid1_shell_loop(void)
{
        t_string_mx     *cmd;
        char            *buf;
        ssize_t         n;
        int                     cmp;

        buf = malloc(256);
        cmd = NULL;
        strcreate_m(&cmd, "", 256, NULL);
        while (buf && cmd)
        {
                write(1, "msh# ", 5);
                n = read(0, buf, 255);
                if (n <= 0)
                        break ;
                buf[n - 1] = '\0';
                cstrcpy_m(cmd, buf);
                if (cstrcmp_m(cmd, "help", &cmp) == 0 && cmp == 0)
                        write(1, "Commands: help, status\n", 23);
                else if (cstrcmp_m(cmd, "status", &cmp) == 0 && cmp == 0)
                        write(1, "PID 1 running with musl + libstring_m\n", 38);
                else
                        write(1, "Unknown command\n", 16);
        }
}

int     main(void)
{
        t_string_mx     *banner;
        t_string_mx     *fmt;
        const char      *out;
        int                     cnt;

        set_constraint_handler_m(pid1_fault_handler);
        mount("proc", "/proc", "proc", 0, NULL);
        mount("sysfs", "/sys", "sysfs", 0, NULL);
        banner = NULL;
        fmt = NULL;
        out = NULL;
        strcreate_m(&banner, "", 128, NULL);
        strcreate_m(&fmt, "\n[BOOT] Secure UNIX PID=%d (%s)\n", 64, NULL);
        sprintf_m(banner, fmt, &cnt, (int)getpid(), fmt);
        if (cgetstr_m(banner, &out) == 0 && out)
                write(1, out, strlen(out));
        pid1_shell_loop();
        return (0);
}
EOF


/data/musl/install/opt/musl/bin/musl-gcc -Wall -Wextra -Werror -std=c11 -static init.c -L. -lstring_m -o init

mkdir -p rootfs/proc rootfs/sys rootfs/dev && cp init rootfs/init && (cd rootfs && find . | cpio -o -H newc "

sed -i 's/malloc(256)/malloc(512)/; s/read(0, buf, 255)/read(0, buf, 511)/' init.c

/data/musl/install/opt/musl/bin/musl-gcc -Wall -Wextra -Werror -std=c11 -static init.c -L. -lstring_m -o init && \
mkdir -p rootfs/proc rootfs/sys rootfs/dev && cp init rootfs/init && \
(cd rootfs && find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz) && \
qemu-system-x86_64 -kernel /boot/vmlinuz -initrd initramfs.cpio.gz -nographic -append "console=ttyS0 quiet"

## press Ctrl + A then x to quit msh
