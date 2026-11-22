python3 norminette_v18.py *.c *.h

bash norminette_v18.sh *.c *.h

make re && gcc -Wall -Wextra -Werror -std=c11 main.c test_scan.c -L. -lstring_m -o test_runner && ./test_runner
