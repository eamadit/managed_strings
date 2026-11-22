


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
