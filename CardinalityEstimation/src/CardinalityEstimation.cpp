//
// You should modify this file.
//
#include <CardinalityEstimation.h>
#include <common/Root.h>

void CEEngine::insertTuple(const std::vector<int> &tuple) {
    // Implement your insert tuple logic here.
    storage.push_back(tuple);
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
}
