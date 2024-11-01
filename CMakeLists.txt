cmake_minimum_required(VERSION 3.23)
project(agorize_playground)

set(CMAKE_CXX_STANDARD 14)

include_directories(CardinalityEstimation/include)
include_directories(CardinalityEstimation/include/common)
include_directories(CardinalityEstimation/include/executer)

add_executable(agorize_playground
        CardinalityEstimation/include/common/Expression.h
        CardinalityEstimation/include/common/Root.h
        CardinalityEstimation/include/executer/DataExecuter.h
        CardinalityEstimation/include/executer/DataExecuterDemo.h
        CardinalityEstimation/include/CardinalityEstimation.h
        CardinalityEstimation/src/CardinalityEstimation.cpp
        CardinalityEstimation/src/DataExecuterDemo.cpp
        CardinalityEstimation/src/main.cpp)
