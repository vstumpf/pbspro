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
 * A quick explanation of the scheduler's data model:
 * To free an object, use the object’s destructor (e.g., free_node_info())
 * To free an array of objects, you need to know if you own the objects yourself
 * or are an array of references
 * If you own the objects(e.g., sinfo->nodes), you call the multi-object object
 * destructor (e.g., free_nodes())
 * If you are an array of references (e.g., sinfo->queues[i]->nodes), you call
 * free().  You are just an array of pointers that are referencing objects.
 */

#ifndef	_DATA_TYPES_H
#define	_DATA_TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <time.h>
#include <pbs_ifl.h>
#include <libutil.h>
#include "constant.hpp"
#include "config.hpp"
#include "pbs_bitmap.hpp"
#include "pbs_share.h"
#include "range.h"
#ifdef NAS
#include "site_queue.h"
#endif

struct server_info;
struct state_count;
struct queue_info;
struct node_info;
struct job_info;
struct schd_resource;
struct resource_req;
struct resource_count;
struct holiday;
struct prev_job_info;
struct group_info;
struct usage_info;
struct counts;
struct nspec;
struct node_partition;
struct range;
struct resource_resv;
struct place;
struct schd_error;
struct np_cache;
struct chunk;
struct selspec;
struct resdef;
struct event_list;
struct status;
struct fairshare_head;
struct node_scratch;
struct te_list;
struct node_bucket;
struct bucket_bitpool;
struct chunk_map;
struct node_bucket_count;
struct preempt_job_st;


typedef struct state_count state_count;
typedef struct server_info server_info;
typedef struct queue_info queue_info;
typedef struct job_info job_info;
typedef struct node_info node_info;
typedef struct schd_resource schd_resource;
typedef struct resource_req resource_req;
typedef struct resource_count resource_count;
typedef struct usage_info usage_info;
typedef struct group_info group_info;
typedef struct prev_job_info prev_job_info;
typedef struct resv_info resv_info;
typedef struct counts counts;
typedef struct nspec nspec;
typedef struct node_partition node_partition;
typedef struct resource_resv resource_resv;
typedef struct place place;
typedef struct schd_error schd_error;
typedef struct np_cache np_cache;
typedef struct chunk chunk;
typedef struct selspec selspec;
typedef struct resdef resdef;
typedef struct timed_event timed_event;
typedef struct event_list event_list;
typedef struct status status;
typedef struct fairshare_head fairshare_head;
typedef struct node_scratch node_scratch;
typedef struct resresv_set resresv_set;
typedef struct te_list te_list;
typedef struct node_bucket node_bucket;
typedef struct bucket_bitpool bucket_bitpool;
typedef struct chunk_map chunk_map;
typedef struct node_bucket_count node_bucket_count;
typedef struct preempt_job_st preempt_job_st;
typedef struct th_task_info th_task_info;
typedef struct th_data_nd_eligible th_data_nd_eligible;
typedef struct th_data_dup_nd_info th_data_dup_nd_info;
typedef struct th_data_query_ninfo th_data_query_ninfo;
typedef struct th_data_free_ninfo th_data_free_ninfo;
typedef struct th_data_dup_resresv th_data_dup_resresv;
typedef struct th_data_query_jinfo th_data_query_jinfo;
typedef struct th_data_free_resresv th_data_free_resresv;


#ifdef NAS
/* localmod 034 */
/*
 * site_j_share_type - How jobs interact with CPU shares
 */
enum    site_j_share_type {
	J_TYPE_ignore =   0		/* share ignored when scheduling */
	, J_TYPE_limited =  1		/* jobs limited to share */
	, J_TYPE_borrow =   2		/* jobs can borrow from other shares */
};
#define	J_TYPE_COUNT	(J_TYPE_borrow+1)

struct share_head;
typedef struct share_head share_head;
struct share_info;
typedef struct share_info share_info;
typedef int sh_amt;
/* localmod 053 */
struct	site_user_info;
typedef struct site_user_info site_user_info;
#endif

typedef RESOURCE_TYPE sch_resource_t;
/* since resource values and usage values are linked */
typedef sch_resource_t usage_t;

typedef void event_ptr_t;
typedef int (*event_func_t)(event_ptr_t*, void *);

struct th_task_info
{
	int task_id;							/* task id, should be set by main thread */
	enum thread_task_type task_type;		/* task type */
	void *thread_data;					/* data for the worker thread to execute the task */
};

struct th_data_nd_eligible
{
	resource_resv *resresv;
	place *pl;
	schd_error *err;
	node_info **ninfo_arr;
	int sidx;
	int eidx;
};

struct th_data_dup_nd_info
{
	unsigned int error:1;
	node_info **onodes;
	node_info **nnodes;
	server_info *nsinfo;
	unsigned int flags;
	int sidx;
	int eidx;
};

struct th_data_query_ninfo
{
	unsigned int error:1;
	struct batch_status *nodes;
	server_info *sinfo;
	node_info **oarr;
	int sidx;
	int eidx;
};

struct th_data_free_ninfo
{
	node_info **ninfo_arr;
	int sidx;
	int eidx;
};

struct th_data_dup_resresv
{
	unsigned int error:1;
	resource_resv **oresresv_arr;
	resource_resv **nresresv_arr;
	server_info *nsinfo;
	queue_info *nqinfo;
	int sidx;
	int eidx;
};

struct th_data_query_jinfo
{
	unsigned int error:1;
	struct batch_status *jobs;
	server_info *sinfo;
	queue_info *qinfo;
	resource_resv **oarr;
	status *policy;
	int pbs_sd;
	int sidx;
	int eidx;
};

struct th_data_free_resresv
{
	resource_resv **resresv_arr;
	int sidx;
	int eidx;
};

struct schd_error
{
	enum sched_error error_code;	/* scheduler error code (see constant.h) */
	enum schd_err_status status_code; /* error status */
	resdef *rdef;			/* resource def if error contains a resource*/
	char *arg1;			/* buffer for error code specific string */
	char *arg2;			/* buffer for error code specific string */
	char *arg3;			/* buffer for error code specific string */
	char *specmsg;			/* buffer to override static error msg */
	schd_error *next;
};

