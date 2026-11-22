/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_printf_api.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:49:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:47:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	vsprintf_m(t_string_mx *buf, const t_string_mx *fmt,
			int *count, va_list args)
{
	const wchar_t	*wfmt;
	wchar_t			*wout;
	t_errno			err;

	if (!buf || !fmt || buf->is_const || fmt->is_null)
		return (sm_raise_constraint("vsprintf_m: invalid arg", buf, EINVAL));
	wfmt = NULL;
	wout = NULL;
	if (wgetstr_m(fmt, &wfmt) != 0)
		return (sm_raise_constraint("vsprintf_m: conv err", buf, EILSEQ));
	err = sm_fmt_engine(wfmt, args, &wout, count);
	sm_free((void *)wfmt);
	if (err != 0)
		return (sm_raise_constraint("vsprintf_m: fmt err", buf, err));
	err = wstrcpy_m(buf, wout);
	sm_free(wout);
	return (err);
}

t_errno	sprintf_m(t_string_mx *buf, const t_string_mx *fmt, int *count, ...)
{
	va_list	args;
	t_errno	err;

	if (!buf || !fmt)
		return (sm_raise_constraint("sprintf_m: null arg", buf, EINVAL));
	va_start(args, count);
	err = vsprintf_m(buf, fmt, count, args);
	va_end(args);
	return (err);
}

t_errno	vfprintf_m(FILE *file, const t_string_mx *fmt,
			int *count, va_list args)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!file || !fmt || fmt->is_null)
		return (sm_raise_constraint("vfprintf_m: invalid arg", NULL, EINVAL));
	if ((fwide(file, 0) > 0 && !fmt->is_wide)
		|| (fwide(file, 0) < 0 && fmt->is_wide))
		return (sm_raise_constraint("vfprintf_m: orientation", NULL, EIO));
	tmp = NULL;
	if (wstrcreate_m(&tmp, L"", 0, NULL) != 0)
		return (ENOMEM);
	err = vsprintf_m(tmp, fmt, count, args);
	if (err == 0 && fputws(tmp->data.wstr, file) < 0)
		err = EIO;
	strdelete_m(&tmp);
	return (err);
}

t_errno	fprintf_m(FILE *file, const t_string_mx *fmt, int *count, ...)
{
	va_list	args;
	t_errno	err;

	if (!file || !fmt)
		return (sm_raise_constraint("fprintf_m: null arg", NULL, EINVAL));
	va_start(args, count);
	err = vfprintf_m(file, fmt, count, args);
	va_end(args);
	return (err);
}

t_errno	printf_m(const t_string_mx *fmt, int *count, ...)
{
	va_list	args;
	t_errno	err;

	if (!fmt)
		return (sm_raise_constraint("printf_m: fmt is NULL", NULL, EINVAL));
	va_start(args, count);
	err = vfprintf_m(stdout, fmt, count, args);
	va_end(args);
	return (err);
}
