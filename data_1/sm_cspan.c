/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_cspan.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:48:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:19:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	strcspn_m(t_string_mx *str, t_string_mx *reject, t_rsize *len)
{
	return (sm_span_common(str, reject, len, false));
}

t_errno	cstrcspn_m(t_string_mx *str, const char *reject, t_rsize *len)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!str || !reject || !len)
		return (sm_raise_constraint("cstrcspn_m: null arg", NULL, EINVAL));
	tmp = NULL;
	err = strcreate_m(&tmp, reject, 0, NULL);
	if (err != 0)
		return (err);
	err = sm_span_common(str, tmp, len, false);
	strdelete_m(&tmp);
	return (err);
}

t_errno	wstrcspn_m(t_string_mx *str, const wchar_t *reject, t_rsize *len)
{
	t_string_mx	*tmp;
	t_errno		err;

	if (!str || !reject || !len)
		return (sm_raise_constraint("wstrcspn_m: null arg", NULL, EINVAL));
	tmp = NULL;
	err = wstrcreate_m(&tmp, reject, 0, NULL);
	if (err != 0)
		return (err);
	err = sm_span_common(str, tmp, len, false);
	strdelete_m(&tmp);
	return (err);
}
