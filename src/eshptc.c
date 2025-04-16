/*
 * Programmer Luis Miguel
 * lmdelbahia@gmail.com
 * luismiguel@godjango.dev
 */

#include <eshptc.h>
#include <eshperr.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>

#define TCBIN "/sbin/tc"
#define TCQDISC "qdisc"
#define TCDEL "del"
#define TCDEV "dev"
#define TCROOT "root"
#define TCADD "add"
#define TCHANDLE "handle"
#define TCHTB "htb"
#define TCCLASSID "classid"
#define TCCLASSIDID "1:"
#define TCPARENT "parent"
#define TCRATE "rate"
#define TCCEIL "ceil"
#define TCCLASS "class"
#define TCQUANTUM "quantum"
#define TCPRIO "prio"
#define TCPARENTID "1:"
#define INT32DIG (sizeof(int) * 3 / 2 + sizeof(int))
#define INT64DIG (sizeof(int64_t) * 3 / 2 + sizeof(int64_t))
#define TCFILTER "filter"
#define TCPROTOCOL "protocol"
#define TCIP "ip"
#define TCMATCH "match"
#define TCSRC "src"
#define TCDST "dst"
#define TCFLOW "flowid"
#define TCFLOWID "1:"
#define TCU32 "u32"
#define TCIPWILDCARD '*'
#define TCANYHOST "0.0.0.0/0"

/* Callback to send output buffer. */
eshp_aclbk eshp_tcclbk;
/* Buffer for TC output pipe. */
static char tcout[PIPE_BUF];

/* Execut TC utility. */
static int exectc(const char **prms);

int eshp_clrdev(const char *dev)
{
	const char *clrdev[] = { TCBIN, TCQDISC, TCDEL, TCDEV, dev, TCROOT, NULL };
	int rc = exectc(clrdev);
	if (rc == -2)
		return EESHPPIPE;
	return 0;
}

int eshp_setdev(const char *dev)
{
	const char *setdev[] = {
		TCBIN, TCQDISC, TCADD, TCDEV, dev, TCROOT,
		TCHANDLE, TCPARENTID, TCHTB, NULL
	};
	int rc = exectc(setdev);
	if (rc == -2)
		return EESHPPIPE;
	else if (rc == -1)
		return EESHPSETDEV;
	return 0;
}

int eshp_setsp(const char *dev, int id, int64_t sp, const char *un, int qm,
	int prio)
{
	char clsid[INT32DIG + 1];
	char rate[INT64DIG + UNNAMLEN + 1];
	sprintf(clsid, "%s%d", TCCLASSIDID, id);
	sprintf(rate, "%lld%s", sp, un);
	char qmstr[INT32DIG + 1];
	sprintf(qmstr, "%d", qm);
	char priostr[INT32DIG + 1];
	sprintf(priostr, "%d", prio);
	const char *setsp[] = {
		TCBIN, TCCLASS, TCADD, TCDEV, dev, TCPARENT, TCPARENTID, TCCLASSID,
		clsid, TCHTB, TCRATE, rate, TCCEIL, rate, TCQUANTUM, qmstr, TCPRIO,
		priostr, NULL
	};
	int rc = exectc(setsp);
	if (rc == -2)
		return EESHPPIPE;
	else if (rc == -1)
		return EESHPSETSP;
	return 0;
}

static int exectc(const char **prms)
{
	int pd[2];
	int rc = pipe(pd);
	if (rc)
		return -2;
	pid_t pid = fork();
	if (!pid) {
		close(pd[0]);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		dup(pd[1]);
		dup(pd[1]);
		rc = execvp(TCBIN, (char *const *)prms);
		if (rc)
			exit(EXIT_FAILURE);
	}
	close(pd[1]);
	int rb;
	do {
		memset(tcout, 0, sizeof tcout);
		do
			rb = read(pd[0], tcout, sizeof tcout);
		while (rb == -1 && errno == EINTR);
		if (eshp_tcclbk)
			eshp_tcclbk(tcout);
	} while (rb > 0);
	int rs;
	do
		rc = waitpid(pid, &rs, 0);
	while (rc == -1 && errno == EINTR);
	if (WEXITSTATUS(rs))
		return -1;
	return 0;
}

int eshp_settf(const char *dev, char *src, char *dst, int spid, int prio)
{
	char priostr[INT32DIG + 1];
	sprintf(priostr, "%d", prio);
	char spidstr[INT32DIG + 1];
	sprintf(spidstr, "%s%d", TCFLOWID, spid);
	if (*src == TCIPWILDCARD)
		strcpy(src, TCANYHOST);
	if (*dst == TCIPWILDCARD)
		strcpy(dst, TCANYHOST);
	const char *settf[] = {
		TCBIN, TCFILTER, TCADD, TCDEV, dev, TCPROTOCOL, TCIP,
		TCPARENT, TCPARENTID, TCPRIO, priostr, TCU32, TCMATCH, TCIP, TCDST, dst,
		TCMATCH, TCIP, TCSRC, src, TCFLOW, spidstr, NULL
	};
	int rc = exectc(settf);
	if (rc == -2)
		return EESHPPIPE;
	else if (rc == -1)
		return EESHPSETTF;
	return 0;
}
