/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_config.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:48:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:23:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

static t_errno	sm_set_wcharset(t_string_mx *s, const t_string_mx *charset)
{
	const wchar_t	*wset;

	if (sm_promote_to_wide(s) != 0)
		return (sm_raise_constraint("setcharset_m: promo err", s, EILSEQ));
	wset = NULL;
	if (wgetstr_m(charset, &wset) != 0)
		return (sm_raise_constraint("setcharset_m: conv err", s, EILSEQ));
	if (!s->is_null && !sm_validate_wset_n(s->data.wstr, s->len, wset))
	{
		sm_free((void *)wset);
		return (sm_raise_constraint("setcharset_m: invalid chars", s, EINVAL));
	}
	sm_free(s->charset.wstr);
	s->charset.wstr = (wchar_t *)wset;
	return (0);
}

t_errno	setcharset_m(t_string_mx *s, const t_string_mx *charset)
{
	char	*cset;

	if (!s || s->is_const)
		return (sm_raise_constraint("setcharset_m: invalid dst", s, EINVAL));
	if (!charset || charset->is_null)
	{
		sm_free(s->charset.cstr);
		s->charset.cstr = NULL;
		return (0);
	}
	if (s->is_wide || charset->is_wide)
		return (sm_set_wcharset(s, charset));
	cset = strdup(charset->data.cstr);
	if (!cset)
		return (sm_raise_constraint("setcharset_m: OOM", s, ENOMEM));
	if (!s->is_null && !sm_validate_cset_n(s->data.cstr, s->len, cset))
	{
		sm_free(cset);
		return (sm_raise_constraint("setcharset_m: invalid chars", s, EINVAL));
	}
	sm_free(s->charset.cstr);
	s->charset.cstr = cset;
	return (0);
}

t_errno	setmaxlen_m(t_string_mx *s, t_rsize maxlen)
{
	size_t	emax;

	if (!s || s->is_const)
		return (sm_raise_constraint("setmaxlen_m: invalid dst", s, EINVAL));
	emax = maxlen;
	if (maxlen == 0)
		emax = RSIZE_MAX;
	if (emax > RSIZE_MAX || (!s->is_null && s->len > emax))
		return (sm_raise_constraint("setmaxlen_m: invalid max", s, ERANGE));
	s->maxlen = emax;
	return (0);
}
