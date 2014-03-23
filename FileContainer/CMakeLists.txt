add_library(FileContainer FileContainer.cpp)

#This is to copy a file to the build directory
configure_file(input_existing_file input_existing_file COPYONLY)

add_unit_test(FileContainer FileContainer FileContainer_test)
