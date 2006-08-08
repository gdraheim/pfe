/**********************************************************************
*
*	C Source:		emu-menu-table.c
*	Instance:		bln_mpt1_1
*	Description:	
*	%created_by:	guidod %
*	%date_created:	Thu Jun 26 16:56:56 2003 %
*
**********************************************************************/
#ifndef lint
static char* id =
  "@(#) $Id: emu-menu-table.c,v 1.1.1.1 2006-08-08 09:09:29 guidod Exp $";
#endif

/* Trace Message Levels */

#ifndef K12_TRC_LVL_NONE
#define K12_TRC_LVL_NONE -1
#endif

static k12_emu_enum_t traceEnum[] =
{
  { "None",      K12_TRC_LVL_NONE    },
  { "Fatal",     K12_TRC_LVL_FATAL   },
  { "Severe",    K12_TRC_LVL_SEVERE  },
  { "Warning",   K12_TRC_LVL_WARN    },
  { "Info",      K12_TRC_LVL_INFO    },
  { "Verbose",   K12_TRC_LVL_VERBOSE },
  { "Debug",     K12_TRC_LVL_DEBUG   }
};

static k12_emu_enum_t tmStateActionEnum[] =
{
    { "NONE", 0 },
    { "START",  1 },
    { "STOP",  2 },
    { "CONTINUE",  3 },
    { "END",  4 },
};

