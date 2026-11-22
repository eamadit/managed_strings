/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_char.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:48:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:23:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	cchar_m(const t_string_mx *s, t_rsize offset, char *c)
{
	int	ch;

	if (!s || !c || s->is_null || offset >= s->len)
		return (sm_raise_constraint("cchar_m: invalid arg", NULL, EINVAL));
	if (!s->is_wide)
	{
		*c = s->data.cstr[offset];
		return (0);
	}
	ch = wctob(s->data.wstr[offset]);
	if (ch == EOF)
		return (sm_raise_constraint("cchar_m: unrepresentable", NULL, EILSEQ));
	*c = (char)ch;
	return (0);
}

t_errno	wchar_m(const t_string_mx *s, t_rsize offset, wchar_t *wc)
{
	const wchar_t	*wstr;

	if (!s || !wc || s->is_null || offset >= s->len)
		return (sm_raise_constraint("wchar_m: invalid arg", NULL, EINVAL));
	if (s->is_wide)
	{
		*wc = s->data.wstr[offset];
		return (0);
	}
	wstr = NULL;
	if (wgetstr_m(s, &wstr) != 0)
		return (sm_raise_constraint("wchar_m: conv err", NULL, EILSEQ));
	if (offset >= wcslen(wstr))
	{
		sm_free((void *)wstr);
		return (sm_raise_constraint("wchar_m: out of bounds", NULL, EINVAL));
	}
	*wc = wstr[offset];
	sm_free((void *)wstr);
	return (0);
}
