/*
 *  Copyright (C) 2000 - 2004 Guido U. Draheim <guidod@gmx.de>
 *  Copyright (C) 2005 - 2008 Guido U. Draheim <guidod@gmx.de>
 *
 *  @see     GNU LGPL
 *  @author  Guido U. Draheim            (modified by $Author: guidod $)
 *  @version $Revision: 1.3 $
 *     (modified $Date: 2008-04-20 04:46:29 $)
 */
#inluce <pfe/def-config.h>
#ifdef PFE_HAVE_SDL_SDL_H

#include <SDL/SDL.h>
#include <pfe/pfe-base.h>

/* a few helper macros */

#define SP p4SP

#define FCODE_0_0(F) \
FCode(F) {      \
	F ();   \
}

#define FCODE_1_0(F) \
FCode(F) {      \
	-- SP;	\
	SP[0] = F ();   \
}

#define FCODE_0_1(F, T0) \
FCode(F) {      \
	F ((T1) SP[0]);   \
	FX_DROP;    \
}

#define FCODE_1_1(F, T0) \
FCode(F) {      \
	SP[0] = (p4cell) F ((T0) SP[0]);   \
}

#define FCODE_0_2(F, T0, T1) \
FCode(F) {      \
	F ((T0) SP[0], (T1) SP[1]);   \
	FX_2DROP;    \
}

#define FCODE_1_2(F, T0, T1) \
FCode(F) {      \
	SP[0] = (p4cell) F ((T0) SP[0], (T1) SP[1]);   \
	FX_DROP;    \
}

#define FCODE_0_3(F, T0, T1, T2) \
FCode(F) {      \
	F ((T0) SP[0], (T1) SP[1], (T2) SP[2]);   \
	FX_3DROP;    \
}

#define FCODE_1_3(F, T0, T1, T2) \
FCode(F) {      \
	SP[0] = (p4cell) F ((T0) SP[0], (T1) SP[1], (T2) SP[2]);   \
	FX_2DROP;    \
}

#define FCODE_0_4(F, T0, T1, T2, T3) \
FCode(F) {      \
	F ((T0) SP[0], (T1) SP[1], (T2) SP[2], (T3) SP[3]);   \
	FX_4DROP;    \
}

#define FCODE_1_4(F, T0, T1, T2, T3) \
FCode(F) {      \
	SP[0] = (p4cell) F ((T0) SP[0], (T1) SP[1], (T2) SP[2], (T3) SP[3]);  \
	FX_3DROP;    \
}

/* ====== Id: SDL.h,v 1.5.2.5 2000/09/07 ====== */

/** SDL_init ( flags -- int )
 * This function loads the SDL dynamically linked library and initializes 
 * the subsystems specified by 'flags' (and those satisfying dependencies)
 * Unless the SDL_INIT_NOPARACHUTE flag is set, it will install cleanup
 * signal handlers for some commonly ignored fatal signals (like SIGSEGV)
 *
 int SDL_Init(Uint32 flags);
 */
FCODE_1_1(SDL_init, Uint32);

/** SDL_InitSubSystem ( flags -- int )
 * This function initializes specific SDL subsystems 
 int SDL_InitSubSystem(Uint32 flags);
 */
FCODE_1_1(SDL_InitSubSystem, Uint32);

/** SDL_QuitSubSystem ( flags -- )
 * This function cleans up specific SDL subsystems 
 void SDL_QuitSubSystem(Uint32 flags);
 */
FCODE_0_1(SDL_QuitSubSystem);

/** SDL_WasInit ( flags -- mask )
 * This function returns mask of the specified subsystems which have
 * been initialized.
 * If 'flags' is 0, it returns a mask of all initialized subsystems.
 Uint32 SDL_WasInit(Uint32 flags);
 */
FCODE_1_1(SDL_WasInit);

/** SDL_Quit ( -- )
 * This function cleans up all initialized subsystems and unloads the
 * dynamically linked library.  You should call it upon all exit conditions.
 void SDL_Quit(void);
 */
