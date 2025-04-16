/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#include <eshp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <malloc.h>

#define MINARGS 2
#define EARGSNO "Configuration file is missing."
#define ESUCESS "ESHP->: Successfully termination."
void tcoutrcv(const char *buf);

int main(int argc, char *argv[])
{
	if (argc < MINARGS) {
		printf("%s\n", EARGSNO);
		exit(EXIT_FAILURE);
	}
	struct eshp eshp;
	int rc;
	const char *cfgfile = *(argv + 1);
	if ((rc = eshp_load(cfgfile, &eshp))) {
		printf("%s\n", eshp_errmsg(rc));
		exit(EXIT_FAILURE);
	}
	if (rc = eshp_apply(&eshp, tcoutrcv)) {
		printf("%s\n", eshp_errmsg(rc));
		exit(EXIT_FAILURE);
	}
	printf("%s\n", ESUCESS);
	return 0;
}

void tcoutrcv(const char *buf)
{
	printf("%s", buf);
}
