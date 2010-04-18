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

#ifndef MNT_DETACH
#define MNT_DETACH  0x00000002
#endif

#define MAX_PATH 256

#define OLD_ROOT "mnt"
#define CMD "/bin/sh"

void usage(char *cmd)
{
        printf("Usage: %s [-u] <new_root> [<old_root>] [<command>]\n", cmd);
        printf("   Perform <command> under a new namespace with <new_root>\n");
        printf("   as the root of the filesystem.\n");
        printf("   If -u is specified, the old root will be unmounted before"
                        " <command> is executed.\n");
        printf("   <old_root> is relative to the old root.");
        printf("   If unspecified, <old_root> is '/mnt'.\n");
        printf("   If unspecified, <command> is '/bin/sh'.\n");
        exit(-EINVAL);
}

void check_mount(const char* root, const char *source, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 const void *data)
{
	char path[MAX_PATH];
	sprintf(path, "%s/%s", root, source);
	printf("mounting %s at %s \n", path, target);
        int ret = mount(path, target, filesystemtype, mountflags, data);
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
        int do_umount;
        uid = getuid();

        if (uid) {
                fprintf(stderr, "Permission denied on clone.\n");
                fprintf(stderr, "You must have CAP_SYS_ADMIN to clone a"
                        " fs namespace.\n");
                exit(-1);
        }

	// Add CLONE_NEWPID here ? 
        pid = syscall(SYS_clone, CLONE_NEWPID | CLONE_NEWNS | SIGCHLD,0);

        if (pid != 0) {
                waitpid(pid, &ret, 0);
                exit(-1);
        }

        argv0 = argv[0];
        if (argc > 1 && strcmp(argv[1], "-u") == 0) {
                do_umount = 1;
                argv++;
                argc--;
        } else
                do_umount = 0;

        if (argc < 2 || strcmp(argv[1], "-h") == 0)
                usage(argv0);

        new_root = argv[1];

        if (argc > 2)
                old_root = argv[2];
        else
                old_root = OLD_ROOT;

        if (argc > 3)
                cmd = argv[3];
        else
                cmd = CMD;

        if (strlen(old_root) + strlen(new_root) >= MAX_PATH-1) {
                printf("paths too long.\n");
                return -1;
        }

        snprintf(full_oldroot, MAX_PATH, "%s/%s", new_root, old_root);

        /* jump into the new root directory */
        printf("going into %s\n", new_root);
        ret = chdir(new_root);
        if (ret) {
                perror("chdir");
                exit(2);
        }

	// Ensure new mount is on a separate vfsmount
	check_mount("/", new_root, new_root, NULL, MS_BIND, NULL);

        /* pivot root */
        printf("switching %s and %s\n", new_root, full_oldroot);
        ret = syscall(SYS_pivot_root, new_root, full_oldroot);
        if (ret) {
                perror("pivot_root");
                printf("Try \"mount --bind %s %s\"\n", new_root, new_root);
                exit(ret);
        }

	check_mount(old_root, "bin", "bin", NULL, MS_BIND, NULL); 
	check_mount(old_root, "usr", "usr", NULL, MS_BIND, NULL); 
	check_mount(old_root, "lib", "lib", NULL, MS_BIND, NULL); 
	check_mount(old_root, "tmp", "tmp", NULL, MS_BIND, NULL); 
	check_mount(old_root, "sbin", "sbin", NULL, MS_BIND, NULL); 
	check_mount(old_root, "opt", "opt", NULL, MS_BIND, NULL); 
	check_mount(old_root, "dev", "dev", NULL, MS_BIND, NULL); 
	check_mount(old_root, "sys", "sys", NULL, MS_BIND, NULL); 

	// Mount a fresh /proc, /etc and /home
	check_mount("/", "proc", "proc", "proc", 0, NULL); 
	check_mount("/", "etc", "etc", "ext3", 0, NULL); 
	check_mount("/", "home", "home", "ext3", 0, NULL); 
	
        /* unmount if requested */
        if (do_umount) {
                ret = umount2(old_root, MNT_DETACH);
                if (ret) {
                        perror("umount");
                        exit(2);
                }
        }

        /* Execute the command */
        execl(cmd, cmd, NULL);
        perror("execl");
        fprintf(stderr, "Cannot exec %s.\n", cmd);
        exit(-1);
}
