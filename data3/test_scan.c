/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_scan.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:51:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:21:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m.h"
#include <assert.h>

static void	test_sscanf_managed(void)
{
	t_string_mx	*src;
	t_string_mx	*fmt;
	t_string_mx	*user;
	int			cnt;
	int			id;

	src = NULL;
	fmt = NULL;
	user = NULL;
	cnt = 0;
	id = 0;
	assert(strcreate_m(&src, "bob 42", 32, NULL) == 0);
	assert(strcreate_m(&fmt, "%s %d", 16, NULL) == 0);
	assert(strcreate_m(&user, NULL, 32, NULL) == 0);
	assert(sscanf_m(src, fmt, &cnt, user, &id) == 0);
	assert(cnt == 2 && id == 42);
	strdelete_m(&src);
	strdelete_m(&fmt);
	strdelete_m(&user);
}

static void	test_strtok_and_charset(void)
{
	t_string_mx	*line;
	t_string_mx	*tok;
	t_string_mx	*rem;
	t_string_mx	*delim;
	t_string_mx	*alpha;

	line = NULL;
	tok = NULL;
	rem = NULL;
	delim = NULL;
	alpha = NULL;
	assert(strcreate_m(&line, "alpha:beta", 32, NULL) == 0);
	assert(strcreate_m(&tok, NULL, 32, NULL) == 0);
	assert(strcreate_m(&rem, NULL, 32, NULL) == 0);
	assert(strcreate_m(&delim, ":", 4, NULL) == 0);
	assert(strcreate_m(&alpha, "abehltp", 16, NULL) == 0);
	assert(strtok_m(tok, line, delim, rem) == 0);
	assert(setcharset_m(tok, alpha) == 0);
	assert(setcharset_m(rem, alpha) == 0);
	strdelete_m(&line);
	strdelete_m(&tok);
	strdelete_m(&rem);
	strdelete_m(&delim);
	strdelete_m(&alpha);
}

void	test_scan_and_tokens(void)
{
	test_sscanf_managed();
	test_strtok_and_charset();
}