struct state_count
{
	int running;			/* number of jobs in the running state*/
	int queued;			/* number of jobs in the queued state */
	int held;			/* number of jobs in the held state */
	int transit;			/* number of jobs in the transit state */
	int waiting;			/* number of jobs in the waiting state */
	int exiting;			/* number of jobs in the exiting state */
	int suspended;			/* number of jobs in the suspended state */
	int userbusy;			/* number of jobs in the userbusy state */
	int begin;			/* number of job arrays in begin state */
	int expired;			/* expired jobs which are no longer running */
	int invalid;			/* number of invalid jobs */
	int total;			/* total number of jobs in all states */
};

struct place
{
	unsigned int free:1;		/* free placement */
	unsigned int pack:1;		/* pack placement */
	unsigned int scatter:1;		/* scatter placement */
	unsigned int vscatter:1;	/* scatter by vnode */
	unsigned int excl:1;		/* need nodes exclusively */
	unsigned int exclhost:1;	/* need whole hosts exclusively */
	unsigned int share:1;		/* will share nodes */

	char *group;			/* resource to node group by */
};

struct chunk
{
	char *str_chunk;		/* chunk in string form */
	int num_chunks;			/* the number of chunks needed */
	int seq_num;			/* the chunk sequence number */
	resource_req *req;		/* the resources in resource_req form */
};

struct selspec
{
	int total_chunks;
	int total_cpus;			/* # of cpus requested in this select spec */
	resdef **defs;			/* the resources requested by this select spec*/
	chunk **chunks;
};

/* for description of these bits, check the PBS admin guide or scheduler IDS */
struct status
{
	unsigned round_robin:1;		/* Round robin around queues */
	unsigned by_queue:1;		/* schedule per-queue */
	unsigned strict_fifo:1;		/* deprecated */
	unsigned strict_ordering:1;
	unsigned fair_share:1;
	unsigned help_starving_jobs:1;
	unsigned backfill:1;
	unsigned sort_nodes:1;
	unsigned backfill_prime:1;
	unsigned preempting:1;
#ifdef NAS /* localmod 034 */
	unsigned shares_track_only:1;
#endif /* localmod 034 */

	unsigned is_prime:1;
	unsigned is_ded_time:1;
	unsigned sync_fairshare_files:1;	/* sync fairshare files to disk */

	struct sort_info *sort_by;		/* job sorting */
	struct sort_info *node_sort;		/* node sorting */
	enum smp_cluster_dist smp_dist;

	unsigned prime_spill;			/* the amount of time a job can spill into the next prime state */
	unsigned int backfill_depth;		/* number of top jobs to backfill around */

	resdef **resdef_to_check;		/* resources to match as definitions */
	resdef **resdef_to_check_no_hostvnode;	/* resdef_to_check without host/vnode*/
	resdef **resdef_to_check_rassn;		/* resdef_to_check intersects res_rassn */
	resdef **resdef_to_check_rassn_select;	/* resdef_to_check intersects res_rassn and host level resource */
	resdef **resdef_to_check_noncons;	/* non-consumable resources to match */
	resdef **equiv_class_resdef;		/* resources to consider for job equiv classes */


	time_t prime_status_end;		/* the end of prime or nonprime */

	resdef **rel_on_susp;	    /* resources to release on suspend */

	/* not really policy... but kinda just left over here */
	time_t current_time;			/* current time in the cycle */
	time_t cycle_start;			/* cycle start in real time */

	unsigned int order;			/* used to assign a ordering to objs */
	int preempt_attempts;			/* number of jobs attempted to preempt */

	unsigned long long iteration;		/* scheduler iteration count */
};

/*
 * All attributes of the qmgr sched object
 * Don't need log_events here, we use log_event_mask from Liblog/log_event.c
 */
struct schedattrs
{
	unsigned do_not_span_psets:1;
	unsigned only_explicit_psets:1;
	unsigned preempt_targets_enable:1;
	unsigned sched_preempt_enforce_resumption:1;
	unsigned throughput_mode:1;
	long attr_update_period;
	char *comment;
	char *job_sort_formula;
	double job_sort_formula_threshold;
	int opt_backfill_fuzzy;
	char *partition;
	long preempt_queue_prio;
	int preempt_prio[NUM_PPRIO][2];
	struct preempt_ordering preempt_order[PREEMPT_ORDER_MAX + 1];
	enum preempt_sort_vals preempt_sort;
	enum runjob_mode runjob_mode; /* set to a numeric version of job_run_wait attribute value */
	long sched_cycle_length;
	char *sched_log;
	char *sched_port;
	char *sched_priv;
	long server_dyn_res_alarm;
};

