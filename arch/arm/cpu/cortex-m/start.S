/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <linux/linkage.h>

.globl	_start
.type _start, %function
_start:
	b	reset

reset:
	bl	_main

.globl	c_runtime_cpu_setup
c_runtime_cpu_setup:
	mov	pc, lr
