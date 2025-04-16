/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#include <eshperr.h>

/* Define error messages. */
#define EESHPUMSGSTR "ESHP->: Undefined error."
#define EESHPFILESTR "ESHP->: Error loading configuration file."
#define EESHPMEMSTR "ESHP->: Error reserving memory."
#define EESHPDEVSTR "ESHP->: Error extracting devices form configuration file."
#define EESHPSPDSTR "ESHP->: Error extracting speeds from configuration file."
#define EESHPTRFSTR "ESHP->: Error extracting traffics from configuration file."
#define EESHPPIPESTR "ESHP->: Error creating pipe for invoking TC."
#define EESHPEXECSTR "ESHP->: Error executing TC binary image."
#define EESHPRUNSTR "ESHP->: Error during TC execution."
#define EESHPAPPLYSTR "ESHP->: Eror applying Traffic Shapping."
#define EESHPSETDEVSTR "ESHP->: Error setting device configuration."
#define ESHPSPSINSTR "ESHP->: Sintax error while parsing speeds."
#define ESHPTFSINSTR "ESHP->: Sintax error while parsing traffics."
#define ESHPDEVSINSTR "ESHP->: Sintax error while parsing devices."
#define EESHPSETSPSTR "ESHP->: Error setting speed configuration."
#define EESHPSETTFSTR "ESHP->: Error setting traffic configuration."

const char *eshp_errmsg(int eno)
{
	switch (eno) {
		case EESHPFILE:
			return EESHPFILESTR;
		case EESHPMEM:
			return EESHPMEMSTR;
		case EESHPDEV:
			return EESHPDEVSTR;
		case EESHPSPD:
			return EESHPSPDSTR;
		case EESHPTRF:
			return EESHPTRFSTR;
		case EESHPPIPE:
			return EESHPPIPESTR;
		case EESHPEXEC:
			return EESHPEXECSTR;
		case EESHPRUN:
			return EESHPRUNSTR;
		case EESHPAPPLY:
			return EESHPAPPLYSTR;
		case EESHPSETDEV:
			return EESHPSETDEVSTR;
		case ESHPSPSIN:
			return ESHPSPSINSTR;
		case ESHPTFSIN:
			return ESHPTFSINSTR;
		case ESHPDEVSIN:
			return ESHPDEVSINSTR;
		case EESHPSETSP:
			return EESHPSETSPSTR;
		case EESHPSETTF:
			return EESHPSETTFSTR;
	}
	return EESHPUMSGSTR;
}
