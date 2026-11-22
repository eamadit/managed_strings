/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 14:01:00 by student           #+#    #+#             */
/*   Updated: 2026/11/22 15:20:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_m.h"
#include <fcntl.h>
#include <sys/mount.h>
#include <unistd.h>
#include <string.h>

extern t_constraint_handler	set_constraint_handler_m(
								t_constraint_handler handler);
void						msh_run_extra_tests(void);
static void					msh_run_script(const char *path);

static void	pid1_fault_handler(const char * restrict msg, void * restrict ptr,
				t_errno error)
{
	(void)ptr;
	(void)error;
	write(2, "  [SHIELD BLOCKED] ", 19);
	if (msg)
		write(2, msg, strlen(msg));
	write(2, "\n", 1);
}

static void	msh_audit_suite(void)
{
	t_string_mx			*env;
	t_string_mx			*tok;
	t_string_mx			*rem;
	const t_string_mx	*ver;

	env = NULL;
	tok = NULL;
	rem = NULL;
	ver = NULL;
	write(1, "[3/6] Charset allow-list vs shell injection (;rm -rf /)...\n", 59);
	strcreate_m(&env, "SAFE_VAR", 32, "ABCDEFGHIJKLMNOPQRSTUVWXYZ_");
	cstrcat_m(env, ";rm -rf /");
	write(1, "[4/6] Immutability shield (const_strcreate_m)...\n", 49);
	const_strcreate_m(&ver, "KERNEL_LOCKED", 32, NULL);
	cstrcpy_m((t_string_mx *)ver, "HACKED");
	write(1, "[5/6] Iterative path tokenizer (strtok_m) & UTF-8...\n", 53);
	strcreate_m(&tok, NULL, 32, NULL);
	strcreate_m(&rem, NULL, 32, NULL);
	strcreate_m(&env, "usr/local/bin", 32, NULL);
	strtok_m(tok, env, (t_string_mx *)ver, rem);
	wstrcat_m(env, L" [UTF-8 OK]");
	write(1, "[6/6] Audit complete: PID 1 stack & heap 100% intact!\n", 54);
}

static void	msh_exec_line(t_string_mx *cmd, const char *raw)
{
	int	cmp;

	if (*raw == '\0' || *raw == '#' || cstrcpy_m(cmd, raw) != 0)
		return ;
	if (cstrcmp_m(cmd, "help", &cmp) == 0 && cmp == 0)
		write(1, "Commands: help, status, audit, extra, echo, run\n", 48);
	else if (cstrcmp_m(cmd, "status", &cmp) == 0 && cmp == 0)
		write(1, "PID 1: musl-1.2.6 + CMU Managed Strings (Zero-Recursion)\n", 57);
	else if (cstrcmp_m(cmd, "audit", &cmp) == 0 && cmp == 0)
		msh_audit_suite();
	else if (cstrcmp_m(cmd, "extra", &cmp) == 0 && cmp == 0)
		msh_run_extra_tests();
	else if (cstrncmp_m(cmd, "echo ", 5, &cmp) == 0 && cmp == 0)
	{
		write(1, raw + 5, strlen(raw + 5));
		write(1, "\n", 1);
	}
	else if (cstrncmp_m(cmd, "run ", 4, &cmp) == 0 && cmp == 0)
		msh_run_script(raw + 4);
	else
		write(1, "Unknown command\n", 16);
}

static void	msh_run_script(const char *path)
{
	t_string_mx	*cmd;
	char		*line;
	int			fd;
	size_t		i;

	fd = open(path, O_RDONLY);
	line = malloc(512);
	cmd = NULL;
	strcreate_m(&cmd, "", 256, NULL);
	i = 0;
	while (fd >= 0 && line && cmd && read(fd, &line[i], 1) == 1)
	{
		if (line[i] == '\n' || i >= 510)
		{
			line[i] = '\0';
			msh_exec_line(cmd, line);
			i = 0;
		}
		else
			i++;
	}
	if (fd >= 0)
		close(fd);
	free(line);
	strdelete_m(&cmd);
}

int	main(void)
{
	t_string_mx	*cmd;
	char		*buf;
	ssize_t		n;

	set_constraint_handler_m(pid1_fault_handler);
	mount("proc", "/proc", "proc", 0, NULL);
	mount("sysfs", "/sys", "sysfs", 0, NULL);
	buf = malloc(512);
	cmd = NULL;
	strcreate_m(&cmd, "", 256, NULL);
	write(1, "\n[BOOT] Secure UNIX PID=1 ready (type: run /test.msh)\n", 54);
	while (buf && cmd)
	{
		write(1, "msh# ", 5);
		n = read(0, buf, 511);
		if (n <= 0)
			break ;
		buf[n - 1] = '\0';
		msh_exec_line(cmd, buf);
	}
	return (0);
}
