/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_m.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 16:42:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 11:35:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_M_H
# define STRING_M_H

# include <errno.h>
# include <stdarg.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>

# define RSIZE_MAX (SIZE_MAX >> 1)

typedef int					t_errno;
typedef size_t				t_rsize;
typedef struct s_string_mx	t_string_mx;
typedef void				(*t_constraint_handler)(const char * restrict msg,
								void * restrict ptr, t_errno error);
typedef void				*(*t_malloc_handler)(size_t size);
typedef void				*(*t_realloc_handler)(void *ptr, size_t size);
typedef void				(*t_free_handler)(void *ptr);

t_constraint_handler		set_constraint_handler_m(
								t_constraint_handler handler);
t_errno						isnull_m(const t_string_mx *s, _Bool *nullstr);
t_errno						isempty_m(const t_string_mx *s, _Bool *emptystr);
t_errno						strcreate_m(t_string_mx **s, const char *cstr,
								const size_t maxsize, const char *charset);
t_errno						wstrcreate_m(t_string_mx **s, const wchar_t *cstr,
								const size_t maxsize,
								const wchar_t *charset);
t_errno						const_strcreate_m(const t_string_mx **str,
								const char *cstr, const size_t maxsize,
								const char *charset);
t_errno						const_wstrcreate_m(t_string_mx **str,
								const wchar_t *wcstr, const size_t maxsize,
								const wchar_t *charset);
t_errno						isntbs_m(const t_string_mx *s, _Bool *ntbstr);
t_errno						iswide_m(const t_string_mx *s, _Bool *widestr);
t_errno						strdelete_m(t_string_mx **s);
t_errno						strlen_m(const t_string_mx *s, t_rsize *size);
t_errno						cgetstr_m(const t_string_mx *s,
								const char **string);
t_errno						wgetstr_m(const t_string_mx *s,
								const wchar_t **wcstr);
t_errno						strdup_m(t_string_mx **s1, const t_string_mx *s2);
t_errno						strcpy_m(t_string_mx *s1, const t_string_mx *s2);
t_errno						cstrcpy_m(t_string_mx *s1, const char *cstr);
t_errno						wstrcpy_m(t_string_mx *s1, const wchar_t *wcstr);
t_errno						strncpy_m(t_string_mx *s1, const t_string_mx *s2,
								t_rsize n);
t_errno						strcat_m(t_string_mx *s1, const t_string_mx *s2);
t_errno						cstrcat_m(t_string_mx *s, const char *cstr);
t_errno						wstrcat_m(t_string_mx *s, const wchar_t *wcstr);
t_errno						strncat_m(t_string_mx *s1, const t_string_mx *s2,
								t_rsize n);
t_errno						cstrncat_m(t_string_mx *s, const char *cstr,
								t_rsize n);
t_errno						wstrncat_m(t_string_mx *s, const wchar_t *wcstr,
								t_rsize n);
t_errno						strcmp_m(const t_string_mx *s1,
								const t_string_mx *s2, int *cmp);
t_errno						cstrcmp_m(const t_string_mx *s1, const char *cstr,
								int *cmp);
t_errno						wstrcmp_m(const t_string_mx *s1,
								const wchar_t *wstr, int *cmp);
t_errno						strncmp_m(const t_string_mx *s1,
								const t_string_mx *s2, t_rsize n, int *cmp);
t_errno						cstrncmp_m(const t_string_mx *s1,
								const char *cstr, t_rsize n, int *cmp);
t_errno						wstrncmp_m(const t_string_mx *s1,
								const wchar_t *wstr, t_rsize n, int *cmp);
t_errno						strtok_m(t_string_mx *token, t_string_mx *str,
								const t_string_mx *delim, t_string_mx *ptr);
t_errno						cstrchr_m(const t_string_mx *str, char c,
								t_rsize *index);
t_errno						wstrchr_m(const t_string_mx *str, wchar_t wc,
								t_rsize *index);
t_errno						strspn_m(t_string_mx *str, t_string_mx *accept,
								t_rsize *len);
t_errno						cstrspn_m(t_string_mx *str, const char *accept,
								t_rsize *len);
t_errno						wstrspn_m(t_string_mx *str, const wchar_t *accept,
								t_rsize *len);
t_errno						strcspn_m(t_string_mx *str, t_string_mx *reject,
								t_rsize *len);
t_errno						cstrcspn_m(t_string_mx *str, const char *reject,
								t_rsize *len);
t_errno						wstrcspn_m(t_string_mx *str,
								const wchar_t *reject, t_rsize *len);
t_errno						setcharset_m(t_string_mx *s,
								const t_string_mx *charset);
t_errno						setmaxlen_m(t_string_mx *s, t_rsize maxlen);
t_errno						strslice_m(t_string_mx *s1, const t_string_mx *s2,
								t_rsize offset, t_rsize len);
t_errno						strleft_m(t_string_mx *s1, const t_string_mx *s2,
								t_rsize len);
t_errno						strright_m(t_string_mx *s1, const t_string_mx *s2,
								t_rsize len);
t_errno						cchar_m(const t_string_mx *s, t_rsize offset,
								char *c);
t_errno						wchar_m(const t_string_mx *s, t_rsize offset,
								wchar_t *wc);
t_errno						sprintf_m(t_string_mx *buf,
								const t_string_mx *fmt, int *count, ...);
t_errno						vsprintf_m(t_string_mx *buf,
								const t_string_mx *fmt, int *count,
								va_list args);
t_errno						printf_m(const t_string_mx *fmt, int *count, ...);
t_errno						vprintf_m(const t_string_mx *fmt, int *count,
								va_list args);
t_errno						fprintf_m(FILE *file, const t_string_mx *fmt,
								int *count, ...);
t_errno						vfprintf_m(FILE *file, const t_string_mx *fmt,
								int *count, va_list args);
t_errno						sscanf_m(t_string_mx *buf, const t_string_mx *fmt,
								int *count, ...);
t_errno						vsscanf_m(t_string_mx *buf,
								const t_string_mx *fmt, int *count,
								va_list args);
t_errno						scanf_m(const t_string_mx *fmt, int *count, ...);
t_errno						vscanf_m(const t_string_mx *fmt, int *count,
								va_list args);
t_errno						fscanf_m(FILE *file, const t_string_mx *fmt,
								int *count, ...);
t_errno						vfscanf_m(FILE *file, const t_string_mx *fmt,
								int *count, va_list args);

#endif
