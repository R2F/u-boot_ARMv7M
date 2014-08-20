/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <linux/linkage.h>

	.globl _start
	.section  .text._start
_start:
	bl	lowlevel_init
	bl	_main

	.globl	c_runtime_cpu_setup
c_runtime_cpu_setup:
	mov	pc, lr
