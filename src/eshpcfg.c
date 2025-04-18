/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#include <eshpcfg.h>
#include <eshperr.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define DEVTOK "devices{"
#define ITEMSEP ";"
#define BEGINOPT "{"
#define ENDOPT "}"
#define SPDTOK "speeds{"
#define SUBITEMSEP ":"
#define SPDKBSYM "kbit"
#define SPDMBSYM "mbit"
#define SPDGBSYM "gbit"
#define TFTOK "traffics{"
#define TFIPSEP ","
#define COMMTOK '#'
#define COMMSUB ' '
#define IPV4 "ipv4"
#define IPV6 "ipv6"

/* Minify the lodad file to better manipulation. */
static void shrkstr(char *str);
/*  Helper function for setpsp. */
static int brksp(char *str, struct eshp_speed *sp);
/*  Helper function for settfs. */
static int brktf(char *str, struct eshp_traffic *tf);
/*  Helper function for setdevs. */
static int brkdev(char *str, struct eshp_dev *dev);
/* Removed commented lines. */
static void rmcomm(char *str);

int eshp_settfs(const char *str, struct eshp_traffic *tfs)
{
	memset(tfs, 0, sizeof(struct eshp_traffic));
	int len = strlen(str);
	char *strcp = malloc(len + 1);
	if (!strcp)
		return EESHPMEM;
	strcpy(strcp, str);
	*(strcp + len) = '\0';
	char *strpt = strstr(strcp, TFTOK);
	if (!strpt) {
		free(strcp);
		return EESHPTRF;
	}
	strpt += strlen(TFTOK);
	char *strend = strstr(strpt, ENDOPT);
	if (!strend) {
		free(strcp);
		return EESHPTRF;
	}
	*strend = '\0';
	char *strtf;
	char *tfstok = strpt;
	struct eshp_traffic **ntfs = &tfs;
	char *toksv = NULL;
	while ((strtf = strtok_r(tfstok, ITEMSEP,  &toksv))) {
		if (tfstok)
			tfstok = NULL;
		if (!*ntfs) {
			*ntfs = malloc(sizeof(struct eshp_traffic));
			if (!*ntfs) {
				eshp_freetfs(tfs);
				free(strcp);
				return EESHPMEM;
			}
			memset(*ntfs, 0, sizeof(struct eshp_traffic));
		}
		int rc = brktf(strtf, *ntfs);
		if (rc) {
			eshp_freetfs(tfs);
			free(strcp);
			return ESHPTFSIN;
		}
		ntfs = &(*ntfs)->ntf;
	}
	free(strcp);
	return 0;
}

int eshp_setsps(const char *str, struct eshp_speed *sps)
{
	memset(sps, 0, sizeof(struct eshp_speed));
	int len = strlen(str);
	char *strcp = malloc(len + 1);
	if (!strcp)
		return EESHPMEM;
	strcpy(strcp, str);
	*(strcp + len) = '\0';
	char *strpt = strstr(strcp, SPDTOK);
	if (!strpt) {
		free(strcp);
		return EESHPSPD;
	}
	strpt += strlen(SPDTOK);
	char *strend = strstr(strpt, ENDOPT);
	if (!strend) {
		free(strcp);
		return EESHPSPD;
	}
	*strend = '\0';
	char *strsp;
	char *spstok = strpt;
	struct eshp_speed **nsps = &sps;
	char *toksv = NULL;
	while ((strsp = strtok_r(spstok, ITEMSEP,  &toksv))) {
		if (spstok)
			spstok = NULL;
		if (!*nsps) {
			*nsps = malloc(sizeof(struct eshp_speed));
			if (!*nsps) {
				eshp_freesps(sps);
				free(strcp);
				return EESHPMEM;
			}
			memset(*nsps, 0, sizeof(struct eshp_speed));
		}
		int rc = brksp(strsp, *nsps);
		if (rc) {
			eshp_freesps(sps);
			free(strcp);
			return ESHPSPSIN;
		}
		nsps = &(*nsps)->nsp;
	}
	free(strcp);
	return 0;
}

int eshp_setdevs(char *str, struct eshp_dev *devs)
{
	memset(devs, 0, sizeof(struct eshp_dev));
	int len = strlen(str);
	char *strcp = malloc(len + 1);
	if (!strcp)
		return EESHPMEM;
	strcpy(strcp, str);
	*(strcp + len) = '\0';
	char *strpt = strstr(strcp, DEVTOK);
	if (!strpt) {
		free(strcp);
		return EESHPDEV;
	}
	strpt += strlen(DEVTOK);
	char *strend = strstr(strpt, ENDOPT);
	if (!strend) {
		free(strcp);
		return EESHPDEV;
	}
	*strend = '\0';
	char *strdev;
	char *devtok = strpt;
	struct eshp_dev **ndev = &devs;
	char *toksv = NULL;
	while ((strdev = strtok_r(devtok, ITEMSEP, &toksv))) {
		if (devtok)
			devtok = NULL;
		if (!*ndev) {
			*ndev = malloc(sizeof(struct eshp_dev));
			if (!*ndev) {
				eshp_freedevs(devs);
				free(strcp);
				return EESHPMEM;
			}
			memset(*ndev, 0, sizeof(struct eshp_dev));
		}
		int rc = brkdev(strdev, *ndev);
		if (rc) {
			eshp_freedevs(devs);
			free(strcp);
			return ESHPDEVSIN;
		}
		ndev = &(*ndev)->ndev;
	}
	free(strcp);
	return 0;
}

