About exact-real
================

Home: https://github.com/polygon-tbd/exact-real

Package license: GPL2

Feedstock license: BSD 3-Clause

Summary: Finite Real Submodules powered by E-ANTIC



Current build status
====================


<table>
    
  <tr>
    <td>Azure</td>
    <td>
      <details>
        <summary>
          <a href="https://dev.azure.com/conda-forge/feedstock-builds/_build/latest?definitionId=&branchName=master">
            <img src="https://dev.azure.com/conda-forge/feedstock-builds/_apis/build/status/exact-real-feedstock?branchName=master">
          </a>
        </summary>
        <table>
          <thead><tr><th>Variant</th><th>Status</th></tr></thead>
          <tbody><tr>
              <td>linux</td>
              <td>
                <a href="https://dev.azure.com/conda-forge/feedstock-builds/_build/latest?definitionId=&branchName=master">
                  <img src="https://dev.azure.com/conda-forge/feedstock-builds/_apis/build/status/exact-real-feedstock?branchName=master&jobName=linux&configuration=linux_" alt="variant">
                </a>
              </td>
            </tr><tr>
              <td>osx</td>
              <td>
                <a href="https://dev.azure.com/conda-forge/feedstock-builds/_build/latest?definitionId=&branchName=master">
                  <img src="https://dev.azure.com/conda-forge/feedstock-builds/_apis/build/status/exact-real-feedstock?branchName=master&jobName=osx&configuration=osx_" alt="variant">
                </a>
              </td>
            </tr>
          </tbody>
        </table>
      </details>
    </td>
  </tr>
  <tr>
    <td>Windows</td>
    <td>
      <img src="https://img.shields.io/badge/Windows-disabled-lightgrey.svg" alt="Windows disabled">
    </td>
  </tr>
</table>

Current release info
====================

| Name | Downloads | Version | Platforms |
| --- | --- | --- | --- |
| [![Conda Recipe](https://img.shields.io/badge/recipe-libexactreal-green.svg)](https://anaconda.org/saraedum/libexactreal) | [![Conda Downloads](https://img.shields.io/conda/dn/saraedum/libexactreal.svg)](https://anaconda.org/saraedum/libexactreal) | [![Conda Version](https://img.shields.io/conda/vn/saraedum/libexactreal.svg)](https://anaconda.org/saraedum/libexactreal) | [![Conda Platforms](https://img.shields.io/conda/pn/saraedum/libexactreal.svg)](https://anaconda.org/saraedum/libexactreal) |

Installing exact-real
=====================

Installing `exact-real` from the `saraedum` channel can be achieved by adding `saraedum` to your channels with:

```
conda config --add channels saraedum
```

Once the `saraedum` channel has been enabled, `libexactreal` can be installed with:

```
conda install libexactreal
```

It is possible to list all of the versions of `libexactreal` available on your platform with:

```
conda search libexactreal --channel saraedum
```




Updating exact-real-feedstock
=============================

If you would like to improve the exact-real recipe or build a new
package version, please fork this repository and submit a PR. Upon submission,
your changes will be run on the appropriate platforms to give the reviewer an
opportunity to confirm that the changes result in a successful build. Once
merged, the recipe will be re-built and uploaded automatically to the
`saraedum` channel, whereupon the built conda packages will be available for
everybody to install and use from the `saraedum` channel.
Note that all branches in the conda-forge/exact-real-feedstock are
immediately built and any created packages are uploaded, so PRs should be based
on branches in forks and branches in the main repository should only be used to
build distinct package versions.

In order to produce a uniquely identifiable distribution:
 * If the version of a package **is not** being increased, please add or increase
   the [``build/number``](https://conda.io/docs/user-guide/tasks/build-packages/define-metadata.html#build-number-and-string).
 * If the version of a package **is** being increased, please remember to return
   the [``build/number``](https://conda.io/docs/user-guide/tasks/build-packages/define-metadata.html#build-number-and-string)
   back to 0.

Feedstock Maintainers
=====================

* [@saraedum](https://github.com/saraedum/)
* [@videlec](https://github.com/videlec/)

