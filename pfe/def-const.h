#ifndef __PFE_DEF_CONST_H
#define	__PFE_DEF_CONST_H "%full_filespec: def-const.h~5.6:incl:bln_12xx!1 %"

/** 
 * --  lots of constants and other #defines
 *
 *  Copyright (C) Tektronix, Inc. 1998 - 2001. All rights reserved.
 *
 *  @see     GNU LGPL
 *  @author  Tektronix CTE             @(#) %derived_by: guidod %
 *  @version %version: 5.6 %
 *    (%date_modified: Mon Mar 12 10:33:20 2001 %) 
 *
 *  @description
 *      this is most of the public part of pfe's constants 
 *
 */
/*@{*/


/*
 * Decompiler style. Each 5 comma separated numbers, meaning:
 * additional spaces after print,
 * cr before print, changed indentation for first cr,
 * cr after print, changed indentation for second cr.
 */

#define	P4_DEFAULT_STYLE	0, 0,  0,  0,  0
#define	P4_LOCALS_STYLE		0, 0,  0,  1,  0
#define	P4_SEMICOLON_STYLE	0, 0, -4,  3,  0
#define	P4_DOES_STYLE		0, 1, -4,  0,  4

#define	P4_IF_STYLE		1, 1,  0,  0,  4
#define	P4_ELSE_STYLE		1, 1, -4,  1,  4
#define	P4_THEN_STYLE		1, 1, -4,  1,  0

#define	P4_BEGIN_STYLE		1, 1,  0,  0,  4
#define	P4_WHILE_STYLE		1, 1, -4,  0,  4
#define	P4_REPEAT_STYLE		1, 1, -4,  1,  0
#define	P4_UNTIL_STYLE		1, 1, -4,  1,  0
#define	P4_AGAIN_STYLE		1, 1, -4,  1,  0

#define	P4_DO_STYLE		1, 1,  0,  0,  4
#define	P4_LOOP_STYLE		1, 1, -4,  1,  0

#define	P4_CASE_STYLE		1, 1,  0,  0,  4
#define	P4_OF_STYLE		1, 1,  0,  0,  4
#define	P4_ENDOF_STYLE		0, 0,  0,  1, -4
#define	P4_ENDCASE_STYLE	0, 1, -4,  1,  0

/*GD* same as BEGIN, but stands alone on a line */
#define P4_BEGINS_STYLE    1, 1,  0,  1,  4

/* value style */
#define P4_NEW1_STYLE      1, 1, -2, 1, 2

/* other constants, do not change! */

#ifndef P4_VERSION
#ifdef     VERSION
#define P4_VERSION      VERSION
#else
#define	P4_VERSION	"0.30.x"
#endif
#endif

#define	P4_FALSE	((p4cell)0)
#define	P4_TRUE	        (~P4_FALSE)

#define	P4xSMUDGED	0x20	/* these bits are flags in */
#define	P4xIMMEDIATE	0x40	/* the count byte of a definition */
#define P4xONxDESTROY	0x10	/* needed only with USE_FFA */
	/* if flag is set, word being freed, shall be executed before */

#ifdef _P4_SOURCE
/* Readable macros for magic numbers suggested by spc@pineal.math.fau.edu */

# if PFE_BYTEORDER == 4321
# define MAKE_MAGIC(A,B,C,D)	((p4cell)A << 24 | (p4cell)B << 16 |\
				 (p4cell)C <<  8 | (p4cell)D)
# else
# define MAKE_MAGIC(A,B,C,D)	((p4cell)D << 24 | (p4cell)C << 16 |\
				 (p4cell)B <<  8 | (p4cell)A)
# endif

# define SAVE_MAGIC		MAKE_MAGIC('P','F','E','S')
# define EXCEPTION_MAGIC	MAKE_MAGIC('X','C','P','T')
# define INPUT_MAGIC		MAKE_MAGIC('S','V','I','N')
# define DEST_MAGIC		MAKE_MAGIC('D','E','S','T')
# define ORIG_MAGIC		MAKE_MAGIC('O','R','I','G')
# define LOOP_MAGIC		MAKE_MAGIC('L','O','O','P')
# define CASE_MAGIC		MAKE_MAGIC('C','A','S','E')
# define OF_MAGIC		MAKE_MAGIC('O','F','O','F')
# define SEMANT_MAGIC		MAKE_MAGIC('S','E','M','A')

#endif
   /* def _P4_SOURCE */

#if PFE_BYTEORDER == 4321
#define	P4_MAGIC_(A,B,C,D)	((p4cell)A << 24 | (p4cell)B << 16 |\
				 (p4cell)C <<  8 | (p4cell)D)
#else
#define	P4_MAGIC_(A,B,C,D)	((p4cell)D << 24 | (p4cell)C << 16 |\
				 (p4cell)B <<  8 | (p4cell)A)
#endif

