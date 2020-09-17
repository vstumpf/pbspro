/*
 * Copyright (C) 1994-2020 Altair Engineering, Inc.
 * For more information, contact Altair at www.altair.com.
 *
 * This file is part of both the OpenPBS software ("OpenPBS")
 * and the PBS Professional ("PBS Pro") software.
 *
 * Open Source License Information:
 *
 * OpenPBS is free software. You can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * OpenPBS is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial License Information:
 *
 * PBS Pro is commercially licensed software that shares a common core with
 * the OpenPBS software.  For a copy of the commercial license terms and
 * conditions, go to: (http://www.pbspro.com/agreement.html) or contact the
 * Altair Legal Department.
 *
 * Altair's dual-license business model allows companies, individuals, and
 * organizations to create proprietary derivative works of OpenPBS and
 * distribute them - whether embedded or bundled with other software -
 * under a commercial license agreement.
 *
 * Use of Altair's trademarks, including but not limited to "PBS™",
 * "OpenPBS®", "PBS Professional®", and "PBS Pro™" and Altair's logos is
 * subject to Altair's trademark licensing policies.
 */


/**
 * @file    parse.c
 *
 * @brief
 * 	parse.c - contains functions to related to parsing the config file.
 *
 * Functions included are:
 * 	parse_config()
 * 	valid_config()
 * 	is_speccase_sort()
 * 	init_config()
 * 	scan()
 * 	preempt_bit_field()
 * 	preempt_cmp()
 *
 */
#include <pbs_config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <log.h>
#include <libutil.h>
#include <unistd.h>
#include "data_types.hpp"
#include "parse.hpp"
#include "constant.hpp"
#include "config.hpp"
#include "misc.hpp"
#include "globals.hpp"
#include "fairshare.hpp"
#include "prime.hpp"
#include "node_info.hpp"
#include "resource.hpp"
#include "pbs_internal.h"


/* strtok delimiters for parsing the sched_config file are space and tab */
#define DELIM "\t "

/**
 * @brief
 * 		parse the config file into the global struct config conf
 *
 * @param[in]	fname	-	file name of the config file
 *
 * @see	GLOBAL:	conf  - config structure
 *
 * @return	success/failure
 * @retval	1	: success
 * @retval	0	: failure
 *
 * @par
 *	FILE FORMAT:
 *	config_name [white space] : [white space] config_value [prime_value]
 *	EX: sort_by: shortest_job_first prime
 */
