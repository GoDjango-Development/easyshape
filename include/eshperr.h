/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#ifndef ESHPERR_H
#define ESHPERR_H

/* Define error numbers. */
/* Undefined error. */
#define EESHPMSG 1
/* Error loading configuration file. */
#define EESHPFILE 2
/* Error reserving memory. */
#define EESHPMEM 3
/* Error extracting devices form configuration file. */
#define EESHPDEV 4
/* Error extracting speeds from configuration file. */
#define EESHPSPD 5
/* Error extracting traffics from configuration file. */
#define EESHPTRF 6
/* Error creating pipe for invoking TC. */
#define EESHPPIPE 7
/* Error executing TC binary image. */
#define EESHPEXEC 8
/* Error during TC running. */
#define EESHPRUN 9
/* Eror applying Traffic Shapping. */
#define EESHPAPPLY 10
/* Error setting device configuration. */
#define EESHPSETDEV 11
/* Sintax error while parsing speeds. */
#define ESHPSPSIN 12
/* Sintax error while parsing traffics. */
#define ESHPTFSIN 13
/* Sintax error while parsing devices. */
#define ESHPDEVSIN 14
/* Error setting speed configuration. */
#define EESHPSETSP 15
/* Error setting traffic configuration. */
#define EESHPSETTF 16

/* Return the error message equivalent to the error number. */
const char *eshp_errmsg(int eno);

#endif