FCODE_0_0(SDL_Quit);

/* ====== Id: SDL_main.h,v 1.3.2.2 2000/08/29 ====== */

/* To Be Done */

/* ====== Id: SDL_types.h,v 1.4.2.4 2000/10/12 ====== */

/* Done */

/* ====== Id: SDL_error.h,v 1.4.2.1 2000/03/16 ====== */

/** SDL_SetError ( str-ptr str-len -- )
 * set it, based on
 void SDL_SetError(const char *fmt, ...);
 */
FCode(SDL_SetError)
{
    SDL_SetError ("%.*s", (int) SP[0], (char*) SP[1]);
    FX_2DROP;
}

/** SDL_GetError ( -- str-ptr str-len )
 * get it, based on
 char * SDL_GetError(void);
 */
FCode(SDL_GetError)
{
    SP -= 2;
    SP[1] = (p4cell) SDL_GetError ();
    SP[0] = p4_strlen ((p4char*) SP[1]);
}

/** SDL_ClearError ( -- )
 * clear it.
 void SDL_ClearError(void);
*/
FCODE_0_0 (SDL_ClearError);

/* ====== Id: SDL_rwops.h,v 1.4.2.2 2000/05/30 ====== */

/** SDL_RWFromFile ( Z-modestr* Z-filename* -- SDL_RWops* )
 * A Functions to create SDL_RWops structures from various data sources 
 SDL_RWops * SDL_RWFromFile(const char *file, const char *mode);
 */
FCODE_1_2 (SDL_RWFromFile, const char*, const char*);

/** SDL_RWFromFP ( autoclose STDIO-FILE* -- SDL_RWops* )
 * A Functions to create SDL_RWops structures from various data sources 
 SDL_RWops * SDL_RWFromFP(FILE *fp, int autoclose);
 */
FCODE_1_2 (SDL_RWFromFP, FILE*, int);

/** SDL_RWFromMem ( size mem* -- SDL_RWops* )
 * A Functions to create SDL_RWops structures from various data sources 
 SDL_RWops * SDL_RWFromMem(void *mem, int size);
 */
FCODE_1_2 (SDL_RWFromMem, void*, size );

/** SDL_AllocRW ( -- SDL_RWops* )
 * A Functions to create SDL_RWops structures from various data sources 
 SDL_RWops * SDL_AllocRW(void);
 */
FCode_1_0 (SDL_AllocRW);

/** SDL_FreeRW ( SDL_RWops* -- )
 * A Functions to create SDL_RWops structures from various data sources 
 void SDL_FreeRW(SDL_RWops *area);
 */
FCode_0_1 (SDL_FreeRW, SDL_RWops*);

/** SDL_RWseek ( whence offset rwops-context -- status )
 * Macro-based call to easily read and write from an SDL_RWops structure 
 define SDL_RWseek(ctx, offset, whence) (ctx)->seek(ctx, offset, whence)
 */
FCODE_1_3 (SDL_RWseek, SDL_RWops*, int, int);

/** SDL_RWtell ( rwops-context -- status )
 * Macro-based call to easily read and write from an SDL_RWops structure 
 define SDL_RWtell(ctx)                 (ctx)->seek(ctx, 0, SEEK_CUR)
 */
FCODE_1_1 (SDL_RWtell, SDL_RWops*);

/** SDL_RWread ( n size ptr rwops-context -- status )
 * Macro-based call to easily read and write from an SDL_RWops structure 
 define SDL_RWread(ctx, ptr, size, n)   (ctx)->read(ctx, ptr, size, n)
 */
FCODE_1_4 (SDL_RWread, SDL_RWops*, void*, int, int);

/** SDL_RWwrite ( n size ptr rwops-context -- status )
 * Macro-based call to easily read and write from an SDL_RWops structure 
 define SDL_RWwrite(ctx, ptr, size, n)   (ctx)->read(ctx, ptr, size, n)
 */
FCODE_1_4 (SDL_RWwrite, SDL_RWops*, void*, int, int);