#define	P4_SAVE_MAGIC		P4_MAGIC_('P','F','E','S')
#define	P4_EXCEPTION_MAGIC	P4_MAGIC_('X','C','P','T')
#define	P4_INPUT_MAGIC		P4_MAGIC_('S','V','I','N')
#define	P4_DEST_MAGIC		P4_MAGIC_('D','E','S','T')
#define	P4_ORIG_MAGIC		P4_MAGIC_('O','R','I','G')
#define	P4_LOOP_MAGIC		P4_MAGIC_('L','O','O','P')
#define	P4_CASE_MAGIC		P4_MAGIC_('C','A','S','E')
#define	P4_OF_MAGIC		P4_MAGIC_('O','F','O','F')
#define P4_SEMANT_MAGIC		P4_MAGIC_('S','E','M','A')
#define P4_MAKE_MAGIC		P4_MAGIC_('M','A','K','E')

/* THROW codes */

#define	P4_ON_ABORT		-1
#define	P4_ON_ABORT_QUOTE	-2
#define	P4_ON_STACK_OVER	-3
#define	P4_ON_STACK_UNDER	-4
#define	P4_ON_RSTACK_OVER	-5
#define	P4_ON_RSTACK_UNDER	-6
#define P4_ON_EXEC_NESTING      -7
#define P4_ON_DICT_OVER         -8
#define	P4_ON_INVALID_MEMORY	-9
#define P4_ON_DIV_ZERO          -10
#define P4_ON_RESULT_RANGE      -11
#define	P4_ON_ARG_TYPE		-12
#define	P4_ON_UNDEFINED	-13
#define	P4_ON_COMPILE_ONLY	-14
#define	P4_ON_INVALID_FORGET	-15
#define	P4_ON_ZERO_NAME	-16
#define	P4_ON_PICNUM_OVER	-17
#define	P4_ON_PARSE_OVER	-18
#define	P4_ON_NAME_TOO_LONG	-19
#define P4_ON_READ_PROT         -20
#define	P4_ON_UNSUPPORTED	-21
#define	P4_ON_CONTROL_MISMATCH	-22
#define	P4_ON_ADDRESS_ALIGNMENT -23
#define P4_ON_INVALID_NUMBER    -24
#define P4_ON_RSTACK_INVALID    -25
#define P4_ON_NO_LOOP_THERE     -26
#define P4_ON_INVALID_RECURSE   -27
#define	P4_ON_USER_INTERRUPT	-28
#define	P4_ON_COMPILER_NESTING	-29
#define P4_ON_OBSOLETED         -30
#define P4_ON_NO_CREATE_BODY    -31
#define	P4_ON_INVALID_NAME	-32
#define	P4_ON_BLOCK_READ	-33
#define	P4_ON_BLOCK_WRITE	-34
#define	P4_ON_INVALID_BLOCK	-35
#define P4_ON_INVALID_FPOS      -36
#define	P4_ON_FILE_ERROR	-37
#define	P4_ON_FILE_NEX		-38
#define	P4_ON_UNEXPECTED_EOF	-39
#define P4_ON_INVALID_BASE      -40
#define P4_ON_PRECISION_LOSS    -41
#define P4_ON_FP_DIV_ZERO       -42
#define P4_ON_FP_RESULT_RANGE   -43
#define	P4_ON_FSTACK_OVER	-44
#define	P4_ON_FSTACK_UNDER	-45
#define P4_ON_FP_INVALID_NUMBER -46
#define	P4_ON_CURRENT_DELETED	-47
#define P4_ON_INVALID_POSTPONE  -48
#define	P4_ON_SEARCH_OVER	-49
#define	P4_ON_SEARCH_UNDER	-50
#define P4_ON_CURRENT_CHANGED   -51
#define P4_ON_CS_STACK_OVER     -52
#define P4_ON_EXCEPTION_OVER    -53
#define P4_ON_FP_UNDER          -54
#define	P4_ON_FP_FAULT		-55
#define	P4_ON_QUIT		-56
#define P4_ON_SIGIO             -57
#define	P4_ON_IF_ELSE		-58

/* gforth's IOR offset is -512, while pfe has traditionally -1024 */
#define FX_IOR  ( -1024 - (errno))
#define P4_IOR(flag) ((flag) ? FX_IOR : 0)

#define	P4_ON_NO_BINARY	        -2048
#define	P4_ON_BIN_TOO_BIG	-2049
#define	P4_ON_OUT_OF_MEMORY	-2050
#define	P4_ON_INDEX_RANGE	-2051
#define P4_ON_COMPILE_FAIL      -2052

#ifdef PFE_WITH_DSTRINGS_EXT  /* (dnw 4feb01) */
# define P4_ON_SCOUNT_OVERFLOW  -2053
# define P4_ON_SSPACE_OVERFLOW  -2054
# define P4_ON_SGARBAGE_LOCK    -2055
# define P4_ON_SSTACK_UNDERFLOW -2056
# define P4_ON_SCAT_LOCK        -2057
# define P4_ON_DSCOUNT_OVERFLOW -2058
# define P4_ON_SFRAME_OVERFLOW  -2059
# define P4_ON_SFRAME_ITEMS     -2060
# define P4_ON_SFRAME_UNDERFLOW -2061
# define P4_ON_SFRAME_MISMATCH  -2062
#endif

/*@}*/
#endif

