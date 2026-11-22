/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_util_extract.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:46:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:18:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	const_wstrcreate_m(t_string_mx **str, const wchar_t *wcstr,
			const size_t maxsize, const wchar_t *charset)
{
	t_errno	err;

	err = wstrcreate_m(str, wcstr, maxsize, charset);
	if (err == 0 && str && *str)
		(*str)->is_const = true;
	return (err);
}

t_errno	cgetstr_m(const t_string_mx *s, const char **string)
{
	char	*buf;
	t_errno	err;

	if (!s || !string || *string != NULL)
	{
		if (string)
			*string = NULL;
		return (sm_raise_constraint("cgetstr_m: invalid arg", NULL, EINVAL));
	}
	if (s->is_null)
		return (0);
	if (!s->is_wide)
	{
		buf = strdup(s->data.cstr);
		if (!buf)
			return (sm_raise_constraint("cgetstr_m: OOM", NULL, ENOMEM));
		*string = buf;
		return (0);
	}
	buf = NULL;
	err = sm_wide_to_mb_dup(s->data.wstr, &buf, NULL);
	if (err != 0)
		return (sm_raise_constraint("cgetstr_m: error", NULL, err));
	*string = buf;
	return (0);
}

t_errno	wgetstr_m(const t_string_mx *s, const wchar_t **wcstr)
{
	wchar_t	*buf;
	t_errno	err;

	if (!s || !wcstr || *wcstr != NULL)
	{
		if (wcstr)
			*wcstr = NULL;
		return (sm_raise_constraint("wgetstr_m: invalid arg", NULL, EINVAL));
	}
	if (s->is_null)
		return (0);
	if (s->is_wide)
	{
		buf = wcsdup(s->data.wstr);
		if (!buf)
			return (sm_raise_constraint("wgetstr_m: OOM", NULL, ENOMEM));
		*wcstr = buf;
		return (0);
	}
	buf = NULL;
	err = sm_mb_to_wide_dup(s->data.cstr, &buf, NULL);
	if (err != 0)
		return (sm_raise_constraint("wgetstr_m: error", NULL, err));
	*wcstr = buf;
	return (0);
}

t_errno	strdup_m(t_string_mx **s1, const t_string_mx *s2)
{
	if (!s1 || !s2)
		return (sm_raise_constraint("strdup_m: null arg", NULL, EINVAL));
	if (s2->is_wide)
	{
		if (s2->is_null)
			return (wstrcreate_m(s1, NULL, s2->maxlen, s2->charset.wstr));
		return (wstrcreate_m(s1, s2->data.wstr, s2->maxlen, s2->charset.wstr));
	}
	if (s2->is_null)
		return (strcreate_m(s1, NULL, s2->maxlen, s2->charset.cstr));
	return (strcreate_m(s1, s2->data.cstr, s2->maxlen, s2->charset.cstr));
}