int
parse_config(const char *fname)
{
	FILE *fp;			/* file pointer to config file */
	char *buf = NULL;
	int buf_size = 0;
	char buf2[8192];		/* general purpose buffer */
	char errbuf[1024];		/* buffer for reporting errors */
	char *config_name;		/* parse first word of line */
	char *config_value;		/* parsed second word - right after colen (:) */
	char *prime_value;		/* optional third word */
	char *tok;			/* used with strtok() */
	const char *obsolete[2];		/* used to log messages for obsolete params */
	int num = -1;			/* used to convert string -> integer */
	char *endp;			/* used for strtol() */
	char error = 0;		/* boolean: is there an error? */
	enum prime_time prime;	/* used to convert string -> prime value */
	int linenum = 0;		/* the current line number in the file */
	int i;

	/* sorting variables */
	int pkey_num = 0;		/* number of prime time keys for job sort */
	int npkey_num = 0;		/* number of nonprime time keys for job sort */
	int node_pkey_num = 0;	/* number of prime time keys for node sort */
	int node_npkey_num = 0;	/* number of nonprime time keys for node sort */
	char *sort_res_name;		/* name of resource to sort */
	enum sort_order sort_ord = NO_SORT_ORDER; /* sort order: ascend or descend */
	enum resource_fields sort_type; /* which res attr to use (avail/assn/etc) */

	/* server resources */
	int res_num = 0;		/* index into conf.dynamic_res */

	/* peer queues */
	int peer_num = 0;		/* index into conf.peer_queues */

	/* temporary char ptrs to be used with strtok/malloc */
	char *tmp1, *tmp2, *tmp3;

	/* resource type for validity checking */
	struct resource_type type = {0};

	if ((fp = fopen(fname, "r")) == NULL) {
		log_eventf(PBSEVENT_SCHED, PBS_EVENTCLASS_FILE, LOG_NOTICE,
			fname, "Can not open file: %s", fname);
		return 0;
	}

#ifdef NAS
	/* localmod 034 */
	conf.max_borrow = UNSPECIFIED;
	conf.per_share_topjobs = 0;
	/* localmod 038 */
	conf.per_queues_topjobs = 0;
	/* localmod 030 */
	conf.min_intrptd_cycle_length = 30;
	conf.max_intrptd_cycles = 1;
#endif

	/* auto-set any internally needed config values before reading the file */
	while (pbs_fgets_extend(&buf, &buf_size, fp) != NULL) {
		errbuf[0] = '\0';
		linenum++;
		error = 0;
		obsolete[0] = obsolete[1] = NULL;
		prime = PT_ALL;
		num = -1;

		/* skip blank lines and comments */
		if (!error && !skip_line(buf)) {
			config_name = scan(buf, ':');
			config_value = scan(NULL, 0);
			prime_value = scan(NULL, 0);
			if (config_name != NULL && config_value != NULL) {
				if (strcasecmp(config_value, "true") == 0) {
					/* value is true */
					num = 1;
				} else if (strcasecmp(config_value, "false") == 0) {
					/* value is false */
					num = 0;
				} else if (isdigit((int)config_value[0])) {
					/* value is number */
					num = strtol(config_value, &endp, 10);
				}

				if (prime_value != NULL) {
					if (!strcmp(prime_value, "prime") || !strcmp(prime_value, "PRIME"))
						prime = PRIME;
					else if (!strcmp(prime_value, "non_prime") ||
						!strcmp(prime_value, "NON_PRIME"))
						prime = NON_PRIME;
					else if (!strcmp(prime_value, "all") || !strcmp(prime_value, "ALL"))
						prime = PT_ALL;
					else if (!strcmp(prime_value, "none") || !strcmp(prime_value, "NONE"))
						prime = PT_NONE;
					else
						error = 1;
				}

				if (!strcmp(config_name, PARSE_ROUND_ROBIN)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_rr = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_rr = num ? 1 : 0;
				}
				else if (!strcmp(config_name, PARSE_BY_QUEUE)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_bq = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_bq = num ? 1 : 0;
				}
				else if (!strcmp(config_name, PARSE_STRICT_FIFO)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_sf = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_sf = num ? 1 : 0;
					obsolete[0] = config_name;
					obsolete[1] = "strict_ordering";
				}
				else if (!strcmp(config_name, PARSE_STRICT_ORDERING)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_so = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_so = num ? 1 : 0;
				}
				else if (!strcmp(config_name, PARSE_FAIR_SHARE)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_fs = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_fs = num ? 1 : 0;
				}
				else if (!strcmp(config_name, PARSE_HELP_STARVING_JOBS)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_hsv = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_hsv = num ? 1 : 0;
				}
				else if (!strcmp(config_name, PARSE_BACKFILL)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_bf = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_bf = num ? 1 : 0;

					obsolete[0] = config_name;
					obsolete[1] = "server's backfill_depth=0";
				}
				else if (!strcmp(config_name, PARSE_SORT_QUEUES)) {
					obsolete[0] = config_name;
				}
				else if (!strcmp(config_name, PARSE_UPDATE_COMMENTS)) {
					conf.update_comments = num ? 1 : 0;
				}
				else if (!strcmp(config_name, PARSE_BACKFILL_PRIME)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_bp = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_bp = num ? 1 : 0;
				}
				else if (!strcmp(config_name, PARSE_PREEMPIVE_SCHED)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_pre = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_pre = num ? 1 : 0;
				}
				else if (!strcmp(config_name, PARSE_PRIME_EXEMPT_ANYTIME_QUEUES))
					conf.prime_exempt_anytime_queues = num ? 1 : 0;
				else if (!strcmp(config_name, PARSE_PREEMPT_STARVING))
					conf.preempt_starving = num ? 1 : 0;
				else if (!strcmp(config_name, PARSE_PREEMPT_FAIRSHARE))
					conf.preempt_fairshare = num ? 1 : 0;
				else if (!strcmp(config_name, PARSE_ASSIGN_SSINODES))
					conf.assign_ssinodes = num ? 1 : 0;
				else if (!strcmp(config_name, PARSE_DONT_PREEMPT_STARVING))
					conf.dont_preempt_starving = num ? 1 : 0;
				else if (!strcmp(config_name, PARSE_ENFORCE_NO_SHARES))
					conf.enforce_no_shares = num ? 1 : 0;
				else if (!strcmp(config_name, PARSE_ALLOW_AOE_CALENDAR))
					conf.allow_aoe_calendar = 1;
				else if (!strcmp(config_name, PARSE_PRIME_SPILL)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_spill = res_to_num(config_value, &type);
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.nonprime_spill = res_to_num(config_value, &type);

					if (!type.is_time)
						error = 1;
				}
				else if (!strcmp(config_name, PARSE_MAX_STARVE)) {
					conf.max_starve = res_to_num(config_value, &type);
					if (!type.is_time)
						error = 1;
				}
				else if (!strcmp(config_name, PARSE_HALF_LIFE) || !strcmp(config_name, PARSE_FAIRSHARE_DECAY_TIME)) {
					if(!strcmp(config_name, PARSE_HALF_LIFE)) {
						obsolete[0] = PARSE_HALF_LIFE;
						obsolete[1] = PARSE_FAIRSHARE_DECAY_TIME " and " PARSE_FAIRSHARE_DECAY_FACTOR " instead";
					}
					conf.decay_time = res_to_num(config_value, &type);
					if (!type.is_time)
						error = 1;
				}
				else if (!strcmp(config_name, PARSE_UNKNOWN_SHARES))
					conf.unknown_shares = num;
				else if (!strcmp(config_name, PARSE_FAIRSHARE_DECAY_FACTOR)) {
					float fnum;
					fnum = strtod(config_value, &endp);
					if(*endp == '\0')  {
						if( fnum <= 0 || fnum >= 1) {
							sprintf(errbuf, "%s: Invalid value: %.*f.  Valid values are between 0 and 1.", PARSE_FAIRSHARE_DECAY_FACTOR, float_digits(fnum, 2), fnum);
							error = 1;
						}
						else
							conf.fairshare_decay_factor = fnum;
					} else
						error = 1;
				}
				else if (!strcmp(config_name, PARSE_FAIRSHARE_RES)) {
					if (conf.fairshare_res != NULL)
						free(conf.fairshare_res);
					conf.fairshare_res = string_dup(config_value);
				}
				else if (!strcmp(config_name, PARSE_FAIRSHARE_ENT)) {
					if (strcmp(config_value, ATTR_euser) &&
						strcmp(config_value, ATTR_egroup) &&
						strcmp(config_value, ATTR_A) &&
						strcmp(config_value, "queue") &&
						strcmp(config_value, "egroup:euser")) {
						error = 1;
						sprintf(errbuf, "%s %s is erroneous (or deprecated).",
							PARSE_FAIRSHARE_ENT, config_value);
					}
					if (conf.fairshare_ent != NULL)
						free(conf.fairshare_ent);
					conf.fairshare_ent = string_dup(config_value);
				}
				else if (!strcmp(config_name, PARSE_NODE_GROUP_KEY)) {
					obsolete[0] = PARSE_NODE_GROUP_KEY;
					obsolete[1] = "nothing - set via qmgr";
				}
				else if (!strcmp(config_name, PARSE_LOG_FILTER)) {
					obsolete[0] = PARSE_LOG_FILTER;
					obsolete[1] = "nothing - set log_events via qmgr";
				}
				else if (!strcmp(config_name, PARSE_PREEMPT_QUEUE_PRIO)) {
					obsolete[0] = PARSE_PREEMPT_QUEUE_PRIO;
					obsolete[1] = "nothing - set via qmgr";
				}
				else if (!strcmp(config_name, PARSE_RES_UNSET_INFINITE)) {
					sprintf(buf2, "%s,mpiprocs,ompthreads", config_value);
					conf.ignore_res = break_comma_list(buf2);
				}
				else if (!strcmp(config_name, PARSE_RESV_CONFIRM_IGNORE)) {
					if (!strcmp(config_value, "dedicated_time"))
						conf.resv_conf_ignore = 1;
					else if (!strcmp(config_value, "none"))
						conf.resv_conf_ignore = 0;
					else {
						error = 1;
						sprintf(errbuf, "%s valid values: dedicated_time or none",
							PARSE_RESV_CONFIRM_IGNORE);
					}
				}
				else if (!strcmp(config_name, PARSE_RESOURCES)) {
					char *valbuf = NULL;
					buf2[0] = '\0';
					/* hack: add in "host" into resources list because this was
					 * done by default prior to 7.1.
					 */
					if (strstr(config_value, "host") == NULL)
						strcpy(buf2, "host,");

					/* hack: add in "vnode" in 8.0 */
					if (strstr(config_value, "vnode") == NULL)
						strcat(buf2, "vnode,");

					if (buf2[0] != '\0') {
						pbs_asprintf(&valbuf, "%s,%s", config_value, buf2);
						config_value = valbuf;
					}

					conf.res_to_check = break_comma_list(config_value);
					if (conf.res_to_check != NULL) {
						for (i = 0; conf.res_to_check[i] != NULL; i++)
							;
						conf.num_res_to_check = i;
					}
					free(valbuf);
				}
				else if (!strcmp(config_name, PARSE_MOM_RESOURCES))
					conf.dyn_res_to_get = break_comma_list(config_value);
				else if (!strcmp(config_name, PARSE_DEDICATED_PREFIX)) {
					if (strlen(config_value) > PBS_MAXQUEUENAME)
						error = 1;
					else
						pbs_strncpy(conf.ded_prefix, config_value, sizeof(conf.ded_prefix));
				}
				else if (!strcmp(config_name, PARSE_PRIMETIME_PREFIX)) {
					if (strlen(config_value) > PBS_MAXQUEUENAME)
						error = 1;
					else
						pbs_strncpy(conf.pt_prefix, config_value, sizeof(conf.pt_prefix));
				}
				else if (!strcmp(config_name, PARSE_NONPRIMETIME_PREFIX)) {
					if (strlen(config_value) > PBS_MAXQUEUENAME)
						error = 1;
					else
						pbs_strncpy(conf.npt_prefix, config_value, sizeof(conf.npt_prefix));
				}
				else if (!strcmp(config_name, PARSE_SMP_CLUSTER_DIST)) {
					for (i = 0; i < HIGH_SMP_DIST; i++)
						if (!strcmp(smp_cluster_info[i].str, config_value)) {
							if (prime == PRIME || prime == PT_ALL)
								conf.prime_smp_dist = (enum smp_cluster_dist) smp_cluster_info[i].value;
							if (prime == NON_PRIME || prime == PT_ALL)
								conf.non_prime_smp_dist = (enum smp_cluster_dist) smp_cluster_info[i].value;
						}
				}
				else if (!strcmp(config_name, PARSE_PREEMPT_PRIO)) {
					obsolete[0] = PARSE_PREEMPT_PRIO;
					obsolete[1] = "nothing - set via qmgr";
				}
				else if (!strcmp(config_name, PARSE_PREEMPT_ORDER)) {
					obsolete[0] = PARSE_PREEMPT_ORDER;
					obsolete[1] = "nothing - set via qmgr";
				}
				else if (!strcmp(config_name, PARSE_PREEMPT_SORT)) {
					obsolete[0] = PARSE_PREEMPT_SORT;
					obsolete[1] = "nothing - set via qmgr";
				}
				else if (!strcmp(config_name, PARSE_JOB_SORT_KEY)) {
					if (((prime == PRIME || prime == PT_ALL) && pkey_num > MAX_SORTS) ||
						((prime == NON_PRIME || prime == PT_ALL) && npkey_num > MAX_SORTS)) {
						log_eventf(PBSEVENT_SCHED, PBS_EVENTCLASS_FILE, LOG_NOTICE, fname,
							"Too many %s sorts.  %s sort ignored.  %d max sorts",
							prime_value, config_value, MAX_SORTS);
					} else {
						tok = strtok(config_value, DELIM);
						sort_res_name = string_dup(tok);

						if (sort_res_name != NULL) {
							tok = strtok(NULL, DELIM);
							if (tok != NULL) {
								if (!strcmp(tok, "high") || !strcmp(tok, "HIGH") ||
									!strcmp(tok, "High")) {
									sort_ord = DESC;
								}
								else if (!strcmp(tok, "low") || !strcmp(tok, "LOW") ||
									!strcmp(tok, "Low")) {
									sort_ord = ASC;
								}
								else {
									free(sort_res_name);
									error = 1;
								}
							}
							else {
								free(sort_res_name);
								error = 1;
							}
						}
						else
							error = 1;

						if (!error) {
							int dup_res_name = 0;
							if (!strcmp(sort_res_name, SORT_PRIORITY)) {
								obsolete[0] = SORT_PRIORITY " in " PARSE_JOB_SORT_KEY;
								obsolete[1] = SORT_JOB_PRIORITY;
								free(sort_res_name);
								sort_res_name = string_dup(SORT_JOB_PRIORITY);
							}
							if (prime == PRIME || prime == PT_ALL) {
								conf.prime_sort[pkey_num].res_name = sort_res_name;
								/* set the flag to indicate that we should dup the memory for sort_res_name
								 * if we are going to use it again.
								 */
								dup_res_name = 1;
								conf.prime_sort[pkey_num].order = sort_ord;
								pkey_num++;
							}

							if (prime == NON_PRIME || prime == PT_ALL) {
								if (dup_res_name)
									conf.non_prime_sort[npkey_num].res_name = string_dup(sort_res_name);
								else
									conf.non_prime_sort[npkey_num].res_name = sort_res_name;

								conf.non_prime_sort[npkey_num].order = sort_ord;
								npkey_num++;
							}
						}
					}
				}
				else if (!strcmp(config_name, PARSE_NODE_SORT_KEY)) {
					if (((prime == PRIME || prime == PT_ALL) && node_pkey_num > MAX_SORTS) ||
						((prime == NON_PRIME || prime == PT_ALL) && node_npkey_num > MAX_SORTS)) {
						log_eventf(PBSEVENT_SCHED, PBS_EVENTCLASS_FILE, LOG_NOTICE, fname,
							"Too many %s node sorts.  %s sort ignored.  %d max sorts",
							prime_value, config_value, MAX_SORTS);
					}
					else {
						tok = strtok(config_value, DELIM);
						sort_res_name = string_dup(tok);

						if (sort_res_name != NULL) {
							tok = strtok(NULL, DELIM);
							if (tok != NULL) {
								if (!strcmp(tok, "high") || !strcmp(tok, "HIGH") ||
									!strcmp(tok, "High")) {
									sort_ord = DESC;
								}
								else if (!strcmp(tok, "low") || !strcmp(tok, "LOW") ||
									!strcmp(tok, "Low")) {
									sort_ord = ASC;
								}
								else {
									free(sort_res_name);
									error = 1;
								}
							}
							else {
								free(sort_res_name);
								error = 1;
							}
						}
						else
							error = 1;

						tok = strtok(NULL, DELIM);
						if (tok == NULL)
							sort_type = RF_AVAIL;
						else {
							if (!strcmp(tok, "total"))
								sort_type = RF_AVAIL;
							else if (!strcmp(tok, "assigned"))
								sort_type = RF_ASSN;
							else if (!strcmp(tok, "unused"))
								sort_type = RF_UNUSED;
							else {
								free(sort_res_name);
								error = 1;
							}
						}

						if (!error) {
							int dup_res_name = 0;
							if (prime == PRIME || prime == PT_ALL) {
								conf.prime_node_sort[node_pkey_num].res_name = sort_res_name;
								/* set the flag to indicate that we should dup the memory for sort_res_name
								 * if we are going to use it again.
								 */
								dup_res_name = 1;
								conf.prime_node_sort[node_pkey_num].order = sort_ord;
								conf.prime_node_sort[node_pkey_num].res_type = sort_type;
								if (sort_type == RF_UNUSED || sort_type == RF_ASSN)
									conf.node_sort_unused = 1;
								node_pkey_num++;
							}

							if (prime == NON_PRIME || prime == PT_ALL) {
								if (dup_res_name)
									conf.non_prime_node_sort[node_npkey_num].res_name = string_dup(sort_res_name);
								else
									conf.non_prime_node_sort[node_npkey_num].res_name = sort_res_name;

								conf.non_prime_node_sort[node_npkey_num].order = sort_ord;
								conf.non_prime_node_sort[node_npkey_num].res_type = sort_type;
								if (sort_type == RF_UNUSED || sort_type == RF_ASSN)
									conf.node_sort_unused = 1;
								node_npkey_num++;
							}
						}
					}
				}
				else if (!strcmp(config_name, PARSE_SERVER_DYN_RES)) {
					tmp1 = NULL;
					tmp2 = NULL;

					/* MAX_SERVER_DYN_RES-1 to leave room for the sentinel */
					if (res_num < MAX_SERVER_DYN_RES-1) {
						char *filename = NULL;
						/* get the resource name */
						tok = strtok(config_value, DELIM);
						if (tok != NULL) {
							tmp1 = string_dup(tok);

							/* tok is the rest of the config_value string - the program */
							tok = strtok(NULL, "");
							while (tok != NULL && isspace(*tok))
								tok++;

							if (tok != NULL && tok[0] == '!') {
								tok++;
								tmp2 = string_dup(tok);
								filename = get_script_name(tok);
								if (filename == NULL)
									error = 1;
								else {
									int err;
									err = tmp_file_sec_user(filename, 0, 1, S_IWGRP|S_IWOTH, 1, getuid());
									if (err != 0) {
										snprintf(errbuf, sizeof(errbuf),
											"error: %s file has a non-secure file access, errno: %d", filename, err);
										error = 1;
									}
									conf.dynamic_res[res_num].res = tmp1;
									conf.dynamic_res[res_num].command_line = tmp2;
									conf.dynamic_res[res_num].script_name = filename;
								}
							}
							else
								error = 1;
						}
						else
							error = 1;

						if (error) {
							if (tmp1 != NULL)
								free(tmp1);

							if (tmp2 != NULL)
								free(tmp2);

							if (filename != NULL)
								free(filename);

							conf.dynamic_res[res_num].res = NULL;
							conf.dynamic_res[res_num].command_line = NULL;
							conf.dynamic_res[res_num].script_name = NULL;
						}
						else
							res_num++;
					}
					else {
						sprintf(errbuf, "Too many %s lines, Max: %d",
							PARSE_SERVER_DYN_RES, MAX_SERVER_DYN_RES);
						error = 1;
					}

				}
				else if (!strcmp(config_name, PARSE_SORT_NODES)) {
					obsolete[0] = config_name;
					obsolete[1] = PARSE_NODE_SORT_KEY;

					if (prime == PRIME || prime == PT_ALL) {
						conf.prime_node_sort[node_pkey_num].res_name = string_dup(SORT_PRIORITY);
						conf.prime_node_sort[node_pkey_num].order = DESC;
						node_pkey_num++;
					}
					if (prime == NON_PRIME || prime == PT_ALL) {
						conf.non_prime_node_sort[node_npkey_num].res_name = string_dup(SORT_PRIORITY);
						conf.non_prime_node_sort[node_npkey_num].order = DESC;
						node_npkey_num++;
					}
				}
				else if (!strcmp(config_name, PARSE_PEER_QUEUE)) {
					tmp1 = tmp2 = tmp3 = NULL;
					if (peer_num < NUM_PEERS) {
						tok = strtok(config_value, DELIM);
						if (tok != NULL) {
							tmp1 = string_dup(tok);

							tok = strtok(NULL, "@");
							if (tok != NULL) {
								while (isspace(*tok)) tok++;
								tmp2 = string_dup(tok);

								tok = strtok(NULL, DELIM);
								if (tok != NULL) {
									tmp3 = string_dup(tok);
									if (tmp3 == NULL)
										error = 1;
								}
								/* check for malloc failures */
								if ((tmp1 != NULL) && (tmp2 != NULL) && (!error)) {
									conf.peer_queues[peer_num].local_queue = tmp1;
									conf.peer_queues[peer_num].remote_queue = tmp2;
									conf.peer_queues[peer_num].remote_server = tmp3;
									peer_num++;
								}
								else
									error = 1;
							}
							else {
								error = 1;
							}
						}
						else
							error = 1;

						if (error) {
							sprintf(errbuf, "Invalid peer queue");

							if (tmp1 != NULL)
								free(tmp1);
							if (tmp2 != NULL)
								free(tmp2);
							if (tmp3 != NULL)
								free(tmp3);
						}
					}
					else {
						error = 1;
						sprintf(errbuf, "Too many peer queues - max: %d", NUM_PEERS);
					}
				}
				else if (!strcmp(config_name, PARSE_PREEMPT_ATTEMPTS))
					conf.max_preempt_attempts = num;
				else if (!strcmp(config_name, PARSE_MAX_JOB_CHECK)) {
					if (!strcmp(config_value, "ALL_JOBS"))
						conf.max_jobs_to_check = SCHD_INFINITY;
					else
						conf.max_jobs_to_check = num;
				} else if (!strcmp(config_name, PARSE_CPUS_PER_SSINODE) ||
					   !strcmp(config_name, PARSE_MEM_PER_SSINODE)) {
					obsolete[0] = config_name;
					obsolete[1] = "nothing";
				} else if (!strcmp(config_name, PARSE_SELECT_PROVISION)) {
					if (config_value != NULL) {
						if (!strcmp(config_value, PROVPOLICY_AVOID))
							conf.provision_policy = AVOID_PROVISION;
					}
					else
						error = 1;
				}
#ifdef NAS
				/* localmod 034 */
				else if (!strcmp(config_name, PARSE_MAX_BORROW)) {
					conf.max_borrow = res_to_num(config_value, &type);
					if (!type.is_time)
						error = 1;
				} else if (!strcmp(config_name, PARSE_SHARES_TRACK_ONLY)) {
					if (prime == PRIME || prime == PT_ALL)
						conf.prime_sto = num ? 1 : 0;
					if (prime == NON_PRIME || prime == PT_ALL)
						conf.non_prime_sto = num ? 1 : 0;
				} else if (!strcmp(config_name, PARSE_PER_SHARE_DEPTH) ||
					   !strcmp(config_name, PARSE_PER_SHARE_TOPJOBS)) {
					conf.per_share_topjobs = num;
				}
				/* localmod 038 */
				else if (!strcmp(config_name, PARSE_PER_QUEUES_TOPJOBS)) {
					conf.per_queues_topjobs = num;
				}
				/* localmod 030 */
				else if (!strcmp(config_name, PARSE_MIN_INTERRUPTED_CYCLE_LENGTH)) {
					conf.min_intrptd_cycle_length = num;
				} else if (!strcmp(config_name, PARSE_MAX_CONS_INTERRUPTED_CYCLES)) {
					conf.max_intrptd_cycles = num;
				}
#endif
				else
					error = 1;
			}
			else
				error = 1;
		}

		if (error)
			log_eventf(PBSEVENT_SCHED, PBS_EVENTCLASS_FILE, LOG_NOTICE, fname,
				"Error reading line %d: %s", linenum, errbuf);

		if (obsolete[0] != NULL) {
			if (obsolete[1] != NULL)
				log_eventf(PBSEVENT_SCHED, PBS_EVENTCLASS_FILE, LOG_NOTICE, fname,
					"Obsolete config name %s, instead use %s", obsolete[0], obsolete[1]);
			else
				log_eventf(PBSEVENT_SCHED, PBS_EVENTCLASS_FILE, LOG_NOTICE, fname,
					"Obsolete config name %s", obsolete[0]);
		}
	}
	fclose(fp);
	if (buf != NULL)
		free(buf);

	if (valid_config() == 0)
		return 0;

	return 1;
}