struct server_info
{
	unsigned has_soft_limit:1;	/* server has a soft user/grp limit set */
	unsigned has_hard_limit:1;	/* server has a hard user/grp limit set */
	unsigned has_mult_express:1;	/* server has multiple express queues */
	unsigned has_user_limit:1;	/* server has user hard or soft limit */
	unsigned has_grp_limit:1;	/* server has group hard or soft limit */
	unsigned has_proj_limit:1;	/* server has project hard or soft limit */
	unsigned has_all_limit:1;	/* server has PBS_ALL limits set on it */
	unsigned has_prime_queue:1;	/* server has a primetime queue */
	unsigned has_ded_queue:1;	/* server has a dedtime queue */
	unsigned has_nonprime_queue:1;	/* server has a non primetime queue */
	unsigned node_group_enable:1;	/* is node grouping enabled */
	unsigned has_nodes_assoc_queue:1; /* nodes are associates with queues */
	unsigned has_multi_vnode:1;	/* server has at least one multi-vnoded MOM  */
	unsigned has_runjob_hook:1;	/* server has at least 1 runjob hook enabled */
	unsigned eligible_time_enable:1;/* controls if we accrue eligible_time  */
	unsigned provision_enable:1;	/* controls if provisioning occurs */
	unsigned power_provisioning:1;	/* controls if power provisioning occurs */
	unsigned has_nonCPU_licenses:1;	/* server has non-CPU (e.g. socket-based) licenses */
	unsigned use_hard_duration:1;	/* use hard duration when creating the calendar */
	unsigned pset_metadata_stale:1;	/* The placement set meta data is stale and needs to be regenerated before the next use */
	char *name;			/* name of server */
	struct schd_resource *res;	/* list of resources */
	void *liminfo;			/* limit storage information */
	int flt_lic;			/* number of free floating licences */
	int num_queues;			/* number of queues that reside on the server */
	int num_nodes;			/* number of nodes associated with the server */
	int num_resvs;			/* number of reservations on the server */
	int num_preempted;		/* number of jobs currently preempted */
	char **node_group_key;		/* the node grouping resources */
	state_count sc;			/* number of jobs in each state */
	queue_info **queues;		/* array of queues */
	queue_info ***queue_list;	/* 3 dimensional array, used to order jobs in round_robin */
	node_info **nodes;		/* array of nodes associated with the server */
	node_info **unassoc_nodes;	/* array of nodes not associated with queues */
	resource_resv **resvs;		/* the reservations on the server */
	resource_resv **running_jobs;	/* array of jobs which are in state R */
	resource_resv **exiting_jobs;	/* array of jobs which are in state E */
	resource_resv **jobs;		/* all the jobs in the server */
	resource_resv **all_resresv;	/* a list of all jobs and adv resvs */
	event_list *calendar;		/* the calendar of events */
	char *job_sort_formula;	/* set via the JSF attribute of either the sched, or the server */

	time_t server_time;		/* The time the server is at.  Could be in the
					 * future if we're simulating
					 */
	/* the number of running jobs in each preempt level
	 * all jobs in preempt_count[NUM_PPRIO] are unknown preempt status's
	 */
	int preempt_count[NUM_PPRIO + 1];

	counts *group_counts;		/* group resource and running counts */
	counts *project_counts;		/* project resource and running counts */
	counts *user_counts;		/* user resource and running counts */
	counts *alljobcounts;		/* overall resource and running counts */

	/*
	 * Resource/Run counts list to store counts for all jobs which
	 * are running/queued/suspended.
	 */
	counts *total_group_counts;
	counts *total_project_counts;
	counts *total_user_counts;
	counts *total_alljobcounts;

	node_partition **nodepart;	/* array pointers to node partitions */
	int num_parts;			/* number of node partitions(node_group_key) */
	node_partition *allpart;	/* node partition for all nodes */
	int num_hostsets;		/* the size of hostsets */
	node_partition **hostsets;	/* partitions for vnodes on a host */

	char **nodesigs;		/* node signatures from server nodes */

	/* cache of node partitions we created.  We cache them all here and
	 * will attempt to find one when we need to use it.  This cache will not
	 * be duplicated.  It would be difficult to duplicate correctly, and it is
	 * just a cache.  It will be regenerated when needed
	 */
	np_cache **npc_arr;

	resource_resv *qrun_job;	/* used if running a job via qrun request */
	/* policy structure for the server.  This is an easy storage location for
	 * the policy struct.  The policy struct will be passed around separately
	 */
	status *policy;
	fairshare_head *fairshare;	/* root of fairshare tree */
	resresv_set **equiv_classes;
	node_bucket **buckets;		/* node bucket array */
	node_info **unordered_nodes;
#ifdef NAS
	/* localmod 034 */
	share_head *share_head;	/* root of share info */
#endif
};

struct queue_info
{
	unsigned is_started:1;		/* is queue started */
	unsigned is_exec:1;		/* is the queue an execution queue */
	unsigned is_route:1;		/* is the queue a routing queue */
	unsigned is_ok_to_run:1;	/* is it ok to run jobs in this queue */
	unsigned is_ded_queue:1;	/* only jobs in dedicated time */
	unsigned is_prime_queue:1;	/* only run jobs in primetime */
	unsigned is_nonprime_queue:1;	/* only run jobs in nonprimetime */
	unsigned has_nodes:1;		/* does this queue have nodes assoc with it */
	unsigned has_soft_limit:1;	/* queue has a soft user/grp limit set */
	unsigned has_hard_limit:1;	/* queue has a hard user/grp limit set */
	unsigned is_peer_queue:1;	/* queue is a peer queue */
	unsigned has_resav_limit:1;	/* queue has resources_available limits */
	unsigned has_user_limit:1;	/* queue has user hard or soft limit */
	unsigned has_grp_limit:1;	/* queue has group hard or soft limit */
	unsigned has_proj_limit:1;	/* queue has project hard or soft limit */
	unsigned has_all_limit:1;	/* queue has PBS_ALL limits set on it */
	struct server_info *server;	/* server where queue resides */
	char *name;			/* queue name */
	state_count sc;			/* number of jobs in different states */
	void *liminfo;			/* limit storage information */
	int priority;			/* priority of queue */
#ifdef NAS
	/* localmod 046 */
	time_t max_starve;		/* eligible job marked starving after this */
	/* localmod 034 */
	time_t max_borrow;		/* longest job that can borrow CPUs */
	/* localmod 038 */
	unsigned is_topjob_set_aside:1; /* draws topjobs from per_queues_topjobs */
	/* localmod 040 */
	unsigned ignore_nodect_sort:1; /* job_sort_key nodect ignored in this queue */
#endif
	int num_nodes;		/* number of nodes associated with queue */
	struct schd_resource *qres;	/* list of resources on the queue */
	resource_resv *resv;		/* the resv if this is a resv queue */
	resource_resv **jobs;		/* array of jobs that reside in queue */
	resource_resv **running_jobs;	/* array of jobs in the running state */
	node_info **nodes;		/* array of nodes associated with the queue */
	counts *group_counts;		/* group resource and running counts */
	counts *project_counts;		/* project resource and running counts */
	counts *user_counts;		/* user resource and running counts */
	counts *alljobcounts;		/* overall resource and running counts */
	/*
	 * Resource/Run counts list to store counts for all jobs which
	 * are running/queued/suspended.
	 */
	counts *total_group_counts;
	counts *total_project_counts;
	counts *total_user_counts;
	counts *total_alljobcounts;

