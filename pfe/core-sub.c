/** 
 * --  Subroutines for the Core Forth-System
 * 
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE            @(#) %derived_by: guidod %
 *  @version %version: 5.19 %
 *    (%date_modified: Mon Mar 12 10:32:03 2001 %)
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) = 
"@(#) $Id: core-sub.c,v 0.30 2001-03-12 09:32:03 guidod Exp $";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-xtra.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>		/* access() if available */
#endif

#if defined HAVE_PWD_H
#include <pwd.h>		/* to resolve ~user/path */
#endif

#include <pfe/double-sub.h>
#include <pfe/block-sub.h>
#include <pfe/file-sub.h>
#include <pfe/term-sub.h>
#include <pfe/lined.h>
#include <pfe/_missing.h>

#include <pfe/logging.h>

#ifndef isascii 
#define isascii(X) ((unsigned char)(X) < 0x80)
#endif

/**
 * return cell-aligned address
 */
_export p4cell
p4_aligned (p4cell n)	
{
    while (!P4_ALIGNED (n))
        n++;
    return n;
}

/**
 * return double float-aligned address
 */
_export p4cell
p4_dfaligned (p4cell n)	
{
    while (!P4_DFALIGNED (n))
        n++;
    return n;
}

/* ********************************************************************** 
 *       miscellaneous execution semantics and runtimes         
 */

/**
 * (DICTVAR) forth-thread variable runtime, => VARIABLE like
 */
FCode (p4_dictvar_RT)
{
    *--SP = (p4cell) ((char *) p4TH + WP_PFA[0]);
}

/**
 * (DICTCONST) forth-thread variable runtime, => CONSTANT like
 */
FCode (p4_dictconst_RT)
{
    *--SP = *(p4cell *) ((char *) p4TH + WP_PFA[0]);
}

/* ********************************************************************* 
 *        strings          
 */

/**
 * push a C-string onto the SP runtime-stack, as if => S" string" was used
 */
_export void
p4_strpush (const char *s)
{
    if (s)
        *--SP = (p4cell)s, *--SP = strlen (s);
    else
        *--SP = 0, *--SP = 0;
}

/**
 * return the next pocket for interactive string input.
 */
_export char *
p4_pocket (void)
{
    char *p = PFE.pockets[PFE.pocket];

    PFE.pocket = (PFE.pocket + 1) % P4_opt.pockets;
    return p;
}

/**
 * chop off trailing spaces for the stringbuffer. returns the new length,
 * so for an internal counted string, use
   *s = p4_dash_trailing (s+1, *s);
 */
_export int
p4_dash_trailing (char *s, int n)
{
    while (n > 0 && isspace ((unsigned char) s[n - 1]))
    {
        n--;
    }
    return n;
}

#ifndef _export
# ifdef _P4_SOURCE
#  if defined HAVE_STRNCASECMP 
#   define p4_strncmpi strncasecmp
#  elif defined HAVE_STRNICMP
#   define p4_strncmpi strnicmp
#  else
    extern int p4_strncmpi ( const char *s1, const char* s2, int n); 
#  endif
#  ifndef strncmpi       /*cygwin32 has it already def'd*/
#   define strncmpi p4_strncmpi
#  endif
# endif
#endif

#ifndef p4_strncmpi 
int
p4_strncmpi (const char* p, const char* q, int n)
{
#if defined HAVE_STRNCASECMP
    return strncasecmp (p, q, n);
#elif defined HAVE_STRNICMP
    return strnicmp (p, q, n);
#else
    for(; n; --n )
    {
        if( !*p || !*q ) return p-q;
        if( toupper(*p) != toupper(*q) )
            return n; /* returns the differing tails, like bcmp */
        p++;
        q++;
    }
    return 0;
#endif /*HAVE_...*/
}
#endif /*strncmpi*/

/**
 * tolower() applied to a stringbuffer
 */
_export void
p4_lower (char *p, int n)
{
    while (--n >= 0)
        *p = tolower (*p), p++;
}

/**
 * toupper() applied to a stringbuffer
 */
_export void
p4_upper (char *p, int n)
{
    while (--n >= 0)
        *p = toupper (*p), p++;
}

/**
 * copy stringbuffer into a field as a zero-terminated string.
 */
_export char *
p4_store_c_string (const char *src, int n, char *dst, int max)
{
    if (n >= max)
        n = max - 1;
    memcpy (dst, src, n);
    dst[n] = '\0';
    return dst;
}

/**
 * copy stringbuffer into a field as a zero-terminated filename-string,
 * a shell-homdir like "~username" will be expanded, and the
 * platform-specific dir-delimiter is copied in on the fly ('/' vs. '\\')
 */
