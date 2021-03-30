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

#include        <print.h>

/* macros */
#define         IsDigit(x)      ( ((x) >= '0') && ((x) <= '9') )
#define         Ctod(x)         ( (x) - '0')

/* forward declaration */
extern int PrintChar(char *, char, int, int);
extern int PrintString(char *, char *, int, int);
extern int PrintNum(char *, unsigned long, int, int, int, int, char, int);

/* private variable */
static const char theFatalMsg[] = "fatal error in lp_Print!";

/* exam struct need*/

typedef struct {
        int a;
        char b;
        char c;
        int d;
} s1;

typedef struct {
        int size;
        int c[1000];
} s2;
/* u need to force transform*/



/* -*-
 * A low level printf() function.
 */
void
lp_Print(void (*output)(void *, char *, int),
         void * arg,
         char *fmt,
         va_list ap)
{

#define         OUTPUT(arg, s, l)  \
  { if (((l) < 0) || ((l) > LP_MAX_BUF)) { \
       (*output)(arg, (char*)theFatalMsg, sizeof(theFatalMsg)-1); for(;;); \
    } else { \
      (*output)(arg, s, l); \
    } \
  }

    char buf[LP_MAX_BUF];

    char c;
    char *s;
    long int num;

    //** add for exam
    int *a_int;
    long int *a_long;

    //struct s1 *a_s1;
    //struct s2 *a_s2;

    s1* a_s1;
    s2* a_s2;
    int size;
    int* arr_int; // type in struct s2


    int var_for;
    int arraySize;
    // end region



    int longFlag; // is long
    int negFlag; // is negative
    int width; // output width
    int prec; // precison
    int ladjust; // left align
    char padc; // fill 0 in reductant space
    int type;

    int length;

    /*
        Exercise 1.5. Please fill in two parts in this file.
    */

    for(;;) {

        /* Part1: your code here */

        {
            /* scan for the next '%' */
                length = 0;
                while (*fmt != '%') {
                        buf[length++] = *fmt++;
                        if (*fmt == '\0')
                                break;
                }
            /* flush the string found so far */
                OUTPUT(arg, buf, length);
            /* check "are we hitting the end?" */
                if (*fmt == '\0')
                        break;
                fmt++;
        }
        /* we found a '%' */
        // check alignment and fill adc
        ladjust = 0;
        if (*fmt == '-') {
                ladjust = 1;
                fmt++;
        }
        padc = ' ';
        if (*fmt == '0')
        {
                padc = '0';
                fmt++;
        }

        width = 0;
        while (IsDigit(*fmt)) {
                width *= 10;
                width += Ctod(*fmt);
                fmt++;
        }

        prec = 0;
        if (*fmt == '.') {
                fmt++;
                while (IsDigit(*fmt)) {
                        prec *= 10;
                        prec += Ctod(*fmt);
                        fmt++;
                }
        }

        /* check for arraySize %#*/
        // get the size
        arraySize = 0;
        if (*fmt == '#') {
                fmt++;
                while (IsDigit(*fmt)) {
                        arraySize = arraySize*10 + Ctod(*fmt);
                        fmt++;
                }
        }

        type = 0;
        /* check %$1 or %$2 */
        if (*fmt == '$') {
                fmt++;
                while (IsDigit(*fmt)) {
                        type = type *10 + Ctod(*fmt);
                        fmt++;
                }
        }

        /* check for long */
        longFlag = 0;
        if (*fmt == 'l') { // it is "L" but not "1"
                longFlag = 1;
                fmt++;
        }
        /* check for other prefixes */

        /* check format flag */
        negFlag = 0; // would be used in PrintNum
        switch (*fmt) {
         case 'b':
            if (longFlag) {
                num = va_arg(ap, long int);
            } else {
                num = va_arg(ap, int);
            }
            length = PrintNum(buf, num, 2, 0, width, ladjust, padc, 0);
            OUTPUT(arg, buf, length);
            break;

         case 'd':
         case 'D':
            if (longFlag) {
                num = va_arg(ap, long int);
            } else {
                num = va_arg(ap, int);
            }

                /*  Part2:
                        your code here.
                        Refer to other part (case 'b',case 'o' etc.) and func PrintNum to complete this part.
                        Think the difference between case 'd' and others. (hint: negFlag).
                */
                if (num < 0) {
                        negFlag = 1;
                        num = -num;
                }
                length = PrintNum(buf, num, 10, negFlag, width, ladjust, padc, 0);
                OUTPUT(arg, buf, length);
                break;

         case 'o':
         case 'O':
            if (longFlag) {
                num = va_arg(ap, long int);
            } else {
                num = va_arg(ap, int);
            }
            length = PrintNum(buf, num, 8, 0, width, ladjust, padc, 0);
            OUTPUT(arg, buf, length);
            break;

         case 'u':
         case 'U':
            if (longFlag) {
                num = va_arg(ap, long int);
            } else {
                num = va_arg(ap, int);
            }
            length = PrintNum(buf, num, 10, 0, width, ladjust, padc, 0);
            OUTPUT(arg, buf, length);
            break;

         case 'x':
            if (longFlag) {
                num = va_arg(ap, long int);
            } else {
                num = va_arg(ap, int);
            }
            length = PrintNum(buf, num, 16, 0, width, ladjust, padc, 0);
            OUTPUT(arg, buf, length);
            break;

         case 'X':
            if (longFlag) {
                num = va_arg(ap, long int);
            } else {
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
            s = (char*)va_arg(ap, char *);
            length = PrintString(buf, s, width, ladjust);
            OUTPUT(arg, buf, length);
            break;

        case 'T':
                if (type == 1) {
                        a_s1 = (s1*)va_arg(ap, s1*);
                } else {
                        a_s2 = (s2*)va_arg(ap, s2*);
                }
                // output the '{'
                length = PrintChar(buf, '{', 1, 0);
                OUTPUT(arg, buf, length);

                if (type == 1) {
                // output int a, char b, char c, int d;
                        int a = a_s1->a;
                        char b = a_s1->b;
                        char c = a_s1->c;
                        int d = a_s1->d;

                        // print int a;
                        negFlag = 0;
                        if (a < 0) {
                                a = -a;
                                negFlag = 1;
                        }
                        length = PrintNum(buf, a, 10, negFlag, width, ladjust, padc, 0);
                        OUTPUT(arg, buf, length);

                        // print ','
                        length = PrintChar(buf, ',', 1, 0);
                        OUTPUT(arg, buf, length);

                        // print char b
                        length = PrintChar(buf, b, width, ladjust);
                        OUTPUT(arg, buf, length);

                        // print ','
                        length = PrintChar(buf, ',', 1, 0);
                        OUTPUT(arg, buf, length);

                        // print char c
                        length = PrintChar(buf, c, width, ladjust);
                        OUTPUT(arg, buf, length);

                        // print ','
                        length = PrintChar(buf, ',', 1, 0);
                        OUTPUT(arg, buf, length);

                        // print int d
                        negFlag = 0;
                        if (d < 0) {
                                d = -d;
                                negFlag = 1;
                        }
                        length = PrintNum(buf, d, 10, negFlag, width, ladjust, padc, 0);
                        OUTPUT(arg, buf, length);
                }
                else {
                        size = a_s2->size;
                        arr_int = a_s2->c;
                        for (var_for = 0; var_for < size; var_for++) {
                                int x = arr_int[var_for];
                                negFlag = 0;
                                if (x < 0) {
                                        x = -x;
                                        negFlag = 1;
                                }
                                length = PrintNum(buf, x, 10, negFlag, width, ladjust, padc, 0);
                                OUTPUT(arg, buf, length);

                                if (var_for != size-1) {
                                        length = PrintChar(buf, ',', 1, 0);
                                        OUTPUT(arg, buf, length);
                                }

                        }
                }
                // print '}'
                length = PrintChar(buf, '}', 1, 0);
                OUTPUT(arg, buf, length);
                break;

         case '\0':
            fmt --;
            break;

         default:
            /* output this char as it is */
            OUTPUT(arg, fmt, 1);
        }       /* switch (*fmt) */

        fmt ++;
    }           /* for(;;) */

    /* special termination call */
    OUTPUT(arg, "\0", 1);
}