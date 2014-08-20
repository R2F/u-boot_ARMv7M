/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <linux/linkage.h>

.section ".text.init", "x"

ENTRY(reset_cpu)
	bx lr
ENDPROC(reset_cpu)
