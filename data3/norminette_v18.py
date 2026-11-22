#!/usr/bin/env python3
import re
import sys
from dataclasses import dataclass
from pathlib import Path

TOKEN_SPEC = [
    ("ML_COMMENT", r"/\*[\s\S]*?\*/"),
    ("SL_COMMENT", r"//[^\n]*"),
    ("STRING",     r'L?"(?:\\.|[^"\\])*"'),
    ("CHAR",       r"L?'(?:\\.|[^'\\])*'"),
    ("PREPROC",    r"^[ \t]*#[^\n]*"),
    ("NEWLINE",    r"\n"),
    ("WS",         r"[ \t]+"),
    ("OP3",        r"<<=|>>=|\.\.\."),
    ("OP2",        r"->|\+\+|--|<<|>>|<=|>=|==|!=|&&|\|\||[+\-*/%&|^]=?"),
    ("OP1",        r"[=<>!~?:.,;()\[\]{}]"),
    ("IDENT",      r"[A-Za-z_][A-Za-z0-9_]*"),
    ("NUMBER",     r"0[xX][0-9a-fA-F]+[uUlL]*|[0-9]+(?:\.[0-9]+)?[fFuUlL]*"),
    ("OTHER",      r"."),
]
MASTER_RE = re.compile("|".join(f"(?P<{n}>{p})" for n, p in TOKEN_SPEC), re.M)

FORBIDDEN_KW = {"for", "do", "switch", "case", "goto"}
CTRL_KW = {"if", "while", "return"}
BIN_OPS = {
    "=", "+=", "-=", "*=", "/=", "%=", "==", "!=", "<", ">", "<=", ">=",
    "&&", "||", "+", "/", "%", "^", "|", "<<", ">>",
}
TYPES = {
    "void", "char", "short", "int", "long", "float", "double", "size_t",
    "ssize_t", "wchar_t", "wint_t", "_Bool", "bool", "FILE", "va_list",
    "unsigned", "signed", "const", "volatile", "register", "static", "extern",
}


@dataclass
class Token:
    kind: str
    val: str
    line: int
    col: int


def tokenize(text: str) -> list[Token]:
    tokens = []
    line = 1
    line_start = 0
    for m in MASTER_RE.finditer(text):
        kind = m.lastgroup
        val = m.group()
        col = m.start() - line_start + 1
        tokens.append(Token(kind, val, line, col))
        if "\n" in val:
            line += val.count("\n")
            line_start = m.end() - (len(val) - val.rfind("\n") - 1)
    return tokens


