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
        switch (expression.columnIdx) {
            case ColumnIdx::COLUMN_A:
                matches += ColumnAStats->handleQuery(expression.value, expression.compareOp);
                break;
            case ColumnIdx::COLUMN_B:
                matches += ColumnBStats->handleQuery(expression.value, expression.compareOp);
                break;
            default:
                // if it gets here were cooked ngl
                // means theyre querying another column that isnt a or b.
                break;
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
