/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_util_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:46:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:17:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	isnull_m(const t_string_mx *s, _Bool *nullstr)
{
	if (!s || !nullstr)
		return (sm_raise_constraint("isnull_m: null arg", NULL, EINVAL));
	*nullstr = s->is_null;
	return (0);
}

t_errno	isempty_m(const t_string_mx *s, _Bool *emptystr)
{
	if (!s || !emptystr)
		return (sm_raise_constraint("isempty_m: null arg", NULL, EINVAL));
	*emptystr = (!s->is_null && s->len == 0);
	return (0);
}

t_errno	isntbs_m(const t_string_mx *s, _Bool *ntbstr)
{
	if (!s || !ntbstr)
		return (sm_raise_constraint("isntbs_m: null arg", NULL, EINVAL));
	*ntbstr = !s->is_wide;
	return (0);
}

t_errno	iswide_m(const t_string_mx *s, _Bool *widestr)
{
	if (!s || !widestr)
		return (sm_raise_constraint("iswide_m: null arg", NULL, EINVAL));
	*widestr = s->is_wide;
	return (0);
}

t_errno	strlen_m(const t_string_mx *s, t_rsize *size)
{
	if (!s || !size)
		return (sm_raise_constraint("strlen_m: null arg", NULL, EINVAL));
	if (s->is_null)
		*size = 0;
	else
		*size = s->len;
	return (0);
}