/**
 * @brief
 *		valid_config - perform validity checks on scheduler configuration
 *		       In a warning situation, we will fix it the best we can
 *		       and continue
 *
 * @return	int
 * @retval	1	: valid config (perfectly valid or with warnings)
 * @retval	0	: invalid config
 *
 */
int
valid_config()
{
	int valid = 1;

	if ((conf.prime_smp_dist != SMP_NODE_PACK ||
		conf.non_prime_smp_dist != SMP_NODE_PACK) && conf.node_sort_unused) {
		log_event(PBSEVENT_SCHED, PBS_EVENTCLASS_FILE, LOG_WARNING, "", "smp_cluster_dist and node sorting by unused/assigned resources are not compatible.  The smp_cluster_dist option is being set to pack.");
		conf.prime_smp_dist = conf.non_prime_smp_dist = SMP_NODE_PACK;
	}

	return valid;
}
/**
 * @brief
 * 		Check if sort_res is a valid special case sorting string
 *
 * @param[in]	sort_res	-	sorting keyword
 * @param[in]	sort_type	-	sorting object type (job or node)
 *
 * @return	int
 * @retval	1	: is special case sort
 * @retval	0	: not special case sort
 */
int is_speccase_sort(char *sort_res, int sort_type) {
	if (sort_type == SOBJ_JOB) {
		if (!strcmp(sort_res, SORT_JOB_PRIORITY))
			return 1;
#ifdef NAS
		/* localmod 034 */
		if (!strcmp(sort_res, SORT_ALLOC))
			return 1;
		/* localmod 039 */
		if (!strcmp(sort_res, SORT_QPRI))
			return 1;
#endif
		else
			return 0;
	} else if (sort_type == SOBJ_NODE) {
		if (!strcmp(sort_res, SORT_PRIORITY) || !strcmp(sort_res, SORT_FAIR_SHARE) || !strcmp(sort_res, SORT_PREEMPT))
			return 1;
		else
			return 0;
	}

	return 0;
}