_export char* 
p4_store_filename (const char *src, int n, char* dst, int max)
{
    int s = 0;
    int d;
    char* p;
    
#  if PFE_DIR_DELIMITER == '\\'
#   define PFE_ANTI_DELIMITER '/'
#  else
#   define PFE_ANTI_DELIMITER '\\'
#  endif

    *dst = '\0';
    if (n && max > n && *src == '~') 
    {
	s = d = 1; 
	while (s < n && d < max && src[s] && src[s] != PFE_DIR_DELIMITER) 
	{ dst[d++] = src[s++]; }
	dst[d] = '\0';

	if (s == 1)
	{
	    p = getenv("HOME");
	    if (p && max > strlen(p)) { strcpy (dst, p); } 
	    /* else *dst = '\0'; */
	}else{
#         if HAVE_PWD_H
	    struct passwd *passwd = getpwnam (dst+1);
	    if (passwd && max > strlen (passwd->pw_dir))
		strcpy (dst, passwd->pw_dir);
	    else
#      endif
		*dst = PFE_DIR_DELIMITER; /* /user/restofpath */
	}
    }
    d = strlen (dst);

    while (d < max && s < n && src[s])
    {
	if (src[s] != PFE_ANTI_DELIMITER)
	    dst[d++] = src[s];
	else
	    dst[d++] = PFE_DIR_DELIMITER;
	s++;
    }
    dst[d] = '\0';
	
    return dst;
}

/**
 * a new pocket with the given filename as asciiz
 */
_export char*
p4_pocket_filename (const char* src, int n)
{
    return p4_store_filename (src, n, p4_pocket (), P4_POCKET_SIZE);
}

/* ********************************************************************** 
 *             expanding file names with paths and extensions      
 */

/*
 * Append all extensions from ext to nm.
 * Check if file exists, if so return true, else false.
 * the nm-string is of max. pocket_size.
 */
static int
try_extensions (char *nm, const char *ext)
{
    int vv,v;

    if (access (nm, F_OK) == 0)
	return 1;

    vv = strlen (nm);
    if (!ext || vv > P4_POCKET_SIZE-4) 
	return 0;

    while (*ext)
    {
	v = vv;
	while (*ext && *ext == PFE_PATH_DELIMITER)
	{ ext++; }
	do { nm[v++] = *ext++; }
	while (*ext && *ext != PFE_PATH_DELIMITER && v < P4_POCKET_SIZE-1);
	if (access (nm, F_OK) == 0)
	    return 1;
    }
    nm[vv] = '\0';
    return 0;
}

#if 0
/*
 * if the src-path starts with "~" then expand the homedir
 * and append the rest of the  path after the pathdelimiter.
 * In any case, the src-string is copied to the dst-string,
 * and the dst-string ist returned for further usage.
 */
static char*
strcpy_homedir (char* dst, const char* src)
{
    const char *s;
    char* d;
    if (*src != '~') { strcpy (dst, src); return dst; }

    s = src+1; d = dst+1; 
    while (*s && *s != PFE_DIR_DELIMITER) { *d++ = *s++; }
    *d = '\0';

    if (s == src+1)
    {
	d = getenv("HOME");
	if (d) { strcpy (dst, d); } else *dst = '\0';
    }else{
#      if HAVE_PWD_H
        struct passwd *passwd = getpwnam (dst+1);
        if (passwd)
	    strcpy (dst, passwd->pw_dir);
	else
#      endif
	    *dst = PFE_DIR_DELIMITER; /* /user/restofpath */
    }
    strcat (dst, s);
    return dst;
}
#endif

/*
 * nm    file name input
 * ln    file name length
 * paths search path for files
 * ext   default file extensions
 * return a pocket with the expanded filename
 */
_export char *
p4_pocket_expanded_filename (const char *nm, int ln, 
			     const char *paths, const char *exts)
{
    if (*nm == PFE_DIR_DELIMITER || *nm == '~')
    {
	char* path = p4_pocket ();
	p4_store_filename (nm, ln, path, P4_POCKET_SIZE);
        try_extensions (path, exts);
	return path;
    }else{
	char* path = p4_pocket ();
	char* pock;

	p4_store_filename (nm, ln, path, P4_POCKET_SIZE);
	if (try_extensions (path, exts))
	    return path;

	pock = p4_pocket ();
        while (*paths)
	{
            char *p = pock;

	    while (*paths && *paths == PFE_PATH_DELIMITER)
	    { paths++; }
	    if (!*paths) break;
	    do { *p++ = *paths++; }
	    while (*paths && *paths != PFE_PATH_DELIMITER);

	    if (p[-1] != PFE_DIR_DELIMITER) *p++ = PFE_DIR_DELIMITER;
	    if (ln + p-pock > P4_POCKET_SIZE) continue;
	    strncpy (p, nm, ln); 
	    p4_store_filename (pock, ln + p-pock, path, P4_POCKET_SIZE);
	    if (try_extensions (path, exts))
		return path;
	}

	p4_store_filename (nm, ln, path, P4_POCKET_SIZE);
	return path;
    }
}

