#ifndef DATAFILEANALYZER_HPP_
#define DATAFILEANALYZER_HPP_

#include "DataSampleAnalyzer.hpp"

class DatafileAnalyzer
{
public:
	DatafileAnalyzer(Parameters parameters)
	{
		//TODO: add parameters offset and column
		DataSample data(parameters.file);
		DataSampleAnalyzer analyzer(data, parameters);
	}
};

#endif /* DATAFILEANALYZER_HPP_ */