	char **node_group_key;		/* node grouping resources */
	struct node_partition **nodepart; /* array pointers to node partitions */
	struct node_partition *allpart;   /* partition w/ all nodes assoc with queue*/
	int num_parts;			/* number of node partitions(node_group_key) */
	int num_topjobs;		/* current number of top jobs in this queue */
	int backfill_depth;		/* total allowable topjobs in this queue*/
	char *partition;		/* partition to which queue belongs to */
};

struct job_info
{
	unsigned is_queued:1;		/* state booleans */
	unsigned is_running:1;
	unsigned is_held:1;
	unsigned is_waiting:1;
	unsigned is_transit:1;
	unsigned is_exiting:1;
	unsigned is_suspended:1;
	unsigned is_susp_sched:1;	/* job is suspended by scheduler */
	unsigned is_userbusy:1;
	unsigned is_begin:1;		/* job array 'B' state */
	unsigned is_expired:1;		/* 'X' pseudo state for simulated job end */
	unsigned is_checkpointed:1;	/* job has been checkpointed */

	unsigned can_not_preempt:1;	/* this job can not be preempted */

	unsigned can_checkpoint:1;    /* this job can be checkpointed */
	unsigned can_requeue:1;       /* this job can be requeued */
	unsigned can_suspend:1;       /* this job can be suspended */

	unsigned is_starving:1;		/* job has waited passed starvation time */
	unsigned is_array:1;		/* is the job a job array object */
	unsigned is_subjob:1;		/* is a subjob of a job array */

	unsigned is_provisioning:1;	/* job is provisioning */
	unsigned is_preempted:1;	/* job is preempted */
	unsigned topjob_ineligible:1;	/* Job is ineligible to be a top job */

	char *job_name;			/* job name attribute (qsub -N) */
	char *comment;			/* comment field of job */
	char *resv_id;			/* identifier of reservation job is in */
	char *alt_id;			/* vendor assigned job identifier */
	queue_info *queue;		/* queue where job resides */
	resource_resv *resv;		/* the reservation the job is part of */
	int priority;			/* PBS priority of job */
	time_t etime;			/* the time the job went to the queued state */
	time_t stime;			/* the time the job was started */
	time_t est_start_time;		/* scheduler estimated start time of job */
	time_t time_preempted;		/* time when the job was preempted */
	char *est_execvnode;		/* scheduler estimated execvnode of job */
	unsigned int preempt_status;	/* preempt levels (bitfield) */
	int preempt;			/* preempt priority */
	int peer_sd;			/* connection descriptor to peer server */
	long long job_id;		/* numeric portion of the job id */
	resource_req *resused;		/* a list of resources used */
	group_info *ginfo;		/* the fair share node for the owner */

	/* subjob information */
	char *array_id;			/* job id of job array if we are a subjob */
	int array_index;		/* array index if we are a subjob */

	/* job array information */
	range *queued_subjobs;		/* a list of ranges of queued subjob indices */

	int accrue_type;		/* type of time job should accrue */
	time_t eligible_time;		/* eligible time accrued until last cycle */

	struct attrl *attr_updates;	/* used to federate all attr updates to server*/
	float formula_value;		/* evaluated job sort formula value */
	nspec **resreleased;		/* list of resources released by the job on each node */
	resource_req *resreq_rel;	/* list of resources released */
	char *depend_job_str;		/* dependent jobs in a ':' separated string */
	resource_resv **dependent_jobs; /* dependent jobs with runone depenency */

#ifdef NAS
	/* localmod 045 */
	int		NAS_pri;	/* NAS version of priority */
	/* localmod 034 */
	sh_amt	*sh_amts;	/* Amount of each type job is requesting */
	share_info	*sh_info;	/* Info about share group job belongs to */
	sch_resource_t accrue_rate;	/* rate at which job uses share resources */
	/* localmod 040 */
	int		nodect;		/* Node count for sorting jobs by */
	/* localmod 031 */
	char		*schedsel;	/* schedselect field of job */
	/* localmod 053 */
	site_user_info *u_info;	/* User associated with job */
#endif
};


struct node_scratch
{
	unsigned int visited:1;		/* visited this node for this type of chunk*/
	unsigned int scattered:1;	/* node allocated to a v/scatter request */
	unsigned int ineligible:1;	/* node is ineligible for the job */
	unsigned int to_be_sorted:1;	/* used for sorting of the nodes while
					 * altering a reservation.
					 */
};

struct node_info
{
	unsigned is_down:1;		/* node is down */
	unsigned is_free:1;		/* node is free to run a job */
	unsigned is_offline:1;	/* node is off-line */
	unsigned is_unknown:1;	/* node is in an unknown state */
	unsigned is_exclusive:1;	/* node is running in exclusive mode */
	unsigned is_job_exclusive:1;	/* node is running in job-exclusive mode */
	unsigned is_resv_exclusive:1;	/* node is reserved exclusively */
	unsigned is_sharing:1;	/* node is running in job-sharing mode */
	unsigned is_busy:1;		/* load on node is too high to schedule */
	unsigned is_job_busy:1;	/* ntype = cluster all vp's allocated */
	unsigned is_stale:1;		/* node is unknown by mom */
	unsigned is_maintenance:1;	/* node is in maintenance */

	/* node types */
	unsigned is_pbsnode:1;	/* this is a PBS node */

	/* license types */
	unsigned lic_lock:1;		/* node has a node locked license */

	unsigned has_hard_limit:1;	/* node has a hard user/grp limit set */
	unsigned no_multinode_jobs:1;	/* do not run multnode jobs on this node */

