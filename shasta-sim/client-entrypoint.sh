#!/bin/bash

USERNAME="pbstest$1"
PORT="3200$1"
NEWUID="120$1"
useradd -ms /bin/bash -b /lus --uid $NEWUID $USERNAME

# create ssh directory/files
dir="/lus/$USERNAME/.ssh"
mkdir "$dir"
cat /root/ssh-keys/id_rsa.pub >> "$dir/authorized_keys"

chmod 700 "$dir"
chmod 644 "$dir/authorized_keys"
chown "$USERNAME:$USERNAME" "/lus/$USERNAME" -R


# start ssh daemon
ssh-keygen -A
/usr/sbin/sshd -p $PORT



while true; do sleep 1; done