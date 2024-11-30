#include "../include/CardinalityEstimation.h"

#include "../include/common/Root.h"

void CEEngine::insertTuple(const std::vector<int> &tuple) {
    ColumnAStats->ProcessNewInput(tuple[0]);
    ColumnBStats->ProcessNewInput(tuple[1]);
    int k = 5;
    int divider = 10;
    // for every k elements, add to sample, to a max of num / divider elements
    if (this->ColumnASize % k == 0) {
        if (ColumnBSample.size() < this->num / divider) {
            this->ColumnASize++;
            if (ColumnBSample.find(tuple[1]) == ColumnBSample.end()){
                ColumnBSample[tuple[1]] = 1;
            }
            else {
                ColumnBSample[tuple[1]] += 1;
            }
        }
    }
    if (this->ColumnBSize % k == 0) {
        this->ColumnBSize++;
        if (ColumnASample.size() < this->num / divider) {
            if (ColumnASample.find(tuple[0]) == ColumnASample.end()){
                ColumnASample[tuple[0]] = 1;
            }
            else {
                ColumnASample[tuple[0]] += 1;
            }
        }
    }
}

void CEEngine::deleteTuple(const std::vector<int> &tuple, int tupleId) {
    ColumnAStats->ProcessDelete(tuple[0]);
    ColumnBStats->ProcessDelete(tuple[1]);

    if (ColumnASample.find(tuple[0]) != ColumnASample.end()){
        this->ColumnASize--;
        ColumnASample[tuple[0]] -= 1;
        if (ColumnASample[tuple[0]] == 0) {
            ColumnASample.erase(tuple[0]);
        }
    }

    if (ColumnBSample.find(tuple[1]) != ColumnBSample.end()){
        this->ColumnBSize--;
        ColumnBSample[tuple[1]] -= 1;
        if (ColumnBSample[tuple[1]] == 0) {
            ColumnBSample.erase(tuple[1]);
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
                else if (expression.compareOp == CompareOp::EQUAL && this->ColumnASample.count(expression.value) > 0) {
                    matches *= ColumnASample[expression.value] / this->ColumnASize;
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
                else if (expression.compareOp == CompareOp::EQUAL && this->ColumnBSample.count(expression.value) > 0) {
                    matches *= ColumnBSample[expression.value] / this->ColumnBSize;
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
    this->ColumnASize = 0;
    this->ColumnBSize = 0;
    this->ColumnAStats = new ColumnStats();
    this->ColumnBStats = new ColumnStats();
}
