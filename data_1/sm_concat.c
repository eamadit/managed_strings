/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_concat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:46:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:29:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	sm_cat_wide_n(t_string_mx *s1, const t_string_mx *s2, t_rsize n)
{
	const wchar_t	*src;
	wchar_t			*buf;
	size_t			alen;
	size_t			clen;

	src = NULL;
	if (sm_promote_to_wide(s1) != 0 || wgetstr_m(s2, &src) != 0)
		return (sm_raise_constraint("strncat_m: conv err", s1, EILSEQ));
	alen = wcslen(src);
	if (n < alen)
		alen = n;
	clen = s1->len * (!s1->is_null);
	if (alen > s1->maxlen - clen
		|| !sm_validate_wset_n(src, alen, s1->charset.wstr))
		return (sm_free((void *)src),
			sm_raise_constraint("strncat_m: limit/set", s1, EINVAL));
	buf = sm_realloc(s1->data.wstr, (clen + alen + 1) * sizeof(wchar_t));
	wmemcpy(buf + clen, src, alen);
	buf[clen + alen] = L'\0';
	sm_free((void *)src);
	s1->data.wstr = buf;
	s1->is_null = false;
	s1->len = clen + alen;
	return (0);
}

t_errno	strncat_m(t_string_mx *s1, const t_string_mx *s2, t_rsize n)
{
	char	*buf;
	size_t	alen;
	size_t	clen;

	if (!s1 || s1->is_const || n > RSIZE_MAX)
		return (sm_raise_constraint("strncat_m: invalid arg", s1, EINVAL));
	if (!s2 || s2->is_null || n == 0)
		return (0);
	if (s1->is_wide || s2->is_wide)
		return (sm_cat_wide_n(s1, s2, n));
	alen = s2->len;
	if (n < alen)
		alen = n;
	clen = s1->len * (!s1->is_null);
	if (alen > s1->maxlen - clen
		|| !sm_validate_cset_n(s2->data.cstr, alen, s1->charset.cstr))
		return (sm_raise_constraint("strncat_m: limit/set", s1, EINVAL));
	buf = sm_realloc(s1->data.cstr, clen + alen + 1);
	memcpy(buf + clen, s2->data.cstr, alen);
	buf[clen + alen] = '\0';
	s1->data.cstr = buf;
	s1->is_null = false;
	s1->len = clen + alen;
	return (0);
}

t_errno	strcat_m(t_string_mx *s1, const t_string_mx *s2)
{
	if (!s1 || !s2)
		return (sm_raise_constraint("strcat_m: null arg", s1, EINVAL));
	return (strncat_m(s1, s2, RSIZE_MAX));
}

t_errno	cstrncat_m(t_string_mx *s, const char *cstr, t_rsize n)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!s || s->is_const)
		return (sm_raise_constraint("cstrncat_m: invalid dst", s, EINVAL));
	if (!cstr)
		return (0);
	tmp = NULL;
	err = strcreate_m(&tmp, cstr, 0, NULL);
	if (err != 0)
		return (err);
	err = strncat_m(s, tmp, n);
	strdelete_m(&tmp);
	return (err);
}

t_errno	cstrcat_m(t_string_mx *s, const char *cstr)
{
	return (cstrncat_m(s, cstr, RSIZE_MAX));
}