/* ********************************************************************** 
 *        string comparision and pattern matching    
 */

/**
 * search for substring p2/u2 in string p1/u1 
 */
_export char *
p4_search (const char *p1, int u1, const char *p2, int u2)
{
    if (u2 == 0)
        return (char *) p1;
    if (u2 > u1)
        return NULL;
    u1 -= u2;
    for (;;)
    {
        char *p = (char *) memchr (p1, *p2, u1 + 1);

        if (p == NULL)
            return NULL;
        if (memcmp (p, p2, u2) == 0)
            return (char *) p;
        u1 -= p - p1;
        if (u1 == 0)
            return NULL;
        p1 = p + 1;
        u1--;
    }
}

/* match with a processed pattern, i.e. one without `\' escapes */
static int
do_match (const short *pattern, const char *string, int ic)
{
    int c;

    for (;;)
    {
        switch (c = *pattern++)
        {
         case '\0':
             return *string == '\0';
         case -'*':
             while (*string && !do_match (pattern, string, ic))
                 string++;
             continue;
         case -'?':
             if (*string++)
                 continue;
             return 0;
         default:
             if (!ic) 
             {
                 if (*string++ == c)
                     continue;
             }else{
                 if (tolower(*string++) == tolower(c))
                     continue;
             }
             return 0;
        }
    }
}


/**
 * Match string against pattern.
 * Pattern knows wildcards `*' and `?' and `\' to escape a wildcard.
 */
_export int
p4_match (const char *pattern, const char *string, int ic)
{
    short buf[0x100], *p = buf;

    /* preprocess pattern, remove `\' */
    for (;;)
    {
        int c = *(unsigned char *) pattern;

        pattern++;
        switch (c)
	{
         default:
             *p++ = c;
             continue;
         case '\0':
             *p = 0;
             break;
         case '?':
             *p++ = -'?';
             continue;
         case '*':
             *p++ = -'*';
             continue;
         case '\\':
             if (*pattern)
                 *p++ = *pattern++;
             else
                 *p++ = c;
             continue;
	}
        break;
    }
    /* match with preprocessed pattern */
    return do_match (buf, string, ic);
}

/* ------------------------------------------------------------- *
 * unsigned and floored divide and number i/o conversion                  
 */

/**
 * unsigned divide procedure, single prec 
 */
_export udiv_t
p4_udiv (p4ucell num, p4ucell denom)
{
    udiv_t res;

    res.quot = num / denom;
    res.rem = num % denom;
    return res;
}

/**
 * floored divide procedure, single prec 
 */
_export fdiv_t
p4_fdiv (p4cell num, p4cell denom)
{
    fdiv_t res;

    res.quot = num / denom;
    res.rem = num % denom;
    if (res.rem && (num ^ denom) < 0)
    {
        res.quot--;
        res.rem += denom;
    }
    return res;
}

/**
 * Divides *ud by denom, leaves result in *ud, returns remainder.
 * For number output conversion: dividing by BASE.
 */
_export p4ucell
p4_u_d_div (p4udcell *ud, p4ucell denom)
{
    p4udcell nom = *ud;
    udiv_t h;

    h = p4_udiv (P4xD0 (nom), denom);
    P4xD0 (*ud) = h.quot;
    P4xD0 (nom) = h.rem;
    h = p4_udiv (nom.hi, denom);
    P4xD1 (*ud) = h.quot;
    P4xD1 (nom) = h.rem;
    h = p4_udiv (P4xCELL (P4xD1 (nom), P4xD2 (nom)), denom);
    P4xD2 (*ud) = h.quot;
    P4xD2 (nom) = h.rem;
    h = p4_udiv (nom.lo, denom);
    P4xD3 (*ud) = h.quot;
    return h.rem;
}

/**
 * Computes *ud * w + c, where w is actually only half of a cell in size.
 * Leaves result in *ud.
 * For number input conversion: multiply by BASE and add digit.
 */
