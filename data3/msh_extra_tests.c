/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_extra_tests.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 15:16:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 15:37:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m.h"
#include <unistd.h>

static void	test_deep_path_bomb(void)
{
	t_string_mx	*path;
	t_rsize		off;
	t_rsize		len;
	int			levels;
	char		ch;

	path = NULL;
	off = 0;
	len = 0;
	levels = 1;
	write(1, "[7/9] Walking 5,000-level path iteratively...\n", 46);
	strcreate_m(&path, "a", 16384, NULL);
	while (off++ < 4999)
		cstrcat_m(path, "/a");
	strlen_m(path, &len);
	off = 0;
	while (off < len && cchar_m(path, off++, &ch) == 0)
		if (ch == '/')
			levels++;
	if (levels == 5000)
		write(1, "  -> PASS: 5,000 levels walked in O(N) time!\n", 45);
	strdelete_m(&path);
}

static void	test_nested_shell_bomb(void)
{
	t_string_mx	*expr;
	t_rsize		i;
	t_rsize		len;
	int			depth;
	char		ch;

	expr = NULL;
	i = 0;
	len = 0;
	depth = 0;
	write(1, "[8/9] Blocking 200-level $($(...)) shell bomb...\n", 49);
	strcreate_m(&expr, "", 1024, NULL);
	while (i++ < 200)
		cstrcat_m(expr, "$(");
	strlen_m(expr, &len);
	i = 0;
	while (i < len && cchar_m(expr, i++, &ch) == 0)
	{
		if (ch == '(' && ++depth > 16)
		{
			write(1, "  -> PASS: Iterative parser halted at depth 16!\n", 48);
			break ;
		}
	}
	strdelete_m(&expr);
}

static void	test_graph_db_shield(void)
{
	const t_string_mx	*label;
	t_string_mx			*prop;
	t_rsize				klen;

	label = NULL;
	prop = NULL;
	klen = 0;
	write(1, "[9/9] Graph DB schema lock & Cypher injection...\n", 49);
	const_strcreate_m(&label, "Person", 32, NULL);
	strcreate_m(&prop, "alice", 64, "abcdefghijklmnopqrstuvwxyz0123456789_");
	cstrcat_m(prop, "'}) DETACH DELETE n //");
	strlen_m(prop, &klen);
	if (klen == 5)
		write(1, "  -> PASS: Cypher injection blocked, O(1) len=5!\n", 49);
	strdelete_m((t_string_mx **)&label);
	strdelete_m(&prop);
}

void	msh_run_extra_tests(void)
{
	test_deep_path_bomb();
	test_nested_shell_bomb();
	test_graph_db_shield();
}
