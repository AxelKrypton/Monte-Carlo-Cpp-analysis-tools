# Changelog

All notable changes to this project from the first release will be documented in this file.
They are listed in a reversal chronological order.

Although this project does not strictly adhere to [Semantic Versioning](https://semver.org/spec/v2.0.0.html), it follows some aspects inspired from it.
Generally, given a version number **X.Y**,

* **Y** is incremented for substantial refactoring and/or the introduction of minor new functionality and
* **X** is incremented for the introduction of substantial new features.

Version **1.0** will be released as the code is in a satisfactory shape also from the programming point of view.
This conversely means that some piece of code have (lots of) room for improvement and some needed refactoring have been already collected in [issues](https://github.com/AxelKrypton/Monte-Carlo-Cpp-analysis-tools/issues).

## Meaning of the symbols in this file

* :new:              &nbsp; New feature
* :white_check_mark: &nbsp; Enhancement
* :recycle:          &nbsp; Substantial refactoring
* :boom:             &nbsp; Breaking change
* :sos:              &nbsp; Bug fix
* :no_entry_sign:    &nbsp; Deprecated
* :x:                &nbsp; Removed feature

#### Note about breaking changes

Staying backward compatibile should be a pillar of any healthy programming environment in the sense that everybody should strive to minimize breaking changes.
However, it is impossible to avoid those, especially in an earlier phase of development, when many decisions are impossible to be taken in a fully aware way.
Although it will be often be the case, not every breaking change will result in a bumping of the main versioning number **X**.

---

## [Unreleased]


## [Version 0.3] &ensp;<sub><sup>13 June 2024</sup></sub>

* :new: The reweighting of the mean of observables can now be done applying a linear correction to it. This is needed e.g. to reweight the chiral condensate to LO.

## [Version 0.2] &ensp;<sub><sup>15 September 2021</sup></sub>

* :white_check_mark: No multiple estimates per observable need to be handled _during_ and _after_ the reweighting. Lots of simplification in the code.
* :new: Use of multiple estimate per observable is now supported and correctly implemented in the full codebase.
* :boom: Header name of quantities has been changed and uniformly written in capital letters.
* :recycle: Calculation of quantities is now uniform in the codebase and functionality is shared between reweighting and analysis.
* :white_check_mark: Set `C++17` as requirement to take advantage of its functionalities.

## [Version 0.1] &ensp;<sub><sup>12 February 2020</sup></sub>

First release of the codebase.


[Unreleased]:  https://github.com/AxelKrypton/Monte-Carlo-Cpp-analysis-tools/compare/v0.3...develop
[Version 0.3]: https://github.com/AxelKrypton/Monte-Carlo-Cpp-analysis-tools/releases/tag/v0.3
[Version 0.2]: https://github.com/AxelKrypton/Monte-Carlo-Cpp-analysis-tools/releases/tag/v0.2
[Version 0.1]: https://github.com/AxelKrypton/Monte-Carlo-Cpp-analysis-tools/releases/tag/v0.1