k12_emu_confque_t p4_emu_menu_tab [] = {
  { "ScriptFile", K12_EMU_TYPE_STRING, K12_EMU_CONFIG,
    255,
    0, 0, 0,
    "include scriptfile", 0, (FUNCPTR) &p4_emu_scriptfile
  },
  { "General", K12_EMU_TYPE_SUBMENU, 0,
    6, /* entries */
  },
  { "Version", K12_EMU_TYPE_STRING, K12_EMU_QUERY,
    80,
    0, 0, 0,
    PFE_VERSION, 0, (FUNCPTR) p4_emu_version
  },
  {
    "State", K12_EMU_TYPE_ENUM, K12_EMU_QUERY,
    sizeof (u32_t),
    0, 6, (k12_emu_enum_t*) p4_emu_state__k12_enum,
    (s8_t*) 0, 0, (FUNCPTR) p4_emu_state,
    0, 0
  },
  {
    "State Description", K12_EMU_TYPE_STRING, K12_EMU_QUERY,
    255,
    0, 6, (k12_emu_enum_t*) p4_emu_state__k12_enum,
    (s8_t*) 0, 0, (FUNCPTR) p4_emu_state_desc,
    0, 0
  },
  {
    "Trace Level",
    K12_EMU_TYPE_ENUM,          /* type of the variable        */
    K12_EMU_CONFIGQUERY,        /* access mode:config or query */
    sizeof (u32_t),             /* size of the variable        */
    0, DIM (traceEnum),         /* min and max/len of enum     */
    traceEnum,                  /* pointer to the enumeral     */
    (char*) 0,                  /* pointer to variable         */
    0,                          /* offset to variable use base */
    (FUNCPTR) p4_conftrace,     /* pointer to function         */
    0,                          /* size of field, 0=no field   */
    K12_TRC_LVL_INFO,           /* default: all                */
  },
  /* "Chart" and "Control" are for MSC support, not for general
     Forth programming or test manager functionality.  Therefore
     these sections are not mentioned in the feaure files pfe.fxt
     or pfe-ap1.fxt. */
  { "Chart", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
    sizeof (p4cell),
    0, 1024*1024, 0,
    "reqstate", 0, (FUNCPTR) p4_emu_tm_config,
  },
  { "Control", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
    sizeof (p4cell), 		/*size of variable */
    0, DIM(tmStateActionEnum), tmStateActionEnum, /* Min, Max, Enum */
    "tmaction", 0, (FUNCPTR) p4_emu_tm_action
  },
  { "Actions", K12_EMU_TYPE_SUBMENU, 0,
    7, /* entries */ 
  }, 
  { "HMSC", K12_EMU_TYPE_INT, K12_EMU_CONFIG,
    sizeof(p4cell),
    0, 0x7FFFFFFF, 0, /* Min, Max, Enum* */
    "set-hmsc", 0, (FUNCPTR) p4_emu_do_or_tm_config,
    0, 0
  },
  { "Verdict", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
    sizeof(p4cell),
    0, 0x7FFFFFFF, 0, /* Min, Max, Enum* */
    "verdict-", 0, (FUNCPTR) p4_emu_do_or_tm_config,
    0, 0
  },
  { "Start", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
    sizeof(p4cell),
    0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
    "HMSC.Start", 0, (FUNCPTR) p4_emu_hmsc_action
  },
  { "Stop", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
    sizeof(p4cell),
    0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
    "HMSC.Stop", 0, (FUNCPTR) p4_emu_hmsc_action
  },
  { "Continue", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
    sizeof(p4cell),
    0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
    "HMSC.Continue", 0, (FUNCPTR) p4_emu_hmsc_action
  },
  { "Reload", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
    sizeof(p4cell),
    0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
    "HMSC.Reload", 0, (FUNCPTR) p4_emu_hmsc_action
  },
  { "Reset", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
    sizeof(p4cell),
    0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
    "HMSC.Reset", 0, (FUNCPTR) p4_emu_hmsc_action
  },

  { "Statistics", K12_EMU_TYPE_SUBMENU, 0,
    7, /* entries */
  }, 
  {   "Number_HMSCs", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
      sizeof(p4cell),
      0, 0x7FFFFFFF, 0, /* Min,Max,Enum*/
      "ver-hmsc", 0, (FUNCPTR) p4_emu_tm_config,
      0, 0
  },
  {   "Verdict_None", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
      sizeof(p4cell),
      0, 0x7FFFFFFF, 0, /* Min,Max,Enum*/
      "ver-none", 0, (FUNCPTR) p4_emu_tm_config,
      0, 0
  },
  {   "Verdict_Pass", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
      sizeof(p4cell),
      0, 0x7FFFFFFF, 0, /* Min,Max,Enum*/
      "ver-pass", 0, (FUNCPTR) p4_emu_tm_config,
      0, 0
  },
  {   "Verdict_Inconc", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
      sizeof(p4cell),
      0, 0x7FFFFFFF, 0, /* Min,Max,Enum*/
      "ver-inco", 0, (FUNCPTR) p4_emu_tm_config,
      0, 0
  },
  {   "Verdict_Fail", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
      sizeof(p4cell),
      0, 0x7FFFFFFF, 0, /* Min,Max,Enum*/
      "ver-fail", 0, (FUNCPTR) p4_emu_tm_config,
      0, 0
  },
  {   "Verdict_Error", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
      sizeof(p4cell),
      0, 0x7FFFFFFF, 0, /* Min,Max,Enum*/
      "ver-errr", 0, (FUNCPTR) p4_emu_tm_config,
      0, 0
  },
  {   "Reset", K12_EMU_TYPE_ENUM, K12_EMU_CONFIG,
      sizeof(p4cell),
      0, DIM(p4_emu_hmsc_action_enum), p4_emu_hmsc_action_enum,
      "verdict.Reset", 0, (FUNCPTR) p4_emu_verdict_action
  },

  { "Forth", K12_EMU_TYPE_SUBMENU, 0,
    12, /* entries */
  },

  { "state", K12_EMU_TYPE_ENUM, K12_EMU_QUERY,
    sizeof (p4cell), 		/*size of variable */
    0, DIM(StatesEnum), StatesEnum,     /* Min, Max, Enum* */
    "p4-state", 0, (FUNCPTR) p4_emu_do_config,
    0, 			/* sizeof of var-array */
    0			/* default : interpret */
  },
  { "total_size", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
    sizeof (p4cell), 		
    8*1024, 1024*1024*1024, 0, /* Min, Max, Enum* */
    "thr-size", 0, (FUNCPTR) p4_emu_do_config,
    0, 			/* sizeof of var-array */
    P4_KB*1024		/* default : options.h */

  },
  { "tmdict_size", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
    sizeof(p4cell),
    0, 0x7FFFFFFF, 0, /* Min,Max,Enum*/
    "tm-dict-", 0, (FUNCPTR) p4_emu_tm_config,
    0, 0
  },
  { "sp_size", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
    sizeof (p4cell), 		/*size of variable */
    1*1024, 1024*1024, 0,   /* Min, Max, Enum* */
    "sp-size-", 0, (FUNCPTR) p4_emu_do_config,
    0, 			/* sizeof of var-array */
    (P4_KB*1024 / 8) / sizeof(p4cell), /* default */
  },
  { "rp_size", K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY,
    sizeof (p4cell), 		/*size of variable */
    1*1024, 1024*1024, 0,
    "rp-size-", 0, (FUNCPTR) p4_emu_do_config,
    0, 			/* sizeof of var-array */
    (P4_KB*1024 / 16) / sizeof(p4cell), /* default */
  },
  { "dict_left", K12_EMU_TYPE_INT, K12_EMU_QUERY,
    sizeof (p4cell),
    0, 0x7fffffff, 0,
    "DictLeft", 0, (FUNCPTR) p4_emu_do_config
  },
  { "sp_depth", K12_EMU_TYPE_INT, K12_EMU_QUERY,
    sizeof (p4cell),
    0, 0x7fffffff, 0,
    "sp-depth", 0, (FUNCPTR) p4_emu_do_config
  },
  { "rp_depth", K12_EMU_TYPE_INT, K12_EMU_QUERY,
    sizeof (p4cell),
    0, INT_MAX, 0,
    "rp-depth", 0, (FUNCPTR) p4_emu_do_config
  },
  { "base", K12_EMU_TYPE_INT, K12_EMU_QUERY,
    sizeof (p4cell),
    2, 16, 0,
    "ttx-base", 0, (FUNCPTR) p4_emu_do_config
  },
  { "incpath", K12_EMU_TYPE_STRING, K12_EMU_CONFIGQUERY,
    255, 			/* size of variable */
    0, 0, 0,                /* Min, Max, Enum* */
    "Forth.incpath", 0, (FUNCPTR) p4_emu_scriptpath,
  },
  { "execute", K12_EMU_TYPE_STRING, K12_EMU_CONFIG,
    255,                    /* Len */
    0, 0, 0,
    "Forth.execute", 0, (FUNCPTR) p4_emu_command,
  },
  { "cpuipcsend", K12_EMU_TYPE_STRING, K12_EMU_CONFIG,
    255,            /* Len */
    0, 0, 0,
    "Forth.cpuipcsend", 0, (FUNCPTR) p4_emu_cpuipcsend,
  },     

  { "Variables", K12_EMU_TYPE_SUBMENU, 0,
    32, /* entries */
  },
#define MSC_INT_CONSTRUCT(i) \
         { \
           "MSC_Int" #i, K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY, \
           sizeof (int), \
           INT_MIN, INT_MAX, 0, \
           (void*) i + 1, 0, \
           (FUNCPTR) p4_config_msc_int, \
         }

  MSC_INT_CONSTRUCT(0),
  MSC_INT_CONSTRUCT(1),
  MSC_INT_CONSTRUCT(2),
  MSC_INT_CONSTRUCT(3),
  MSC_INT_CONSTRUCT(4),
  MSC_INT_CONSTRUCT(5),
  MSC_INT_CONSTRUCT(6),
  MSC_INT_CONSTRUCT(7),
  MSC_INT_CONSTRUCT(8),
  MSC_INT_CONSTRUCT(9),
  MSC_INT_CONSTRUCT(10),
  MSC_INT_CONSTRUCT(11),
  MSC_INT_CONSTRUCT(12),
  MSC_INT_CONSTRUCT(13),
  MSC_INT_CONSTRUCT(14),
  MSC_INT_CONSTRUCT(15),
#undef MSC_INT_CONSTRUCT

#define MSC_STRING_CONSTRUCT(i) \
         { \
           "MSC_String" #i, K12_EMU_TYPE_STRING, K12_EMU_CONFIGQUERY, \
           81, \
           0, 0, 0, \
           (void*) i + 1, 0, \
           (FUNCPTR) p4_config_msc_string, \
         }

  MSC_STRING_CONSTRUCT(0),
  MSC_STRING_CONSTRUCT(1),
  MSC_STRING_CONSTRUCT(2),
  MSC_STRING_CONSTRUCT(3),
  MSC_STRING_CONSTRUCT(4),
  MSC_STRING_CONSTRUCT(5),
  MSC_STRING_CONSTRUCT(6),
  MSC_STRING_CONSTRUCT(7),
  MSC_STRING_CONSTRUCT(8),
  MSC_STRING_CONSTRUCT(9),
  MSC_STRING_CONSTRUCT(10),
  MSC_STRING_CONSTRUCT(11),
  MSC_STRING_CONSTRUCT(12),
  MSC_STRING_CONSTRUCT(13),
  MSC_STRING_CONSTRUCT(14),
  MSC_STRING_CONSTRUCT(15),
#undef MSC_STRING_CONSTRUCT

  { "Timers", K12_EMU_TYPE_SUBMENU, 0,
    MSC_MAX_TIMERS, /* entries */
  },

#define MSC_TIMER_CONSTRUCT(i) \
         { \
           "MSC_TIMER_" #i, K12_EMU_TYPE_INT, K12_EMU_CONFIGQUERY, \
           sizeof (int), \
           0, UINT_MAX, 0, \
           (void*) i + 1, 0, \
           (FUNCPTR) p4_config_msc_timer, \
         }

  MSC_TIMER_CONSTRUCT(0),
  MSC_TIMER_CONSTRUCT(1),
  MSC_TIMER_CONSTRUCT(2),
  MSC_TIMER_CONSTRUCT(3),
  MSC_TIMER_CONSTRUCT(4),
  MSC_TIMER_CONSTRUCT(5),
  MSC_TIMER_CONSTRUCT(6),
  MSC_TIMER_CONSTRUCT(7),
  MSC_TIMER_CONSTRUCT(8),
  MSC_TIMER_CONSTRUCT(9),
  MSC_TIMER_CONSTRUCT(10),
  MSC_TIMER_CONSTRUCT(11),
  MSC_TIMER_CONSTRUCT(12),
  MSC_TIMER_CONSTRUCT(13),
  MSC_TIMER_CONSTRUCT(14),
  MSC_TIMER_CONSTRUCT(15),
  MSC_TIMER_CONSTRUCT(16),
  MSC_TIMER_CONSTRUCT(17),
  MSC_TIMER_CONSTRUCT(18),
  MSC_TIMER_CONSTRUCT(19),
  MSC_TIMER_CONSTRUCT(20),
  MSC_TIMER_CONSTRUCT(21),
  MSC_TIMER_CONSTRUCT(22),
  MSC_TIMER_CONSTRUCT(23),
  MSC_TIMER_CONSTRUCT(24),
  MSC_TIMER_CONSTRUCT(25),
  MSC_TIMER_CONSTRUCT(26),
  MSC_TIMER_CONSTRUCT(27),
  MSC_TIMER_CONSTRUCT(28),
  MSC_TIMER_CONSTRUCT(29),
  MSC_TIMER_CONSTRUCT(30),
  MSC_TIMER_CONSTRUCT(31),
  MSC_TIMER_CONSTRUCT(32),
  MSC_TIMER_CONSTRUCT(33),
  MSC_TIMER_CONSTRUCT(34),
  MSC_TIMER_CONSTRUCT(35),
  MSC_TIMER_CONSTRUCT(36),
  MSC_TIMER_CONSTRUCT(37),
  MSC_TIMER_CONSTRUCT(38),
  MSC_TIMER_CONSTRUCT(39),
  MSC_TIMER_CONSTRUCT(40),
  MSC_TIMER_CONSTRUCT(41),
  MSC_TIMER_CONSTRUCT(42),
  MSC_TIMER_CONSTRUCT(43),
  MSC_TIMER_CONSTRUCT(44),
  MSC_TIMER_CONSTRUCT(45),
  MSC_TIMER_CONSTRUCT(46),
  MSC_TIMER_CONSTRUCT(47),
  MSC_TIMER_CONSTRUCT(48),
  MSC_TIMER_CONSTRUCT(49),
  MSC_TIMER_CONSTRUCT(50),
  MSC_TIMER_CONSTRUCT(51),
  MSC_TIMER_CONSTRUCT(52),
  MSC_TIMER_CONSTRUCT(53),
  MSC_TIMER_CONSTRUCT(54),
  MSC_TIMER_CONSTRUCT(55),
  MSC_TIMER_CONSTRUCT(56),
  MSC_TIMER_CONSTRUCT(57),
  MSC_TIMER_CONSTRUCT(58),
  MSC_TIMER_CONSTRUCT(59),
  MSC_TIMER_CONSTRUCT(60),
  MSC_TIMER_CONSTRUCT(61),
  MSC_TIMER_CONSTRUCT(62),
  MSC_TIMER_CONSTRUCT(MSC_MAX_TIMERS - 1),
#undef MSC_TIMER_CONSTRUCT
};
