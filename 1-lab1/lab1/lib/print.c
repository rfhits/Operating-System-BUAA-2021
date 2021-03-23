/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <print.h>

/* macros */
#define IsDigit(x) (((x) >= '0') && ((x) <= '9'))
#define Ctod(x) ((x) - '0')

/* forward declaration */
extern int PrintChar(char *, char, int, int);
extern int PrintString(char *, char *, int, int);
extern int PrintNum(char *, unsigned long, int, int, int, int, char, int);

/* private variable */
static const char theFatalMsg[] = "fatal error in lp_Print!";

/* -*-
 * A low level printf() function.
 */
void lp_Print(void (*output)(void *, char *, int),
			  void *arg, char *fmt, va_list ap)
{

#define OUTPUT(arg, s, l)                                                 \
	{                                                                     \
		if (((l) < 0) || ((l) > LP_MAX_BUF))                              \
		{                                                                 \
			(*output)(arg, (char *)theFatalMsg, sizeof(theFatalMsg) - 1); \
			for (;;)                                                      \
				;                                                         \
		}                                                                 \
		else                                                              \
		{                                                                 \
			(*output)(arg, s, l);                                         \
		}                                                                 \
	}

	char buf[LP_MAX_BUF]; // 80 = LP_MAX_BUF

	char c;
	char *s;
	long int num;

	int longFlag; // 标记是否为long 型
	int negFlag;  // 标记是否为负数
	int width;	  // 标记输出宽度
	int prec;	  // 标记小数精度
	int ladjust;  // 标记是否左对齐
	char padc;	  // 填充多余位置所用的字符

	int length; // BUF里面有多少个数

	/*
        Exercise 1.5. Please fill in two parts in this file.
    */

	for (;;)
	{

		/* Part1: your code here */
		{ /* scan for the '%'，将%前的字符串都读入到buf中*/
			length = 0;
			while (*fmt != '%')
			{
				buf[length++] = *fmt++;
				if (*fmt == '\0')
				{
					break;
				}
			}
			/* flush the string found so far */

			/* check "are we hitting the end?" */
		}

		/* we found a '%' */

		/* check for long */

		/* check for other prefixes */

		/* check format flag */

		negFlag = 0;
		switch (*fmt)
		{
		case 'b':
			if (longFlag)
			{
				num = va_arg(ap, long int);
			}
			else
			{
				num = va_arg(ap, int);
			}
			length = PrintNum(buf, num, 2, 0, width, ladjust, padc, 0);
			OUTPUT(arg, buf, length);
			break;

		case 'd':
		case 'D':
			if (longFlag)
			{
				num = va_arg(ap, long int);
			}
			else
			{
				num = va_arg(ap, int);
			}

			/*  Part2:
			your code here.
			Refer to other part (case 'b',case 'o' etc.) and func PrintNum to complete this part.
			Think the difference between case 'd' and others. (hint: negFlag).
		*/

			break;

		case 'o':
		case 'O':
			if (longFlag)
			{
				num = va_arg(ap, long int);
			}
			else
			{
				num = va_arg(ap, int);
			}
			length = PrintNum(buf, num, 8, 0, width, ladjust, padc, 0);
			OUTPUT(arg, buf, length);
			break;

		case 'u':
		case 'U':
			if (longFlag)
			{
				num = va_arg(ap, long int);
			}
			else
			{
				num = va_arg(ap, int);
			}
			length = PrintNum(buf, num, 10, 0, width, ladjust, padc, 0);
			OUTPUT(arg, buf, length);
			break;

		case 'x':
			if (longFlag)
			{
				num = va_arg(ap, long int);
			}
			else
			{
				num = va_arg(ap, int);
			}
			length = PrintNum(buf, num, 16, 0, width, ladjust, padc, 0);
			OUTPUT(arg, buf, length);
			break;

		case 'X':
			if (longFlag)
			{
				num = va_arg(ap, long int);
			}
			else
			{
				num = va_arg(ap, int);
			}
			length = PrintNum(buf, num, 16, 0, width, ladjust, padc, 1);
			OUTPUT(arg, buf, length);
			break;

		case 'c':
			c = (char)va_arg(ap, int);
			length = PrintChar(buf, c, width, ladjust);
			OUTPUT(arg, buf, length);
			break;

		case 's':
			s = (char *)va_arg(ap, char *);
			length = PrintString(buf, s, width, ladjust);
			OUTPUT(arg, buf, length);
			break;

		case '\0':
			fmt--;
			break;

		default:
			/* output this char as it is */
			OUTPUT(arg, fmt, 1);
		} /* switch (*fmt) */

		fmt++;
	} /* for(;;) */

	/* special termination call */
	OUTPUT(arg, "\0", 1);
}

/* --------------- local help functions --------------------- */
int PrintChar(char *buf, char c, int length, int ladjust)
{
	int i;

	if (length < 1)
		length = 1;
	if (ladjust)
	{
		*buf = c;
		for (i = 1; i < length; i++)
			buf[i] = ' ';
	}
	else
	{
		for (i = 0; i < length - 1; i++)
			buf[i] = ' ';
		buf[length - 1] = c;
	}
	return length;
}

int PrintString(char *buf, char *s, int length, int ladjust)
{
	int i;
	int len = 0;
	char *s1 = s;
	while (*s1++)
		len++;
	if (length < len)
		length = len;

	if (ladjust)
	{
		for (i = 0; i < len; i++)
			buf[i] = s[i];
		for (i = len; i < length; i++)
			buf[i] = ' ';
	}
	else
	{
		for (i = 0; i < length - len; i++)
			buf[i] = ' ';
		for (i = length - len; i < length; i++)
			buf[i] = s[i - length + len];
	}
	return length;
}

int PrintNum(char *buf, unsigned long u, int base, int negFlag,
			 int length, int ladjust, char padc, int upcase)
{
	/* algorithm :
     *  1. prints the number from left to right in reverse form.
     *  2. fill the remaining spaces with padc if length is longer than
     *     the actual length
     *     TRICKY : if left adjusted, no "0" padding.
     *		    if negtive, insert  "0" padding between "0" and number.
     *  3. if (!ladjust) we reverse the whole string including paddings
     *  4. otherwise we only reverse the actual string representing the num.
     */

	int actualLength = 0;
	char *p = buf;
	int i;

	do
	{
		int tmp = u % base;
		if (tmp <= 9)
		{
			*p++ = '0' + tmp;
		}
		else if (upcase)
		{
			*p++ = 'A' + tmp - 10;
		}
		else
		{
			*p++ = 'a' + tmp - 10;
		}
		u /= base;
	} while (u != 0);

	if (negFlag)
	{
		*p++ = '-';
	}

	/* figure out actual length and adjust the maximum length */
	actualLength = p - buf;
	if (length < actualLength)
		length = actualLength;

	/* add padding */
	if (ladjust)
	{
		padc = ' ';
	}
	if (negFlag && !ladjust && (padc == '0'))
	{
		for (i = actualLength - 1; i < length - 1; i++)
			buf[i] = padc;
		buf[length - 1] = '-';
	}
	else
	{
		for (i = actualLength; i < length; i++)
			buf[i] = padc;
	}

	/* prepare to reverse the string */
	{
		int begin = 0;
		int end;
		if (ladjust)
		{
			end = actualLength - 1;
		}
		else
		{
			end = length - 1;
		}

		while (end > begin)
		{
			char tmp = buf[begin];
			buf[begin] = buf[end];
			buf[end] = tmp;
			begin++;
			end--;
		}
	}

	/* adjust the string pointer */
	return length;
}
