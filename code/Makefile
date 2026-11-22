# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: student <student@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/11/21 16:50:00 by student           #+#    #+#              #
#    Updated: 2026/11/22 11:37:00 by student          ###   ########.fr       #
#                                                                              #
# **************************************************************************** #

NAME	= libstring_m.a
CC		= gcc
CFLAGS	= -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L
AR		= ar rcs
RM		= rm -f

SRCS	= sm_core.c \
		  sm_convert.c \
		  sm_util_state.c \
		  sm_util_create.c \
		  sm_util_extract.c \
		  sm_copy.c \
		  sm_concat.c \
		  sm_wconcat.c \
		  sm_compare.c \
		  sm_wcompare.c \
		  sm_search.c \
		  sm_span.c \
		  sm_cspan.c \
		  sm_config.c \
		  sm_slice.c \
		  sm_char.c \
		  sm_printf_core.c \
		  sm_printf_api.c \
		  sm_scanf_core.c \
		  sm_scanf_api.c \
		  sm_stdio_v.c

OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)

%.o: %.c string_m.h string_m_internal.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
