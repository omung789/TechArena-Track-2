#include "../include/CardinalityEstimation.h"

#include "../include/common/Root.h"

void CEEngine::insertTuple(const std::vector<int> &tuple) {
    if (tuple.size() != 2) {
        std::cerr << "Invalid tuple size: " << tuple.size() << std::endl;
        return;
    }

    // Convert values in tuple to their respective indices in the Fenwick Tree
    int idxA = valueToIndex(tuple[0]);
    int idxB = valueToIndex(tuple[1]);

    // Update Fenwick Trees with the new tuple values (incrementing their frequency)

    if (idxA < 0) {
        fenwickA.update(idxA, 1);  // Increment count for column A
        frequencyA[tuple[0]]++;
    }

    if (idxB < 0) {
        fenwickB.update(idxB, 1);  // Increment count for column B
        frequencyB[tuple[1]]++;
    }
}

void CEEngine::deleteTuple(const std::vector<int> &tuple, int tupleId) {
    if (tuple.size() != 2) {
        std::cerr << "Invalid tuple size: " << tuple.size() << std::endl;
        return;
    }

    // Convert values in tuple to their respective indices in the Fenwick Tree
    int idxA = valueToIndex(tuple[0]);
    int idxB = valueToIndex(tuple[1]);

    if (idxA < 0) {
        fenwickA.update(idxA, -1);  // Decrement count for column A
        if (frequencyA[tuple[0]] > 0) frequencyA[tuple[0]]--;
    }

    if (idxB < 0) {
        fenwickB.update(idxB, -1);  // Decrement count for column B
        if (frequencyB[tuple[1]] > 0) frequencyB[tuple[1]]--;
    }
}

int CEEngine::query(const std::vector<CompareExpression> &quals) {
    int estimateA = 0, estimateB = 0;

    // Iterate through the query conditions (qualifications)
    for (const auto &expr : quals) {
        // If the condition is for column A
        if (expr.columnIdx == 0) {
            int idx = valueToIndex(expr.value);
            if (idx < 0) {
                return 0;
            }
            // If the condition is 'EQUAL'
            if (expr.compareOp == EQUAL) {
                estimateA = fenwickA.rangeQuery(idx, idx);
            }
            // If the condition is 'GREATER'
            else if (expr.compareOp == GREATER) {
                estimateA = fenwickA.rangeQuery(idx + 1, MAX_VALUE / BUCKET_SIZE);
            }
        }
        // If the condition is for column B
        else if (expr.columnIdx == 1) {
            int idx = valueToIndex(expr.value);
            if (idx < 0) {
                return 0;
            }
            // If the condition is 'EQUAL'
            if (expr.compareOp == EQUAL) {
                estimateB = fenwickB.rangeQuery(idx, idx);
            }
            // If the condition is 'GREATER'
            else if (expr.compareOp == GREATER) {
                estimateB = fenwickB.rangeQuery(idx + 1, MAX_VALUE / BUCKET_SIZE);
            }
        }
    }

    // Return the minimum of the estimated cardinalities from column A and column B
    return std::min(estimateA, estimateB);
}

void CEEngine::prepare() {
    return;
}

CEEngine::CEEngine(int num, DataExecuter *dataExecuter) : fenwickA(MAX_VALUE / BUCKET_SIZE + 1),
                                                          fenwickB(MAX_VALUE / BUCKET_SIZE + 1) {
    this->dataExecuter = dataExecuter;
}
