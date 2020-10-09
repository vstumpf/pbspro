#!/bin/bash

curdir=$(dirname $(readlink -f $0))
cd ${curdir}

resultdir=${curdir}/results/$1
num_jobs=$2
jtype=$3
num_subjobs=$4
concmd="podman exec pbs-server-1 "

###############################
# utility funcs
###############################
function wait_jobs() {
	local _ct _last=0 _nct=0

	while true
	do
		_ct=$(${concmd} /opt/pbs/bin/qstat -Bf 2>/dev/null | grep total_jobs | awk -F' = ' '{ print $2 }')
		if [ "x${_ct}" == "x0" ]; then
			echo "total_jobs: ${_ct}"
			break
		elif [ "x${_ct}" == "x${_last}" ]; then
			if [ ${_nct} -gt 10 ]; then
				echo "Looks like job counts is not changing, force exiting wait loop"
				break
			fi
			_nct=$(( _nct + 1 ))
		fi
		_last=${_ct}
		echo "total_jobs: ${_ct}"
		sleep 10
	done
}

function collect_logs() {
	local host svr svrs _d fsvr="" _destd=${resultdir}/$1

	mkdir -p ${_destd}
	for host in $(cat ${curdir}/nodes)
	do
		svrs=$(ssh ${host} ls -1 /tmp/pbs 2>/dev/null | grep pbs-server | sort -u)
		if [ "x${svrs}" == "x" ]; then
			continue
		fi
		for _d in ${svrs}
		do
			echo "Saving logs from ${_d}"
			rm -rf ${_destd}/${_d}
			mkdir -p ${_destd}/${_d}
			if [ -d /tmp/pbs/${_d} ]; then
				cp -rf /tmp/pbs/${_d}/server_logs ${_destd}/${_d}/
				truncate -s0 /tmp/pbs/${_d}/server_logs/*
				cp -rf /tmp/pbs/${_d}/server_priv/accounting ${_destd}/${_d}/
				truncate -s0 /tmp/pbs/${_d}/server_priv/accounting/*
				if [ "x${fsvr}" == "x" ]; then
					cp -rf /tmp/pbs/${_d}/sched_logs ${_destd}/${_d}/
					truncate -s0 /tmp/pbs/${_d}/sched_logs/*
					fsvr=${_d}
				fi
			else
				scp -qr ${host}:/tmp/pbs/${_d}/server_logs ${_destd}/${_d}/
				ssh ${host} truncate -s0 /tmp/pbs/${_d}/server_logs/\*
				scp -qr ${host}:/tmp/pbs/${_d}/server_priv/accounting ${_destd}/${_d}/
				ssh ${host} truncate -s0 /tmp/pbs/${_d}/server_priv/accounting/\*
				if [ "x${fsvr}" == "x" ]; then
					scp -qr ${host}:/tmp/pbs/${_d}/sched_logs ${_destd}/${_d}/sched_logs
					ssh ${host} truncate -s0 /tmp/pbs/${_d}/sched_logs/\*
					fsvr=${_d}
				fi
			fi
		done
	done
}
###############################
# end of utility funcs
###############################


############################
# test funcs
############################
function test_with_sched_off() {
	${concmd} /opt/pbs/bin/qmgr -c "s s scheduling=0"
	${curdir}/submit-jobs.sh ${num_jobs} ${jtype} ${num_subjobs}
	${concmd} /opt/pbs/bin/qmgr -c "s s scheduling=1"
	wait_jobs
	collect_logs sched_off
}

function test_with_sched_on() {
	${concmd} /opt/pbs/bin/qmgr -c "s s scheduling=1"
	${curdir}/submit-jobs.sh ${num_jobs} ${jtype} ${num_subjobs}
	wait_jobs
	collect_logs sched_on
}
############################
# end test funcs
############################

test_with_sched_off
test_with_sched_on
