/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_slice.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:48:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:23:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

static t_errno	sm_slice_copy(t_string_mx *s1, const t_string_mx *s2,
					t_rsize offset, t_rsize clen)
{
	t_string_mx	*tmp;
	t_errno		err;

	tmp = NULL;
	if (s2->is_wide)
		err = wstrcreate_m(&tmp, s2->data.wstr + offset, 0, NULL);
	else
		err = strcreate_m(&tmp, s2->data.cstr + offset, 0, NULL);
	if (err != 0)
		return (err);
	err = strncpy_m(s1, tmp, clen);
	strdelete_m(&tmp);
	return (err);
}

t_errno	strslice_m(t_string_mx *s1, const t_string_mx *s2,
			t_rsize offset, t_rsize len)
{
	t_rsize	clen;

	if (!s1 || !s2 || s1->is_const || offset > RSIZE_MAX || len > RSIZE_MAX)
		return (sm_raise_constraint("strslice_m: invalid arg", s1, EINVAL));
	if (s2->is_null || offset > s2->len)
		return (cstrcpy_m(s1, NULL));
	if (offset == s2->len || len == 0)
	{
		if (s2->is_wide)
			return (wstrcpy_m(s1, L""));
		return (cstrcpy_m(s1, ""));
	}
	clen = s2->len - offset;
	if (len < clen)
		clen = len;
	return (sm_slice_copy(s1, s2, offset, clen));
}

t_errno	strleft_m(t_string_mx *s1, const t_string_mx *s2, t_rsize len)
{
	if (!s1 || !s2)
		return (sm_raise_constraint("strleft_m: null arg", s1, EINVAL));
	return (strslice_m(s1, s2, 0, len));
}

t_errno	strright_m(t_string_mx *s1, const t_string_mx *s2, t_rsize len)
{
	t_rsize	offset;

	if (!s1 || !s2 || len > RSIZE_MAX)
		return (sm_raise_constraint("strright_m: invalid arg", s1, EINVAL));
	if (s2->is_null)
		return (cstrcpy_m(s1, NULL));
	offset = 0;
	if (len < s2->len)
		offset = s2->len - len;
	return (strslice_m(s1, s2, offset, len));
}
