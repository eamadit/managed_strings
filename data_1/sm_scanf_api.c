/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_scanf_api.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:50:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:20:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	vsscanf_m(t_string_mx *buf, const t_string_mx *fmt,
			int *count, va_list args)
{
	const wchar_t	*wbuf;
	const wchar_t	*wfmt;
	t_scan_src		src;
	t_errno			err;

	if (!buf || !fmt || buf->is_null || fmt->is_null)
		return (sm_raise_constraint("vsscanf_m: invalid arg", NULL, EINVAL));
	wbuf = NULL;
	wfmt = NULL;
	if (wgetstr_m(buf, &wbuf) != 0 || wgetstr_m(fmt, &wfmt) != 0)
	{
		sm_free((void *)wbuf);
		return (sm_raise_constraint("vsscanf_m: conv err", NULL, EILSEQ));
	}
	src.file = NULL;
	src.buf = wbuf;
	src.pos = 0;
	src.is_wide = buf->is_wide;
	err = sm_scan_engine(&src, wfmt, count, args);
	sm_free((void *)wbuf);
	sm_free((void *)wfmt);
	return (err);
}

t_errno	sscanf_m(t_string_mx *buf, const t_string_mx *fmt, int *count, ...)
{
	va_list	args;
	t_errno	err;

	if (!buf || !fmt)
		return (sm_raise_constraint("sscanf_m: null arg", NULL, EINVAL));
	va_start(args, count);
	err = vsscanf_m(buf, fmt, count, args);
	va_end(args);
	return (err);
}

t_errno	vfscanf_m(FILE *file, const t_string_mx *fmt,
			int *count, va_list args)
{
	const wchar_t	*wfmt;
	t_scan_src		src;
	t_errno			err;

	if (!file || !fmt || fmt->is_null)
		return (sm_raise_constraint("vfscanf_m: invalid arg", NULL, EINVAL));
	if ((fwide(file, 0) > 0 && !fmt->is_wide)
		|| (fwide(file, 0) < 0 && fmt->is_wide))
		return (sm_raise_constraint("vfscanf_m: orientation", NULL, EIO));
	wfmt = NULL;
	if (wgetstr_m(fmt, &wfmt) != 0)
		return (sm_raise_constraint("vfscanf_m: conv err", NULL, EILSEQ));
	src.file = file;
	src.buf = NULL;
	src.pos = 0;
	src.is_wide = (fwide(file, 0) > 0);
	err = sm_scan_engine(&src, wfmt, count, args);
	sm_free((void *)wfmt);
	return (err);
}

t_errno	fscanf_m(FILE *file, const t_string_mx *fmt, int *count, ...)
{
	va_list	args;
	t_errno	err;

	if (!file || !fmt)
		return (sm_raise_constraint("fscanf_m: null arg", NULL, EINVAL));
	va_start(args, count);
	err = vfscanf_m(file, fmt, count, args);
	va_end(args);
	return (err);
}

t_errno	scanf_m(const t_string_mx *fmt, int *count, ...)
{
	va_list	args;
	t_errno	err;

	if (!fmt)
		return (sm_raise_constraint("scanf_m: fmt is NULL", NULL, EINVAL));
	va_start(args, count);
	err = vfscanf_m(stdin, fmt, count, args);
	va_end(args);
	return (err);
}
