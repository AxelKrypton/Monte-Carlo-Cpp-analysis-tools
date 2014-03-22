add_library(dataSample dataSample.cpp)

configure_file(datafile.example datafile.example COPYONLY)
configure_file(datafile2.example datafile2.example COPYONLY)
configure_file(datafileWithTwoColumns.example datafileWithTwoColumns.example)

add_unit_test(dataSample dataSample_test)
