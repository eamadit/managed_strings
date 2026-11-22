/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sm_stdio_v.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:50:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:20:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m_internal.h"

t_errno	vprintf_m(const t_string_mx *fmt, int *count, va_list args)
{
	return (vfprintf_m(stdout, fmt, count, args));
}

t_errno	vscanf_m(const t_string_mx *fmt, int *count, va_list args)
{
	return (vfscanf_m(stdin, fmt, count, args));
}
