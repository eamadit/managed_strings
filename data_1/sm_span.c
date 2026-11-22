/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_span.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:48:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:19:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

static t_errno	sm_span_wide(const t_string_mx *str, const t_string_mx *set,
					t_rsize *len, _Bool accept)
{
	const wchar_t	*ws;
	const wchar_t	*wset;

	ws = NULL;
	wset = NULL;
	if (wgetstr_m(str, &ws) != 0)
		return (sm_raise_constraint("span_m: conv err", NULL, EILSEQ));
	if (wgetstr_m(set, &wset) != 0)
	{
		sm_free((void *)ws);
		return (sm_raise_constraint("span_m: conv err", NULL, EILSEQ));
	}
	if (accept)
		*len = wcsspn(ws, wset);
	else
		*len = wcscspn(ws, wset);
	sm_free((void *)ws);
	sm_free((void *)wset);
	return (0);
}

t_errno	sm_span_common(const t_string_mx *str, const t_string_mx *set,
			t_rsize *len, _Bool accept)
{
	if (!str || !set || !len)
		return (sm_raise_constraint("span_m: null arg", NULL, EINVAL));
	if (str->is_null || str->len == 0)
		*len = 0;
	else if (set->is_null || set->len == 0)
	{
		*len = 0;
		if (!accept)
			*len = str->len;
	}
	else if (str->is_wide || set->is_wide)
		return (sm_span_wide(str, set, len, accept));
	else if (accept)
		*len = strspn(str->data.cstr, set->data.cstr);
	else
		*len = strcspn(str->data.cstr, set->data.cstr);
	return (0);
}

t_errno	strspn_m(t_string_mx *str, t_string_mx *accept, t_rsize *len)
{
	return (sm_span_common(str, accept, len, true));
}

t_errno	cstrspn_m(t_string_mx *str, const char *accept, t_rsize *len)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!str || !accept || !len)
		return (sm_raise_constraint("cstrspn_m: null arg", NULL, EINVAL));
	tmp = NULL;
	err = strcreate_m(&tmp, accept, 0, NULL);
	if (err != 0)
		return (err);
	err = sm_span_common(str, tmp, len, true);
	strdelete_m(&tmp);
	return (err);
}

t_errno	wstrspn_m(t_string_mx *str, const wchar_t *accept, t_rsize *len)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!str || !accept || !len)
		return (sm_raise_constraint("wstrspn_m: null arg", NULL, EINVAL));
	tmp = NULL;
	err = wstrcreate_m(&tmp, accept, 0, NULL);
	if (err != 0)
		return (err);
	err = sm_span_common(str, tmp, len, true);
	strdelete_m(&tmp);
	return (err);
}