_export void
p4_u_d_mul (p4udcell *ud, p4ucell w, p4ucell c)
{
    c += P4xD3 (*ud) * w, P4xD3 (*ud) = P4xW1 (c), c >>= (sizeof(p4cell)*4);
    c += P4xD2 (*ud) * w, P4xD2 (*ud) = P4xW1 (c), c >>= (sizeof(p4cell)*4);
    c += P4xD1 (*ud) * w, P4xD1 (*ud) = P4xW1 (c), c >>= (sizeof(p4cell)*4);
    P4xD0 (*ud) = P4xD0 (*ud) * w + c;
}

/**
 * Get value of digit c into *n, return flag: valid digit.
 */
_export int
p4_dig2num (p4char c, p4ucell *n, p4ucell base)
{
    if (c < '0')
        return P4_FALSE;
    if (c <= '9')
        c -= '0';
    else
    {
        if (LOWER_CASE)
	c = toupper (c);
        if (c < 'A')
            return P4_FALSE;
        if (c <= 'Z')
            c -= 'A' - ('9' - '0' + 1);
        else
	{
            if (LOWER_CASE || c < 'a')
                return P4_FALSE;
            c -= 'a' - ('9' - '0' + 1) - ('Z' - 'A' + 1);
	}
    }
    if (c >= base)
        return P4_FALSE;
    *n = c;
    return P4_TRUE;
}

/**
 * make digit 
 */
_export char
p4_num2dig (p4ucell n)
{
    if (n < 10)
        return n + '0';
    if (n < 10 + 'Z' - 'A' + 1)
        return n - 10 + 'A';
    else
        return n - (10 + 'Z' - 'A' + 1) + 'a';
}

/**
 * insert into pictured numeric output string
 */
_export void
p4_hold (char c)
{
    if (p4_HLD <= (char *) DP)
        p4_throw (P4_ON_PICNUM_OVER);
    *--p4_HLD = c;
}

/**
 * try to convert into numer, see => >NUMBER
 */
_export const char *
p4_to_number (const char *p, p4ucell *n, p4udcell *d, p4ucell base)
{
#ifdef DEBUG /* good place to check some assertions (for debugging) */
    {
        auto p4udcell udbl;      
        auto p4ucell_hi_lo hilo; 
        memset(&udbl, 0, sizeof(udbl));
        memset(&hilo, 0, sizeof(hilo));
        if (sizeof(hilo) != sizeof(p4cell))
        { p4_outs(" {double-halfcell is not the size of cell} "); }
        if (sizeof(hilo.lo) != sizeof(p4cell)/2)
        { p4_outs(" {halfcell is not half the size of cell} "); }
        if (sizeof(hilo) != sizeof(udbl)/2)
        { p4_outs(" {double-halfcell is not half the size of double} "); }
        hilo.lo = 1;
        if ( (*(p4cell*)&hilo) != ((p4cell)1) )
        { p4_outs(" {double-halfcell is in incorrect (byteorder?)} "); }
        P4xD3(udbl) = 1;
        if ( udbl.lo != 1 )
        { p4_outs(" {double-lo-accessor is in incorrect (byteorder?)} "); }
        P4xD1(udbl) = 1;
        if ( udbl.hi != 1 )
        { p4_outs(" {double-hi-accessor is in incorrect (byteorder?)} "); }
    }
#endif

    for (; *n > 0; p++, --*n)
    {
        p4ucell c;
        
        if (!p4_dig2num (*p, &c, base))
            break;
        p4_u_d_mul (d, base, c);
        if (p4_DPL >= 0)
            p4_DPL++;
    }
    return p;
}

/*
 * Options controlling input and output:
 */

#ifndef USE_DOLLARHEX           /* USER-CONFIG: */
#define USE_DOLLARHEX    1      /* allow $XXX and %BBB input for hex and bin */
#endif

#ifndef PREFIX_HEX		/* USER-CONFIG: */
#define	PREFIX_HEX	'$'	/* 0 or prefix for input of hex numbers */
#endif

#ifndef PREFIX_BINARY		/* USER-CONFIG: */
#define	PREFIX_BINARY	'%'	/* 0 or prefix for input of binary numbers */
#endif

#ifndef PREFIX_DECIMAL		/* USER-CONFIG: */
#define	PREFIX_DECIMAL	'&'	/* 0 or prefix for input of decimal numbers */
#endif

