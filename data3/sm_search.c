/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_search.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:48:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:19:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

static t_errno	sm_tok_split(t_string_mx *token, t_string_mx *str,
					t_string_mx *ptr, t_rsize idx)
{
	t_errno	err;

	err = strslice_m(token, str, 0, idx);
	if (err != 0)
		return (err);
	return (strslice_m(ptr, str, idx + 1, RSIZE_MAX));
}

t_errno	strtok_m(t_string_mx *token, t_string_mx *str,
			const t_string_mx *delim, t_string_mx *ptr)
{
	t_rsize	idx;

	if (!token || !str || !delim || !ptr || token->is_const || ptr->is_const)
		return (sm_raise_constraint("strtok_m: invalid arg", NULL, EINVAL));
	if (str->is_null)
	{
		if (cstrcpy_m(token, NULL) != 0)
			return (EINVAL);
		return (cstrcpy_m(ptr, NULL));
	}
	if (delim->is_null || delim->len == 0)
	{
		if (strcpy_m(token, str) != 0)
			return (EINVAL);
		return (cstrcpy_m(ptr, NULL));
	}
	idx = 0;
	if (strcspn_m(str, (t_string_mx *)delim, &idx) != 0)
		return (EINVAL);
	if (idx >= str->len && strcpy_m(token, str) == 0)
		return (cstrcpy_m(ptr, NULL));
	return (sm_tok_split(token, str, ptr, idx));
}

t_errno	cstrchr_m(const t_string_mx *str, char c, t_rsize *index)
{
	const char		*found;
	const wchar_t	*wfound;

	if (!str || !index)
		return (sm_raise_constraint("cstrchr_m: null arg", NULL, EINVAL));
	*index = ~(t_rsize)0;
	if (str->is_null)
		return (0);
	if (!str->is_wide)
	{
		found = strchr(str->data.cstr, (unsigned char)c);
		if (found)
			*index = (t_rsize)(found - str->data.cstr);
		return (0);
	}
	if (btowc((unsigned char)c) == WEOF)
		return (0);
	wfound = wcschr(str->data.wstr, btowc((unsigned char)c));
	if (wfound)
		*index = (t_rsize)(wfound - str->data.wstr);
	return (0);
}

t_errno	wstrchr_m(const t_string_mx *str, wchar_t wc, t_rsize *index)
{
	const wchar_t	*wstr;
	const wchar_t	*found;

	if (!str || !index)
		return (sm_raise_constraint("wstrchr_m: null arg", NULL, EINVAL));
	*index = ~(t_rsize)0;
	if (str->is_null)
		return (0);
	wstr = NULL;
	if (wgetstr_m(str, &wstr) != 0)
		return (sm_raise_constraint("wstrchr_m: conv err", NULL, EILSEQ));
	found = wcschr(wstr, wc);
	if (found)
		*index = (t_rsize)(found - wstr);
	sm_free((void *)wstr);
	return (0);
}
