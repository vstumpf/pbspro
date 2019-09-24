# base image for building pbspro
FROM centos:7 as builder
# install dependencies for building
RUN yum install -y gcc make rpm-build libtool hwloc-devel libX11-devel \
    libXt-devel libedit-devel libical-devel ncurses-devel perl \
    postgresql-devel postgresql-contrib python3-devel tcl-devel  tk-devel swig expat-devel \
    openssl-devel libXext libXft git

WORKDIR /src/pbspro

# COPY buildutils buildutils
# COPY doc doc
# COPY m4 m4
# COPY docker docker
# COPY COPYRIGHT COPYRIGHT
# COPY INSTALL INSTALL
# COPY LICENSE LICENSE
# COPY Makefile.am Makefile.am
# COPY README.md README.md
# COPY autogen.sh autogen.sh
# COPY configure.ac configure.ac
# COPY pbspro-rpmlintrc pbspro-rpmlintrc
# COPY pbspro.spec pbspro.spec
# COPY pbspro.spec.in pbspro.spec.in
# COPY src src
# COPY test test

RUN git clone https://github.com/pbspro/pbspro.git /src/pbspro && \
    bash /src/pbspro/docker/centos7/build.sh



# base daemon image
FROM centos:7 AS pbs-daemon-image

# install dependencies for running
RUN yum install -y epel-release
RUN yum install -y expat libedit postgresql-server postgresql-contrib python3 \
    sendmail sudo tcl tk libical python3-pip python-devel perl which openssh-clients




FROM pbs-daemon-image AS pbs-server-image
LABEL maintainer="Vincent Stumpf <vincents.995@gmail.com>"

WORKDIR /root/pbspro

COPY shasta-sim/id_rsa /root/.ssh/id_rsa
RUN chmod 700 /root/.ssh && \
    chmod 600 /root/.ssh/id_rsa && \
    chown root:root /root/.ssh -R

RUN useradd -ms /bin/bash pbsdata

COPY --from=builder /root/rpmbuild/RPMS/x86_64/pbspro-server-*.rpm .
#COPY --from=builder /src/pbspro/ptl.tar.gz .

ENV PBS_SERVER pbs-server
RUN yum install -y pbspro-server-*.rpm

#RUN tar -xzf ptl.tar.gz && \
#    pip install -U -r test/fw/requirements.txt test/fw/.

COPY shasta-sim/server-entrypoint.sh .

ENTRYPOINT ["bash", "server-entrypoint.sh"]




FROM pbs-daemon-image AS pbs-mom-image
LABEL maintainer="Vincent Stumpf <vincents.995@gmail.com>"

RUN yum install -y openssh-server
# RUN python -m pip install --no-cache-dir requests-unixsocket

COPY shasta-sim/id_rsa.pub /root/.ssh/id_rsa.pub
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys && \
    chmod 700 /root/.ssh && \
    chmod 644 /root/.ssh/authorized_keys && \
    chown root:root /root/.ssh -R

COPY --from=builder /root/rpmbuild/RPMS/x86_64/pbspro-execution-*.rpm .

ENV PBS_SERVER pbs-server
RUN yum install -y pbspro-execution-*.rpm

COPY shasta-sim/mom-entrypoint.sh .

ENTRYPOINT ["bash", "mom-entrypoint.sh"]



FROM centos:7 as pbs-client-image
LABEL maintainer="Vincent Stumpf <vincents.995@gmail.com>"

RUN yum install -y openssh-server openssh-clients

COPY shasta-sim/id_rsa.pub /root/ssh-keys/id_rsa.pub

COPY --from=builder /root/rpmbuild/RPMS/x86_64/pbspro-client-*.rpm .

ENV PBS_SERVER pbs-server
RUN yum install -y pbspro-client-*.rpm

COPY shasta-sim/client-entrypoint.sh .
ENTRYPOINT ["bash", "client-entrypoint.sh"]

