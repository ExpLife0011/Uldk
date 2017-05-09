/* misc.h - prototypes for misc functions */
/*
*  GRUB  --  GRand Unified Bootloader
*  Copyright (C) 2002,2003,2005,2006,2007,2008,2009,2010  Free Software Foundation, Inc.
*
*  GRUB is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GRUB is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GRUB_MISC_HEADER
#define GRUB_MISC_HEADER	1

#include <stdarg.h>
#include <grub/types.h>
//#include <grub/symbol.h>
#include "err.h"
//#include <grub/i18n.h>
//#include <grub/compiler.h>

#define ALIGN_UP(addr, align) \
	((addr + (typeof (addr)) align - 1) & ~((typeof (addr)) align - 1))
#define ALIGN_UP_OVERHEAD(addr, align) ((-(addr)) & ((typeof (addr)) (align) - 1))
#define ALIGN_DOWN(addr, align) \
	((addr) & ~((typeof (addr)) align - 1))
#define ARRAY_SIZE(array) (sizeof (array) / sizeof (array[0]))
#define COMPILE_TIME_ASSERT(cond) switch (0) { case 1: case !(cond): ; }

static inline char *
grub_strncpy(char *dest, const char *src, int c)
{
	char *p = dest;

	while ((*p++ = *src++) != '\0' && --c)
		;

	return dest;
}

static inline char *
grub_stpcpy(char *dest, const char *src)
{
	char *d = dest;
	const char *s = src;

	do
		*d++ = *s;
	while (*s++ != '\0');

	return d - 1;
}

/* Copied from gnulib.
Written by Bruno Haible <bruno@clisp.org>, 2005. */
static inline char *
grub_strstr(const char *haystack, const char *needle)
{
	/* Be careful not to look at the entire extent of haystack or needle
	until needed.  This is useful because of these two cases:
	- haystack may be very long, and a match of needle found early,
	- needle may be very long, and not even a short initial segment of
	needle may be found in haystack.  */
	if (*needle != '\0')
	{
		/* Speed up the following searches of needle by caching its first
		character.  */
		char b = *needle++;

		for (;; haystack++)
		{
			if (*haystack == '\0')
				/* No match.  */
				return 0;
			if (*haystack == b)
				/* The first character matches.  */
			{
				const char *rhaystack = haystack + 1;
				const char *rneedle = needle;

				for (;; rhaystack++, rneedle++)
				{
					if (*rneedle == '\0')
						/* Found a match.  */
						return (char *)haystack;
					if (*rhaystack == '\0')
						/* No match.  */
						return 0;
					if (*rhaystack != *rneedle)
						/* Nothing in this round.  */
						break;
				}
			}
		}
	}
	else
		return (char *)haystack;
}

static inline int
grub_isprint(int c)
{
	return (c >= ' ' && c <= '~');
}

static inline int
grub_iscntrl(int c)
{
	return (c >= 0x00 && c <= 0x1F) || c == 0x7F;
}

static inline int
grub_isalpha(int c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline int
grub_islower(int c)
{
	return (c >= 'a' && c <= 'z');
}

static inline int
grub_isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}

static inline int
grub_isgraph(int c)
{
	return (c >= '!' && c <= '~');
}

static inline int
grub_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

static inline int
grub_isxdigit(int c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline int
grub_isalnum(int c)
{
	return grub_isalpha(c) || grub_isdigit(c);
}

static inline int
grub_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 'a';

	return c;
}

static inline int
grub_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 'A';

	return c;
}

static inline int
grub_strcasecmp(const char *s1, const char *s2)
{
	while (*s1 && *s2)
	{
		if (grub_tolower((UINT8)*s1)
			!= grub_tolower((UINT8)*s2))
			break;

		s1++;
		s2++;
	}

	return (int)grub_tolower((UINT8)*s1)
		- (int)grub_tolower((UINT8)*s2);
}

static inline int
grub_strncasecmp(const char *s1, const char *s2, UINT64 n)
{
	if (n == 0)
		return 0;

	while (*s1 && *s2 && --n)
	{
		if (grub_tolower(*s1) != grub_tolower(*s2))
			break;

		s1++;
		s2++;
	}

	return (int)grub_tolower((UINT8)*s1)
		- (int)grub_tolower((UINT8)*s2);
}

static inline char *
grub_memchr(const void *p, int c, UINT64 len)
{
	const char *s = (const char *)p;
	const char *e = s + len;

	for (; s < e; s++)
		if (*s == c)
			return (char *)s;

	return 0;
}


static inline unsigned int
grub_abs(int x)
{
	if (x < 0)
		return (unsigned int)(-x);
	else
		return (unsigned int)x;
}


#define grub_max(a, b) (((a) > (b)) ? (a) : (b))
#define grub_min(a, b) (((a) < (b)) ? (a) : (b))

#endif /* ! GRUB_MISC_HEADER */