/** SDL_RWclose ( rwops-context -- status )
 * Macro-based call to easily read and write from an SDL_RWops structure 
 define SDL_RWtell(ctx)                 (ctx)->seek(ctx, 0, SEEK_CUR)
 */
FCODE_1_1 (SDL_RWclose, SDL_RWops*);

/* ====== Id: SDL_timer.h,v 1.4.2.6 2000/10/02 ====== */


/** SDL_GetTicks ( -- value )
 * Get the number of milliseconds since the SDL library initialization.
 * Note that this value wraps if the program runs for more than ~49 days.
 Uint32 SDL_GetTicks(void);
 */ 
FCODE_1_0(SDL_GetTicks);

/** SDL_Delay ( value -- )
 * Wait a specified number of milliseconds before returning 
 void SDL_Delay(Uint32 ms);
 */
FCODE_0_1(SDL_Delay);

/* AddTimer/RemoveTimer TO BE DONE */

/* ====== Id: SDL_audio.h,v 1.4.2.6 2000/03/16 ====== */

/** SDL_AudioInit ( Z-str -- status )
 * One of the functions that are used internally, 
 * and should not be used unless you
 * have a specific need to specify the audio driver you want to use.
 int SDL_AudioInit(const char *driver_name);
 */
FCODE_1_1(SDL_AudioInit, const char*);

/** SDL_AudioQuit ( -- )
 * One of the functions that are used internally, 
 * and should not be used unless you
 * have a specific need to specify the audio driver you want to use.
 void SDL_AudioQuit(void);
 */
FCODE_0_0(SDL_AudioQuit);

/** SDL_AudioDriverName ( maxlen namebuf -- namebuf | 0 )
 * This function fills the given character buffer with the name of the
 * audio driver, and returns a pointer to it if the audio driver has
 * been initialized.  It returns NULL if no driver has been initialized.
 char *SDL_AudioDriverName(char *namebuf, int maxlen);
 */
FCODE_1_1(SDL_AudioDriverName, char* , int );



/** SDL_OpenAudio ( SDL_AudioSpec-obtained SDL_AudioSpec-desired -- status );
 * This function opens the audio device with the desired parameters, and
 * returns 0 if successful, placing the actual hardware parameters in the
 * structure pointed to by 'obtained'.  If 'obtained' is NULL, the audio
 * data passed to the callback function will be guaranteed to be in the
 * requested format, and will be automatically converted to the hardware
 * audio format if necessary.  This function returns -1 if it failed 
 * to open the audio device, or couldn't set up the audio thread.
 *
 * When filling in the desired audio spec structure,
 *  'desired->freq' should be the desired audio frequency in samples-per-second.
 *  'desired->format' should be the desired audio format.
 *  'desired->samples' is the desired size of the audio buffer, in samples.
 *     This number should be a power of two, and may be adjusted by the audio
 *     driver to a value more suitable for the hardware.  Good values seem to
 *     range between 512 and 8096 inclusive, depending on the application and
 *     CPU speed.  Smaller values yield faster response time, but can lead
 *     to underflow if the application is doing heavy processing and cannot
 *     fill the audio buffer in time.  A stereo sample consists of both right
 *     and left channels in LR ordering.
 *     Note that the number of samples is directly related to time by the
 *     following formula:  ms = (samples*1000)/freq
 *  'desired->size' is the size in bytes of the audio buffer, and is
 *     calculated by SDL_OpenAudio().
 *  'desired->silence' is the value used to set the buffer to silence,
 *     and is calculated by SDL_OpenAudio().
 *  'desired->callback' should be set to a function that will be called
 *     when the audio device is ready for more data.  It is passed a pointer
 *     to the audio buffer, and the length in bytes of the audio buffer.
 *     This function usually runs in a separate thread, and so you should
 *     protect data structures that it accesses by calling SDL_LockAudio()
 *     and SDL_UnlockAudio() in your code.
 *  'desired->userdata' is passed as the first parameter to your callback
 *     function.
 *
 * The audio device starts out playing silence when it's opened, and should
 * be enabled for playing by calling SDL_PauseAudio(0) when you are ready
 * for your audio callback function to be called.  Since the audio driver
 * may modify the requested size of the audio buffer, you should allocate
 * any local mixing buffers after you open the audio device.
 *
 int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
 */
