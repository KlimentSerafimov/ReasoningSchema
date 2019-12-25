cmake_minimum_required(VERSION 3.13)
project(CompactPoset)

set(CMAKE_CXX_STANDARD 14)

add_executable(CompactPoset main.cpp CompactPoset.cpp CompactPoset.h Node.cpp Node.h MetaExample.cpp MetaExample.h util.h PartialFunction.cpp PartialFunction.h util.cpp DecisionTree.cpp DecisionTree.h LanguagesOverBooleanFunctions.cpp LanguagesOverBooleanFunctions.h ReasoningSchema.cpp ReasoningSchema.h MinimalFactoringSchema.cpp MinimalFactoringSchema.h Module.cpp Module.h UnionOfPartialFunctions.cpp UnionOfPartialFunctions.h)