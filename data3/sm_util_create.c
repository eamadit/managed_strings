/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_util_create.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:46:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:35:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

static t_string_mx	*sm_alloc_obj(size_t maxsize, _Bool is_wide,
						_Bool is_null)
{
	t_string_mx	*obj;
	size_t		emax;

	emax = maxsize;
	if (maxsize == 0)
		emax = RSIZE_MAX;
	if (emax > RSIZE_MAX)
		return (NULL);
	obj = sm_malloc(sizeof(t_string_mx));
	if (!obj)
		return (NULL);
	memset(obj, 0, sizeof(t_string_mx));
	obj->is_wide = is_wide;
	obj->is_null = is_null;
	obj->maxlen = emax;
	return (obj);
}

t_errno	strcreate_m(t_string_mx **s, const char *cstr, const size_t maxsize,
			const char *charset)
{
	t_string_mx	*obj;
	size_t		len;

	if (!s || (charset && *charset == '\0'))
		return (sm_raise_constraint("strcreate_m: invalid arg", NULL, EINVAL));
	len = 0;
	if (cstr)
		len = strlen(cstr);
	if ((cstr && !sm_validate_cset_n(cstr, len, charset))
		|| (maxsize != 0 && len > maxsize))
		return (sm_raise_constraint("strcreate_m: constraint", NULL, EINVAL));
	obj = sm_alloc_obj(maxsize, false, (cstr == NULL));
	if (!obj)
		return (sm_raise_constraint("strcreate_m: OOM", NULL, ENOMEM));
	if (charset)
		obj->charset.cstr = strdup(charset);
	if (cstr)
	{
		obj->len = len;
		obj->data.cstr = strdup(cstr);
	}
	*s = obj;
	return (0);
}

t_errno	wstrcreate_m(t_string_mx **s, const wchar_t *cstr,
			const size_t maxsize, const wchar_t *charset)
{
	t_string_mx	*obj;
	size_t		len;

	if (!s || (charset && *charset == L'\0'))
		return (sm_raise_constraint("wstrcreate_m: invalid arg", NULL, EINVAL));
	len = 0;
	if (cstr)
		len = wcslen(cstr);
	if ((cstr && !sm_validate_wset_n(cstr, len, charset))
		|| (maxsize != 0 && len > maxsize))
		return (sm_raise_constraint("wstrcreate_m: constraint", NULL, EINVAL));
	obj = sm_alloc_obj(maxsize, true, (cstr == NULL));
	if (!obj)
		return (sm_raise_constraint("wstrcreate_m: OOM", NULL, ENOMEM));
	if (charset)
		obj->charset.wstr = wcsdup(charset);
	if (cstr)
	{
		obj->len = len;
		obj->data.wstr = wcsdup(cstr);
	}
	*s = obj;
	return (0);
}

t_errno	const_strcreate_m(const t_string_mx **str, const char *cstr,
			const size_t maxsize, const char *charset)
{
	t_string_mx	*tmp;
	t_errno		err;

	tmp = NULL;
	err = strcreate_m(&tmp, cstr, maxsize, charset);
	if (err == 0 && tmp)
	{
		tmp->is_const = true;
		*str = tmp;
	}
	return (err);
}

t_errno	strdelete_m(t_string_mx **s)
{
	if (!s || !*s)
		return (sm_raise_constraint("strdelete_m: invalid ptr", NULL, EINVAL));
	if ((*s)->is_wide)
	{
		sm_free((*s)->data.wstr);
		sm_free((*s)->charset.wstr);
	}
	else
	{
		sm_free((*s)->data.cstr);
		sm_free((*s)->charset.cstr);
	}
	sm_free(*s);
	*s = NULL;
	return (0);
}
