## A library of analysis tools

Data analysis is a mandatory step in LQCD and to have an efficient, well tested library of tools is very important.
Between 2014 and 2016, a quite basic but complete set of standard tools was developed in C++, in order of achieve good performance.
Basic analysis of typical Monte Carlo data has been implementing using *"Markov Chain Monte Carlo Simulations and Their Statistical Analysis"* book by B.A.Berg and *"Monte Carlo Methods in Statistical Physics"* book by G.T.Barkema and M.E.J.Newman as reference.
The main available features are the following.

*  The calculation of mean, variance, skewness and kurtosis of a given set of correlated data, with a correct estimate of the correspondent statistical error using either the Jackknife or the bootstrap method.
*  The multiple histogram method, also known as Ferrenberg-Swendsen reweighting, which has been implemented in a very general fashion.

A couple of executables have been, then, written for the user, in order to comfortably use the tools offered.
Clearly, you could use the code as external library for another project, but this might not be necessary.

### Getting started

Unfortunately, at the moment, no documentation is available and you need to explore the repository to understand what has been done how.
However, as far as building the code is concerned, the procedure is quite standard via CMake.

*  After having cloned the repository, create a build folder in it and move into it.
*  Here, you can simply run `cmake ..` in order to run a default setup or you can use the `ccmake ..` command to have access to configuration options.
*  If no error occurs (in case have a look to the error and act accordingly), you can run `make`
*  At this stage there should be no compilation error and a couple of executable should be created (also tests executable are built, so that you can run the code base tests using ctest).

Use

*  `dataAnalysis` to calculate moments of observables given some data files;
*  `lqcdReweighting` to reweight Monte Carlo observables and/or their moments.

Every executable accepts command line options and you can get some documentation running the executables with the `-h` or `--help` option.