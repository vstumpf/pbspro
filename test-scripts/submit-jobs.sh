#!/bin/bash

_curdir=$(dirname $(readlink -f $0))
_self=${_curdir}/$(basename $0)
cd ${_curdir}

if [ $# -gt 4 ]; then
	echo "syntax: $0 <no. jobs> [j|ja] <no. subjob if ja>"
	exit 1
fi

if [ "x$1" == "xsubmit" ]; then
	njobs=$2
	jtype=$3
	nsj=$4

	. /etc/pbs.conf

	if [ "x${jtype}" == "xja" ]; then
		for i in $(seq 1 $njobs)
		do
			/opt/pbs/bin/qsub -J1-${nsj} -koe -- /bin/date > /dev/null
		done
	else
		for i in $(seq 1 $njobs)
		do
			/opt/pbs/bin/qsub -koe -- /bin/date > /dev/null
		done
    fi
	exit 0
fi

if [ "x$1" == "xsvr-submit" ]; then
	njobs=$2
	jtype=$3
	nsj=$4
	users=$(awk -F: '/^(pbsu|tst)/ {print $1}' /etc/passwd)
	_tu=$(awk -F: '/^(pbsu|tst)/ {print $1}' /etc/passwd | wc -l)
	_jpu=$(( njobs / _tu ))

	for usr in ${users}
	do
		( setsid sudo -Hiu ${usr} ${_self} submit ${_jpu} ${jtype} ${nsj}) &
	done
	wait
	exit 0
fi

njobs=$1
jtype=$2
nsj=$3
if [ "x${jtype}" == "xja" ]; then
	echo "Total jobs array: ${njobs}, Total Subjob per array: ${nsj}, Total svrs: 1, Jobs array per Svr: ${njobs}"
else
	echo "Total jobs: ${njobs}, Total svrs: 1, Jobs per Svr: ${njobs}"
fi
podman exec pbs-server-1 ${_self} svr-submit ${njobs} ${jtype} ${nsj}
wait