#ifndef PREFIX_0x               /* USER-CONFIG: */
#define PREFIX_0x       1       /* 0x10 =16, 0X100 = 256 */
#endif
#ifndef PREFIX_0o               /* USER-CONFIG: */
#define PREFIX_0o       1   	/* 0o10 = 8, 0O100 = 64 */
#endif
#ifndef PREFIX_0b               /* USER-CONFIG: */
#define PREFIX_0b       1       /* 0b10 = 2, 0B100 = 4 */
#endif

/**
 * try to convert into number, see => ?NUMBER
 */
_export int
p4_number_question (const char *p, p4ucell n, p4dcell *d)
{
    p4ucell base = 0;
    int sign = 0;

    if (*p == '-') { p++; n--; sign = 1; }
  
#if USE_DOLLARHEX
    if (p4_FLOAT_INPUT && n > 1)
    {
        switch (*p)
	{
	case PREFIX_HEX:
	    base = 16; p++; n--;
	    break;
	case PREFIX_BINARY:
	    base = 2; p++; n--;
	    break;
	case PREFIX_DECIMAL:
	    base = 10; p++; n--;
	    break;
	}
    }

    if (*p == '-') { if (sign) { return 0; } else { p++; n--; sign = 1; } }
#endif
    
#if PREFIX_0x || PREFIX_0o || PREFIX_0b
    if( ! base && n > 2 && *p == '0' ) 
    { 
        switch(*(p+1))
        {
#      if (PREFIX_0x)
         case 'x':
         case 'X':
             if (BASE <= 10+'X'-'A') { base = 16; p+=2; n-=2; }
             break;
#      endif
#      if (PREFIX_0o)
         case 'o':
         case 'O':
             if (BASE <= 10+'O'-'A') { base = 8; p+=2; n-=2; }
             break;
#      endif
#      if (PREFIX_0b)
         case 'b':
         case 'B':
             if (BASE <= 10+'B'-'A') { base = 2; p+=2; n-=2; }
             break;
#      endif
        }
    }
#endif
    if (base == 0)
        base = BASE;

    d->lo = d->hi = 0;
    p4_DPL = -1;
    p = p4_to_number (p, &n, (p4udcell *) d, base);
    if (n == 0)
        goto happy;
    if (*p != '.') 	
        return 0;   	
    p4_DPL = 0;
    p++;
    n--;
    p = p4_to_number (p, &n, (p4udcell *) d, base);
    if (n != 0) 		
        return 0; 		
 happy:
    if (sign)
        p4_d_negate (d);
    return 1;
}

/**
 * This is for internal use only (SEE and debugger),
 * The real `UD.R' etc. words use HOLD and the memory area below PAD
 */
_export char *
p4_str_ud_dot_r (p4udcell ud, char *p, int w, int base)
{
    *--p = '\0';
    do {
        *--p = p4_num2dig (p4_u_d_div (&ud, base));
        w--;
    } while (ud.lo || ud.hi);

    while (w > 0) { *--p = ' '; w--; }
    return p;
}

/**
 * This is for internal use only (SEE and debugger),
 * The real `UD.R' etc. words use HOLD and the memory area below PAD
 */
_export char *
p4_str_d_dot_r (p4dcell d, char *p, int w, int base)
{
    int sign = 0;

    if (d.hi < 0)
        p4_d_negate (&d), sign = 1;
    *--p = '\0';

    do {
        *--p = p4_num2dig (p4_u_d_div ((p4udcell *) &d, base));
        w--;
    } while (d.lo || d.hi);

    if (sign) { *--p = '-'; w--; }
    while (w > 0) { *--p = ' '; w--; }
    return p;
}

/**
 * This is for internal use only (SEE and debugger),
 * The real `UD.R' etc. words use HOLD and the memory area below PAD
 */
_export char *
p4_str_dot (p4cell n, char *p, int base)
{
    p4dcell d;
    char *bl;

    *--p = '\0';
    bl = p - 1;
    d.lo = n;
    d.hi = n < 0 ? -1 : 0;
    p = p4_str_d_dot_r (d, p, 0, base);
    *bl = ' ';
    return p;
}

/* ********************************************************************** */
/* console i/o                                                            */
/* ********************************************************************** */

/**
 * emit single character
                            (output adjusting the OUT variable)
 */
_export void
p4_outc (char c)
{
    int x, y;

    p4_putc (c);
    p4_wherexy (&x, &y);
    OUT = x;
}

/**
 * type a string
                            (output adjusting the OUT variable)
 */
_export void
p4_outs (const char *s)		/* type a string */
{
    int x = 0, y = 0;

    p4_puts (s);
    p4_wherexy (&x, &y);
    OUT = x;
}
/**
 * type a string with formatting
                            (output adjusting the OUT variable)
 */
