/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_copy.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:46:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 13:18:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	sm_copy_wide_n(t_string_mx *s1, const t_string_mx *s2, t_rsize n)
{
	const wchar_t	*src;
	wchar_t			*buf;
	size_t			clen;

	if (sm_promote_to_wide(s1) != 0)
		return (sm_raise_constraint("strncpy_m: promo err", s1, EILSEQ));
	src = NULL;
	if (wgetstr_m(s2, &src) != 0)
		return (sm_raise_constraint("strncpy_m: conv err", s1, EILSEQ));
	clen = wcslen(src);
	if (n < clen)
		clen = n;
	if (clen > s1->maxlen || !sm_validate_wset_n(src, clen, s1->charset.wstr))
		return (sm_free((void *)src),
			sm_raise_constraint("strncpy_m: limit/set", s1, EINVAL));
	buf = sm_malloc((clen + 1) * sizeof(wchar_t));
	if (!buf)
		return (sm_free((void *)src), ENOMEM);
	wmemcpy(buf, src, clen);
	buf[clen] = L'\0';
	sm_free((void *)src);
	sm_free(s1->data.wstr);
	s1->data.wstr = buf;
	s1->is_null = false;
	return (s1->len = clen, 0);
}

t_errno	strncpy_m(t_string_mx *s1, const t_string_mx *s2, t_rsize n)
{
	size_t	clen;

	if (!s1 || !s2 || s1->is_const || n > RSIZE_MAX)
		return (sm_raise_constraint("strncpy_m: invalid arg", s1, EINVAL));
	if (s2->is_null)
	{
		sm_free(s1->data.cstr);
		s1->data.cstr = NULL;
		s1->is_null = true;
		return (s1->len = 0, 0);
	}
	if (s1->is_wide || s2->is_wide)
		return (sm_copy_wide_n(s1, s2, n));
	clen = s2->len;
	if (n < clen)
		clen = n;
	if (clen > s1->maxlen
		|| !sm_validate_cset_n(s2->data.cstr, clen, s1->charset.cstr))
		return (sm_raise_constraint("strncpy_m: limit/set", s1, EINVAL));
	sm_free(s1->data.cstr);
	s1->data.cstr = strndup(s2->data.cstr, clen);
	s1->is_null = false;
	return (s1->len = clen, 0);
}

t_errno	strcpy_m(t_string_mx *s1, const t_string_mx *s2)
{
	if (!s1 || !s2)
		return (sm_raise_constraint("strcpy_m: null arg", s1, EINVAL));
	return (strncpy_m(s1, s2, RSIZE_MAX));
}

t_errno	cstrcpy_m(t_string_mx *s1, const char *cstr)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!s1 || s1->is_const)
		return (sm_raise_constraint("cstrcpy_m: invalid dst", s1, EINVAL));
	tmp = NULL;
	err = strcreate_m(&tmp, cstr, 0, NULL);
	if (err != 0)
		return (err);
	err = strcpy_m(s1, tmp);
	strdelete_m(&tmp);
	return (err);
}

t_errno	wstrcpy_m(t_string_mx *s1, const wchar_t *wcstr)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!s1 || s1->is_const)
		return (sm_raise_constraint("wstrcpy_m: invalid dst", s1, EINVAL));
	tmp = NULL;
	err = wstrcreate_m(&tmp, wcstr, 0, NULL);
	if (err != 0)
		return (err);
	err = strcpy_m(s1, tmp);
	strdelete_m(&tmp);
	return (err);
}
