/*
 * vmctl.c
 * Virtual Machine or rather Container controller 
 *
 * Modified from: 
 * chroot_ns.c
 * Author: Serge Hallyn <serue@us.ibm.com>
 * Date: Jan 25, 2005
 *
 * This version acts as "chroot" using namespaces.
 *
 * Copyright (C) 2004 International Business Machines <serue@us.ibm.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sys/mount.h>

#ifndef CLONE_NEWNS
#define CLONE_NEWNS 0x00020000
#endif

#ifndef CLONE_NEWPID
#define CLONE_NEWPID   0x20000000      /* New pid namespace.  */
#endif

#ifndef CLONE_NEWIPC
#define CLONE_NEWIPC	0x08000000	/* New ipcs.  */
#endif

#ifndef CLONE_NEWUTS
#define CLONE_NEWUTS	0x04000000	/* New utsname group.  */
#endif

#ifndef CLONE_NEWUSER
#define CLONE_NEWUSER	0x10000000	/* New user namespace.  */
#endif

#ifndef MNT_DETACH
#define MNT_DETACH  0x00000002
#endif

#define MAX_PATH 256

#define OLD_ROOT "/mnt"
#define CMD "/bin/sh"

void usage(char *cmd)
{
        printf("Usage: %s <new_root> [<old_root>] [<command>]\n", cmd);
        printf("   Perform <command> under a new namespace with <new_root>\n");
        printf("   as the root of the filesystem.\n");
        printf("   <old_root> is relative to the old root.");
        printf("   If unspecified, <old_root> is '/mnt'.\n");
        printf("   If unspecified, <command> is '/bin/sh'.\n");
        exit(-EINVAL);
}

void check_mount(const char *source, const char* target_prefix,
		 const char *target, const char *filesystemtype,
		 unsigned long mountflags, const void *data)
{
	char target_path[MAX_PATH];
	sprintf(target_path, "%s%s", target_prefix, target);
        int ret = mount(source, target_path, filesystemtype, mountflags, data);
        if (ret) {
                perror("mount");
                exit(ret);
        }
}

int main(int argc, char *argv[])
{
        int pid;
        int ret;
        uid_t uid;
        char *new_root, *old_root, *cmd, *argv0;
        char full_oldroot[MAX_PATH];
        uid = getuid();

        if (uid) {
                fprintf(stderr, "Permission denied on clone.\n");
                fprintf(stderr, "You must have CAP_SYS_ADMIN to clone a"
                        " fs namespace.\n");
                exit(-1);
        }

        argv0 = argv[0];

        if (argc < 2 || strcmp(argv[1], "-h") == 0)
                usage(argv0);

        new_root = argv[1];
        old_root = OLD_ROOT;
        cmd = CMD;

        pid = syscall(SYS_clone, CLONE_NEWUSER | CLONE_NEWPID | CLONE_NEWNS |
		      CLONE_NEWUTS | CLONE_NEWIPC | SIGCHLD,0);

        if (pid != 0) {
                waitpid(pid, &ret, 0);
                exit(-1);
        }

        if (strlen(old_root) + strlen(new_root) >= MAX_PATH-1) {
                printf("paths too long.\n");
                return -1;
        }

        snprintf(full_oldroot, MAX_PATH, "%s%s", new_root, old_root);

        /* jump into the new root directory */
        printf("going into %s\n", new_root);
        ret = chdir(new_root);
        if (ret) {
                perror("chdir");
                exit(2);
        }

	// Ensure new mount is on a separate vfsmount
	check_mount(new_root, "", new_root, NULL, MS_BIND, NULL);

	check_mount("/bin", new_root, "/bin", NULL, MS_BIND, NULL); 
	check_mount("/usr", new_root, "/usr", NULL, MS_BIND, NULL); 
	check_mount("/lib", new_root, "/lib", NULL, MS_BIND, NULL); 
	check_mount("/sbin",new_root, "/sbin", NULL, MS_BIND, NULL); 
	check_mount("/opt", new_root, "/opt", NULL, MS_BIND, NULL); 
	check_mount("/sys", new_root, "/sys", NULL, MS_BIND, NULL); 
	check_mount("/dev", new_root, "/dev", NULL, MS_BIND, NULL); 

        /* pivot root */
        printf("switching %s and %s\n", new_root, full_oldroot);
        ret = syscall(SYS_pivot_root, new_root, full_oldroot);
        if (ret) {
                perror("pivot_root");
                printf("Try \"mount --bind %s %s\"\n", new_root, new_root);
                exit(ret);
        }

        ret = chdir("/");
        if (ret) {
                perror("chdir");
                exit(2);
        }

	// Mount a new devpts and proc and tmp
	check_mount("none", "", "/dev/pts", "devpts", 0, NULL); 
	check_mount("none", "", "/proc", "proc", 0, NULL); 
	check_mount("none", "", "/tmp", "tmpfs", 0, NULL);

        /* unmount the old_root */
	ret = umount2(old_root, MNT_DETACH);
	if (ret) {
	      perror("umount");
	      exit(2);
	}
        
        /* Execute the command */
        execl(cmd, cmd, NULL);
        perror("execl");
        fprintf(stderr, "Cannot exec %s.\n", cmd);
        exit(-1);
}
