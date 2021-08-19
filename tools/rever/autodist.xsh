######################################################################
#  This file is part of flatsurf.
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
from rever.activity import Activity

class AutotoolsDist(Activity):
    def __init__(self, **kwargs):
        super().__init__()
        self.name = "autodist"
        self.desc = "Creates a source tarball with make dist"
        self.requires = {"commands": {"make": "make"}}

    def __call__(self):
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

$DAG['autodist'] = AutotoolsDist()