_export int
p4_outf (const char *s,...)
{
    char buf[0x200];
    va_list p;
    int r;

    va_start (p, s);
    r = vsprintf (buf, s, p);
    p4_outs (buf);
    va_end (p);
    return r;
}

/**
 * type counted string to terminal
                            (output adjusting the OUT variable)
 */
_export void
p4_type (const char *s, p4cell n)
{
    int x, y;
    
    while (--n >= 0)
        p4_putc_noflush (*s++);
    p4_wherexy (&x, &y);
    OUT = x;
    p4_put_flush ();
}

/**
 * type counted string to terminal, if it does not fit in full on
 * the current line, emit a => CR before
                            (output adjusting the OUT variable)
 */
_export void
p4_type_on_line (const char *s, p4cell n)
{
    if (OUT + n >= PFE.cols)
        FX (p4_cr);
    p4_type (s, n);
}

/**
 * type a string of chars, usually a string of spaces,
 * see => SPACES
                            (output adjusting the OUT variable)
 */
_export void
p4_emits (int n, const char c)
{
    int x, y;

    while (--n >= 0)
        p4_putc_noflush (c);
    fflush (stdout);
    p4_wherexy (&x, &y);
    OUT = x;
}

/**
 * type a string of space up to the next tabulator column
                            (output adjusting the OUT variable)
 */
_export void
p4_tab (int n)
{
    p4_emits (n - OUT % n, ' ');
}

/**
 */
_export void
p4_dot_line (p4_File *fid, p4cell n, p4cell l)
{
    char *p = p4_block (fid, n) + l * 64;
    p4_type (p, p4_dash_trailing (p, 64));
}

/**
 * input 
 */
static int
p4_get_line (char *p, p4cell n)
{
    char *q, buf[0x100];
    extern FCode (p4_bye);

    q = fgets (buf, n, stdin);
    if (q == NULL)
        FX (p4_bye);
    q = strrchr (q, '\n');
    if (q)
        *q = '\0';
    strcpy (p, buf);
    return strlen (p);
}

/** EXPECT counted string from terminal
 * no editing, no echo
 */
static int
p4_expect_canonical (char *p, p4cell n)	
{
    int i;		
    char c;
    int out = 0;
    
    for (i = 0; i < n;)
    {
        switch (c = p4_getkey ())
	{
         default:
             p[i++] = c; out++;
             continue;
         case '\t':
             while (i < n)
             {
                 p[i++] = ' '; out++;
                 if (out % 8 == 0)
                     break;
             }
             continue;
         case '\33':
         case '\r':
         case '\n':
             goto fin;
         case 127:
         case '\b':
             if (i <= 0)
                 continue;
             i--; out--;
             continue;
	}
    }
 fin:
    p[i] = 0;
    SPAN = i;
    return i;
}

/** EXPECT counted string from terminal
 * simple editing facility with backspace,
 * very traditional, use lined-like function instead! 
 */
_export int
p4_expect (char *p, p4cell n) 
{ 
    int i; 
    char c;

    if (P4_opt.canonical == 2)
        return p4_expect_canonical (p, n);
    if (P4_opt.canonical)
        return p4_get_line (p, n);
    for (i = 0; i < n;)
    {
        switch (c = p4_getkey ())
	{
         default:
             p[i++] = c;
             p4_outc (c);
             continue;
         case 27:
             for (; i > 0; i--)
                 FX (p4_backspace);
	  continue;
         case '\t':
             while (i < n)
             {
                 p[i++] = ' ';
                 FX (p4_space);
                 if (OUT % 8 == 0)
                     break;
             }
             continue;
         case '\r':
         case '\n':
             FX (p4_space);
             goto fin;
         case 127:
         case '\b':
             if (i <= 0)
             {
                 p4_dot_bell ();
                 continue;
             }
             i--;
             FX (p4_backspace);
             continue;
	}
    }
 fin:
    p[i] = 0;
    SPAN = i;
    return i;
}

/** 
 * better input facility using lined 
 */
_export int
p4_accept (char *p, int n) 
{
    if (P4_opt.canonical == 2)
        return p4_expect_canonical (p, n);
    if (P4_opt.canonical)
        return p4_get_line (p, n);
    PFE.accept_lined.string = p;
    PFE.accept_lined.max_length = n;
    p4_lined (&PFE.accept_lined, NULL);
    FX (p4_space);
    return PFE.accept_lined.length;
}

/** 
 * check for 'q' pressed 
 */
_export int
p4_Q_stop (void) 
{
    if (p4_ekeypressed ())
    {
        register int ch;  	
        ch = p4_getkey ();  
        if (tolower (ch) == 'q') return 1;
    }
    return 0;
}

