/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#include <eshp.h>
#include <eshptc.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>

/* Clear any configuration in all devices configuration. */
static int clrdev_loop(struct eshp_dev *devs);
/* Set root handle Hierarchical Token Bucket (HTB) for all devices. */
static int setdev_loop(struct eshp_dev *devs);
/* Set speed clases for all devices. */
static int setsp_loop(struct eshp_dev *devs, struct eshp_speed *sps);
/* Return device from id. */
static char *devfid(struct eshp_dev *devs, int id);
/* Set traffic for speed classes in the the devices. */
static int settf_loop(struct eshp_dev *devs, struct eshp_speed *sps,
	struct eshp_traffic *tfs);
/* Return speed from id. */
static struct eshp_speed *spfid(struct eshp_speed *sps, int id);

int eshp_load(const char *path, struct eshp *eshp)
{
	if (!path || !eshp)
		return EESHPFILE;
	int rc;
	char *buf = eshp_ldcfg(path, &rc);
	if (rc)
		return rc;
	rc = eshp_setdevs(buf, &eshp->devs);
	if (rc) {
		free(buf);
		return rc;
	}
	rc = eshp_setsps(buf, &eshp->sps);
	if (rc) {
		free(buf);
		return rc;
	}
	rc = eshp_settfs(buf, &eshp->tfs);
	if (rc) {
		free(buf);
		return rc;
	}
	return 0;
}

int eshp_apply(struct eshp *eshp, eshp_aclbk clbk)
{
	if (!eshp)
		return EESHPAPPLY;
	eshp_tcclbk = clbk;
	int rc;
	rc = clrdev_loop(&eshp->devs);
	if (rc)
		return rc;
	rc = setdev_loop(&eshp->devs);
	if (rc) {
		/* Try to rollback any device change. No error checking needed.
			Best effort approach here. */
		clrdev_loop(&eshp->devs);
		return rc;
	}
	rc = setsp_loop(&eshp->devs, &eshp->sps);
	if (rc) {
		/* Try to rollback any device change. No error checking needed.
			Best effort approach here. */
		clrdev_loop(&eshp->devs);
		return rc;
	}

	rc = settf_loop(&eshp->devs, &eshp->sps, &eshp->tfs);
	if (rc) {
		/* Try to rollback any device change. No error checking needed.
			Best effort approach here. */
		clrdev_loop(&eshp->devs);
		return rc;
	}
	return 0;
}

void eshp_free(struct eshp *eshp)
{
	if (eshp) {
		eshp_freedevs(&eshp->devs);
		eshp_freesps(&eshp->sps);
		eshp_freetfs(&eshp->tfs);
		memset(eshp, 0, sizeof(struct eshp));
	}
}

static int clrdev_loop(struct eshp_dev *devs)
{
	int rc;
	while (devs) {
		rc = eshp_clrdev(devs->nam);
		if (rc)
			return rc;
		devs = devs->ndev;
	}
	return 0;
}

static int setdev_loop(struct eshp_dev *devs)
{
	int rc;
	while (devs) {
		rc = eshp_setdev(devs->nam);
		if (rc)
			return rc;
		devs = devs->ndev;
	}
	return 0;
}

static int setsp_loop(struct eshp_dev *devs, struct eshp_speed *sps)
{
	int rc;
	char *devnam;
	while (sps) {
		devnam = devfid(devs, sps->pid);
		if (!devnam)
			return EESHPSETSP;
		rc = eshp_setsp(devnam, sps->id, sps->sp, sps->un, sps->qm, sps->prio);
		if (rc)
			return rc;
		sps = sps->nsp;
	}
	return 0;
}

static char *devfid(struct eshp_dev *devs, int id)
{
	while (devs) {
		if (devs->id == id)
			return devs->nam;
		devs = devs->ndev;
	}
	return NULL;
}

static int settf_loop(struct eshp_dev *devs, struct eshp_speed *sps,
	struct eshp_traffic *tfs)
{
	int rc;
	const char *devnam;
	while (tfs) {
		struct eshp_speed *sp = spfid(sps, tfs->spid);
		if (!sp)
			return EESHPSETTF;
		devnam = devfid(devs, sp->pid);
		if (!devnam)
			return EESHPSETTF;
		rc = eshp_settf(devnam, tfs->src, tfs->dst, tfs->spid, tfs->prio);
		if (rc)
			return rc;
		tfs = tfs->ntf;
	}
	return 0;
}

static struct eshp_speed *spfid(struct eshp_speed *sps, int id)
{
	while (sps) {
		if (sps->id == id)
			return sps;
		sps = sps->nsp;
	}
	return NULL;
}
