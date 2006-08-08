#include <pfe/os-string.h>

/*
 * GCC3 on i386 uses special assembler codes for memcpy, strcpy, strlen
 * which do require SI/DI registers to be used. Those are also used for
 * pfe vm registers. The GCC developers have been unable to add automatic
 * save/restore code since two years and the situation has gone worse
 * from one version to the next. It seems there is no way to fix it.
 */

#ifndef p4_strlen
size_t p4_strlen(const char *s) {
    return strlen(s);
}
#endif

#ifndef p4_memcpy
void *p4_memcpy(void *dest, const void *src, size_t n) {
    return memcpy(dest, src, n);
}
#endif

#ifndef p4_memset
void *p4_memset(void *s, int c, size_t n) {
    return memset(s, c, n);
}
#endif

#ifndef p4_strchr
char *p4_strchr(const char *s, int c) {
    return strchr(s, c);
}
#endif

#ifndef p4_strncmpi
/* _strncmpi_ ( str1* str2* max# -- cmp? ) */
int
p4_strncmpi (const char* p, const char* q, int n)
{
#if defined PFE_HAVE_STRNCASECMP
    return strncasecmp (p, q, n);
#elif defined PFE_HAVE_STRNICMP
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
#endif /*PFE_HAVE_...*/
}
#endif /*p4_strncmpi*/

#ifndef p4_memmove
void p4_memmove (char *d, const char *s, size_t n)
{
    if (n)
    {
        if (s > d)
        {
            do {
                *d++ = *s++;
            } while (--n > 0);
        }else{
            do {
                --n;
                d [n] = s [n];
            } while (n > 0);
        }
    }
}
#endif /*p4_memmove*/

#ifndef p4_strlcat
/* strlcat differs from strncat in that the latter does
 * copy at most n source characters while the other does
 * limit the target buffer to be at most n dest characters.
 * <p>
 * There are many many many programming errors when people
 * wanted to prevent a target buffer overflow and they took
 * errornously strncat instead of strlcat. 
 * <p>
 * - strncat = prevent source buffer overflow
 * - strlcat = prevent target buffer overflow
 */
size_t p4_strlcat (char *dst, const char* src, size_t n)
{
    size_t dst_len = p4_strlen (dst);
    size_t max_len = p4_strlen (src) + dst_len;
    if (max_len > --n) n -= dst_len;
    p4_strncat (dst, src, n);
    return max_len; /* to allow overflow detection */
}
#endif /*p4_strlcat*/