/**
 * @brief
 *      init_config - initalize the config struture
 *
 * @return	success / failure
 *
 * @par MT-safe: No
 */
int
init_config()
{
	static const char *ignore[] = { "mpiprocs", "ompthreads", NULL };

	free_fairshare_head(conf.fairshare);
	free_sort_info(PRIME_SORT);
	free_sort_info(NON_PRIME_SORT);
	free_sort_info(PRIME_NODE_SORT);
	free_sort_info(NON_PRIME_NODE_SORT);

	if (conf.fairshare_res != NULL) {
		free(conf.fairshare_res);
		conf.fairshare_res = NULL;
	}
	if (conf.fairshare_ent != NULL) {
		free(conf.fairshare_ent);
		conf.fairshare_ent = NULL;
	}
	if (conf.res_to_check != NULL)
		free_string_array(conf.res_to_check);
	if (conf.dyn_res_to_get != NULL)
		free_string_array(conf.dyn_res_to_get);

	if (conf.dynamic_res[0].res != NULL) {
		int i;
		for (i = 0; conf.dynamic_res[i].res != NULL; i++) {
			free(conf.dynamic_res[i].res);
			free(conf.dynamic_res[i].command_line);
			free(conf.dynamic_res[i].script_name);
		}
	}

	/* default everyone OFF */
	memset(&conf, 0, sizeof(struct config));
	memset(&cstat, 0, sizeof(struct status));

	if ((conf.prime_sort = (sort_info *)malloc((MAX_SORTS + 1) * sizeof(struct sort_info))) == NULL) {
		log_err(errno, __func__, MEM_ERR_MSG);
		return 0;
	}
	memset(conf.prime_sort, 0, (MAX_SORTS + 1) * sizeof(struct sort_info));

	if ((conf.non_prime_sort = (sort_info *)malloc((MAX_SORTS + 1) * sizeof(struct sort_info))) == NULL) {
		log_err(errno, __func__, MEM_ERR_MSG);
		return 0;
	}
	memset(conf.non_prime_sort, 0, (MAX_SORTS + 1) *
		sizeof(struct sort_info));

	if ((conf.prime_node_sort = (sort_info *)malloc((MAX_SORTS + 1) * sizeof(struct sort_info))) == NULL) {
		log_err(errno, __func__, MEM_ERR_MSG);
		return 0;
	}
	memset(conf.prime_node_sort, 0, (MAX_SORTS + 1) * sizeof(struct sort_info));

	if ((conf.non_prime_node_sort = (sort_info *)malloc((MAX_SORTS + 1) * sizeof(struct sort_info))) == NULL) {
		log_err(errno, __func__, MEM_ERR_MSG);
		return 0;
	}
	memset(conf.non_prime_node_sort, 0, (MAX_SORTS + 1) *
		sizeof(struct sort_info));

	/* for backwards compatibility */
	conf.update_comments = 1;

	conf.decay_time = 86400;	/* default decay time period is 24 hours */

	if ((conf.fairshare_res = string_dup("cput")) == NULL)
		return 0;

	if ((conf.fairshare_ent = string_dup("euser")) == NULL)
		return 0;

	conf.enforce_no_shares = 1;
	conf.fairshare_decay_factor = .5;

	conf.max_preempt_attempts = SCHD_INFINITY;
	conf.max_jobs_to_check = SCHD_INFINITY;

	/* default value for ignore_res is the pseudo resources */
	conf.ignore_res = const_cast<char **>(ignore);

	/* deprecated parameters which are needed to be set to true */
	conf.assign_ssinodes = 1;
	conf.preempt_starving = 1;
	conf.preempt_fairshare = 1;
	conf.prime_bf = 1;
	conf.non_prime_bf = 1;

	conf.provision_policy = AGGRESSIVE_PROVISION;

	/* set parts of cstat which don't change per cycle */
	cstat.sync_fairshare_files = 1;
	cstat.backfill_depth = 1;

	return 1;
}