class NormChecker:
    def __init__(self, path: Path, text: str):
        self.path = path
        self.lines = text.splitlines()
        self.tokens = tokenize(text)
        self.errors: list[tuple[int, str]] = []

    def err(self, line: int, msg: str):
        self.errors.append((line, msg))

    def check_lines(self):
        if not self.lines or not re.match(r"^/\* \*{74} \*/$", self.lines[0]):
            self.err(1, "Missing or invalid 42 header")
        for idx, raw in enumerate(self.lines, 1):
            if raw.endswith((" ", "\t")):
                self.err(idx, "Trailing whitespace or non-empty blank line")
            expanded = raw.expandtabs(4)
            if len(expanded) > 80:
                self.err(idx, f"Line exceeds 80 columns ({len(expanded)})")

    def _check_func_params(self, sig: list[Token], brace_idx: int):
        j = brace_idx - 1
        if j < 0 or sig[j].val != ")":
            return
        paren_depth = 0
        commas = 0
        while j >= 0:
            if sig[j].val == ")":
                paren_depth += 1
            elif sig[j].val == "(":
                paren_depth -= 1
                if paren_depth == 0:
                    break
            elif sig[j].val == "," and paren_depth == 1:
                commas += 1
            j -= 1
        if commas >= 4:
            self.err(
                sig[brace_idx].line,
                f"Function has more than 4 parameters ({commas + 1})",
            )

    def _check_typedef(self, sig: list[Token], idx: int):
        j = idx + 1
        pdepth = 0
        bdepth = 0
        last_id = None
        while j < len(sig) and not (sig[j].val == ";" and pdepth == 0 and bdepth == 0):
            tok = sig[j]
            if tok.val == "{":
                bdepth += 1
            elif tok.val == "}":
                bdepth -= 1
            elif tok.val == "(":
                pdepth += 1
            elif tok.val == ")":
                pdepth -= 1
            elif tok.kind == "IDENT" and bdepth == 0 and pdepth == 0:
                last_id = tok
            elif tok.kind == "IDENT" and bdepth == 0 and pdepth == 1 and tok.val.startswith("t_"):
                last_id = tok
            j += 1
        if last_id and not last_id.val.startswith("t_"):
            self.err(last_id.line, f"Typedef '{last_id.val}' must start with 't_'")

    def check_tokens(self):
        sig = [t for t in self.tokens if t.kind not in ("WS", "NEWLINE")]
        depth = 0
        in_func = False
        func_start = 0
        func_count = 0
        decl_phase = False
        var_count = 0
        blank_seen = False

        for i, tok in enumerate(sig):
            prev_tok = sig[i - 1] if i > 0 else None
            next_tok = sig[i + 1] if i + 1 < len(sig) else None

            if tok.kind == "SL_COMMENT":
                self.err(tok.line, "C++ // comment is forbidden")
            elif tok.kind == "ML_COMMENT":
                if in_func:
                    self.err(tok.line, "Comment inside function body is forbidden")
                clines = tok.val.splitlines()
                for offset, cline in enumerate(clines[1:-1], 1):
                    if not cline.lstrip().startswith("**"):
                        self.err(tok.line + offset, "Multiline comment must start with **")
            elif tok.kind == "IDENT":
                if tok.val in FORBIDDEN_KW:
                    self.err(tok.line, f"Forbidden keyword '{tok.val}'")
                if tok.val in CTRL_KW and next_tok and next_tok.val == "(":
                    if next_tok.col == tok.col + len(tok.val):
                        self.err(tok.line, f"Missing space after '{tok.val}'")
                if tok.val == "return" and next_tok and next_tok.val not in (";", "("):
                    self.err(tok.line, "Return value must be enclosed in parentheses")
                if tok.val == "struct" and next_tok and not next_tok.val.startswith("s_"):
                    self.err(tok.line, f"Struct name '{next_tok.val}' must start with 's_'")
                if tok.val == "union" and next_tok and not next_tok.val.startswith("u_"):
                    self.err(tok.line, f"Union name '{next_tok.val}' must start with 'u_'")
                if tok.val == "enum" and next_tok and not next_tok.val.startswith("e_"):
                    self.err(tok.line, f"Enum name '{next_tok.val}' must start with 'e_'")
                if tok.val == "typedef":
                    self._check_typedef(sig, i)
            elif tok.val in (",", ";") and next_tok and next_tok.line == tok.line:
                if next_tok.col == tok.col + 1 and next_tok.val not in (";", ")"):
                    self.err(tok.line, f"Missing space after '{tok.val}'")
            elif tok.val in BIN_OPS and prev_tok and next_tok:
                if prev_tok.line == tok.line and prev_tok.col + len(prev_tok.val) == tok.col:
                    self.err(tok.line, f"Missing space before operator '{tok.val}'")
                if next_tok.line == tok.line and tok.col + len(tok.val) == next_tok.col:
                    self.err(tok.line, f"Missing space after operator '{tok.val}'")

            if tok.val == "{":
                if depth == 0 and prev_tok and prev_tok.val == ")":
                    in_func = True
                    func_start = tok.line
                    decl_phase = True
                    var_count = 0
                    blank_seen = False
                    func_count += 1
                    self._check_func_params(sig, i)
                    if self.path.suffix == ".c" and func_count > 5:
                        self.err(tok.line, "More than 5 functions in .c file")
                depth += 1
            elif tok.val == "}":
                depth -= 1
                if depth == 0 and in_func:
                    body_lines = tok.line - func_start - 1
                    if body_lines > 25:
                        self.err(func_start, f"Function body exceeds 25 lines ({body_lines})")
                    in_func = False
            elif in_func and depth == 1 and prev_tok and prev_tok.line < tok.line:
                if tok.line - prev_tok.line > 1 and var_count > 0:
                    blank_seen = True
                is_type = (
                    tok.val in TYPES
                    or tok.val.startswith(("t_", "s_", "u_", "e_"))
                    or tok.val in ("struct", "union", "enum")
                )
                if decl_phase and is_type:
                    if blank_seen:
                        self.err(tok.line, "Blank line inside variable declarations")
                    var_count += 1
                    if var_count > 5:
                        self.err(tok.line, "More than 5 variables in block")
                    j = i
                    while j < len(sig) and sig[j].val != ";":
                        if sig[j].val == "=":
                            self.err(tok.line, "Variable initialization on declaration line")
                        j += 1
                elif decl_phase and not is_type:
                    if var_count > 0 and not blank_seen:
                        self.err(tok.line, "Missing blank line after variable declarations")
                    decl_phase = False
                elif not decl_phase and is_type:
                    self.err(tok.line, "Variable declaration after implementation started")

    def run(self) -> bool:
        self.check_lines()
        self.check_tokens()
        for line, msg in sorted(set(self.errors)):
            print(f"{self.path}: Error (line {line}): {msg}")
        if not self.errors:
            print(f"{self.path}: OK!")
            return True
        return False


def main() -> int:
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <file.c | file.h> ...")
        return 1
    ok = True
    for arg in sys.argv[1:]:
        p = Path(arg)
        if not p.is_file():
            print(f"Error: {p} not found")
            ok = False
            continue
        if not NormChecker(p, p.read_text(encoding="utf-8")).run():
            ok = False
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
