/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_wcompare.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:47:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:19:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	cstrncmp_m(const t_string_mx *s1, const char *cstr,
			t_rsize n, int *cmp)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!s1 || !cmp || n > RSIZE_MAX)
		return (sm_raise_constraint("cstrncmp_m: invalid arg", NULL, EINVAL));
	tmp = NULL;
	err = strcreate_m(&tmp, cstr, 0, NULL);
	if (err != 0)
		return (err);
	err = strncmp_m(s1, tmp, n, cmp);
	strdelete_m(&tmp);
	return (err);
}

t_errno	wstrncmp_m(const t_string_mx *s1, const wchar_t *wstr,
			t_rsize n, int *cmp)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!s1 || !cmp || n > RSIZE_MAX)
		return (sm_raise_constraint("wstrncmp_m: invalid arg", NULL, EINVAL));
	tmp = NULL;
	err = wstrcreate_m(&tmp, wstr, 0, NULL);
	if (err != 0)
		return (err);
	err = strncmp_m(s1, tmp, n, cmp);
	strdelete_m(&tmp);
	return (err);
}

t_errno	cstrcmp_m(const t_string_mx *s1, const char *cstr, int *cmp)
{
	if (!s1 || !cmp)
		return (sm_raise_constraint("cstrcmp_m: null arg", NULL, EINVAL));
	return (cstrncmp_m(s1, cstr, RSIZE_MAX, cmp));
}

t_errno	wstrcmp_m(const t_string_mx *s1, const wchar_t *wstr, int *cmp)
{
	if (!s1 || !cmp)
		return (sm_raise_constraint("wstrcmp_m: null arg", NULL, EINVAL));
	return (wstrncmp_m(s1, wstr, RSIZE_MAX, cmp));
}
