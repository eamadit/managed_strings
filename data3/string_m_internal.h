/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_m_internal.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:51:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:37:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_M_INTERNAL_H
# define STRING_M_INTERNAL_H

# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200809L
# endif

# include "string_m.h"
# include <string.h>
# include <wchar.h>

# define sm_realloc realloc

union u_str_buf
{
	char		*cstr;
	wchar_t		*wstr;
};

struct s_string_mx
{
	_Bool			is_null;
	_Bool			is_wide;
	_Bool			is_const;
	size_t			len;
	size_t			maxlen;
	union u_str_buf	data;
	union u_str_buf	charset;
};

typedef struct s_scan_src
{
	FILE			*file;
	const wchar_t	*buf;
	size_t			pos;
	_Bool			is_wide;
}	t_scan_src;

t_errno		sm_raise_constraint(const char *msg, void *ptr, t_errno err);
void		*sm_malloc(size_t size);
void		sm_free(void *ptr);
_Bool		sm_validate_cset_n(const char *str, size_t n, const char *cset);
_Bool		sm_validate_wset_n(const wchar_t *wstr, size_t n,
				const wchar_t *wset);
t_errno		sm_promote_to_wide(t_string_mx *s);
t_errno		sm_mb_to_wide_dup(const char *cstr, wchar_t **out, size_t *len);
t_errno		sm_wide_to_mb_dup(const wchar_t *wstr, char **out, size_t *len);
t_errno		sm_copy_wide_n(t_string_mx *s1, const t_string_mx *s2, t_rsize n);
t_errno		sm_cat_wide_n(t_string_mx *s1, const t_string_mx *s2, t_rsize n);
int			sm_cmp_bytes_n(const char *a, const char *b, size_t n);
int			sm_cmp_wide_n(const wchar_t *a, const wchar_t *b, size_t n);
t_errno		sm_span_common(const t_string_mx *str, const t_string_mx *set,
				t_rsize *len, _Bool accept);
t_errno		sm_fmt_engine(const wchar_t *wfmt, va_list args, wchar_t **out,
				int *cnt);
t_errno		sm_scan_engine(t_scan_src *src, const wchar_t *wfmt, int *cnt,
				va_list args);

#endif
