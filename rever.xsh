######################################################################
#  This file is part of exact-real.
#
#        Copyright (C) 2020 Julian Rüth
#
#  pyeantic is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  pyeantic is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with pyeantic. If not, see <https://www.gnu.org/licenses/>.
#####################################################################

import sys

try:
  input("Are you sure you are on the master branch which is identical to origin/master and the only pending changes are a version bump in the configure.ac of the library? [ENTER]")
except KeyboardInterrupt:
  sys.exit(1)

sys.path.insert(0, 'recipe/snippets/rever')

import dist

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
    ('recipe/meta.yaml', r"\{% set version =", r"{% set version = '$VERSION' %}"),
    ('recipe/meta.yaml', r"\{% set build_number =", r"{% set build_number = '0' %}"),
]

print("Make sure you bump the version_info in libexactreal/configure.ac manually!")

$CHANGELOG_FILENAME = 'ChangeLog'
$CHANGELOG_TEMPLATE = 'TEMPLATE.rst'
$PUSH_TAG_REMOTE = 'git@github.com:flatsurf/exact-real.git'

$GITHUB_ORG = 'flatsurf'
$GITHUB_REPO = 'exact-real'

$GHRELEASE_ASSETS = ['exact-real-' + $VERSION + '.tar.gz']
