#include "../include/CardinalityEstimation.h"

#include "../include/common/Root.h"

void CEEngine::insertTuple(const std::vector<int> &tuple) {
    ColumnAStats->ProcessNewInput(tuple[0]);
    ColumnBStats->ProcessNewInput(tuple[1]);
}

void CEEngine::deleteTuple(const std::vector<int> &tuple, int tupleId) {
    ColumnAStats->ProcessDelete(tuple[0]);
    ColumnBStats->ProcessDelete(tuple[1]);
}

int CEEngine::query(const std::vector<CompareExpression> &quals) {
    double matches = 1;

    for (CompareExpression expression : quals) {
        switch (expression.columnIdx) {
            case ColumnIdx::COLUMN_A:
                matches *= ColumnAStats->HandleQuery(expression.value, expression.compareOp) / ColumnAStats->getRecords();
                break;
            case ColumnIdx::COLUMN_B:
                matches *= ColumnBStats->HandleQuery(expression.value, expression.compareOp) / ColumnBStats->getRecords();
                break;
            default:
                // if it gets here were cooked ngl
                // means theyre querying another column that isnt a or b.
                matches = 0;
                 break;
        }
        if (matches == 0) {
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

    this->ColumnAStats = new ColumnStats();
    this->ColumnBStats = new ColumnStats();
}