	unsigned resv_enable:1;	/* is this node available for reservations */
	unsigned provision_enable:1;	/* is this node available for provisioning */

	unsigned is_provisioning:1;	/* node is provisioning */
	/* node in wait-provision is considered as node in provisioning state
	 * nodes in provisioning and wait provisioning states cannot run job
	 * NOTE:
	 * If node is provisioning an aoe and job needs this aoe then it could have
	 * run on this node. However, within the same cycle, this cannot be handled
	 * since we can't make the other job wait. In another cycle, the node is
	 * either free or provisioning, then, the case is clear.
	 */
	unsigned is_multivnoded:1;	/* multi vnode */
	unsigned power_provisioning:1;	/* can this node can power provision */
	unsigned is_sleeping:1;		/* node put to sleep through power on/off or ramp rate limit */
	unsigned has_ghost_job:1;	/* race condition occurred: recalculate resources_assigned */

	/* sharing */
	enum vnode_sharing sharing;	/* deflt or forced sharing/excl of the node */

	char *name;			/* name of the node */
	char *mom;			/* host name on which mom resides */
	int   port;			/* port on which Mom is listening */

	char **jobs;			/* the name of the jobs currently on the node */
	char **resvs;			/* the name of the reservations currently on the node */
	resource_resv **job_arr;	/* ptrs to structs of the jobs on the node */
	resource_resv **run_resvs_arr;	/* ptrs to structs of resvs holding resources on the node */

	int pcpus;			/* the number of physical cpus */

	/* This element is the server the node is associated with.  In the case
	 * of a node which is part of an advanced reservation, the nodes are
	 * a copy of the real nodes with the resources modified to what the
	 * reservation gets.  This element points to the server the non-duplicated
	 * nodes do.  This means ninfo is not part of ninfo -> server -> nodes.
	 */
	server_info *server;
	char *queue_name;		/* the queue the node is associated with */

	int num_jobs;			/* number of jobs running on the node */
	int num_run_resv;		/* number of running advanced reservations */
	int num_susp_jobs;		/* number of suspended jobs on the node */

	int priority;			/* node priority */

	counts *group_counts;		/* group resource and running counts */
	counts *user_counts;		/* user resource and running counts */

	int max_running;		/* max number of jobs on the node */
	int max_user_run;		/* max number of jobs running by a user */
	int max_group_run;		/* max number of jobs running by a UNIX group */

	schd_resource *res;		/* list of resources max/current usage */

	int rank;			/* unique numeric identifier for node */

#ifdef NAS
	/* localmod 034 */
	int	sh_cls;			/* Share class supplied by node */
	int	sh_type;		/* Share type of node */
#endif

	char *current_aoe;		/* AOE name instantiated on node */
	char *current_eoe;		/* EOE name instantiated on node */
	char *nodesig;			/* resource signature */
	int nodesig_ind;		/* resource signature index in server array */
	node_info *svr_node;		/* ptr to svr's node if we're a resv node */
	node_partition *hostset;	/* other vnodes on on the same host */
	unsigned int nscr;		/* scratch space local to node search code */
	char *partition;		/* partition to which node belongs to */
	time_t last_state_change_time;	/* Node state change at time stamp */
	time_t last_used_time;		/* Node was last active at this time */
	te_list *node_events;		/* list of run events that affect the node */
	int bucket_ind;			/* index in server's bucket array */
	int node_ind;			/* node's index into sinfo->unordered_nodes */
	node_partition **np_arr;	/* array of node partitions node is in */
};

struct resv_info
{
	unsigned is_standing:1;		/* set to 1 for a standing reservation */
	unsigned is_running:1;		/* the reservation is running (not necessarily in the running state) */
	char *queuename;		/* the name of the queue */
	char *rrule;			/* recurrence rule for standing reservations */
	char *execvnodes_seq;		/* sequence of execvnodes for standing resvs */
	time_t *occr_start_arr;		/* occurrence start time */
	char *timezone;			/* timezone associated to a reservation */
	int resv_idx;			/* the index of standing resv occurrence */
	int count;			/* the total number of occurrences */
	time_t req_start;		/* user requested start time of resv */
	time_t req_start_orig;		/* For altered reservations, this has the original start time */
	time_t req_start_standing;		/* For standing reservations, this will be used to get start time of future occurrences */
	time_t req_end;			/* user requested end tiem of resv */
	time_t req_duration;		/* user requested duration of resv */
	time_t req_duration_orig;		/* For altered reservations, this has the original duration */
	time_t req_duration_standing;	/* For standing reservations, this will be used to get duration of future occurrences */
	time_t retry_time;		/* time at which a reservation is to be reconfirmed */
	enum resv_states resv_state;	/* reservation state */
	enum resv_states resv_substate;	/* reservation substate */
	queue_info *resv_queue;		/* general resv: queue which is owned by resv */
	node_info **resv_nodes;		/* node universe for reservation */
	char *partition;		/* name of the partition in which the reservation was confirmed */
	selspec *select_orig;		/* original schedselect pre-alter */
	nspec **orig_nspec_arr;		/* original non-shrunk exec_vnode with exec_vnode chunk mapped to select chunk */
};

/* resource reservation - used for both jobs and advanced reservations */
struct resource_resv
{
	unsigned can_not_run : 1;	/* res resv can not run this cycle */
	unsigned can_never_run : 1;	/* res resv can never run and will be deleted */
	unsigned can_not_fit : 1;	/* res resv can not fit into node group */
	unsigned is_invalid : 1;	/* res resv is invalid and will be ignored */
	unsigned is_peer_ob : 1;	/* res resv can from a peer server */

	unsigned is_job : 1;		/* res resv is a job */
	unsigned is_prov_needed : 1;	/* res resv requires provisioning */
	unsigned is_shrink_to_fit : 1;	/* res resv is a shrink-to-fit job */
	unsigned is_resv : 1;		/* res resv is an advanced reservation */

	unsigned will_use_multinode:1;	/* res resv will use multiple nodes */