FCode_1_2(SDL_OpenAudio, SDL_AudioSpec*, SDL_AudioSpec*);


/** SDL_GetAudioStatus ( -- status )
 * Get the current audio state:
  typedef enum {
        SDL_AUDIO_STOPPED = 0,
        SDL_AUDIO_PLAYING,
        SDL_AUDIO_PAUSED
  } SDL_audiostatus;
 int SDL_audiostatus SDL_GetAudioStatus(void);
 */
FCODE_1_0(SDL_GetAudioStatus);

/** SDL_LoadWAV_RW ( audio_len audio_buf spec* freesrc rwops* -- spec* )
 * This function loads a WAVE from the data source, automatically freeing
 * that source if 'freesrc' is non-zero.  For example, to load a WAVE file,
 * you could do:
 *      SDL_LoadWAV_RW(SDL_RWFromFile("sample.wav", "rb"), 1, ...);
 *
 * If this function succeeds, it returns the given SDL_AudioSpec,
 * filled with the audio data format of the wave data, and sets
 * 'audio_buf' to a malloc()'d buffer containing the audio data,
 * and sets 'audio_len' to the length of that audio buffer, in bytes.
 * You need to free the audio buffer with SDL_FreeWAV() when you are 
 * done with it.
 *
 * This function returns NULL and sets the SDL error message if the 
 * wave file cannot be opened, uses an unknown data format, or is 
 * corrupt.  Currently raw and MS-ADPCM WAVE files are supported.
 SDL_AudioSpec *SDL_LoadWAV_RW (
		SDL_RWops *src, int freesrc,
                SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len);
 */
FCODE_1_5(SDL_LoadWAV_RW);


/** SDL_LoadWAV ( audio_len audio_buf spec* rwops* -- spec* )
 * Compatibility convenience function -- loads a WAV from a file 
 * see => SDL_LoadWAV_RW
 */
FCODE_1_4(SDL_LoadWAV);


/** SDL_FreeWAV ( audiobuf* -- )
 * This function frees data previously allocated with SDL_LoadWAV_RW()
 void SDL_FreeWAV(Uint8 *audio_buf);
 */
FCODE_0_1 (SDL_FreeWAV);

/** SD_BuildAudioCVT ( dst_rate dst_channels dst_format 
			src_rate dst_channels src_forat audioCVT* )
 * This function takes a source format and rate and a destination format
 * and rate, and initializes the 'cvt' structure with information needed
 * by SDL_ConvertAudio() to convert a buffer of audio data from one format
 * to the other.
 * This function returns 0, or -1 if there was an error.
 int SDL_BuildAudioCVT(SDL_AudioCVT *cvt,
                Uint16 src_format, Uint8 src_channels, int src_rate,
                Uint16 dst_format, Uint8 dst_channels, int dst_rate);
 */
FCODE_1_1 (SDL_BuildAudioCVT);


/** SDL_ConvertAudio ( cvt* -- standard-inst )
 * Once you have initialized the 'cvt' structure using SDL_BuildAudioCVT(),
 * created an audio buffer cvt->buf, and filled it with cvt->len bytes of
 * audio data in the source format, this function will convert it in-place
 * to the desired format.
 * The data conversion may expand the size of the audio data, so the buffer
 * cvt->buf should be allocated after the cvt structure is initialized by
 * SDL_BuildAudioCVT(), and should be cvt->len*cvt->len_mult bytes long.
 int SDL_ConvertAudio(SDL_AudioCVT *cvt);
 */
FCODE_1_1(SDL_ConvertAudio);

