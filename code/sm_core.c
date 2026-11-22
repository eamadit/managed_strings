/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_core.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:42:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:40:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

static void	sm_default_abort(const char * restrict msg, void * restrict ptr,
				t_errno error)
{
	(void)ptr;
	fprintf(stderr, "Constraint error (%d): %s\n", error, msg);
	abort();
}

static t_constraint_handler	g_handler = sm_default_abort;

t_constraint_handler	set_constraint_handler_m(t_constraint_handler handler)
{
	t_constraint_handler	prev;

	prev = g_handler;
	if (handler == NULL)
		g_handler = sm_default_abort;
	else
		g_handler = handler;
	return (prev);
}

t_errno	sm_raise_constraint(const char *msg, void *ptr, t_errno err)
{
	errno = err;
	if (g_handler != NULL)
		g_handler(msg, ptr, err);
	return (err);
}

void	*sm_malloc(size_t size)
{
	return (malloc(size));
}

void	sm_free(void *ptr)
{
	free(ptr);
}
