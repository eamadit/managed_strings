/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_printf_core.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:49:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:20:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"
#include <wctype.h>

static t_errno	sm_append_wstr_arg(t_string_mx *acc, const t_string_mx *ms,
					wchar_t mod)
{
	const char		*cstr;
	const wchar_t	*wstr;
	t_errno			err;

	if (!ms)
		return (EINVAL);
	if (ms->is_null)
		return (wstrcat_m(acc, L"(null)"));
	if (mod == L'h')
	{
		cstr = NULL;
		err = cgetstr_m(ms, &cstr);
		if (err == 0)
			err = cstrcat_m(acc, cstr);
		sm_free((void *)cstr);
		return (err);
	}
	wstr = NULL;
	err = wgetstr_m(ms, &wstr);
	if (err == 0)
		err = wstrcat_m(acc, wstr);
	sm_free((void *)wstr);
	return (err);
}

static t_errno	sm_append_scalar(t_string_mx *acc, const wchar_t *sub,
					wchar_t conv, va_list args)
{
	wchar_t	tmp[128];
	int		ret;

	ret = -1;
	if (wcschr(L"diouxXc", conv))
		ret = swprintf(tmp, 128, sub, va_arg(args, int));
	else if (wcschr(L"fFeEgGaA", conv))
		ret = swprintf(tmp, 128, sub, va_arg(args, double));
	else if (conv == L'p')
		ret = swprintf(tmp, 128, sub, va_arg(args, void *));
	if (ret < 0)
		return (EINVAL);
	return (wstrcat_m(acc, tmp));
}

static t_errno	sm_parse_spec(t_string_mx *acc, const wchar_t **p,
					va_list args)
{
	const wchar_t	*start;
	wchar_t			sub[64];
	wchar_t			mod;

	start = (*p)++;
	if (**p == L'%')
	{
		(*p)++;
		return (wstrcat_m(acc, L"%"));
	}
	while (**p && wcschr(L"-+ #0.*0123456789", **p))
		(*p)++;
	mod = 0;
	if (**p == L'h' || **p == L'l' || **p == L'z' || **p == L'L')
		mod = *(*p)++;
	if (**p == L'\0' || **p == L'n' || (size_t)(*p - start) >= 62)
		return (EINVAL);
	(*p)++;
	if (*(*p - 1) == L's')
		return (sm_append_wstr_arg(acc,
				va_arg(args, const t_string_mx *), mod));
	wmemcpy(sub, start, (size_t)(*p - start));
	sub[(size_t)(*p - start)] = L'\0';
	return (sm_append_scalar(acc, sub, *(*p - 1), args));
}

static t_errno	sm_append_lit(t_string_mx *acc, const wchar_t **p)
{
	wchar_t	ch[2];

	ch[0] = **p;
	ch[1] = L'\0';
	(*p)++;
	return (wstrcat_m(acc, ch));
}

t_errno	sm_fmt_engine(const wchar_t *wfmt, va_list args,
			wchar_t **out, int *cnt)
{
	t_string_mx		*acc;
	const wchar_t	*p;
	t_errno			err;

	acc = NULL;
	if (wstrcreate_m(&acc, L"", 0, NULL) != 0)
		return (ENOMEM);
	p = wfmt;
	err = 0;
	while (*p != L'\0' && err == 0)
	{
		if (*p == L'%')
			err = sm_parse_spec(acc, &p, args);
		else
			err = sm_append_lit(acc, &p);
	}
	if (err == 0 && cnt)
		*cnt = (int)acc->len;
	if (err == 0)
		err = wgetstr_m(acc, (const wchar_t **)out);
	strdelete_m(&acc);
	return (err);
}
