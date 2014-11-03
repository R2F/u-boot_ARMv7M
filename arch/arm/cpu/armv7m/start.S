/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <linux/linkage.h>

.globl	reset
.type reset, %function
reset:
	bl	_main

.globl	c_runtime_cpu_setup
c_runtime_cpu_setup:
	mov	pc, lr
