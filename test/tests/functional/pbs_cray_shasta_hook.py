# coding: utf-8

# Copyright (C) 1994-2019 Altair Engineering, Inc.
# For more information, contact Altair at www.altair.com.
#
# This file is part of the PBS Professional ("PBS Pro") software.
#
# Open Source License Information:
#
# PBS Pro is free software. You can redistribute it and/or modify it under the
# terms of the GNU Affero General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# PBS Pro is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.
# See the GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Commercial License Information:
#
# For a copy of the commercial license terms and conditions,
# go to: (http://www.pbspro.com/UserArea/agreement.html)
# or contact the Altair Legal Department.
#
# Altair’s dual-license business model allows companies, individuals, and
# organizations to create proprietary derivative works of PBS Pro and
# distribute them - whether embedded or bundled with other software -
# under a commercial license agreement.
#
# Use of Altair’s trademarks, including but not limited to "PBS™",
# "PBS Professional®", and "PBS Pro™" and Altair’s logos is subject to Altair's
# trademark licensing policies.

from tests.functional import *
import os
import time
import json


@tags('shasta', 'smoke')
class TestShastaSmokeTest(TestFunctional):
    """
    Set of tests that qualifies as smoketest for Shasta platform
    """

    curl_cmd = ['curl', '--unix-socket',
                '/var/run/jacsd/jacsd.sock', '-s']

    hook_name = 'PBS_cray_jacs'

    cfg = """
{
    "post_timeout": %d,
    "delete_timeout": %d,
    "unix_socket_file": "/var/run/jacsd/jacsd.sock"
}
"""

    def get_jobs(self, jid=None, node=None):
        cmd = self.curl_cmd + ['-XGET']
        if jid:
            cmd += ['http://localhost/rm/v1/jobs/%s' % jid]
        else:
            cmd += ['http://localhost/rm/v1/jobs']

        if not node:
            node = self.mom.shortname

        r = self.du.run_cmd(node, cmd)
        js = json.loads(r['out'][0])
        return js

    def load_config(self, cfg):
        """
        Create a hook configuration file with the provided contents.
        """
        fn = self.du.create_temp_file(hostname=self.server.shortname, body=cfg)
        self.logger.info('Current config: %s' % cfg)
        a = {'content-type': 'application/x-config',
             'content-encoding': 'default',
             'input-file': fn}
        self.server.manager(MGR_CMD_IMPORT, PBS_HOOK, a, self.hook_name)
        self.mom.log_match('PBS_cray_jacs.CF;copy hook-related '
                           'file request received',
                           starttime=self.server.ctime)
        pbs_home = self.server.pbs_conf['PBS_HOME']
        svr_conf = os.path.join(
            os.sep, pbs_home, 'server_priv', 'hooks', 'PBS_cray_jacs.CF')
        pbs_home = self.mom.pbs_conf['PBS_HOME']
        mom_conf = os.path.join(
            os.sep, pbs_home, 'mom_priv', 'hooks', 'PBS_cray_jacs.CF')
        # reload config if server and mom cfg differ up to count times
        count = 5
        while (count > 0):
            r1 = self.du.run_cmd(
                cmd=['cat', svr_conf], sudo=True, hosts=self.server.shortname)
            r2 = self.du.run_cmd(
                cmd=['cat', mom_conf], sudo=True, hosts=self.mom.shortname)
            if r1['out'] != r2['out']:
                self.logger.info('server & mom PBS_cray_jacs.CF differ')
                self.server.manager(
                    MGR_CMD_IMPORT, PBS_HOOK, a, self.hook_name)
                self.mom.log_match('PBS_cray_jacs.CF;copy hook-'
                                   'related file request received',
                                   starttime=self.server.ctime)
            else:
                self.logger.info('server & mom PBS_cray_jacs.CF match')
                break
            time.sleep(1)
            count -= 1
            self.cfg = cfg

    def test_create_and_delete_job(self):
        """
        Test that checks the before and after of the POST and DELETE /jobs
        """
        a = {'job_history_enable': 'True'}
        self.server.manager(MGR_CMD_SET, SERVER, a)
        self.load_config(self.cfg % (30, 30))

        js = self.get_jobs()
        self.assertFalse(js['jobs'])

        j = Job(TEST_USER)
        j.set_sleep_time(30)
        jid = self.server.submit(j)
        self.server.expect(JOB, {'job_state': 'R'}, id=jid)
        self.mom.log_match('Job %s registered' % jid)

        js = self.get_jobs()
        self.assertEquals(len(js['jobs']), 1)
        self.assertEquals(js['jobs'][0]['jobid'], jid)

        self.server.expect(JOB, {'job_state': 'F'}, extend='x', offset=30,
                           interval=1, id=jid)

        js = self.get_jobs()
        self.assertFalse(js['jobs'])

    def test_create_job_already_exists(self):
        """
        Test that checks that when the job already exists on JACS,
        we delete it and create a new one
        """
        a = {'job_history_enable': 'True'}
        self.server.manager(MGR_CMD_SET, SERVER, a)
        self.load_config(self.cfg % (30, 30))

        a = {ATTR_h: None}
        j = Job(TEST_USER, attrs=a)
        j.set_sleep_time(30)
        jid = self.server.submit(j)

        cmd = (self.curl_cmd +
               ['-XPOST', '-d', '{"jobid": "%s", "uid": 1}' % jid,
                'http://localhost/rm/v1/jobs'])
        self.du.run_cmd(self.mom.shortname, cmd)
        self.server.rlsjob(jid, None)
        self.server.expect(JOB, {'job_state': 'R'}, id=jid)
        self.mom.log_match('Job %s registered' % jid)
        self.server.expect(JOB, {'job_state': 'F'},
                           extend='x', offset=30, interval=1, id=jid)
        self.mom.log_match('Job %s deleted' % jid)

    def test_post_timeout(self):
        """
        Test that if the POST timesout, the node is offlined.
        """
        self.load_config(self.cfg % (0.000001, 30))
        j = Job(TEST_USER)
        jid = self.server.submit(j)
        self.server.status(NODE, {'state': 'offline'}, id=self.mom.shortname)
        self.mom.log_match('POST timed out')

        js = self.get_jobs(jid=jid)
        if 'jobid' in js:
            cmd = self.curl_cmd + ['-XDELETE',
                                   'http://localhost/rm/v1/jobs/%s' % jid]
            self.du.run_cmd(self.mom.shortname, cmd)

    def test_delete_timeout(self):
        """
        Test that if the DELETE timesout, the node is not offlined.
        """
        a = {'job_history_enable': 'True'}
        self.server.manager(MGR_CMD_SET, SERVER, a)
        self.load_config(self.cfg % (30, 0.000001))
        j = Job(TEST_USER)
        j.set_sleep_time(30)
        jid = self.server.submit(j)
        self.server.expect(JOB, {'job_state': 'F'},
                           extend='x', offset=30, interval=1, id=jid)

        self.server.status(NODE, {'state': 'free'}, id=self.mom.shortname)
        self.mom.log_match('DELETE timed out')

        js = self.get_jobs(jid=jid)
        if 'jobid' in js:
            cmd = self.curl_cmd + ['-XDELETE',
                                   'http://localhost/rm/v1/jobs/%s' % jid]
            self.du.run_cmd(self.mom.shortname, cmd)
