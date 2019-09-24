#!/bin/bash


useradd -ms /bin/bash -b /lus --uid 1201 pbstest1
useradd -ms /bin/bash -b /lus --uid 1202 pbstest2
useradd -ms /bin/bash -b /lus --uid 1203 pbstest3
useradd -ms /bin/bash -b /lus --uid 1204 pbstest4
useradd -ms /bin/bash -b /lus --uid 1205 pbstest5


# chmod ssh directory/files
chmod 700 ~/.ssh

# start ssh daemon
ssh-keygen -A
/usr/sbin/sshd

echo "\$usecp *:/lus/ /lus/" >> /var/spool/pbs/mom_priv/config


# start pbs mom
/etc/init.d/pbs start

while true; do sleep 1; done
