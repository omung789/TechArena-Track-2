#include "../include/CardinalityEstimation.h"

#include "../include/common/Root.h"

void CEEngine::insertTuple(const std::vector<int> &tuple) {
    if (tuple.size() != 2) {
        std::cerr << "Invalid tuple size: " << tuple.size() << std::endl;
        return;
    }
    updateHistogram(histogramA, cumulativeA, tuple[0], 1);
    updateHistogram(histogramB, cumulativeB, tuple[1], 1);
    totalRows++;
}

void CEEngine::deleteTuple(const std::vector<int> &tuple, int tupleId) {
    if (tuple.size() != 2) {
        std::cerr << "Invalid tuple size: " << tuple.size() << std::endl;
        return;
    }
    updateHistogram(histogramA, cumulativeA, tuple[0], -1);
    updateHistogram(histogramB, cumulativeB, tuple[1], -1);
    totalRows--;
}

int CEEngine::query(const std::vector<CompareExpression> &quals) {
    int estimateA = totalRows, estimateB = totalRows;

    for (const auto &expr : quals) {
        if (expr.columnIdx == 0) {
            estimateA = estimateCardinality(histogramA, cumulativeA, expr);
        } else if (expr.columnIdx == 1) {
            estimateB = estimateCardinality(histogramB, cumulativeB, expr);
        }
    }

    return std::min(estimateA, estimateB);
}

void CEEngine::prepare() {
    // Precompute cumulative frequencies
    std::partial_sum(histogramA.begin(), histogramA.end(), cumulativeA.begin());
    std::partial_sum(histogramB.begin(), histogramB.end(), cumulativeB.begin());
}

CEEngine::CEEngine(int num, DataExecuter *dataExecuter) {
    this->dataExecuter = dataExecuter;

    histogramA.resize(MAX_VALUE / BUCKET_SIZE + 1, 0);
    histogramB.resize(MAX_VALUE / BUCKET_SIZE + 1, 0);
    cumulativeA.resize(MAX_VALUE / BUCKET_SIZE + 1, 0);
    cumulativeB.resize(MAX_VALUE / BUCKET_SIZE + 1, 0);
}
