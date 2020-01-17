# How to contribute

This codebase is not huge, but still it has a size for which a uniform style and way of behaving by all the authors is crucial to contribute keeping it clean and ordered.

* The code is formatted using `clang-format` which is part of LLVM and [it can be downloaded](http://releases.llvm.org/) as part if it.
  In the repository, you will find a `_clang-format` file, which is automatically used by clang-format, when using the `-style=file` option.
  It is possible to incorporate clang-format in many editors and IDE, but also tools like `git-clang-format` are available, which can be [simply](https://electronjs.org/docs/development/clang-format) used on modified code.
  In particular, git-clang-format comes for free downloading LLVM.
  **It is your responsibility to run clang-format on your code before committing!**
* You are highly encouraged to use the git-flow developement scheme and, to facilitate it, you can use the [git-flow extension](https://github.com/petervanderdoes/gitflow-avh) of git.
  If you do not want to use it, still **do not work for any reason on the master branch directly!**
* Strive to write [good commit messages](https://chris.beams.io/posts/git-commit/); other people working on the code will be grateful to you.
