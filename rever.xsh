######################################################################
#  This file is part of exact-real.
#
#        Copyright (C) 2020-2025 Julian Rüth
#
#  exact-real is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  exact-real is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with exact-real. If not, see <https://www.gnu.org/licenses/>.
#####################################################################

import sys

try:
  input("Are you sure you are on the master branch which is identical to origin/master and the only pending changes are a version bump in the configure.ac of the library? [ENTER]")
except KeyboardInterrupt:
  sys.exit(1)

@activity
def dist():
    r"""
    Run make dist and collect the resulting tarball.
    """
    from tempfile import TemporaryDirectory
    from xonsh.dirstack import DIRSTACK
    with TemporaryDirectory() as tmp:
        ./bootstrap
        pushd @(tmp)
        @(DIRSTACK[-1])/configure
        make dist
        mv *.tar.gz @(DIRSTACK[-1])
        popd
    return True

$PROJECT = 'exact-real'

$ACTIVITIES = [
    'version_bump',
    'changelog',
    'dist',
    'tag',
    'push_tag',
    'ghrelease',
]

$VERSION_BUMP_PATTERNS = [
    ('configure.ac', r'AC_INIT', r'AC_INIT([exact-real], [$VERSION], [julian.rueth@fsfe.org])'),
    ('libexactreal/configure.ac', r'AC_INIT', r'AC_INIT([libexactreal], [$VERSION], [julian.rueth@fsfe.org])'),
    ('pyexactreal/configure.ac', r'AC_INIT', r'AC_INIT([pyexactreal], [$VERSION], [julian.rueth@fsfe.org])'),
    ('pyexactreal/src/setup.py', r'version=', r"version='$VERSION'"),
]

$CHANGELOG_FILENAME = 'ChangeLog'
$CHANGELOG_TEMPLATE = 'TEMPLATE.rst'
$CHANGELOG_NEWS = 'doc/news'
$CHANGELOG_CATEGORIES = ('Added', 'Changed', 'Deprecated', 'Removed', 'Fixed', 'Performance')
$PUSH_TAG_REMOTE = 'git@github.com:flatsurf/exact-real.git'

$GITHUB_ORG = 'flatsurf'
$GITHUB_REPO = 'exact-real'

$GHRELEASE_ASSETS = ['exact-real-' + $VERSION + '.tar.gz']
