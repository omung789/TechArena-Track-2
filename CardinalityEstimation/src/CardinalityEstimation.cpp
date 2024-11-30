#include "../include/CardinalityEstimation.h"

#include "../include/common/Root.h"

void CEEngine::insertTuple(const std::vector<int> &tuple) {
    if (tuple.size() != 2) {
        std::cerr << "Invalid tuple size: " << tuple.size() << std::endl;
        return;
    }
    hllA.add(tuple[0]);  // Add value to HLL for column A
    hllB.add(tuple[1]);  // Add value to HLL for column B
}

void CEEngine::deleteTuple(const std::vector<int> &tuple, int tupleId) {
    return;
}

int CEEngine::query(const std::vector<CompareExpression> &quals) {
    double estimateA = hllA.estimate();
    double estimateB = hllB.estimate();

    // Handle multi-column queries (assume conjunctive conditions)
    if (quals.size() == 2) {
        // Use inclusion-exclusion principle for intersection estimation
        double intersectionEstimate = estimateA * estimateB / std::max(estimateA + estimateB, 1.0);
        return static_cast<int>(intersectionEstimate);
    }

    // Single-column queries
    return static_cast<int>(quals[0].columnIdx == 0 ? estimateA : estimateB);
}

void CEEngine::prepare() {
    return;
}

CEEngine::CEEngine(int num, DataExecuter *dataExecuter) : hllA(14), hllB(14) {
    this->dataExecuter = dataExecuter;
}
