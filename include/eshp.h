/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#ifndef ESHP_H
#define ESHP_H

#include <eshpcfg.h>
#include <eshperr.h>

/* For the sake of the easiest possible use of TC (Traffic Control):
	implementing traffic shaping rules for specific IP addresses.
	This approach uses u32 filters to match traffic by source and destination.*/

struct eshp {
	struct eshp_dev devs;
	struct eshp_speed sps;
	struct eshp_traffic tfs;
};

/* eshp_apply callback to return the TC output .*/
typedef void (*eshp_aclbk)(const char *buf);

/* This function loads the configuration file specified by the first argument.
	It return 0 for sucessful and the configuration file structred in the eshp
	pointer. Otherwise it return a error number. The pointer to eshp cannot be
	NULL. */
int eshp_load(const char *path, struct eshp *eshp);
/* This function applys the loaded and verified configuration pointed by the
	eshp pointer.The result of calling eshp_apply will be an integer; 0 for
	successful or an error number. Any output from the TC tool will come
	throguh the callback. */
int eshp_apply(struct eshp *eshp, eshp_aclbk clbk);
/* This function free all memory used by the struct eshp. */
void eshp_free(struct eshp *eshp);

#endif
