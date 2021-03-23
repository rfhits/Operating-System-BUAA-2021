# Common includes in Makefile
#
# Copyright (C) 2007 Beihang University
# Written By Zhu Like ( zlike@cse.buaa.edu.cn )



# Exercise 1.1. Please modify the CROSS_COMOILE path.

CROSS_COMPILE :=  bin/mips_4KC-
CC			  := $(CROSS_COMPILE)gcc
CFLAGS		  := -O -G 0 -mno-abicalls -fno-builtin -Wa,-xgot -Wall -fPIC -march=r3000
LD			  := $(CROSS_COMPILE)ld
