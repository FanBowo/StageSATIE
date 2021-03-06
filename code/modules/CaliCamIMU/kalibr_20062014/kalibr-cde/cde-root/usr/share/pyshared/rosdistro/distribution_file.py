# Software License Agreement (BSD License)
#
# Copyright (c) 2013, Open Source Robotics Foundation, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of Open Source Robotics Foundation, Inc. nor
#    the names of its contributors may be used to endorse or promote
#    products derived from this software without specific prior
#    written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from .package import Package
from .repository import Repository


class DistributionFile(object):

    _type = 'distribution'

    def __init__(self, name, data):
        self.name = name

        assert 'type' in data, "Expected file type is '%s'" % DistributionFile._type
        assert data['type'] == DistributionFile._type, "Expected file type is '%s', not '%s'" % (DistributionFile._type, data['type'])

        assert 'version' in data, "Source file for '%s' lacks required version information" % self.name
        assert int(data['version']) == 1, "Unable to handle '%s' format version '%d', please update rosdistro (e.g. on Ubuntu/Debian use: sudo apt-get update && sudo apt-get install --only-upgrade python-rosdistro)" % (DistributionFile._type, int(data['version']))
        self.version = int(data['version'])

        self.repositories = {}
        self.release_packages = {}
        if 'repositories' in data and data['repositories']:
            for repo_name in sorted(data['repositories'].keys()):
                repo_data = data['repositories'][repo_name]
                repo = Repository(repo_name, repo_data.get('doc', None), repo_data.get('release', None), repo_data.get('source', None), repo_data)
                self.repositories[repo_name] = repo

                if repo.release_repository:
                    for pkg_name in repo.release_repository.package_names:
                        self._add_package(pkg_name, repo)

                if repo.doc_repository:
                    for dep in repo.doc_repository.depends:
                        assert dep in data['repositories'].keys(), "Doc repository '%s' depends on non-existing repository '%s'" % (repo_name, dep)

        self.release_platforms = {}
        if 'release_platforms' in data and data['release_platforms']:
            for os_name in data['release_platforms'].keys():
                self.release_platforms[os_name] = []
                for os_code_name in data['release_platforms'][os_name]:
                    assert os_code_name not in self.release_platforms[os_name], "Distribution '%s' specifies the os_code_name '%s' multiple times for the os_name '%s'" % (self.name, os_code_name, os_name)
                    self.release_platforms[os_name].append(os_code_name)

    def _add_package(self, pkg_name, repo):
        assert pkg_name not in self.release_packages, "Duplicate package name '%s' exists in repository '%s' as well as in repository '%s'" % (pkg_name, repo.name, self.release_packages[pkg_name].repository_name)
        self.release_packages[pkg_name] = Package(pkg_name, repo.name)

    def get_data(self):
        data = {}
        data['type'] = DistributionFile._type
        data['version'] = self.version
        data['repositories'] = {}
        for repo_name in sorted(self.repositories.keys()):
            repo = self.repositories[repo_name]
            data['repositories'][repo_name] = repo.get_data()
        data['release_platforms'] = self.release_platforms
        return data