/** SDL_MixAudio ( volume len src dst -- )
 * This takes two audio buffers of the playing audio format and mixes
 * them, performing addition, volume adjustment, and overflow clipping.
 * The volume ranges from 0 - 128, and should be set to SDL_MIX_MAXVOLUME
 * for full audio volume.  Note this does not change hardware volume.
 * This is provided for convenience -- you can mix your own audio data.
 define SDL_MIX_MAXVOLUME 128
 void SDL_MixAudio(Uint8 *dst, Uint8 *src, Uint32 len, int volume);
 */
FCODE_0_4(SDL_MixAudio, int, Uint32, Uint8*, Uint8*);

/** SDL_LockAudio ( -- )
 * The lock manipulated by these functions protects the callback function.
 * During a LockAudio/UnlockAudio pair, you can be guaranteed that the
 * callback function is not running.  Do not call these from the callback
 * function or you will cause deadlock.
 void SDL_LockAudio(void);
 */
FCODE_0_0(SDL_LockAudio);

/** SDL_UnlockAudio ( -- )
 * The lock manipulated by these functions protects the callback function.
 * During a LockAudio/UnlockAudio pair, you can be guaranteed that the
 * callback function is not running.  Do not call these from the callback
 * function or you will cause deadlock.
 void SDL_UnlockAudio(void);
 */
FCODE_0_0(SDL_UnlockAudio);

/* SDL_CloseAudio ( -- )
 * This function shuts down audio processing and closes the audio device.
 void SDL_CloseAudio(void);
 */
FCODE_0_0(SDL_CloseAudio);


/* #################################################################
SDL_audio
SDL_cdrom
SDL_joystick
SDL_event
SDL_video
SDL_byteorder
*/

/* ====== Id: SDL_version.h,v 1.2.2.9 2000/09/28 ====== */

/** SDL_Linked_Version ( -- SDL_version* )
 *  This function gets the version of the dynamically linked SDL library.
 *  it should NOT be used to fill a version structure, instead you should
 *  use the SDL_Version() macro. see => SDL_LinkedVersion
 extern DECLSPEC const SDL_version * SDL_Linked_Version(void);
 */
FCODE_1_0(SDL_Linked_Version);

/** SDL_LinkedVersion ( -- SDL_versionnum )
 *  This function gets the version of the dynamically linked SDL library.
 *  it should NOT be used to fill a version structure, instead you should
 *  use the SDL_Version() macro. see => SDL_LinkedVersion
 implementation applies the SDL_VERSIONNUM macro to
 extern DECLSPEC const SDL_version * SDL_Linked_Version(void);
 */
FCode(SDL_LinkedVersion)
{
    SDL_version* version = SDL_Linked_Version ();
    FX_PUSH (SDL_VERSIONNUM (version->major, version->minor, version->patch));
}

/** SDL_CompiledVersion ( -- SDL_versionnum )
 *  This function gets the version of the SDL library headers the PFE 
 *  has been linked with. It returns the compact code from SDL_VERSIONNUM
 *  macro
 */
FCode(SDL_CompiledVersion)
{
    SDL_version version;
    SDL_VERSION(&version);
    FX_PUSH (SDL_VERSIONNUM (version.major, version.minor, version.patch));
}

