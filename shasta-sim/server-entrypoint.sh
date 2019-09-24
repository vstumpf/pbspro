#!/bin/bash

term_handler() {
    /etc/init.d/pbs stop
    exit 143; # 128 + 15 -- SIGTERM
}

trap 'term_handler' SIGTERM

# chmod ssh directory/files
chmod 700 ~/.ssh

# run habitat
/opt/pbs/libexec/pbs_habitat

# do the "shasta" thing
mkdir /etc/cray
touch /etc/cray/xname

# enable hook
# sed -i 's/enabled=false/enabled=true/g' /opt/pbs/lib/python/altair/pbs_hooks/PBS_cray_jacs.HK

# copy pbs.conf
cp /etc/pbs.conf /etc/pbs.conf2

# start pbs daemons
/etc/init.d/pbs start

# create nodes
/opt/pbs/bin/qmgr -c "c n pbs-mom"
/opt/pbs/bin/qmgr -c "s s flatuid=t"

# install ptl
pip3 install -U -r test/fw/requirements.txt test/fw/.
/usr/local/bin/pbs_config --make-ug

cat <<EOF > test/tests/paramfile
test-users=pbstest1@pbs-client-1+32001:pbstest2@pbs-client-2+32002:pbstest3@pbs-client-3+32003:pbstest4@pbs-client-4+32004:pbstest5@pbs-client-5+32005
server=pbs-server@/etc/pbs.conf2
mom=pbs-mom
EOF

while true; do sleep 1; done
