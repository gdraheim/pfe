#define _POSIX_SOURCE

/* this include should be changed somewhen!! */
#include <pfe/engine-set.h>
#include <pfe/option-set.h>

int main (int argc, char** argv)
{
    int i;
    p4_Session session;
    p4_Thread  pfe;
    p4_SetOptions (&session, sizeof(session), 0, 0);
    session.quiet = 1;
    if (i=p4_InitVM(&pfe, &session))
    {
	fputs ("error during initVM", stderr);
	return 1;
    }

    for (i=1; i<argc; i++)
    {
	fputs ("\n", stdout);
	fputs (argv[i], stdout);
	fputs ("\n", stdout);
	p4_Evaluate (&pfe, argv[i], strlen (argv[i]));
    }
    fputs ("\nOK\n", stdout);
    p4_DeinitVM(&pfe);
    return 0;
}

	