/**
 * @brief
 *		scan - Scan through the string looking for a white space delimited
 *	       word or quoted string.  If the target parameter is not 0, then
 *	       use that as a delimiter as well.
 *
 * @param[in]	str	-	the string to scan through.  If NULL, start where we left
 *		   				off last time.
 * @param[in]	target	-	if target is 0, set it to a space.  It is already a delimiter
 *
 * @return	scanned string or NULL
 *
 */
char *
scan(char *str, char target)
{
	static char *isp = NULL;	/* internal state pointer used if a NULL is
					 * passed in to str
					 */
	char *ptr;			/* pointer used to search through the str */
	char quote;			/* pointer used to store the quote ' or " */
	char *start;

	if (str == NULL && isp == NULL)
		return NULL;

	if (str == NULL)
		ptr = isp;
	else
		ptr = str;

	/* if target is 0, set it to a space.  It is already a delimiter */
	if (target == 0)
		target = ' ';

	while (isspace(*ptr) || *ptr == target) ptr++;

	start = ptr;

	if (*ptr != '\0') {
		if (*ptr == '\"' || *ptr == '\'') {
			quote = *ptr;
			start = ++ptr;
			while (*ptr != '\0' && *ptr != quote)
				ptr++;
		}
		else {
			while (*ptr != '\0' && !isspace(*ptr) && *ptr != target)
				ptr++;
		}
		if (*ptr == '\0')
			isp = NULL;
		else {
			*ptr = '\0';
			isp = ptr + 1;
		}
		return start;
	}

	isp = NULL;
	return NULL;
}

