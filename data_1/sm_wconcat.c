/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_wconcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:47:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:18:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	wstrncat_m(t_string_mx *s, const wchar_t *wcstr, t_rsize n)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!s || s->is_const)
		return (sm_raise_constraint("wstrncat_m: invalid dst", s, EINVAL));
	if (!wcstr)
		return (0);
	tmp = NULL;
	err = wstrcreate_m(&tmp, wcstr, 0, NULL);
	if (err != 0)
		return (err);
	err = strncat_m(s, tmp, n);
	strdelete_m(&tmp);
	return (err);
}

t_errno	wstrcat_m(t_string_mx *s, const wchar_t *wcstr)
{
	if (!s || !wcstr)
		return (sm_raise_constraint("wstrcat_m: null arg", s, EINVAL));
	return (wstrncat_m(s, wcstr, RSIZE_MAX));
}
