cmake_minimum_required(VERSION 3.0)
project(Hopsan)

message(WARNING "Hopsan CMake support is still experimental. Qt components and dependency lookup are not yet included")
add_subdirectory(HopsanCore)
add_subdirectory(componentLibraries)
add_subdirectory(HopsanCLI)
#add_subdirectory(HopsanGenerator)
add_subdirectory(Ops)
