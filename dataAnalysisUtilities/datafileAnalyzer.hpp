#ifndef DATAFILEANALYZER_HPP_
#define DATAFILEANALYZER_HPP_

#include "DataSampleAnalyzer.hpp"

class DatafileAnalyzer
{
public:
	DatafileAnalyzer(std::string filename, Parameters parameters)
	{
		//TODO: add parameters offset and column
		DataSample data(filename);
		DataSampleAnalyzer analyzer(data, parameters);
	}
};

#endif /* DATAFILEANALYZER_HPP_ */
