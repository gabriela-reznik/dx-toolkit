#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (C) 2013-2016 DNAnexus, Inc.
#
# This file is part of dx-toolkit (DNAnexus platform client libraries).
#
#   Licensed under the Apache License, Version 2.0 (the "License"); you may not
#   use this file except in compliance with the License. You may obtain a copy
#   of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#   WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#   License for the specific language governing permissions and limitations
#   under the License.
from __future__ import print_function, unicode_literals, division, absolute_import

import os, sys, unittest, json
from dxpy.nextflow.nextflow_templates import get_nextflow_src
from dxpy.nextflow.nextflow_templates import get_nextflow_dxapp
from dxpy.nextflow.nextflow_templates import get_default_inputs

import uuid
from dxpy_testutil import (DXTestCase, DXTestCaseBuildApps, DXTestCaseBuildWorkflows, check_output, temporary_project,
                           select_project, cd, override_environment, generate_unique_username_email,
                           without_project_context, without_auth, as_second_user, chdir, run, DXCalledProcessError)
import dxpy_testutil as testutil
from dxpy.exceptions import DXAPIError, DXSearchError, EXPECTED_ERR_EXIT_STATUS, HTTPError
from dxpy.compat import USING_PYTHON2, str, sys_encoding, open
from dxpy.utils.resolver import ResolutionError, _check_resolution_needed as check_resolution
if USING_PYTHON2:
    spawn_extra_args = {}
else:
    # Python 3 requires specifying the encoding
    spawn_extra_args = {"encoding" : "utf-8" }


def build_nextflow_applet(app_dir):
    with temporary_project('test proj', reclaim_permissions=True, cleanup=False) as temp_project:

        updated_app_dir = app_dir + str(uuid.uuid1())
        # updated_app_dir = os.path.abspath(os.path.join(tempdir, os.path.basename(app_dir)))
        # shutil.copytree(app_dir, updated_app_dir)
        print(run(['pwd']))
        print(run(['ls']))
        build_output = run(['dx', 'build', '--nextflow', './nextflow', '-f', f'--project', temp_project.get_id()])
        print(build_output, file=sys.stderr)
        return json.loads(build_output)['id']


class TestNextflow(DXTestCase):
    # def test_temp(self):
    #     print("test-message")
    #     # assert False

    def test_basic_hello(self):
        applet = build_nextflow_applet("./nextflow/")
        print(applet)

class TestNextflowTemplates(DXTestCase):
    def test_inputs(self):
        inputs = get_default_inputs()
        print(len(inputs))
        self.assertEqual(len(inputs)), 7)

if __name__ == '__main__':
    if 'DXTEST_FULL' not in os.environ:
        sys.stderr.write('WARNING: env var DXTEST_FULL is not set; tests that create apps or run jobs will not be run\n')
    unittest.main()