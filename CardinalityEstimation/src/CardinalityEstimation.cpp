#include "../include/CardinalityEstimation.h"

#include "../include/common/Root.h"

void CEEngine::insertTuple(const std::vector<int> &tuple) {
    ColumnAStats->ProcessNewInput(tuple[0]);
    ColumnBStats->ProcessNewInput(tuple[1]);
    // for every 5 elements, add to sample, to a max of num / 1000 elements
    if (iterations % 5 == 0) {
        if (ColumnBSample.size() <this->num / 1000) {
            if (ColumnBSample.find(tuple[0]) == ColumnBSample.end()){
                ColumnBSample[tuple[0]] = 1;
            }
            else {
                ColumnBSample[tuple[0]] += 1;
            }
        }

        if (ColumnASample.size() <this->num / 1000) {
            if (ColumnASample.find(tuple[1]) == ColumnASample.end()){
                ColumnASample[tuple[1]] = 1;
            }
            else {
                ColumnASample[tuple[1]] += 1;
            }
        }
    }
    iterations++;
}

void CEEngine::deleteTuple(const std::vector<int> &tuple, int tupleId) {
    ColumnAStats->ProcessDelete(tuple[0]);
    ColumnBStats->ProcessDelete(tuple[1]);

    if (ColumnASample.find(tuple[1]) != ColumnASample.end()){
        ColumnASample[tuple[1]] -= 1;
        if (ColumnASample[tuple[1]] == 0) {
            ColumnASample.erase(tuple[1]);
        }
    }

    if (ColumnBSample.find(tuple[0]) != ColumnBSample.end()){
        ColumnBSample[tuple[0]] -= 1;
        if (ColumnBSample[tuple[0]] == 0) {
            ColumnBSample.erase(tuple[0]);
        }
    }

}

int CEEngine::query(const std::vector<CompareExpression> &quals) {
    double matches = 1;

    for (CompareExpression expression : quals) {
        switch (expression.columnIdx) {
            case ColumnIdx::COLUMN_A:
                if (ColumnAStats->getRecords() == 0) {
                    matches = 0;
                }
                // if the compare operation is equal and we have the value in our sample
                else if (expression.compareOp == CompareOp::EQUAL && this->ColumnASample.find(expression.value) != this->ColumnASample.end()) {
                    matches *= ColumnASample[expression.value] / ColumnASample.size();
                } 
                else {
                    matches *= ColumnAStats->HandleQuery(expression.value, expression.compareOp) / ColumnAStats->getRecords();
                }
                break;
            case ColumnIdx::COLUMN_B:
                if (ColumnBStats->getRecords() == 0) {
                    matches = 0;
                }
                // if the compare operation is equal and we have the value in our sample
                else if (expression.compareOp == CompareOp::EQUAL && this->ColumnBSample.find(expression.value) != this->ColumnBSample.end()) {
                    matches *= ColumnBSample[expression.value] / ColumnBSample.size();
                } 
                else {
                    matches *= ColumnBStats->HandleQuery(expression.value, expression.compareOp) / ColumnBStats->getRecords();
                }
                break;
            default:
                // if it gets here were cooked ngl
                // means theyre querying another column that isnt a or b.
                matches = 0;
                break;
        }
        if (matches == 0 || std::isnan(matches) || std::isinf(matches)) {
            return 0;
        }
    }

    return int(matches * ColumnAStats->getRecords());
}

void CEEngine::prepare() {
    return;
}

CEEngine::CEEngine(int num, DataExecuter *dataExecuter) {
    this->dataExecuter = dataExecuter;
    this->ColumnASample = std::unordered_map<int, int>();
    this->ColumnBSample = std::unordered_map<int, int>();
    this->num = num;
    this->iterations = 0;
    this->ColumnAStats = new ColumnStats();
    this->ColumnBStats = new ColumnStats();
}
