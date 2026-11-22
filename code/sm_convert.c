/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_convert.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:46:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:17:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

_Bool	sm_validate_cset_n(const char *str, size_t n, const char *cset)
{
	size_t	i;

	if (!str || !cset)
		return (true);
	i = 0;
	while (i < n)
	{
		if (!strchr(cset, (unsigned char)str[i]))
			return (false);
		i++;
	}
	return (true);
}

_Bool	sm_validate_wset_n(const wchar_t *wstr, size_t n, const wchar_t *wset)
{
	size_t	i;

	if (!wstr || !wset)
		return (true);
	i = 0;
	while (i < n)
	{
		if (!wcschr(wset, wstr[i]))
			return (false);
		i++;
	}
	return (true);
}

t_errno	sm_mb_to_wide_dup(const char *cstr, wchar_t **out, size_t *len)
{
	size_t	wlen;
	wchar_t	*buf;

	if (!cstr)
	{
		*out = NULL;
		if (len)
			*len = 0;
		return (0);
	}
	wlen = mbstowcs(NULL, cstr, 0);
	if (wlen == (size_t)-1)
		return (EILSEQ);
	buf = sm_malloc((wlen + 1) * sizeof(wchar_t));
	if (!buf)
		return (ENOMEM);
	mbstowcs(buf, cstr, wlen + 1);
	*out = buf;
	if (len)
		*len = wlen;
	return (0);
}

t_errno	sm_wide_to_mb_dup(const wchar_t *wstr, char **out, size_t *len)
{
	size_t	blen;
	char	*buf;

	if (!wstr)
	{
		*out = NULL;
		if (len)
			*len = 0;
		return (0);
	}
	blen = wcstombs(NULL, wstr, 0);
	if (blen == (size_t)-1)
		return (EILSEQ);
	buf = sm_malloc(blen + 1);
	if (!buf)
		return (ENOMEM);
	wcstombs(buf, wstr, blen + 1);
	*out = buf;
	if (len)
		*len = blen;
	return (0);
}

t_errno	sm_promote_to_wide(t_string_mx *s)
{
	wchar_t	*nw;
	wchar_t	*nset;
	size_t	nlen;

	if (!s || s->is_wide)
		return (s ? 0 : EINVAL);
	nw = NULL;
	nset = NULL;
	nlen = 0;
	if (!s->is_null && s->data.cstr
		&& (sm_mb_to_wide_dup(s->data.cstr, &nw, &nlen) != 0
			|| nlen > s->maxlen))
		return (sm_free(nw), ERANGE);
	if (s->charset.cstr
		&& sm_mb_to_wide_dup(s->charset.cstr, &nset, NULL) != 0)
		return (sm_free(nw), EILSEQ);
	sm_free(s->data.cstr);
	sm_free(s->charset.cstr);
	s->data.wstr = nw;
	s->charset.wstr = nset;
	s->len = nlen;
	s->is_wide = true;
	return (0);
}
