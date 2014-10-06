#ifndef REWEIGHTINGDATAHANDLER_H_
#define REWEIGHTINGDATAHANDLER_H_

#include "SimulationDataContainer.hpp"

/*
 * The following object is supposed to deal with all the I/O operations and to prepare a
 * proper file to be used in the Reweighter class constructor. In particular, all the
 * checks on the input files (configuration file and data files) have to be done here.
 * Afterwards, using the meta-information given in the configuration file (some of which
 * has to be available to be passed to the Reweighter constructor), one has to do a proper
 * binning of the data. This works in the following way:
 *
 *  1 - In the configuration file the binsize that should be used for each file is provided
 *      (using "binsize" as parameter label).
 *  2 - Depending on the length of each file (i.e. number of trajectories), the corresponding
 *      number of bins is calculated.
 *  3 - The minimum number of bins among the files is found. This will be used to bin the data
 *      in all the files. Therefore, it will happen that in a lot of files the actual binsize used
 *      will be bigger than the provided one. The idea behind this is that a binsize almost equal
 *      to the double of the autocorrelation time should be given, in order to obtain after the
 *      binning procedure a set of uncorrelated data. If then one use a bigger binsize than that
 *      provided, the new data will be as well uncorrelated.
 *
 * NOTE: If on some lines of the configuration file the binsize parameter is not given, it is not
 *       a problem: those given on the other lines will be used. If no binsize is provided, an
 *       exception will be thrown!
 */

class ReweighterAbstract;
class Reweighter;

class ReweightingDataHandler {
    friend class ReweighterAbstract;
public:
    ReweightingDataHandler();
    ReweightingDataHandler(std::string configurationFileIn);
    //Getters
    std::vector<std::string> getNamesOfParametersIgnoringMetaParameters();
    std::vector<std::vector<double> > getValuesOfSimulationParametersIgnoringMetaParameters();
    void extractAndSetProvidedValuesOfLogZAtSimulatedPoints(std::vector<double>& logZ);
    //Output to file
    void writeNewConfigurationFileWithMetaparameters(Reweighter reweighter, std::string newConfigFileName = "");
    void writeNewPointsToFileWithLogZ(Reweighter reweighter, std::string outputFileName = "logZAtNewPoints");

protected:
    int numberOfBinsToBeUsed;

private:
    std::string configurationFile;
    SimulationDataContainer simulationDataContainer;
    int numberOfObservablesToBeReweighted;

    /*
     * These are metaparameters that must NOT be interpreted as reweighting parameters,
     * though they can be given in the configurationFile and then be stored in the
     * SimulationData parameters. Note that they are static in order to be accessible
     * from outside without an object.
     */
    static const std::vector<std::string> metaParameters;
};










#endif /* REWEIGHTINGDATAHANDLER_H_ */