/**
 * Like CR but stop after one screenful and return flag if 'q' pressed.
 * Improved by aph@oclc.org (Andrew Houghton)
 */
_export int
p4_Q_cr (void)
{
    static char more[] = "more? ";
    static char help[] = "\r[next line=<return>, next page=<space>, quit=q] ";

    FX (p4_cr);
    if (P4_opt.canonical)
        return 0;
    if (PFE.lines < PFE.more)
        return 0;
    PFE.lines = 0;
    for (;;)
    {
        register int ch;
        p4_outs (more);
        ch = p4_getkey (); 	/* tolower(..) may be a macro ! *gud*/
        switch (tolower (ch)) 
	{
         case 'n':		/* no more */
         case 'q':		/* quit    */
             return 1;
         case 'y':		/* more    */
         case ' ':		/* page    */
             while (OUT)
                 FX (p4_backspace);
             PFE.more = PFE.rows - 1;
             return 0;
         case '\r':		/* line    */
         case '\n':		/* line    */
             while (OUT)
                 FX (p4_backspace);
             PFE.more = 1;
             return 0;
         default:		/* unknown */
             p4_dot_bell ();
             /* ... */
         case '?':		/* help    */
         case 'h':		/* help    */
             p4_outs (help);
             break;
	}
    }
}

/* ********************************************************************** */

#ifndef _export
# ifndef HASNT_SYSTEM
  _extern int p4_systemf (const char* s, ...);
# endif
#endif

#ifndef NO_SYSTEM
/**
 * issue a system() call, after formatting
 */
/*export*/ int
p4_systemf (const char *s,...)
{
    char buf[0x100];
    va_list p;
    int r;

    va_start (p, s);
    vsprintf (buf, s, p);
    va_end (p);
    if (! PFE_set.bye) p4_system_terminal ();
    p4_swap_signals ();
    r = system (buf);
    p4_swap_signals ();
    if (! PFE_set.bye) p4_interactive_terminal ();
    p4_dot_normal ();
    return r;
}
#endif /* NO_SYSTEM */

/* ********************************************************************** 
 * source input								  
 */

/**
 * source input:  read from terminal 
 */
FCode (p4_query)
{
    SOURCE_ID = 0;
    BLK = 0;
    TO_IN = 0;
    TIB = PFE.tib;
    NUMBER_TIB = p4_accept (TIB, TIB_SIZE);
    if (PFE.query_hook) 
        NUMBER_TIB = (*PFE.query_hook)(NUMBER_TIB); 
    SPAN = NUMBER_TIB;
}

/**
 * source input: read from text-file 
 */
_export int
p4_next_line (void)
{
    p4cell ior;
    p4ucell len;
    
    len = sizeof SOURCE_FILE->buffer;
    if (!p4_read_line (SOURCE_FILE->buffer, &len, SOURCE_FILE, &ior))
    {
        SOURCE_FILE->len = len;
        return 0;
    }
    TIB = SOURCE_FILE->buffer;
    NUMBER_TIB = SOURCE_FILE->len = len;
    BLK = 0;
    TO_IN = 0;
    return 1;
}

/** 
 * SOURCE - dispatch input source 
 */
_export void
p4_source (char **p, int *n)
{
    switch (SOURCE_ID)
    {
     case -1:			/* string from EVALUATE */
         *p = TIB;			
         *n = NUMBER_TIB;
         break;
     case 0:			/* string from QUERY or BLOCK */
         if (BLK)
         {
             *p = p4_block (BLOCK_FILE, BLK);
             *n = BPBUF;
         }else{                       
             *p = TIB;
             *n = NUMBER_TIB;
         }
         break;
     default:			/* source line from text file */
         *p = SOURCE_FILE->buffer; 
         *n = SOURCE_FILE->len;
    }
}

/**
 * SAVE-INPUT
 */
_export void *
p4_save_input (void *p)
{
    Iframe *iframe = (Iframe *) p;
    
    --iframe;
    iframe->magic = INPUT_MAGIC;
    iframe->input = PFE.input;
    iframe->prev = PFE.saved_input;
    PFE.saved_input = iframe;
    
    return ((void*) iframe);
}

/**
 * RESTORE-INPUT
 */
_export void *
p4_restore_input (void *p)
{
    Iframe *iframe = (Iframe *) p;

    if (iframe->magic != INPUT_MAGIC)
        p4_throw (P4_ON_ARG_TYPE);
    PFE.input = iframe->input;
    PFE.saved_input = iframe->prev;
    ++iframe;

    p = (void *) iframe;
    return p;
}
/**
 * REFILL
 */
