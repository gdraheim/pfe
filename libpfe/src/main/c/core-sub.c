/**
 * --  Subroutines for the Core Forth-System
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001.
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author$)
 *  @version $Revision$
 *     (modified $Date$)
 *
 *  @description
 *         Subroutines for the Forth Core System - especially the
 *         general input/output routines like ACCEPT/QUERY/WORD/PARSE
 *         and converters like UD.DR and >NUMBER
 */
/*@{*/
#if defined(__version_control__) && defined(__GNUC__)
static char* id __attribute__((unused)) =
"@(#) $Id$";
#endif

#define _P4_SOURCE 1

#include <pfe/pfe-base.h>
#include <pfe/def-limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <pfe/os-string.h>
#include <pfe/os-ctype.h>

#ifdef PFE_HAVE_UNISTD_H
#include <unistd.h>		/* access() if available */
#endif

#if defined PFE_HAVE_PWD_H
#include <pwd.h>		/* to resolve ~user/path */
#endif

#include <pfe/double-sub.h>
#include <pfe/block-sub.h>
#include <pfe/file-sub.h>
#include <pfe/term-sub.h>
#include <pfe/lined.h>
#include <pfe/_missing.h>

#include <pfe/logging.h>

extern void FXCode(p4_bye);

/***********************************************************************/

/* removed this one from general def-types */
#define SPAN    p4_SPAN

/**
 * return cell-aligned address
 */
P4_GCC_CONST p4cell
p4_aligned (p4cell n)
{
    while (!P4_ALIGNED (n))
        n++;
    return n;
}

/* *********************************************************************
 *        strings
 */

/** _strpush_ ( zstr* -- S: str* str# )
 * push a C-string onto the SP runtime-stack, as if => S" string" was used
 : _strpush_ s! _strlen_ s! ;
 */
void
p4_strpush (const char *s)
{
    if (s) {
        *--SP = (p4cell)s; *--SP = p4_strlen (s);
    } else {
        *--SP = 0; *--SP = 0;
    }
}

/** _pocket_ ( -- str* )
 * return the next pocket for interactive string input.
 : _pocket_ _pockets@_ _pocket@_ th  _pocket@_ 1+ _pockets#_ mod to _pocket@_ ;
 */
P4_GCC_MALLOC void*
p4_pocket (void)
{
    register void *p = PFE.pocket->buffer;
    if (++ PFE.pocket >= PFE.pockets_top) PFE.pocket = PFE.pockets_ptr;
    return p;
}

/** _-trailing_ ( str* str# -- str#' )
 * chop off trailing spaces for the stringbuffer. returns the new length,
 * so for an internal counted string, use
   <x> dup count _-trailing_ c!
   : _-trailing_ begin dup while
      2dup + c@ bl <> if nip exit then
      1- repeat nip ;
 */
P4_GCC_WARN_UNUSED_RESULT int
p4_dash_trailing (p4_char_t *s, int n)
{
    while (n > 0 && p4_isspace (s[n - 1]))
    {
        n--;
    }
    return n;
}

/** _lower_ ( str* str# -- )
 * _tolower_ applied to a stringbuffer
 : _lower_ 0 do dup c@ _tolower_ over c! 1+ loop drop ;
 */
void
p4_lower (p4_char_t *p, int n)
{
    while (--n >= 0)
    {
        *p = (p4_char_t) tolower ((char) *p);
        p++;
    }
}

/** _upper_ ( str* str# -- )
 * _toupper_ applied to a stringbuffer
 : _upper_ 0 do dup c@ _toupper_ over c! 1+ loop drop ;
 */
void
p4_upper (p4_char_t *p, int n)
{
    while (--n >= 0)
    {
        *p = (p4_char_t) toupper ((char) *p);
        p++;
    }
}

/** _zplaced_ ( str* str# dst* max# -- dst* ) [alias] _store_c_string_
 * copy stringbuffer into a field as a zero-terminated string.
 : _zsplaced_ rot 2dup > if drop 1- else nip then _zplace_ ;
 */
char*
p4_store_c_string (const p4_char_t *src, int n, char *dst, int max)
{
    /* RENAME: p4_zplaced */
    if (n >= max)
        n = max - 1;
    p4_memcpy (dst, src, n);
    dst[n] = '\0';
    return dst;
}

/** _pocket_zplaced ( str* str# -- pocket* ) [alias] _pocket_c_string_
 * store a string-span as a zero-terminated string into another pocket-pad
 : _pocket_zplaced _pocket_ _/pocket_ _zplaced_ ;
*/
P4_GCC_MALLOC char*
p4_pocket_c_string (const p4_char_t* src, int n)
{
    /* RENAME: p4_pocket_zplace */ /* REQUIRE: p4_pocket_place */
    return p4_store_c_string (src, n, p4_pocket (), P4_POCKET_SIZE);
}

/** _zplaced_filename_ ( str* str# dst* max# -- dst* ) [alias] _store_filename_
 * copy stringbuffer into a field as a zero-terminated filename-string,
 * a shell-homedir like "~username" will be expanded, and the
 * platform-specific dir-delimiter is converted in on the fly ('/' vs. '\\')
 */