/**
 * @brief
 *		preempt_bit_field - take list of preempt names seperated by +'s and
 * 			    create a bitfield representing it.  The bitfield
 *			    is created by taking the name in the prempt enum
 *			    and shifting a bit into that position.
 *
 * @param[in]	plist	-	a preempt list
 *
 * @return	a bitfield of -1 on error
 *
 */
int
preempt_bit_field(char *plist)
{
	int bitfield = 0;
	int obitfield;
	int i;
	char *tok;

	tok = strtok(plist, "+");

	while (tok != NULL) {
		obitfield = bitfield;
		for (i = 0; i < PREEMPT_HIGH; i++) {
			if (!strcmp(preempt_prio_info[i].str, tok))
				bitfield |= PREEMPT_TO_BIT(preempt_prio_info[i].value);
		}

		/* invalid preempt string */
		if (obitfield == bitfield) {
			bitfield = -1;
			break;
		}

		tok = strtok(NULL, "+");
	}

	return bitfield;
}

/**
 * @brief
 * 	sort compare function for preempt status's
 * 	sort by descending number of bits in the bitfields (most number of preempt
 * 	statuses at the top) and then priorities
 *
 * @param[in]	p1	-	preempt status 1
 * @param[in]	p2	-	preempt status 2
 *
 * @return	int
 * @retval	1	: p1 < p2
 * @retval	-1	: p1 > p2
 * @retval	0	: Equal
 */
