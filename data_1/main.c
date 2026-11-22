/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:50:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:29:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m.h"
#include <assert.h>
#include <locale.h>
#include <string.h>

void		test_scan_and_tokens(void);

static int	g_violations = 0;

static void	test_ignore_handler(const char * restrict msg,
				void * restrict ptr, t_errno error)
{
	(void)msg;
	(void)ptr;
	(void)error;
	g_violations++;
}

static void	test_charset_and_bounds(void)
{
	t_string_mx	*digits;
	t_rsize		len;

	digits = NULL;
	len = 0;
	assert(strcreate_m(&digits, "12345", 10, "0123456789") == 0);
	assert(cstrcat_m(digits, "9a") != 0);
	assert(cstrcat_m(digits, "678901") != 0);
	assert(g_violations == 2);
	assert(cstrcat_m(digits, "67890") == 0);
	assert(strlen_m(digits, &len) == 0 && len == 10);
	assert(strdelete_m(&digits) == 0 && digits == NULL);
}

static void	test_wide_and_slices(void)
{
	t_string_mx	*s;
	t_string_mx	*left;
	t_string_mx	*right;
	int			cmp;

	s = NULL;
	left = NULL;
	right = NULL;
	cmp = 1;
	assert(strcreate_m(&s, "Hello, ", 64, NULL) == 0);
	assert(wstrcat_m(s, L"World!") == 0);
	assert(wstrcreate_m(&left, NULL, 64, NULL) == 0);
	assert(wstrcreate_m(&right, NULL, 64, NULL) == 0);
	assert(strleft_m(left, s, 5) == 0);
	assert(strright_m(right, s, 6) == 0);
	assert(wstrcmp_m(left, L"Hello", &cmp) == 0 && cmp == 0);
	assert(wstrcmp_m(right, L"World!", &cmp) == 0 && cmp == 0);
	strdelete_m(&s);
	strdelete_m(&left);
	strdelete_m(&right);
}

static void	test_formatted_io(void)
{
	t_string_mx	*out;
	t_string_mx	*fmt;
	t_string_mx	*name;
	const char	*raw;
	int			cnt;

	out = NULL;
	fmt = NULL;
	name = NULL;
	raw = NULL;
	cnt = 0;
	assert(strcreate_m(&out, "", 128, NULL) == 0);
	assert(strcreate_m(&fmt, "User=%s Score=%d", 64, NULL) == 0);
	assert(strcreate_m(&name, "Alice", 32, NULL) == 0);
	assert(sprintf_m(out, fmt, &cnt, name, 42) == 0);
	assert(cgetstr_m(out, &raw) == 0);
	assert(strcmp(raw, "User=Alice Score=42") == 0);
	free((void *)raw);
	strdelete_m(&out);
	strdelete_m(&fmt);
	strdelete_m(&name);
}

int	main(void)
{
	setlocale(LC_ALL, "");
	set_constraint_handler_m(test_ignore_handler);
	test_charset_and_bounds();
	test_wide_and_slices();
	test_formatted_io();
	test_scan_and_tokens();
	printf("All 42-Norm Managed Strings tests passed!\n");
	return (0);
}
