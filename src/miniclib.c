#include <stdio.h>
bcopy( src, dst, len )
    char*       src;
    char*       dst;
    int         len;
  {
    char*       b;

    for( b=dst; len; --len ) *b++ = *src++;
  }

bzero( dst, len )
    char*       dst;
    int         len;
  {
    char*       b;
    for( b=dst; len; --len ) *b++ = '\0';
  }

strlen( dst )
    char*       dst;
  {
    int i=0;
    while(dst[i] != '\0') ++i;
    return(i);
  }

bcmp( dst, src, len )
    char*       dst;
    char*       src;
    int         len;
  {
    char*       b;
    char*       a;

    for( a=dst,b=src; len--; ++a,++b )
        if( *a != *b )
            return( *a > *b ? 1 : -1 );
    return( 0 );
  }

isdigit(c)
    char       c;
  {
    if ( c >= '0' && c <= '9' ) return ( 1 );
    return( 0 );
  }
isspace(c)
    char       c;
  {
    switch(c)
     {
        case ' '  :              /* Blank           */
        case '\t' :              /* Horizontal tab  */
        case '\f' :              /* Form feed       */
        case '\r' :              /* Return          */
        case '\n' :              /* Newline         */
          return ( 1 );
        default :
          return ( 0 );
    }
  }

int
strindex( str, c )
    char        *str;
    char        c;
  {
    char        *s;

    for( s=str; *s; ++s )
    if( *s == c )
      return( s-str );

    return( -1 );
  }

int
strnindex( str, c , l)
    char        *str;
    char        c;
    int         l;
  {
    char        *s;
    int         rl;

    for( s=str, rl=0 ; *s, rl<l ; ++s, ++rl )
    if( *s == c )
      return( s-str );

    return( -1 );
  }

