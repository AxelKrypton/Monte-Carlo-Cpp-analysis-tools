#ifndef DATAFILEANALYZER_HPP_
#define DATAFILEANALYZER_HPP_

#include "DataSampleAnalyzer.hpp"

class DatafileAnalyzer
{
public:
	DatafileAnalyzer(Parameters parameters)
	{
		DataSample data(parameters.file, parameters.column, parameters.offset);
		DataSampleAnalyzer analyzer(data, parameters);
	}
};

#endif /* DATAFILEANALYZER_HPP_ */
