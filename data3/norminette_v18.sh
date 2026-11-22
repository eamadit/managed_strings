#!/usr/bin/env bash
if [ "$#" -eq 0 ]; then
    echo "Usage: $0 <file.c | file.h> ..."
    exit 1
fi
STATUS=0
for file in "$@"; do
    if [ ! -f "$file" ]; then
        echo "Error: $file not found"
        STATUS=1
        continue
    fi
    awk -v fname="$file" '
    BEGIN {
        errors = 0; func_count = 0; in_func = 0; brace_depth = 0
        func_lines = 0; decl_phase = 0; var_count = 0; saw_blank = 0
        prev_code = ""
        types_re = "^[ \t]*(const[ \t]+)?(unsigned[ \t]+|signed[ \t]+)?" \
                   "(void|char|short|int|long|float|double|size_t|ssize_t|" \
                   "wchar_t|wint_t|_Bool|bool|FILE|va_list|t_[a-z0-9_]+|" \
                   "struct[ \t]+s_[a-z0-9_]+|union[ \t]+u_[a-z0-9_]+)[ \t*]+"
    }
    function col_width(s,    i, c, w) {
        w = 0
        for (i = 1; i <= length(s); i++) {
            c = substr(s, i, 1)
            if (c == "\t")
                w += 4 - (w % 4)
            else
                w++
        }
        return w
    }
    function err(line, msg) {
        printf("%s: Error (line %d): %s\n", fname, line, msg)
        errors++
    }
    {
        raw = $0
        if (NR == 1 && (length(raw) != 80 || substr(raw, 1, 4) != "/* *" || substr(raw, 77, 4) != "* */"))
            err(NR, "Missing or invalid 42 header")
        if (raw ~ /[ \t]+$/)
            err(NR, "Trailing whitespace or whitespace on empty line")
        w = col_width(raw)
        if (w > 80)
            err(NR, "Line exceeds 80 columns (" w ")")
        if (raw ~ /[/][/]/)
            err(NR, "C++ style // comment is forbidden")
        code = raw
        gsub(/[/][*].*[*][/]/, "", code)
        if (code ~ /(^|[^a-zA-Z0-9_])(for|do|switch|case|goto)([^a-zA-Z0-9_]|$)/)
            err(NR, "Forbidden keyword (for, do, switch, case, or goto)")
        if (code ~ /(^|[^a-zA-Z0-9_])(if|while|return)[(]/)
            err(NR, "Missing space after keyword (if, while, return)")
        if (code ~ /[,;][^ \t\n]/)
            err(NR, "Missing space after comma or semicolon")
        if (brace_depth == 0 && code ~ /[a-zA-Z0-9_]+[ \t]*[(][^{;]*[)][ \t]*$/ && code !~ /^[ \t]*#/) {
            func_count++
            if (func_count > 5 && fname ~ /[.]c$/)
                err(NR, "More than 5 functions in a .c file")
            params = code
            sub(/^.*[(]/, "", params); sub(/[)].*$/, "", params)
            if (gsub(/,/, ",", params) >= 4)
                err(NR, "Function has more than 4 parameters")
        }
        if (in_func == 1) {
            if (code !~ /^[ \t]*[{][ \t]*$/ && code !~ /^[ \t]*[}][ \t]*$/)
                func_lines++
            if (brace_depth == 1 && code !~ /^[ \t]*[{][ \t]*$/ && code !~ /^[ \t]*[}][ \t]*$/) {
                is_decl = (code ~ types_re && code !~ /[(]/)
                if (decl_phase == 1) {
                    if (code ~ /^$/) {
                        if (var_count > 0) saw_blank = 1
                    } else if (is_decl) {
                        if (saw_blank == 1)
                            err(NR, "Empty line inside variable declarations")
                        var_count++
                        if (var_count > 5)
                            err(NR, "More than 5 variables in function block")
                        if (code ~ /=/)
                            err(NR, "Variable initialization on declaration line")
                    } else {
                        if (var_count > 0 && saw_blank == 0)
                            err(NR, "Missing empty line after variable declarations")
                        decl_phase = 0
                    }
                } else if (is_decl) {
                    err(NR, "Variable declaration after implementation started")
                }
            }
        }
        opens = gsub(/[{]/, "{", code)
        closes = gsub(/[}]/, "}", code)
        if (brace_depth == 0 && opens > 0 && prev_code !~ /^[ \t]*(typedef[ \t]+)?(struct|union|enum)[ \t]+/) {
            in_func = 1; func_lines = 0; decl_phase = 1
            var_count = 0; saw_blank = 0
        }
        brace_depth += (opens - closes)
        if (brace_depth <= 0) {
            if (in_func == 1 && func_lines > 25)
                err(NR, "Function exceeds 25 lines (" func_lines " lines)")
            in_func = 0; brace_depth = 0; decl_phase = 0
        }
        if (code !~ /^[ \t]*$/)
            prev_code = code
    }
    END {
        if (errors == 0) printf("%s: OK!\n", fname)
        else exit 1
    }' "$file" || STATUS=1
done
exit $STATUS