	char *name;			/* name of res resv */
	char *user;			/* username of the owner of the res resv */
	char *group;			/* exec group of owner of res resv */
	char *project;			/* exec project of owner of res resv */
	char *nodepart_name;		/* name of node partition to run res resv in */

	long sch_priority;		/* scheduler priority of res resv */
	int rank;			/* unique numeric identifier for resource_resv */
	int ec_index;			/* Index into server's job_set array*/

	time_t qtime;			/* time res resv was submitted */
	long qrank;			/* time on which we might need to stabilize the sort */
	time_t start;			/* start time (UNDEFINED means no start time */
	time_t end;			/* end time (UNDEFINED means no end time */
	time_t duration;		/* duration of resource resv request */
	time_t hard_duration;		/* hard duration of resource resv request */
	time_t min_duration;		/* minimum duration of STF job */

	resource_req *resreq;		/* list of resources requested */
	selspec *select;		/* select spec */
	selspec *execselect;		/* select spec from exec_vnode and resv_nodes */
	place *place_spec;		/* placement spec */

	server_info *server;		/* pointer to server which owns res resv */
	node_info **ninfo_arr; 		/* nodes belonging to res resv */
	nspec **nspec_arr;		/* exec vnode of object in internal sched form (one nspec per node) */

	job_info *job;			/* pointer to job specific structure */
	resv_info *resv;		/* pointer to reservation specific structure */

	char *aoename;			   /* store name of aoe if requested */
	char *eoename;			   /* store name of eoe if requested */
	char **node_set_str;		   /* user specified node string */
	node_info **node_set;		   /* node array specified by node_set_str */
#ifdef NAS				   /* localmod 034 */
	enum site_j_share_type share_type; /* How resv counts against group share */
#endif					   /* localmod 034 */
	int resresv_ind;		   /* resource_resv index in all_resresv array */
	timed_event *run_event;		   /* run event in calendar */
	timed_event *end_event;		   /* end event in calendar */
};

struct resource_type
{
	/* non consumable - used for selection only (e.g. arch) */
	unsigned is_non_consumable:1;
	unsigned is_string:1;
	unsigned is_boolean:1; /* value == 1 for true and 0 for false */

	/* consumable - numeric resource which is consumed and may have a max limit */
	unsigned is_consumable:1;
	unsigned is_num:1;
	unsigned is_long:1;
	unsigned is_float:1;
	unsigned is_size:1;	/* all sizes are converted into kb */
	unsigned is_time:1;
};

struct schd_resource
{
	const char *name;			/* name of the resource - reference to the definition name */
	struct resource_type type;	/* resource type */

	char *orig_str_avail;		/* original resources_available string */

	char *indirect_vnode_name;	/* name of vnode where to get value */
	schd_resource *indirect_res;	/* ptr to indirect resource */

	sch_resource_t avail;		/* availble amount of the resource */
	char **str_avail;		/* the string form of avail */
	sch_resource_t assigned;	/* amount of the resource assigned */
	char *str_assigned;		/* the string form of assigned */

	resdef *def;			/* resource definition */

	struct schd_resource *next;	/* next resource in list */
};

struct resource_req
{
	char *name;			/* name of the resource - reference to the definition name */
	struct resource_type type;	/* resource type information */

	sch_resource_t amount;		/* numeric value of resource */
	char *res_str;			/* string value of resource */
	resdef *def;			/* definition of resource */
	struct resource_req *next;	/* next resource_req in list */
};

struct resdef
{
	char *name;			/* name of resource */
	struct resource_type type;	/* resource type */
	unsigned int flags;		/* resource flags (see pbs_ifl.h) */
};

struct prev_job_info
{
	char *name;			/* name of job */
	char *entity_name;		/* fair share entity of job */
	resource_req *resused;	/* resources used by the job */
};

struct mom_res
{
	char name[MAX_RES_NAME_SIZE];		/* name of resources for addreq() */
	char ans[MAX_RES_RET_SIZE];		/* what is returned from getreq() */
	unsigned eol:1;			/* set for sentinal value */
};

struct counts
{
	char *name;			/* name of entitiy */
	int running;			/* count of running jobs in object */
	int soft_limit_preempt_bit;	/* Place to store preempt bit if entity is over limits */
	resource_count *rescts;		/* resources used */
	counts *next;
};

struct resource_count
{
	char *name;		    /* resource name */
	resdef *def;		    /* definition of resource */
	sch_resource_t amount;	    /* amount of resource used */
	int soft_limit_preempt_bit; /* Place to store preempt bit if resource of an entity is over limits */
	struct resource_count *next;
};

/* global data types */

/* fairshare head structure */
struct fairshare_head
{
	group_info *root;			/* root of fairshare tree */
	time_t last_decay;			/* last time tree was decayed */
};

/* a path from the root to a group_info in the tree */
struct group_path
{
	group_info *ginfo;
	struct group_path *next;
};


struct group_info
{
	char *name;				/* name of user/group */
	int resgroup;				/* resgroup the group is in */
	int cresgroup;				/* resgroup of the children of group */
	int shares;				/* number of shares this group has */
	float tree_percentage;			/* overall percentage the group has */
	float group_percentage;			/* percentage within fairshare group (i.e., shares/group_shares) */

	/* There are two usage element per entity.  The usage element is used to
	 * hold the real usage for the entity.  The temp_usage is more of a sractch
	 * variable.  At the beginning of the cycle, usage is copied into temp_usage
	 * and from then on, only temp_usage is consulted for fairshare usage
	 */
	usage_t usage;				/* calculated usage info */
	usage_t temp_usage;			/* usage plus any temporary usage */
	float usage_factor;			/* usage calculation taking parent's usage into account: number between 0 and 1 */

	struct group_path *gpath;		/* path from the root of the tree */

	group_info *parent;			/* parent node */
	group_info *sibling;			/* sibling node */
	group_info *child;			/* child node */
};

