/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_compare.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:47:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:19:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

int	sm_cmp_bytes_n(const char *a, const char *b, size_t n)
{
	size_t			i;
	unsigned char	ca;
	unsigned char	cb;

	i = 0;
	while (i < n && (a[i] != '\0' || b[i] != '\0'))
	{
		ca = (unsigned char)a[i];
		cb = (unsigned char)b[i];
		if (ca != cb)
			return ((int)ca - (int)cb);
		i++;
	}
	return (0);
}

int	sm_cmp_wide_n(const wchar_t *a, const wchar_t *b, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && (a[i] != L'\0' || b[i] != L'\0'))
	{
		if (a[i] != b[i])
		{
			if (a[i] < b[i])
				return (-1);
			return (1);
		}
		i++;
	}
	return (0);
}

static t_errno	sm_cmp_wide_pair(const t_string_mx *s1, const t_string_mx *s2,
					t_rsize n, int *cmp)
{
	const wchar_t	*w1;
	const wchar_t	*w2;

	w1 = NULL;
	w2 = NULL;
	if (wgetstr_m(s1, &w1) != 0)
		return (sm_raise_constraint("strncmp_m: conv err", NULL, EILSEQ));
	if (wgetstr_m(s2, &w2) != 0)
	{
		sm_free((void *)w1);
		return (sm_raise_constraint("strncmp_m: conv err", NULL, EILSEQ));
	}
	*cmp = sm_cmp_wide_n(w1, w2, n);
	sm_free((void *)w1);
	sm_free((void *)w2);
	return (0);
}

t_errno	strncmp_m(const t_string_mx *s1, const t_string_mx *s2,
			t_rsize n, int *cmp)
{
	if (!s1 || !s2 || !cmp || n > RSIZE_MAX)
		return (sm_raise_constraint("strncmp_m: invalid arg", NULL, EINVAL));
	if (n == 0 || (s1->is_null && s2->is_null))
	{
		*cmp = 0;
		return (0);
	}
	if (s1->is_null)
		*cmp = -1;
	else if (s2->is_null)
		*cmp = 1;
	else if (s1->is_wide || s2->is_wide)
		return (sm_cmp_wide_pair(s1, s2, n, cmp));
	else
		*cmp = sm_cmp_bytes_n(s1->data.cstr, s2->data.cstr, n);
	return (0);
}

t_errno	strcmp_m(const t_string_mx *s1, const t_string_mx *s2, int *cmp)
{
	if (!s1 || !s2 || !cmp)
		return (sm_raise_constraint("strcmp_m: null arg", NULL, EINVAL));
	return (strncmp_m(s1, s2, RSIZE_MAX, cmp));
}
