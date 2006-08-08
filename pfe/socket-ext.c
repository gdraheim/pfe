/**
 * SOCKET-EXT - socket interface
 * --
 * @description
 *  The api is largely modelled after the glib ones for maximum compatibility
 */

/** gethostbyname ( name -- hostent )
 */

/** socket ( class type proto -- fd )
 */

/** connect ( fd sock size -- err )
 */

/** fdopen ( fd fileattr -- file )
 */

/** htonl ( x -- x' )
 */

/* structures */
/* .......... */

/* c-string */
/* host-addr */

/* .... c-string ( addr u -- addr' ) */


/** "host>addr" ( addr u -- x )
 * coverts an internet path into a IPv$ adress
 * the resulting address in network by order
 */

/* PF_INET ( -- 2 ) */
/* SOCK_STREAM ( -- 1 ) */
/* IPPROTO_TCP ( -- 6 ) */

/** open-socket ( addr u port -- fid )
 */