/**
 * Set of equivalent resresvs.  It is used to keep track if one can't run, the rest cannot.
 * The set is defined by a number of attributes of the resresv.  If the attributes do
 * not matter, they won't be used and set to NULL
 * @see create_resresv_set_by_resresv() for reasons why members can be NULL
 */
struct resresv_set
{
	unsigned can_not_run:1;		/* set can not run */
	schd_error *err;		/* reason why set can not run*/
	char *user;			/* user of set, can be NULL */
	char *group;			/* group of set, can be NULL */
	char *project;			/* project of set, can be NULL */
	selspec *select_spec;		/* select spec of set */
	place *place_spec;		/* place spec of set */
	resource_req *req;		/* ATTR_L (qsub -l) resources of set.  Only contains resources on the resources line */
	queue_info *qinfo;		/* The queue the resresv is in if the queue has nodes associated */
};

struct node_partition
{
	unsigned int ok_break:1;	/* OK to break up chunks on this node part */
	unsigned int excl:1;		/* partition should be allocated exclusively */
	char *name;			/* res_name=res_val */
	/* name of resource and value which define the node partition */
	resdef *def;
	char *res_val;
	int tot_nodes;		/* the total number of nodes  */
	int free_nodes;		/* the number of nodes in state Free  */
	schd_resource *res;		/* total amount of resources in node part */
	node_info **ninfo_arr;	/* array of pointers to node structures  */
	node_bucket **bkts;	/* node buckets for node part */
	int rank;		/* unique numeric identifier for node partition */
};

struct np_cache
{
	char **resnames;		/* resource names used to create partitions */
	node_info **ninfo_arr;	/* ptr to array of nodes used to create pools */
	int num_parts;		/* number of partitions in nodepart */
	node_partition **nodepart;	/* node partitions */
};

/* header to usage file.  Needs to be EXACTLY the same size as a
 * group_node_usage for backwards compatibility
 * tag defined in config.h
 */
struct group_node_header
{
	char tag[9];		/* usage file "magic number" */
	usage_t version;	/* usage file version number */
};

/* This structure is used to write out the usage to disk
 * Version 1 was just successive group_node_usage structures written to disk
 * with not header or anything.
 */
struct group_node_usage_v1
{
	char name[9];
	usage_t usage;
};

/* This is the second attempt at a good usage file.  The first became obsolete
 * when users became entities and entities were no longer constrained by the
 * 8 characters of usernames.  Usage file version 2 also contains the last
 * 8 characters of usernames.  Usage file version 2 also contains the last
 * 8 characters of usernames.  Usage file version 2 also contains the last
 * decay time so it can be saved over restarts of the scheduler
 */
struct group_node_usage_v2
{
	char name[USAGE_NAME_MAX];
	usage_t usage;
};

struct usage_info
{
	char *name;			/* name of the user */
	struct resource_req *reslist;	/* list of resources */
	int computed_value;		/* value computed from usage info */
};

struct t
{
	unsigned int hour;
	unsigned int min;
	unsigned int none;
	unsigned int all;
};

struct sort_info
{
	char *res_name;			/* Name of sorting resource */
	resdef *def;			/* Definition of sorting resource */
	enum sort_order order;		/* Ascending or Descending sort */
	enum resource_fields res_type;  /* resources_available, resources_assigned, etc */
};

struct sort_conv
{
	const char *config_name;
	const char *res_name;
	enum sort_order order;
};

/* structure to convert an enum to a string or back again */
struct enum_conv
{
	int value;
	const char *str;
};

struct timegap
{
	time_t from;
	time_t to;
};

struct dyn_res
{
	char *res;
	char *command_line;
	char *script_name;
};

struct peer_queue
{
	char *local_queue;
	char *remote_queue;
	char *remote_server;
	int peer_sd;
};

struct nspec
{
	unsigned int end_of_chunk:1; /* used for putting parens into the execvnode */
	unsigned int go_provision:1; /* used to mark a node to be provisioned */
	int seq_num;			/* sequence number of chunk */
	int sub_seq_num;		/* sub sequence number for sort stabilization */
	node_info *ninfo;
	resource_req *resreq;
	chunk *chk;
};

struct nameval
{
	unsigned int is_set:1;
	char *str;
	int value;
};

struct config
{
	/* these bits control the scheduling policy
	 * prime_* is the prime time setting
	 * non_prime_* is the non-prime setting
	 */
	unsigned prime_rr	:1;	/* round robin through queues*/
	unsigned non_prime_rr	:1;
	unsigned prime_bq	:1;	/* by queue */
	unsigned non_prime_bq	:1;
	unsigned prime_sf	:1;	/* strict fifo */
	unsigned non_prime_sf	:1;
	unsigned prime_so	:1;	/* strict ordering */
	unsigned non_prime_so	:1;
	unsigned prime_fs	:1;	/* fair share */
	unsigned non_prime_fs	:1;
	unsigned prime_hsv	:1;	/* help starving jobs */
	unsigned non_prime_hsv:1;
	unsigned prime_bf	:1;	/* back filling */
	unsigned non_prime_bf	:1;
	unsigned prime_sn	:1;	/* sort nodes by priority */
	unsigned non_prime_sn	:1;
	unsigned prime_bp	:1;	/* backfill around prime time */
	unsigned non_prime_bp	:1;	/* backfill around non prime time */
	unsigned prime_pre	:1;	/* preemptive scheduling */
	unsigned non_prime_pre:1;
	unsigned update_comments:1;	/* should we update comments or not */
	unsigned prime_exempt_anytime_queues:1; /* backfill affects anytime queues */
	unsigned assign_ssinodes:1;	/* assign the ssinodes resource */
	unsigned preempt_starving:1;	/* once jobs become starving, it can preempt */
	unsigned preempt_fairshare:1; /* normal jobs can preempt over usage jobs */
	unsigned dont_preempt_starving:1; /* don't preempt staving jobs */
	unsigned enforce_no_shares:1;	/* jobs with 0 shares don't run */
	unsigned node_sort_unused:1;	/* node sorting by unused/assigned is used */
	unsigned resv_conf_ignore:1;  /* if we want to ignore dedicated time when confirming reservations.  Move to enum if ever expanded */
	unsigned allow_aoe_calendar:1;        /* allow jobs requesting aoe in calendar*/
#ifdef NAS /* localmod 034 */
	unsigned prime_sto	:1;	/* shares_track_only--no enforce shares */
	unsigned non_prime_sto:1;
#endif /* localmod 034 */