P4_LISTWORDS(lib_sdl) =
{
    P4_INTO ("[SDL]", 0),
/* ====== Id: SDL.h,v 1.5.2.5 2000/09/07 ====== */

/** These are the flags which may be passed to SDL_Init() -- you should
   specify the subsystems which you will be using in your application.
*/
    P4_OCON ("SDL_INIT_TIMER",		SDL_INIT_TIMER),
    P4_OCON ("SDL_INIT_AUDIO",		SDL_INIT_AUDIO),
    P4_OCON ("SDL_INIT_VIDEO",		SDL_INIT_VIDEO),
    P4_OCON ("SDL_INIT_CDROM",		SDL_INIT_CDROM),
    P4_OCON ("SDL_INIT_JOYSTICK",	SDL_INIT_JOYSTICK),
    P4_OCON ("SDL_INIT_NOPARACHUTE",	SDL_INIT_NOPARACHUTE),
    P4_OCON ("SDL_INIT_EVENTTHREAD",	SDL_INIT_EVENTTHREAD),
    P4_OCON ("SDL_INIT_EVERYTHING",	SDL_INIT_EVERYTHING),

    P4_FXCO ("SDL_init",		SDL_init),
    P4_FXCO ("SDL_InitSubSystem",	SDL_InitSubSystem),
    P4_FXCO ("SDL_QuitSubSystem",	SDL_QuitSubSystem),
    P4_FXCO ("SDL_WasInit",		SDL_WasInit),
    P4_FXCO ("SDL_Quit",		SDL_Quit),

/* ====== Id: SDL_main.h,v 1.3.2.2 2000/08/29 ====== */

/* To Be Done */

/* ====== Id: SDL_types.h,v 1.4.2.4 2000/10/12 ====== */
    P4_OCON ("SDL_FALSE",		SDL_FALSE),
    P4_OCON ("SDL_TRUE",		SDL_TRUE),
    P4_OCON ("SDL_PRESSED",		SDL_PRESSED),
    P4_OCON ("SDL_RELEASED",		SDL_RELEASED),

/* ====== Id: SDL_error.h,v 1.4.2.1 2000/03/16 ====== */

    P4_FXCO ("SDL_SetError",		SDL_SetError),
    P4_FXCO ("SDL_GetError",		SDL_GetError),
    P4_FXCO ("SDL_ClearError",		SDL_ClearError),

/* ====== Id: SDL_rwops.h,v 1.4.2.2 2000/05/30 ====== */

    P4_FXCO ("SDL_RWFromFile",		SDL_RWFromFile),
    P4_FXCO ("SDL_RWFromFP",		SDL_RWFromFP),
    P4_FXCO ("SDL_RWFromMem",		SDL_RWFromMem),
    P4_FXCO ("SDL_AllocRW",		SDL_AllocRW),
    P4_FXCO ("SDL_FreeRW",		SDL_FreeRW),
    P4_FXCO ("SDL_RWseek",		SDL_RWseek),
    P4_FXCO ("SDL_RWtell",		SDL_RWtell),
    P4_FXCO ("SDL_RWread",		SDL_RWread),
    P4_FXCO ("SDL_RWwrite",		SDL_RWwrite),
    P4_FXCO ("SDL_RWclose",		SDL_RWclose),

/* ====== Id: SDL_timer.h,v 1.4.2.6 2000/10/02 ====== */

    P4_FXCO ("SDL_GetTick",		SDL_GetTick),
    P4_FXCO ("SDL_Delay",		SDL_Delay),

/* ====== Id: SDL_audio.h,v 1.4.2.6 2000/03/16 ====== */
    P4_OFFS ("->SDL_AudioSpec.freq",	  offsetof(SDL_AudioSpec,freq)),
    P4_OFFS ("->SDL_AudioSpec.format",    offsetof(SDL_AudioSpec,format)),
    P4_OFFS ("->SDL_AudioSpec.channels",  offsetof(SDL_AudioSpec,channels)),
    P4_OFFS ("->SDL_AudioSpec.silence",   offsetof(SDL_AudioSpec,silence)),
    P4_OFFS ("->SDL_AudioSpec.samples",   offsetof(SDL_AudioSpec,samples)),
    P4_OFFS ("->SDL_AudioSpec.size",      offsetof(SDL_AudioSpec,size)),
    P4_OCON ("/SDL_AudioSpec",		  sizeof(SDL_AudioSpec)),

    P4_OCON ("AUDIO_U8",		AUDIO_U8),
    P4_OCON ("AUDIO_S8",		AUDIO_S8),
    P4_OCON ("AUDIO_U16LSB",		AUDIO_U16LSB),
    P4_OCON ("AUDIO_S16LSB",		AUDIO_S16LSB),
    P4_OCON ("AUDIO_U16MSB",		AUDIO_U16MSB),
    P4_OCON ("AUDIO_S16MSB",		AUDIO_S16MSB),
    P4_OCON ("AUDIO_U16",		AUDIO_U16),
    P4_OCON ("AUDIO_S16",		AUDIO_S16),
    P4_OCON ("AUDIO_U16SYS",		AUDIO_U16SYS),
    P4_OCON ("AUDIO_S16SYS",		AUDIO_S16SYS),

    P4_OFFS ("->SDL_AudioCVT.needed",	  offsetof(SDL_AudioCVT,needed)),
    P4_OFFS ("->SDL_AudioCVT.src_format", offsetof(SDL_AudioCVT,scr_format)),
    P4_OFFS ("->SDL_AudioCVT.dst_format", offsetof(SDL_AudioCVT,dst_format)),
    P4_OFFS ("->SDL_AudioCVT.rate_incr",  offsetof(SDL_AudioCVT,rate_incr)),
    P4_OFFS ("->SDL_AudioCVT.buf",        offsetof(SDL_AudioCVT,buf)),
    P4_OFFS ("->SDL_AudioCVT.len",        offsetof(SDL_AudioCVT,len)),
    P4_OFFS ("->SDL_AudioCVT.len_cvt",    offsetof(SDL_AudioCVT,len_cvt)),
    P4_OFFS ("->SDL_AudioCVT.len_mult",   offsetof(SDL_AudioCVT,len_mult)),
    P4_OFFS ("->SDL_AudioCVT.len_ratio",  offsetof(SDL_AudioCVT,ratio)),
    P4_OCON ("/SDL_AudioCVT",		  sizeof(SDL_AudioCVT)),

    P4_FXCO("SDL_AudioInit",		SDL_AudioInit),
    P4_FXCO("SDL_AudioQuit",		SDL_AudioQuit),
    P4_FXCO("SDL_AudioDriverName",	SDL_AudioDriverName),
    P4_FXCO("SDL_OpenAudio",		SDL_OpenAudio),
    P4_FXCO("SDL_GetAudioStatus",	SDL_GetAudioStatus),
    P4_OCON("SDL_AUDIO_STOPPED",        SDL_AUDIO_STOPPED),
    P4_OCON("SDL_AUDIO_PLAYING",	SDL_AUDIO_PLAYING),
    P4_OCON("SDL_AUDIO_PAUSED",		SDL_AUDIO_PAUSED),
    P4_FXCO("SDL_LoadWAV_RW",		SDL_LoadWAV_RW),
    P4_FXCO("SDL_LoadWAV",		SDL_LoadWAV),
    P4_FXCO("SDL_FreeWAV",		SDL_FreeWAV),
    P4_FXCO("SDL_BuildAudioCVT",	SDL_BuildAudioCVT),
    P4_FXCO("SDL_ConvertCVT",		SDL_ConvertCVT),
    P4_FXCO("SDL_MixAudio",		SDL_MixAudio),
    P4_FXCO("SDL_LockAudio",		SDL_LockAudio),
    P4_FXCO("SDL_UnLockAudio",		SDL_UnLockAudio),
    P4_FXCO("SDL_LockAudio",		SDL_LockAudio),
    P4_FXCO("SDL_UnlockAudio",		SDL_UnlockAudio),
    P4_FXCO("SDL",			SDL_CloseWindow,


/*ajsdfkajask*/

/* ====== Id: SDL_version.h,v 1.2.2.9 2000/09/28 ====== */
    P4_OFFS ("->SDL_version.major",	offsetof(SDL_version,major)),
    P4_OFFS ("->SDL_version.minor",	offsetof(SDL_version,minor)),
    P4_OFFS ("->SDL_version.patch",	offsetof(SDL_version,patch)),
    P4_FXCO ("SDL_Linked_Version",	SDL_Linked_Version),
    P4_FXCO ("SDL_LinkedVersion",	SDL_LinkedVersion),
    P4_FXCO ("SDL_CompiledVersion",	SDL_CompiledVersion),
};
P4_COUNTWORDS(lib_sdl, "libSDL a Simple Direct Layer for graphic frontends");

   /* PFE_HAVE_SDL_SDL_H */
#endif







