/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#ifndef ESHPCFG_H
#define ESHPCFG_H

#include <inttypes.h>
#define DEVNAMLEN 64
#define UNNAMLEN 12
#define IPLEN 64

/* Network devices info. */
struct eshp_dev {
	char nam[DEVNAMLEN];
	int id;
	struct eshp_dev *ndev;
};

/* Speeds info. */
struct eshp_speed {
	int id;
	int pid;
	int64_t sp;
	char un[UNNAMLEN];
	int prio;
	int64_t qm;
	struct eshp_speed *nsp;
};

/* Traffics info. */
struct eshp_traffic {
	char src[IPLEN];
	char dst[IPLEN];
	int spid;
	int prio;
	struct eshp_traffic *ntf;
};

/* Return a string buffer containing the configuration file. */
char *eshp_ldcfg(const char *path, int *rc);
/* Extract the devices from str and set them in the eshp_dev structure. */
int eshp_setdevs(char *str, struct eshp_dev *devs);
/* Extract the speeds from str and set them in the eshp_speed structure. */
int eshp_setsps(const char *str, struct eshp_speed *sps);
/* Extract the traffic from str and set them in the eshp_traffic structure. */
int eshp_settfs(const char *str, struct eshp_traffic *tfs);
/* Free the esph_traffic linked-list strcutre. */
void eshp_freetfs(struct eshp_traffic *tfs);
/* Free the esph_speed linked-list strcutre. */
void eshp_freesps(struct eshp_speed *sps);
/* Free the esph_dev linked-list strcutre. */
void eshp_freedevs(struct eshp_dev *devs);

#endif
