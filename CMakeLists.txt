cmake_minimum_required(VERSION 3.15)
project(CompactPoset)

set(CMAKE_CXX_STANDARD 17)

add_executable(CompactPoset main.cpp CompactPoset.cpp CompactPoset.h Node.cpp Node.h MetaExample.cpp MetaExample.h util.h PartialFunction.cpp PartialFunction.h util.cpp DecisionTree.cpp DecisionTree.h LanguagesOverBooleanFunctions.cpp LanguagesOverBooleanFunctions.h RemainderAndTwoFactorsSchema.cpp RemainderAndTwoFactorsSchema.h ReasoningSchemaOptimizer.cpp ReasoningSchemaOptimizer.h Module.cpp Module.h UnionOfPartialFunctions.cpp UnionOfPartialFunctions.h enumerate_meta_training_sets.cpp enumerate_meta_training_sets.h BitvectorTasks.cpp BitvectorTasks.h BitInBittree.cpp BitInBittree.h BittreeTaskType.cpp BittreeTaskType.h DeltaBittreeType.cpp DeltaBittreeType.h Task.cpp Task.h TraceVersionSpace.cpp TraceVersionSpace.h Bitvector.cpp Bitvector.h BittreeTypeExpression.cpp BittreeTypeExpression.h NodeTemplate.cpp NodeTemplate.h MetricType.h MetricType.cpp LabeledGraph.cpp LabeledGraph.h Mask.cpp Mask.h)