char *eshp_ldcfg(const char *path, int *rc)
{
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		*rc = EESHPFILE;
		return NULL;
	}
	struct stat st;
	if (stat(path, &st) == -1) {
		close(fd);
		*rc = EESHPFILE;
		return NULL;
	}
	char *buf = malloc(st.st_size + 1);
	if (!buf) {
		close(fd);
		*rc = EESHPMEM;
		return NULL;
	}
	int rb;
    do
        rb = read(fd, buf, st.st_size);
    while (rb == -1 && errno == EINTR);
	if (rb == -1) {
		close(fd);
		free(buf);
		*rc = EESHPFILE;
		return NULL;
	}
	*(buf + st.st_size) = '\0';
	*rc = 0;
	shrkstr(buf);
	return buf;
}

void eshp_freedevs(struct eshp_dev *devs)
{
	struct eshp_dev *svdev, *ndev = devs->ndev;
	while (ndev) {
		svdev = ndev->ndev;
		free(ndev);
		ndev = svdev;
	}
	memset(devs, 0, sizeof(struct eshp_dev));
}

void eshp_freesps(struct eshp_speed *sps)
{
	struct eshp_speed *svsp, *nsp = sps->nsp;
	while (nsp) {
		svsp = nsp->nsp;
		free(nsp);
		nsp = svsp;
	}
	memset(sps, 0, sizeof(struct eshp_speed));
}

void eshp_freetfs(struct eshp_traffic *tfs)
{
	struct eshp_traffic *svtf, *ntf = tfs->ntf;
	while (ntf) {
		svtf = ntf->ntf;
		free(ntf);
		ntf = svtf;
	}
	memset(tfs, 0, sizeof(struct eshp_traffic));
}

static void shrkstr(char *str)
{
	rmcomm(str);
	const char rmsym[] = "\n\t\v ";
	int c = 0, c1, c2 = 0;
	int len = strlen(str);
	int lensym = strlen(rmsym);
	int jump = 0;
	for (; c < len; c++) {
		for (c1 = 0; c1 < lensym; c1++)
			if (*(str + c) == rmsym[c1]) {
				jump = 1;
				break;
			}
		if (jump) {
			jump = 0;
			continue;
		}
		*(str + c2) = *(str + c);
		c2++;
	}
	*(str + c2) = '\0';
}

static int brksp(char *str, struct eshp_speed *sp)
{
	char *strcp = malloc(strlen(str) + 1);
	if (!strcp)
		return -1;
	char *toksv = NULL;
	strcpy(strcp, str);
	char *pt = strtok_r(strcp, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	sp->pid = atoi(pt);
	pt = strtok_r(NULL, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	sp->id = atoi(pt);
	pt = strtok_r(NULL, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	sp->sp = atoll(pt);
	if (strstr(pt, SPDKBSYM))
		strcpy(sp->un, SPDKBSYM);
	else if (strstr(pt, SPDMBSYM))
		strcpy(sp->un, SPDMBSYM);
	else if (strstr(pt, SPDGBSYM))
		strcpy(sp->un, SPDGBSYM);
	else {
		free(strcp);
		return -1;
	}
	pt = strtok_r(NULL, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	sp->qm = atoll(pt);
	pt = strtok_r(NULL, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	sp->prio = atoi(pt);
	free(strcp);
	return 0;
}

static int brktf(char *str, struct eshp_traffic *tf)
{
	char *strcp = malloc(strlen(str) + 1);
	if (!strcp)
		return -1;
	char *toksv = NULL;
	strcpy(strcp, str);
	char *pt = strtok_r(strcp, TFIPSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	strcpy(tf->src, pt);
	pt = strtok_r(NULL, TFIPSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	strcpy(tf->dst, pt);
	pt = strtok_r(NULL, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	tf->spid = atoi(pt);
	pt = strtok_r(NULL, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	tf->prio = atoi(pt);
	pt = strtok_r(NULL, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	if (!strcmp(pt, IPV4))
		tf->iptype = 4;
	else if (!strcmp(pt, IPV6))
		tf->iptype = 6;
	else {
		free(strcp);
		return -1;
	}
	free(strcp);
	return 0;
}

static void rmcomm(char *str)
{
	int in = 0;
	while (*str) {
		if (*str == COMMTOK && !in) {
			in = 1;
			continue;
		} else if (*str == '\n' && in) {
			in = 0;
			continue;
		}
		if (in)
			*str = COMMSUB;
		str++;
	}
}

static int brkdev(char *str, struct eshp_dev *dev)
{
	char *strcp = malloc(strlen(str) + 1);
	if (!strcp)
		return -1;
	strcpy(strcp, str);
	char *toksv = NULL;
	char *pt = strtok_r(strcp, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	strcpy(dev->nam, pt);
	pt = strtok_r(NULL, SUBITEMSEP, &toksv);
	if (!pt) {
		free(strcp);
		return -1;
	}
	dev->id = atoi(pt);
	free(strcp);
	return 0;
}
