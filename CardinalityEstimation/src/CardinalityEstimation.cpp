#include <include/CardinalityEstimation.h>
#include <include/common/Root.h>

void CEEngine::insertTuple(const std::vector<int> &tuple) {
    // Implement your insert tuple logic here.
    storage.push_back(tuple);

    ColumnAStats->ProcessNewInput(tuple[0]);
    ColumnBStats->ProcessNewInput(tuple[1]);
}

void CEEngine::deleteTuple(const std::vector<int> &tuple, int tupleId) {
    // Implement your delete tuple logic here.
    if (tupleId >= 0 && tupleId < storage.size() && storage[tupleId] == tuple) {
        storage.erase(storage.begin() + tupleId);
    }
}

int CEEngine::query(const std::vector<CompareExpression> &quals) {
    // Implement your query logic here.
    int matches = 0;

    for (CompareExpression expression : quals) {
        if (expression.columnIdx == 0) {
            // Querying Column A
            if (ColumnAStats->getRecords() == 0) {
                // there will be no matches to the query
                continue;
            }

            if (expression.value < ColumnAStats->getMin()) {
                // there will no matches to the query
                continue;
            }

            if (expression.value > ColumnAStats->getMax() && expression.compareOp != CompareOp::GREATER) {
                // there will no matches to the query
                continue;
            }
        } else {
            // Querying Column B
            if (ColumnBStats->getRecords() == 0) {
                // there will be no matches to the query
                continue;
            }

            if (expression.value < ColumnBStats->getMin()) {
                // there will no matches to the query
                continue;
            }

            if (expression.value > ColumnBStats->getMax() && expression.compareOp != CompareOp::GREATER) {
                // there will no matches to the query
                continue;
            }
        }

        for (std::vector<int> tuple : storage) {
            switch (expression.compareOp) {
                case CompareOp::EQUAL:
                    if (tuple[expression.columnIdx] == expression.value) {
                        matches++;
                    }
                    break;
                case CompareOp::GREATER:
                    if (tuple[expression.columnIdx] > expression.value) {
                        matches++;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    return matches;
}

void CEEngine::prepare() {
    // Implement your prepare logic here.
    // runs before every action, i.e. prepare -> insertTuple -> prepare -> deleteTuple -> ...
}

CEEngine::CEEngine(int num, DataExecuter *dataExecuter) {
    // Implement your constructor here.
    this->dataExecuter = dataExecuter;
    this->ColumnAStats = new ColumnStats();
    this->ColumnBStats = new ColumnStats();
}
