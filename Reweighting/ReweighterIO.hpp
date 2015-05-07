#ifndef REWEIGHTERIO_HPP_
#define REWEIGHTERIO_HPP_

#include "SimulationDataContainer.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"

class Binsizes{
public:
	Binsizes(): binsizes(std::map<int, int>()) {};
	int& operator[](const unsigned int& whichMoment){ return binsizes[whichMoment]; };
	int at(const unsigned int& whichMoment){ try{ return binsizes.at(whichMoment); }
											 catch(std::out_of_range& exceptionThrown){ return getDefaultValue(); } };
	std::vector<int> at(const std::initializer_list<unsigned int>& whichMoments){ std::vector<int> result;
																				  for(auto i : whichMoments) result.push_back(this->at(i));
																				  return result; };
	bool empty(){ return binsizes.empty(); };
	int getDefaultValue(){ try{ binsizes.at(-1);}
						   catch(std::out_of_range& exceptionThrown){
							   throw std::invalid_argument("Default value UNSET for Binsizes object! Hint: if performing Reweighting, is the configuration file correct?");
						   }
						   return binsizes[-1]; };
	void setDefaultValue(const int& valueIn){ binsizes[-1] = valueIn; };
	void print(){for(auto elem : binsizes) std::cout << "Binsize[" << elem.first << "] = " << elem.second << "\n"; };
	//TODO: Implement operator == between Binsizes object and remove this method that has been created only for testing purposes!
	std::map<int, int> getMap(){ return binsizes; };
private:
	std::map<int, int> binsizes;
};


class ReweighterIO {
	friend class Reweighter;
	friend class ReweighterIOTester;
public:
	ReweighterIO() = delete;
	/*
	 * ATTENTION: The ctor MUST take an argument of type ReweightingParameters not the particular
	 *            case LqcdReweightingParameters. It is temporary in order to make everything work!
	 */
	ReweighterIO(LqcdReweightingParameters parameters);
    //Output to file
	//void writeNewConfigurationFileWithMetaparameters(MomentsReweighter reweighter, std::string newConfigFileName = "");
    //void writeNewPointsToFileWithLogZ(MomentsReweighter reweighter, std::string outputFileName = "logZAtNewPoints");
private:
	SimulationDataContainer readFromFileDataContainer;
	bool isMeanKnownToBeZero;
	std::vector<unsigned int> columnsToBeReweightedUsingMultipleColumns;
	std::vector<std::string> namesOfParametersIgnoringMetaParameters; //just vector of string because the simulation parameters are the same for ALL data files!
	std::vector<std::vector<double> > valuesOfSimulationParametersIgnoringMetaParameters;
	//Specific member for each meta parameter
	std::vector<Binsizes> valuesOfSpecifiedBinsizes;
	std::vector<double> valuesOfSpecifiedLogZ;
	ErrorCalculationMethod errorMethod;
	std::shared_ptr<int> bootstrapNumber;
};


#endif /* REWEIGHTERIO_HPP_ */