	struct sort_info *prime_sort;		/* prime time sort */
	struct sort_info *non_prime_sort;	/* non-prime time sort */

	enum smp_cluster_dist prime_smp_dist;	/* how to dist jobs during prime*/
	enum smp_cluster_dist non_prime_smp_dist;/* how do dist jobs during nonprime*/
	time_t prime_spill;			/* the amount of time a job can
						 * spill into primetime
						 */
	time_t nonprime_spill;			/* vice versa for prime_spill */
	fairshare_head *fairshare;		/* fairshare tree */
	time_t decay_time;			/*  time in seconds for the decay period*/
	struct t prime[HIGH_DAY][HIGH_PRIME];	/* prime time start and prime time end*/
	int holidays[MAX_HOLIDAY_SIZE];		/* holidays in Julian date */
	int holiday_year;			/* the year the holidays are for */
	int num_holidays;			/* number of actual holidays */
	struct timegap ded_time[MAX_DEDTIME_SIZE];/* dedicated times */
	int unknown_shares;			/* unknown group shares */
	int max_preempt_attempts;		/* max num of preempt attempts per cyc*/
	int max_jobs_to_check;			/* max number of jobs to check in cyc*/
	char ded_prefix[PBS_MAXQUEUENAME +1];	/* prefix to dedicated queues */
	char pt_prefix[PBS_MAXQUEUENAME +1];	/* prefix to primetime queues */
	char npt_prefix[PBS_MAXQUEUENAME +1];	/* prefix to non primetime queues */
	char *fairshare_res;			/* resource to calc fairshare usage */
	float fairshare_decay_factor;		/* decay factor used when decaying fairshare tree */
	char *fairshare_ent;			/* job attribute to use as fs entity */
	char **dyn_res_to_get;			/* dynamic resources to get from moms */
	char **res_to_check;			/* the resources schedule on */
	resdef **resdef_to_check;		/* the res to schedule on in def form */
	char **ignore_res;			/* resources - unset implies infinite */
	int num_res_to_check;			/* the size of res_to_check */
	time_t max_starve;			/* starving threshold */
	/* order to preempt jobs */
	struct sort_info *prime_node_sort;	/* node sorting primetime */
	struct sort_info *non_prime_node_sort;	/* node sorting non primetime */
	struct dyn_res dynamic_res[MAX_SERVER_DYN_RES]; /* for server_dyn_res */
	struct peer_queue peer_queues[NUM_PEERS];/* peer local -> remote queue map */
#ifdef NAS
	/* localmod 034 */
	time_t max_borrow;			/* job share borrowing limit */
	int per_share_topjobs;		/* per share group guaranteed top jobs*/
	/* localmod 038 */
	int per_queues_topjobs;		/* per queues guaranteed top jobs */
	/* localmod 030 */
	int min_intrptd_cycle_length;		/* min length of interrupted cycle */
	int max_intrptd_cycles;		/* max consecutive interrupted cycles */
#endif

	/* selection criteria of nodes for provisioning */
	enum provision_policy_types provision_policy;
};

struct rescheck
{
	char *name;
	char *comment_msg;
	char *debug_msg;
};

struct event_list
{
	unsigned int eol:1;		/* we've reached the end of time */
	timed_event *events;		/* the calendar of events */
	timed_event *next_event;	/* the next event to be performed */
	timed_event *first_run_event;	/* The first run event in the calendar */
	time_t *current_time;		/* [reference] current time in the calendar */
};

struct timed_event
{
	unsigned int disabled:1;	/* event is disabled - skip it in simulation */
	const char *name;			/* [reference] name of event */
	enum timed_event_types event_type;
	time_t event_time;
	event_ptr_t *event_ptr;
	event_func_t event_func;
	void *event_func_arg;		/* optional argument to function - not freed */
	timed_event *next;
	timed_event *prev;
};

struct te_list {
	te_list *next;
	timed_event *event;
};

struct bucket_bitpool {
	pbs_bitmap *truth;		/* The actual bits.  This only changes if the bitmaps are changing */
	int truth_ct;			/* number of 1 bits in truth bitmap*/
	pbs_bitmap *working;		/* Used for short lived operations.  Usually truth is copied into working. */
	int working_ct;			/* number of 1 bits in working bitmap */
};

struct node_bucket {
	char *name;			/* Name of bucket: resource spec + queue + priority */
	schd_resource *res_spec;	/* resources that describe the bucket */
	queue_info *queue;		/* queue that nodes in the bucket are associated with */
	int priority;			/* priority of nodes in the bucket */
	pbs_bitmap *bkt_nodes;		/* bitmap of the nodes in the bucket */
	bucket_bitpool *free_pool;	/* bit pool of free_pool nodes*/
	bucket_bitpool *busy_later_pool;/* bit pool of nodes that are free now, but are busy_pool later */
	bucket_bitpool *busy_pool;	/* bit pool of nodes that are busy now */
	int total;			/* total number of nodes in bucket */
};

struct node_bucket_count {
	node_bucket *bkt;		/* node bucket */
	int chunk_count;		/* number of chunks bucket can satisfy */
};

struct chunk_map {
	chunk *chunk;
	node_bucket_count **bkt_cnts;	/* buckets job can run in and chunk counts */
	pbs_bitmap *node_bits;		/* assignment of nodes from buckets */
};

struct resresv_filter {
	resource_resv *job;
	schd_error *err;		/* reason why set can not run*/
};
#ifdef	__cplusplus
}
#endif
#endif	/* _DATA_TYPES_H */
