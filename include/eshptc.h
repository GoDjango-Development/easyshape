/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#ifndef ESHPTC_H
#define ESHPTC_H

#include <eshp.h>
#include <inttypes.h>

/* Callback to receive TC output buffer. */
extern eshp_aclbk eshp_tcclbk;

/* Clear all rules in device. */
int eshp_clrdev(const char *dev);
/* Set HTB for the device. */
int eshp_setdev(const char *dev);
/* Set class priority with speed rate for the device. */
int eshp_setsp(const char *dev, int id, int64_t sp, const char *un, int qm,
	int prio);
/* Set traffic for the specified speed class and device. */
int eshp_settf(const char *dev, char *src, char *dst, int spid, int prio,
	int iptype);

#endif