_export int
p4_refill (void)
{
    switch (SOURCE_ID)
    {
     case -1:
         return 0;
     case 0:
         if (BLK)
         {
             BLK++;
             TO_IN = 0;
         }else{
             FX (p4_query);
         }
         return 1;
    default:
        return p4_next_line ();
    }
}
/**
 * SKIP-DELIMITER
 */
_export void
p4_skip_delimiter (char del)
{
    char *q;
    int i, n;

    p4_source (&q, &n);
    if (del == ' ')
    {
        for (i = TO_IN;
             i < n && isascii (q[i]) && isspace ((unsigned char) q[i]);
             i++)
        {
            ;
        }
    }else{
        for (i = TO_IN; i < n && q[i] == del; i++)
        {
            ;
        }
    }
    TO_IN = i;
}

/**
 * PARSE
 */
_export int
p4_parse (char del, char **p, p4ucell *l)
{
    char *q;
    int i, n; 

    p4_source (&q, &n);
    *p = q + TO_IN;
    if (del == ' ')
    {
        for (i = TO_IN;
             i < n && !(isascii (q[i]) && isspace ((unsigned char) q[i]));
             i++)
        {
            ;
        }
    }else{
        for (i = TO_IN; i < n && q[i] != del; i++)
        {
            ;
        }
    }
    *l = i - TO_IN;
    if (i == n)
    {
        TO_IN = i;
        return 0;
    }else{
        TO_IN = i + 1;
        return 1;
    }
}

/**
 * WORD
 */
_export char *
p4_word (char del)
{
    char *p, *q;
    int n, i;

    p4_skip_delimiter (del);
    p4_source (&q, &n);
    q += TO_IN;
    n -= TO_IN;
    p = (char *) DP + 1;
    if (del == ' ')
    {
        for (i = 0; 
             i < n && !(isascii (*q) && isspace ((unsigned char) *q)); i++)
        {
            *p++ = *q++;
        }
    }else{
        for (i = 0; i < n && *q != del; i++)
        {
            *p++ = *q++;
        }
    }
    TO_IN += i + (i < n);
    *p = '\0';
    if (i > 255)
    {
        p4_throw (P4_ON_PARSE_OVER);
    }
    *DP = i;
    return (char *) DP;
}

/*
 * PARSE-WORD
 *
 * return and args mean the same as for ~parse, 
 * but it really scans like ~word. It most cases you can replace
 * word(.) with a sequence of p4_parseword (.) and p4_hereword (.);
 * The point is, that p4_parseword _doesn't_ copy the next word onto
 * here, it just returns the pointers. In some cases, esp. where
 * a failure could be p4_thrown , it must be copied to here later.
 * You can use p4_hereword (.) for that. See interpret() for an example.
 */
_export int
p4_parseword (char del, char** p, p4ucell* len)
{
    char *q;
    int n, i;
  
    p4_skip_delimiter (del);
    p4_source (&q, &n);	
    q += TO_IN;
    n -= TO_IN;
    *p = q;
    if (del == ' ')
    {
        for (i = 0; 
             i < n && !(isascii (*q) && isspace ((unsigned char) *q)); i++)
        {
            q++;
        }
    }else{
        for (i = 0; i < n && *q != del; i++)
        {
            q++;
        }
    }
    *len = i;
    TO_IN += i + (i<n);

    return (i<n);
}

/**
 * complement p4_parseword to  arrive at the normal => WORD implementation
 */
_export char*
p4_hereword (char* p, p4ucell len)
{
    if (len > 255)
        p4_throw (P4_ON_PARSE_OVER);
    memcpy (DP+1, p, len);
    *DP = len;
    return DP;
}

/* ------------------------------------------------------------------------- */

/**
 * ABORT" string" impl.
 */
_export void
p4_abortq (const char *fmt,...)
{
    char buf[128];
    int n;
    va_list p;

    va_start (p, fmt);
    n = vsprintf (buf, fmt, p);
    va_end (p);
    p4_throws (P4_ON_ABORT_QUOTE, buf, n);
}

/**
 * ?PAIRS
 */
_export void
p4_Q_pairs (p4cell n)
{
    if (n != *SP++)
        p4_throw (P4_ON_CONTROL_MISMATCH);
}

/**
 * ?OPEN
 */
_export void
p4_Q_file_open (p4_File *fid)
{
    if (fid == NULL || fid->f == NULL)
        p4_throw (P4_ON_FILE_NEX);
}

/*@}*/





