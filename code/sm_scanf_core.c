/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_scanf_core.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:50:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 12:27:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"
#include <wctype.h>

static wint_t	sm_sgetc(t_scan_src *src)
{
	int	ch;

	if (src->file && src->is_wide)
		return (fgetwc(src->file));
	if (src->file)
	{
		ch = fgetc(src->file);
		if (ch == EOF)
			return (WEOF);
		return (btowc(ch));
	}
	if (!src->buf || src->buf[src->pos] == L'\0')
		return (WEOF);
	return (src->buf[src->pos++]);
}

static void	sm_sungetc(wint_t wc, t_scan_src *src)
{
	if (wc == WEOF)
		return ;
	if (src->file && src->is_wide)
		ungetwc(wc, src->file);
	else if (src->file)
		ungetc(wctob(wc), src->file);
	else if (src->pos > 0)
		src->pos--;
}

static t_errno	sm_scan_mstr(t_scan_src *src, t_string_mx *dst, wchar_t mod)
{
	t_string_mx	*tmp;
	wchar_t		wch[2];
	wint_t		c;
	t_errno		err;

	if (!dst || dst->is_const || wstrcreate_m(&tmp, L"", 0, NULL) != 0)
		return (EINVAL);
	wch[1] = L'\0';
	c = sm_sgetc(src);
	while (c != WEOF && iswspace(c))
		c = sm_sgetc(src);
	while (c != WEOF && !iswspace(c))
	{
		wch[0] = (wchar_t)c;
		wstrcat_m(tmp, wch);
		c = sm_sgetc(src);
	}
	sm_sungetc(c, src);
	if (tmp->len == 0)
		return (strdelete_m(&tmp), EINVAL);
	err = strcpy_m(dst, tmp);
	if (mod == L'h' && err == 0)
		err = cstrcpy_m(dst, (const char *)tmp->data.wstr);
	strdelete_m(&tmp);
	return (err);
}

static t_errno	sm_scan_item(t_scan_src *src, const wchar_t **p,
					va_list args, int *cnt)
{
	wchar_t	mod;
	wchar_t	tok[64];
	size_t	i;
	wint_t	c;

	(*p)++;
	mod = 0;
	if (**p == L'h' || **p == L'l')
		mod = *(*p)++;
	if (**p == L's' && (*p)++)
	{
		if (sm_scan_mstr(src, va_arg(args, t_string_mx *), mod) != 0)
			return (EINVAL);
		return ((*cnt)++, 0);
	}
	i = 0;
	c = sm_sgetc(src);
	while (c != WEOF && iswspace(c))
		c = sm_sgetc(src);
	while (i < 63 && c != WEOF && !iswspace(c))
	{
		tok[i++] = (wchar_t)c;
		c = sm_sgetc(src);
	}
	tok[i] = L'\0';
	sm_sungetc(c, src);
	if (*(*p)++ == L'd' && swscanf(tok, L"%d", va_arg(args, int *)) == 1)
		(*cnt)++;
	return (0);
}

t_errno	sm_scan_engine(t_scan_src *src, const wchar_t *wfmt,
			int *cnt, va_list args)
{
	const wchar_t	*p;
	int				assigned;
	wint_t			c;

	p = wfmt;
	assigned = 0;
	while (*p != L'\0')
	{
		if (iswspace(*p))
		{
			while (iswspace(*p))
				p++;
			c = sm_sgetc(src);
			while (c != WEOF && iswspace(c))
				c = sm_sgetc(src);
			sm_sungetc(c, src);
		}
		else if (*p == L'%' && sm_scan_item(src, &p, args, &assigned) == 0)
			assigned += 0;
		else if (sm_sgetc(src) != (wint_t)(*p++))
			break ;
	}
	if (cnt)
		*cnt = assigned;
	return (0);
}