char*
p4_store_filename (const p4_char_t* str, int n, char* dst, int max)
{
    /* RENAME: p4_zplace_filename */
    int s = 0;
    int d;
    char* p;
    char* src = (char*) str;

    if (!src || !n) { *dst = '\0'; return dst; }

#  if PFE_DIR_DELIMITER == '\\'
#   define PFE_ANTI_DELIMITER '/'
#  else
#   define PFE_ANTI_DELIMITER '\\'
#  endif

# define PFE_HOMEDIR_CHAR '~'

    *dst = '\0';
    if (n && max > n && *src == PFE_HOMEDIR_CHAR)
    {
        s = d = 1;
        while (s < n && d < max && src[s] && src[s] != PFE_DIR_DELIMITER)
        { dst[d++] = src[s++]; }
        dst[d] = '\0';

        if (s == 1)
        {
            p = getenv("HOME");
            if (p && max > p4_strlen(p)) { p4_strcpy (dst, p); }
            /* else *dst = '\0'; */
        }else{
#         if PFE_HAVE_PWD_H
            struct passwd *passwd = getpwnam (dst+1);
            if (passwd && max > p4_strlen (passwd->pw_dir))
                p4_strcpy (dst, passwd->pw_dir);
            else
#      endif
                *dst = PFE_DIR_DELIMITER; /* /user/restofpath */
        }
    }
    d = p4_strlen (dst);

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

/** _pocket_fileame_ ( str* str# -- dst* )
 * a new pocket with the given filename as asciiz
 : _pocket_filename_ _pocket_ /pocket _zplaced_filename_
 */
P4_GCC_MALLOC char*
p4_pocket_filename (const p4_char_t* src, int n)
{
    /* RENAME: p4_pocket_zplace_filename */
    return p4_store_filename (src, n, p4_pocket (), P4_POCKET_SIZE);
}

/* **********************************************************************
 *             expanding file names with paths and extensions
 */

/* <try-extensions> ( zstr* zext* -- ?ok )
 * Append all extensions from ext to nm (which is assumed to be like a POCKET)
 * Check if file exists, if so return true, else false.
 * The nm-string is expected to be writeable up to max. pocket_size.
 */
static int
try_extensions (char* nm, const char *ext)
{
    if (_P4_access (nm, F_OK) == 0)
        return 1;

    int vv = p4_strlen (nm);
    if (!ext || vv > P4_POCKET_SIZE-4)
        return 0;

    while (*ext)
    {
        int v = vv;
        while (*ext && *ext == PFE_PATH_DELIMITER)
        { ext++; }
        do { nm[v++] = *ext++; }
        while (*ext && *ext != PFE_PATH_DELIMITER && v < P4_POCKET_SIZE-1);
        nm[v] = '\0';
        if (_P4_access (nm, F_OK) == 0)
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
    if (*src != PFE_HOMEDIR_CHAR) { p4_strcpy (dst, src); return dst; }

    const char* s = src+1; char* d = dst+1;
    while (*s && *s != PFE_DIR_DELIMITER) { *d++ = *s++; }
    *d = '\0';

    if (s == src+1)
    {
        d = getenv("HOME");
        if (d) { p4_strcpy (dst, d); } else *dst = '\0';
    }else{
#      if PFE_HAVE_PWD_H
        struct passwd *passwd = getpwnam (dst+1);
        if (passwd)
            p4_strcpy (dst, passwd->pw_dir);
        else
#      endif
            *dst = PFE_DIR_DELIMITER; /* /user/restofpath */
    }
    p4_strcat (dst, s);
    return dst;
}
#endif

/** _pocket-expanded-filename_ ( str* str# zpaths* zexts* -- dst* )
 * str*,str#  file name input
 * paths search path for files (a delimited series of dirname prefixes )
 * ext   default file extensions (a delimited series of ext suffixes )
 * -> result in a pocket with the expanded filename, basically operate
 * as foreach dirname prefix run => <store-filename> to expand shellparticles
 * and if then => <try-extensions> returns true then return that one. If no
 * file was found to exist that way then just <store-filename> and return.
 */
char*
p4_pocket_expanded_filename (const p4_char_t *nm, int ln,
                             const char *paths, const char *exts)
{
    if (*nm == PFE_DIR_DELIMITER || *nm == PFE_HOMEDIR_CHAR)
    {
        char* path = p4_pocket ();
        p4_store_filename (nm, ln, path, P4_POCKET_SIZE);
        try_extensions (path, exts);
        return path;
    }else{
        char* path = p4_pocket ();

        p4_store_filename (nm, ln, path, P4_POCKET_SIZE);
        if (try_extensions (path, exts))
            return path;

        char* pock = p4_pocket ();
        while (*paths)
        {
            char *p = pock;

            while (*paths && *paths == PFE_PATH_DELIMITER)
            { paths++; }
            if (!*paths) break;
            do { *p++ = *paths++; }
            while (*paths && *paths != PFE_PATH_DELIMITER);

            if (p[-1] != PFE_DIR_DELIMITER) *p++ = PFE_DIR_DELIMITER;
            int fill = ln + (p - pock);
            if (fill > P4_POCKET_SIZE) continue;
            p4_strncpy (p, (char*) nm, ln);
            p4_store_filename ((p4_char_t*) pock, fill, path, P4_POCKET_SIZE);
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

/** _search_ ( str* str# key* key# -- 0 | key-in-str* )
 * search for substring p2/u2 in string p1/u1, returns null if not found
 * or a pointer into str*,str# that has lenght of key#
 */
char *
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
        if (p4_memcmp (p, p2, u2) == 0)
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
do_match (const short *pattern, const p4char *str, int len, int uppermax)
{
    int c;
    const p4char* end = str+len;

    for (; str < end; /*str++*/)
    {
        --uppermax;
        switch (c = *pattern++)
        {
         case '\0':
             break;
         case -'*':
             while (*str && end-str && !do_match (pattern, str, end-str, uppermax))
             { --uppermax; str++; }
             continue;
         case -'?':
             if (*str++)
                 continue;
             return 0;
         default:
             if (uppermax < 0)
             {
                 if (*str++ == c)
                     continue;
             }else{
                 if (*str == c || *str == toupper(c))
                 { str++; continue; }
             }
             return 0;
        }
    }
    return str == end || *str == '\0';
}


/** _match_ ( zpattern* zstring* ignorecase? -- yes? )
 * Match string against pattern.
 * Pattern knows wildcards `*' and `?' and `\' to escape a wildcard.
 */
int
p4_match (const p4char *pattern, const p4char *str, int len, int ic)
{
    /* RENAME: p4_wild_match - move near p4_wild_words - possibly export */
    short preprocessed[POCKET_SIZE], *p = preprocessed;

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
#  define UPPERMAX 32
    return do_match (preprocessed, str, len, (ic ? UPPERMAX : 0));
}

/* _________________________________________________________________________
 * unsigned and floored divide and number i/o conversion
 */

/** _U/_
 * unsigned divide procedure, single prec
 */
P4_GCC_CONST udiv_t
p4_udiv (p4ucell num, p4ucell denom)
{
    udiv_t res;

    res.quot = num / denom;
    res.rem = num % denom;
    return res;
}

/** _/_
 * floored divide procedure, single prec
 */
P4_GCC_CONST fdiv_t
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

/** _ud/_
 * Divides *ud by denom, leaves result in *ud, returns remainder.
 * For number output conversion: dividing by BASE.
 */
p4ucell
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

/** _ud*_
 * Computes *ud * w + c, where w is actually only half of a cell in size.
 * Leaves result in *ud.
 * For number input conversion: multiply by BASE and add digit.
 */
void
p4_u_d_mul (p4udcell *ud, p4ucell w, p4ucell c)
{
    c += P4xD3 (*ud) * w, P4xD3 (*ud) = P4xW1 (c), c >>= (sizeof(p4cell)*4);
    c += P4xD2 (*ud) * w, P4xD2 (*ud) = P4xW1 (c), c >>= (sizeof(p4cell)*4);
    c += P4xD1 (*ud) * w, P4xD1 (*ud) = P4xW1 (c), c >>= (sizeof(p4cell)*4);
    P4xD0 (*ud) = P4xD0 (*ud) * w + c;
}

/** _dig>num_ ( c n* base -- ?ok )
 * Get value of digit c into *n, return flag: valid digit.
 */
int
p4_dig2num (p4_char_t c, p4ucell *n, p4ucell base)
{
    if (c < '0')
        return P4_FALSE;
    if (c <= '9')
        c -= '0';
    else
    {
        if (UPPER_CASE)
        c = toupper (c);
        if (c < 'A')
            return P4_FALSE;
        if (c <= 'Z')
            c -= 'A' - ('9' - '0' + 1);
        else
        {
            if (UPPER_CASE || c < 'a')
                return P4_FALSE;
            c -= 'a' - ('9' - '0' + 1) - ('Z' - 'A' + 1);
        }
    }
    if (c >= base)
        return P4_FALSE;
    *n = c;
    return P4_TRUE;
}

/** _num2dig_ ( val -- c )
 * make digit
 */
P4_GCC_CONST char
p4_num2dig (p4ucell n)
{
    if (n < 10)
        return n + '0';
    if (n < 10 + 'Z' - 'A' + 1)
        return n - 10 + 'A';
    else
        return n - (10 + 'Z' - 'A' + 1) + 'a';
}

/** _hold_ ( c -- )
 * insert into pictured numeric output string
 */
void
p4_hold (char c)
{
    if (p4_HLD <= HERE)
        p4_throw (P4_ON_PICNUM_OVER);
    *--p4_HLD = c;
}

/** _>number_
 * try to convert into numer, see => >NUMBER
 */
const p4_char_t *
p4_to_number (const p4_char_t *p, p4ucell *n, p4udcell *d, p4ucell base)
{
#ifdef DEBUG /* good place to check some assertions (for debugging) */
    {
        auto p4udcell udbl;
        auto p4ucell_hi_lo hilo;
        p4_memset(&udbl, 0, sizeof(udbl));
        p4_memset(&hilo, 0, sizeof(hilo));
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
#define	PREFIX_DECIMAL	'#'	/* 0 or prefix for input of decimal numbers */
#define PREFIX_DECIMAL_OLD '&'
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

/** _?number_ ( str* str# dcell* -- ?ok )
 * try to convert into number, see => ?NUMBER
 */
int
p4_number_question (const p4_char_t *p, p4ucell n, p4dcell *d)
{
    p4ucell base = 0;
    int sign = 0;
#  ifdef PREFIX_DECIMAL_OLD
    p4_bool_t old_decimal_prefix = P4_FALSE;
#  endif

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
#     ifdef PREFIX_DECIMAL_OLD
        case PREFIX_DECIMAL_OLD:
            old_decimal_prefix = P4_TRUE;
            base = 10; p++; n--;
            break;
#      endif
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

#  ifdef PREFIX_DECIMAL_OLD
    {   /* TODO: remove PREFIX_DECIMAL_OLD in pfe-34 */
        static int shown = 0;
        if (old_decimal_prefix && ! shown && REDEFINED_MSG) {
            p4_outf ("\n> oops, a usage of the old decimal prefix '%c' was detected,", PREFIX_DECIMAL_OLD);
            p4_outf ("\n> need to change it to the forth200x new decimal prefix '%c'", PREFIX_DECIMAL);
            FX (p4_cr_show_input);
            shown ++;
        }
    }
#  endif

    return P4_TRUE;
}

/** _ud.r_ ( d,d str* str# base -- str* )
 * This is for internal use only (SEE and debugger),
 * The real => UD.R etc. words uses => HOLD and the memory area below => PAD
 */
char *
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

/** _d.r_ ( d,d str* str# base -- str* )
 * This is for internal use only (SEE and debugger),
 * The real => UD.R etc. words use => HOLD and the memory area below => PAD
 */
char *
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

/** _._ ( i str* str# base -- str* )
 * This is for internal use only (SEE and debugger),
 * The real => . etc. words use => HOLD and the memory area below => PAD
 */
char *
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

/** _outc_ ( char -- ) [alias] _outc
 * emit single character,
 * (output adjusting the => OUT variable, see => _putc_ to do without)
 : _emit_ _putc_ _?xy_ drop out ! ;
 */
void
p4_outc (char c)
{
    int x, y;

    p4_putc (c);
    p4_wherexy (&x, &y);
    p4_OUT = x;
}

/** _ztype_ ( zstr* -- ) [alias] _outs
 * type a string
 * (output adjusting the => OUT variable, see => _puts_ to do without)
 : _ztype_ _puts_ _?xy_ drop out ! ;
 */
void
p4_outs (const char *s)
{
    int x = 0, y = 0;

    p4_puts (s);
    p4_wherexy (&x, &y);
    p4_OUT = x;
}

/** _outf_ ( ... zstr* -- n# )
 * type a string with formatting
 * (output adjusting the => OUT variable, see => _puts_ and => _outs_ )
 : _outf_ 0x200 lbuffer: buf[]  buf[] _vsprintf_  buf[] _outs_ ;
 */
P4_GCC_PRINTF int
p4_outf (const char *s,...)
{
    char buf[P4_PIPE_BUF];
    va_list p;
    int r;

    va_start (p, s);
    r = vsprintf (buf, s, p);
    p4_outs (buf);
    va_end (p);
    return r;
}

/** _type_ ( str* str# -- )
 * type counted string to terminal
 * (output adjusting the => OUT variable, see => _puts_ and => _outs_ )
 : _type_ 0 do c@++ _putc_ loop drop _flush_ _?xy drop out ! ;
 */
void
p4_type (const p4_char_t *str, p4cell len)
{
    int x, y; const char* s = (const char*) str;

    while (--len >= 0)
        p4_putc_noflush (*s++);
    p4_wherexy (&x, &y);
    p4_OUT = x;
    p4_put_flush ();
}

/** _typeline_ ( str* str# -- )
 * type counted string to terminal, if it does not fit in full on
 * the current line, emit a => CR before
 * (output adjusting the OUT variable, see => _type_ and => _outs_ )
 : _typeline_ out @ over + cols @ > if cr then _type_ ;
 */
void
p4_type_on_line (const p4_char_t *str, p4cell len)
{
    /* RENAME: ... might need p4_Q_cr variant... make macro from this? */
    if (p4_OUT + len >= p4_COLS)
        FX (p4_cr);
    p4_type (str, len);
}

/** _emits_ ( n# ch -- )
 * type a string of chars by repeating a single character which
 * is usually a space, see => SPACES
 * (output adjusting the OUT variable, see => _type_ and => _outs_ )
 : _emits_ swap 0 do dup _putc_ loop drop _flush_ _?xy_ drop out ! ;
 */
void
p4_emits (int n, const char c)
{
    int x, y;

    while (--n >= 0)
        p4_putc_noflush (c);
    fflush (stdout);
    p4_wherexy (&x, &y);
    p4_OUT = x;
}

/** _tab_ ( n# -- )
 * type a string of space up to the next tabulator column
 * (output adjusting the OUT variable, see => _emits and => _typeonline )
 : _tab_ dup out @ - swap mod bl _emits_ ;
 */
void
p4_tab (int n)
{
    p4_emits (n - p4_OUT % n, ' ');
}

/** _expect_noecho_ ( str* str# -- span# )
 * EXPECT counted string from terminal, without echo, so no real editing
 * it will however convert backspace and tabulators, break on newline/escape
 */
static int
p4_expect_noecho (char *p, p4cell n)
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

int p4_expect_line(char* p, p4cell n)
{
        char *q = fgets (p, n, stdin);
        if (q == NULL) FX (p4_bye); /* ?? */
        q = strchr (p, '\n');
        if (q) *q = '\0';
        return p4_strlen (p);
}

/** _expect_ ( str* str# -- span# )
 * EXPECT counted string from terminal, with echo, so one can use
 * simple editing facility with backspace, but nothing more.
 * it's very traditional, you want to use a lined-like function instead!
 */
int
p4_expect (char *p, p4cell n)
{
    int i;
    char c;

    if (P4_opt.isnotatty) {
            if (P4_opt.isnotatty == P4_TTY_NOECHO)
            return p4_expect_noecho (p, n);
            else {
                    return p4_expect_line (p, n);
            }
    }
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
                 if (p4_OUT % 8 == 0)
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

int p4_accept_line (char *tib, int tiblen)
{
        char inputbuf[P4_MAX_INPUT];
    register char *buf;
    int len;
    buf = fgets (inputbuf, sizeof(inputbuf), stdin);
    if (buf == NULL) FX (p4_bye);
    buf = strchr (buf, '\n');
    len = (buf) ? (buf-inputbuf) : p4_strlen(inputbuf);
    if (len > tiblen) len = tiblen;
    memcpy (tib, inputbuf, len);
    return len;
}

int p4_accept_noecho (char *tib, int tiblen)
{
        char inputbuf[P4_MAX_INPUT];
    int len = p4_expect_noecho (inputbuf, sizeof(inputbuf));
    if (len > tiblen) len = tiblen;
    memcpy (tib, inputbuf, len);
    return len;
}

/** _accept_ ( str* str# -- span# )
 * better input facility using lined if possible, otherwise
 * call _expect_noecho when running in a pipe or just _expect_ if no
 * real terminal attached.
 */
int
p4_accept (p4_char_t *tib, int n)
{
    char* p = (char*) tib;
    if (P4_opt.isnotatty) {
            if (P4_opt.isnotatty == P4_TTY_NOECHO)
            return p4_accept_noecho (p, n);
            else
            return p4_accept_line (p, n);
    }
    PFE.accept_lined.string = p;
    PFE.accept_lined.max_length = n;
    p4_lined (&PFE.accept_lined, NULL);
    FX (p4_space);
    return PFE.accept_lined.length;
}

/* **********************************************************************
 * source input
 */

/** QUERY ( -- )
 * source input:  read from terminal using => _accept_ with the
 * returned string to show up in => TIB of => /TIB size.
 */
void FXCode (p4_query)
{
    SOURCE_ID = 0;
    BLK = 0;
    TO_IN = 0;
    TIB = PFE.tib;
    NUMBER_TIB = p4_accept (PFE.tib, TIB_SIZE);
    /* if (PFE.query_hook) // please use lined.h:lined->intercept now
     *     NUMBER_TIB = (*PFE.query_hook)(NUMBER_TIB);
     */
    SPAN = NUMBER_TIB;
}

/**
 * source input: read from text-file
 */
p4_bool_t
p4_next_line (void)
{
    p4cell ior;
    p4ucell len;

    len = sizeof SOURCE_FILE->buffer;
    if (!p4_read_line (SOURCE_FILE->buffer, &len, SOURCE_FILE, &ior))
    {
        SOURCE_FILE->len = len;
        return P4_FALSE;
    }
    TIB = SOURCE_FILE->buffer;
    NUMBER_TIB = SOURCE_FILE->len = len;
    BLK = 0;
    TO_IN = 0;
    return P4_TRUE;
}

/** _source_ ( str*& str#& -- )
 * see => SOURCE - dispatch input source
 */
void
p4_source (const p4_char_t **p, int *n)
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
             *p = p4_blockfile_block (BLOCK_FILE, BLK);
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

/** _size_saved_input_ ( -- iframe-size )
 */
p4ucell
p4_size_saved_input (void)
{
    return sizeof (Iframe);
}

/*NOTE: the (void* p) is often the RP being aligned to 32bit on most
 * platforms but an Iframe contains an .input.off possibly being 64bit
 * wide when off_t=64bit. This creates an alignment problem. We fix it
 * here with a CP operation. A better variant would be align the input
 * pointer magically in p4_save_input, until someone calls p4_link_s..
 */

/* I hate compiler bugs, especially this one for solaris gcc 2.95 : */
/* #define CP(X,I,Y) p4_memcpy ((char*)&(X), (char*)&(Y), (int)sizeof(X)) */
#define CP(X,I,Y) { register int i = sizeof((X)); \
                    p4_memcpy ((char*)&(X), (char*)&(Y), i); }

/** _link_saved_input_ ( iframe* -- )
 * see => SAVE-INPUT
 */
void
p4_link_saved_input (void *p)
{
    Iframe *iframe = (Iframe *) p;

    iframe->magic = P4_INPUT_MAGIC;
    CP(iframe->input, =, PFE.input);
    CP(iframe->prev, =, PFE.saved_input);
    CP(PFE.saved_input, =, iframe);
}

/** _save_input_ ( iframe-stack* -- iframe-stack*' )
 * see => SAVE-INPUT
 */
void *
p4_save_input (void *p)
{
    Iframe *iframe = (Iframe *) p;
    --iframe;
    p4_link_saved_input (iframe);
    return ((void*) iframe);
}

/** _unlink_saved_input_ ( iframe* -- )
 * see => RESTORE-INPUT
 */
void
p4_unlink_saved_input (void *p)
{
    Iframe *iframe = (Iframe *) p;

    if (iframe->magic != P4_INPUT_MAGIC)
        p4_throw (P4_ON_ARG_TYPE);
    CP(PFE.input, = ,iframe->input);
    CP(PFE.saved_input, =, iframe->prev);
}

/** _restore_input_ ( iframe-stack* -- iframe-stack*' )
 * see => RESTORE-INPUT
 */
void *
p4_restore_input (void *p)
{
    Iframe *iframe = (Iframe *) p;
    p4_unlink_saved_input (p);
    ++iframe;
    return ((void *) iframe);
}

/** _refill_ ( -- flag )
 * see => REFILL
 */
p4_bool_t
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
         return P4_TRUE;
    default:
        return p4_next_line ();
    }
}
/** _skip_delimiter_ ( del -- )
 * => SKIP-DELIMITER
 */
void
p4_skip_delimiter (char del)
{
    const char *q;
    int i, n;

    p4_source ((const p4_char_t**) &q, &n);
    if (del == ' ')
    {
        for (i = TO_IN;
             i < n && p4_isascii (q[i]) && p4_isspace (q[i]);
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

/** _word:parse_ ( delim -- <end?> )
 */
p4_cell_t
p4_word_parse (char del)
{
    const char *q;
    int i, n;


    p4_source ((const p4_char_t**) &q, &n);
    PFE.word.ptr = (p4_char_t*) q + TO_IN;

    i = TO_IN;
    if (i >= n)
        goto empty;

    if (del != ' ') /* no BL */
    {
        while (1)
        {
            if (q[i] == del)
                goto delimfound;
            i++;
            if (i == n)
                goto empty;
        }
    }else if (! p4_QUOTED_PARSE) /* BL and no QUOTED-PARSE */
    {
        while (1)
        {
            if (p4_isascii (q[i]) && p4_isspace (q[i]))
                goto delimfound;
            i++;
            if (i == n)
                goto empty;
        }
#if 0
    }else if (q[i] == '"') { /* scan "..." strings - including quotes */
        i++;
        while (1)
        {
            if (q[i++] == '"')
                goto keepnextchar;
            if (i == n)
                goto empty;
        }
#endif
    }else{ /* BL && QUOTED -> before whitespace and after doublequote */
        while (1)
        {
            if (p4_isascii (q[i]) && p4_isspace (q[i]))
                goto delimfound;
            if (q[i++] == '"')
                goto keepnextchar;
            if (i == n)
                goto empty;
        }
    }

    /* two exit sequences */
 delimfound:
    /* put the ">IN" pointer just after the delimiter that was found */
        PFE.word.len = i - TO_IN;
        TO_IN = i + 1;
        return 1;
 keepnextchar:
    /* put the ">IN" pointer just after the delimiter that was found */
        PFE.word.len = i - TO_IN;
        TO_IN = i;
        return 1;
 empty:
    /* no delimiter but end of parse area -> set ">IN" to n -> empty state */
        PFE.word.len = i - TO_IN;
        TO_IN = i; /* = n */
        return 0;
}

/** _parse_ ( delim -- ptr len )
 : _parse_ _word:parse_ _word*_ s! _word#_ s! ;
 */
p4_cell_t
p4_parse (char del, const p4_char_t **p, p4ucell *l)
{
    register p4_cell_t x = p4_word_parse(del);
    *p = PFE.word.ptr;
    *l = PFE.word.len;
    return x;
}

/** _word>here_ ( -- here* )
 * complement => _word:parse_ to  arrive at the normal => WORD implementation
 * will also ensure the string is zero-terminated - this makes a lot of
 * operations easier since most forth function can receive a string-span
 * directly but some need a string-copy and that is usually because it has
 * to be passed down into a C-defined function with zerotermined string. Just
 * use HERE+1 (which is also the returnvalue of this function!) to have
 * the start of the zero-terminated string. Note that this function may throw
 * with P4_ON_PARSE_OVER if the string is too long (it has set *HERE=0 to
 * ensure again that => THROW will report PFE.word. as the offending string)
 */
char*
p4_word_to_here (void)
{
    if (PFE.word.len > 255) /* (1<<CHAR_BITS)-1 */
    { *HERE = 0;  p4_throw (P4_ON_PARSE_OVER); }

    *HERE = PFE.word.len;
    p4_memcpy (HERE+1, PFE.word.ptr, PFE.word.len);
    (HERE+1)[PFE.word.len] = 0; /* zero-terminated */
    return (char*) (HERE+1); /* HERE+1 -> start of z-string */
}

/** _word_ ( del -- here* )
 : _word_ dup _skip_delimiter_ _word:parse_ _word>here_ ;
 */
p4_char_t *
p4_word (char del)
{
    p4_skip_delimiter (del);
    p4_word_parse (del);
    p4_word_to_here ();
    return HERE;
}

/**
 * PARSE-WORD a.k.a. BL PARSEWORD
 *
 * return and args mean the same as for => _parse_ but it really
 * scans like => _word_. It most cases you can replace => _word_ with
 * a sequence of _parseword_ and _word>here_ (.);
 * The point is, that _parseword_ *doesn't* copy the next word onto
 * here, it just returns the pointers. In some cases, esp. where
 * a failure could be p4_thrown , it must be copied to HERE later.
 * You can use _word2here_ for that. See _interpret_ for an example.
 */
p4_cell_t
p4_word_parseword (char del)
{
    /* quick path for wordset-loader: */
    if (SOURCE_ID == -1 && PFE.word.len == -1) goto tib_static_string;
    p4_skip_delimiter (del);
    return p4_word_parse (del);

 tib_static_string:
    PFE.word.len = p4_strlen ((char*) PFE.word.ptr);
    /* if (! FENCE) return; // libpfe.so is firing up */

    /* assume: PFE.word.ptr points to the static_string we like to have */
    TIB = PFE.word.ptr; NUMBER_TIB = PFE.word.len; TO_IN = 0;
    return PFE.word.len;
}

#if 0
p4_cell_t
p4_parseword (char del, p4_char_t** p, p4ucell* l)
{
    p4_skip_delimiter (del);
    p4_cell_t x = p4_word_parse (del);
    *p = PFE.word.ptr;
    *l = PFE.word.len;
    return x;
}
#endif

/*@}*/

/* _________________________________________________________________________ */
/* _________________________________________________________________________ */
#if 0
/*
 * here are a few implemenations to show you how we came to the above
 * parsing code.
 */

/**
 * PARSE
 */
int
# if 0 /* standard implementation */
p4_parse (char del, p4_char_t **p, p4ucell *l) /*1*/
{
    char *q;
    int i, n;


    p4_source ((p4_char_t**) &q, &n);
    *p = (p4_char_t*) q + TO_IN;

    i = TO_IN;
    if (del == ' ')
    {
        while (i < n && !(p4_isascii (q[i]) && p4_isspace (q[i])))
        {
            i++;
        }
    }else{
        while (i < n && q[i] != del)
        {
            i++;
        }
    }
    *l = i - TO_IN;
    if (i == n)
    {/* no delimiter but end of parse area -> set ">IN" to n -> empty state */
        TO_IN = i;
        return 0;
    }else
    {/* put the ">IN" pointer just after the delimiter that was found */
        TO_IN = i + 1;
        return 1;
    }

}
# elif 0 /* split the while loop condition */
p4_parse (char del, p4_char_t **p, p4ucell *l) /*2*/
{
    char *q;
    int i, n;


    p4_source ((p4_char_t**) &q, &n);
    *p = (p4_char_t*) q + TO_IN;

    i = TO_IN;
    if (del == ' ')
    {
        while (1)
        {
            if (i >= n)
                break;
            if (p4_isascii (q[i]) && p4_isspace (q[i]))
                break;
            i++;
        }
    }else{
        while (1)
        {
            if (i >= n)
                break;
            if (q[i] == del)
                break;
            i++;
        }
    }
    *l = i - TO_IN;
    if (i == n)
    {/* no delimiter but end of parse area -> set ">IN" to n -> empty state */
        TO_IN = i;
        return 0;
    }else
    {/* put the ">IN" pointer just after the delimiter that was found */
        TO_IN = i + 1;
        return 1;
    }

}
# elif 0 /* move the length setting inside the last if-check */
p4_parse (char del, char **p, p4ucell *l) /*3*/
{
    char *q;
    int i, n;


    p4_source ((p4_char_t**) &q, &n);
    *p = q + TO_IN;

    i = TO_IN;
    if (del == ' ')
    {
        while (1)
        {
            if (i >= n)
                break;
            if (p4_isascii (q[i]) && p4_isspace (q[i]))
                break;
            i++;
        }
    }else{
        while (1)
        {
            if (i >= n)
                break;
            if (q[i] == del)
                break;
            i++;
        }
    }
    if (i == n)
    {/* no delimiter but end of parse area -> set ">IN" to n -> empty state */
        *l = i - TO_IN;
        TO_IN = i;
        return 0;
    }else
    {/* put the ">IN" pointer just after the delimiter that was found */
        *l = i - TO_IN;
        TO_IN = i + 1;
        return 1;
    }

}
# elif 0 /* move an if(usedup)-check up front, and reverse order in whiles */
p4_parse (char del, char **p, p4ucell *l) /*4*/
{
    char *q;
    int i, n;


    p4_source ((p4_char_t**) &q, &n);
    *p = q + TO_IN;

    i = TO_IN;
    if (i >= n)
        goto empty;

    if (del == ' ')
    {
        while (1)
        {
            if (p4_isascii (q[i]) && p4_isspace (q[i]))
                break;
            i++;
            if (i == n)
                break;
        }
    }else{
        while (1)
        {
            if (q[i] == del)
                break;
            i++;
            if (i == n)
                break;
        }
    }

 empty:
    if (i == n)
    {/* no delimiter but end of parse area -> set ">IN" to n -> empty state */
        *l = i - TO_IN;
        TO_IN = i;
        return 0;
    }else
    {/* put the ">IN" pointer just after the delimiter that was found */
        *l = i - TO_IN;
        TO_IN = i + 1;
        return 1;
    }

}
# elif 0 /* bind [if (i==n)] occurences */
p4_parse (char del, char **p, p4ucell *l) /*5*/
{
    char *q;
    int i, n;


    p4_source ((p4_char_t**) &q, &n);
    *p = q + TO_IN;

    i = TO_IN;
    if (i >= n)
        goto empty;

    if (del == ' ')
    {
        while (1)
        {
            if (p4_isascii (q[i]) && p4_isspace (q[i]))
                break;
            i++;
            if (i == n)
                goto empty;
        }
    }else{
        while (1)
        {
            if (q[i] == del)
                break;
            i++;
            if (i == n)
                goto empty;
        }
    }

    /* put the ">IN" pointer just after the delimiter that was found */
        *l = i - TO_IN;
        TO_IN = i + 1;
        return 1;
 empty:
    /* no delimiter but end of parse area -> set ">IN" to n -> empty state */
        *l = i - TO_IN;
        TO_IN = i;
        return 0;

}
# elif 0 /* make delimfound exit */
p4_parse (char del, char **p, p4ucell *l) /*6*/
{
    char *q;
    int i, n;


    p4_source ((p4_char_t**) &q, &n);
    *p = q + TO_IN;

    i = TO_IN;
    if (i >= n)
        goto empty;

    if (del == ' ')
    {
        while (1)
        {
            if (p4_isascii (q[i]) && p4_isspace (q[i]))
                goto delimfound;
            i++;
            if (i == n)
                goto empty;
        }
    }else{
        while (1)
        {
            if (q[i] == del)
                goto delimfound;
            i++;
            if (i == n)
                goto empty;
        }
    }

    /* two exit sequences */
 delimfound:
    /* put the ">IN" pointer just after the delimiter that was found */
        *l = i - TO_IN;
        TO_IN = i + 1;
        return 1;
 empty:
    /* no delimiter but end of parse area -> set ">IN" to n -> empty state */
        *l = i - TO_IN;
        TO_IN = i;
        return 0;

}
# elif 0 /* use global variables instead of p and l */
p4_parse (char del, char **p, p4ucell *l) /*7*/
{
    register int x = _p4_parse(del);
    *p = PFE.word.ptr;
    *l = PFE.word.len;
    return x;
}

int
p4_word_parse (char del)
{
    char *q;
    int i, n;


    p4_source ((p4_char_t**) &q, &n);
    PFE.word.ptr = q + TO_IN;

    i = TO_IN;
    if (i >= n)
        goto empty;

    if (del == ' ')
    {
        while (1)
        {
            if (p4_isascii (q[i]) && p4_isspace (q[i]))
                goto delimfound;
            i++;
            if (i == n)
                goto empty;
        }
    }else{
        while (1)
        {
            if (q[i] == del)
                goto delimfound;
            i++;
            if (i == n)
                goto empty;
        }
    }

    /* two exit sequences */
 delimfound:
    /* put the ">IN" pointer just after the delimiter that was found */
        PFE.word.len = i - TO_IN;
        TO_IN = i + 1;
        return 1;
 empty:
    /* no delimiter but end of parse area -> set ">IN" to n -> empty state */
        PFE.word.len = i - TO_IN;
        TO_IN = i; /* = n */
        return 0;

}
#else
/*
  and finally, make p4_word depend also on p4_word_parse, and use the
  global word.ptr/len to copy it to HERE afterwards. On the upside, we
  can make the visual at p4_throw a bit better, since we can now show
  the complete offending word-span, not just the point where ">in" had
  stopped. And we avoid multiple code areas doing more or less the same
  thing.
*/
# endif

/*show parsecode */
#endif