int
preempt_cmp(const void *p1, const void *p2)
{
	int *i1, *i2;

	i1 = (int *) p1;
	i2 = (int *) p2;

	if (BITCOUNT16(*i1) < BITCOUNT16(*i2))
		return 1;
	else if (BITCOUNT16(*i1) > BITCOUNT16(*i2))
		return -1;
	else {
		if (*(i1+1) < *(i2+1))
			return 1;
		else if (*(i1+1) > *(i2+1))
			return -1;
		else
			return 0;
	}
}

/**
 * @brief
 *	       Frees the memory for the corresponding sort_info structure variable given.
 *
 * @param[in]	si_type	- type of sort_info structure
 *
 * @return void
 *
 */
void
free_sort_info(enum sort_info_type si_type)
{
	struct sort_info *so_info;

	switch (si_type) {
		case PRIME_SORT:
			so_info = conf.prime_sort;
			break;
		case NON_PRIME_SORT:
			so_info = conf.non_prime_sort;
			break;
		case PRIME_NODE_SORT:
			so_info = conf.prime_node_sort;
			break;
		case NON_PRIME_NODE_SORT:
			so_info = conf.non_prime_node_sort;
			break;
		default:
			return;
	}

	if (so_info != NULL) {
		int i;
		for (i = 0; i < MAX_SORTS; i++) {
			if (so_info[i].res_name != NULL)
				free(so_info[i].res_name);
		}
		free(so_info);
	}
}
