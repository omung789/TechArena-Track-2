#include <../include/CardinalityEstimation.h>
#include <../include/common/Root.h>

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
    // print quals
    std::cout << "Query parameters:" << std::endl;
    for (const auto &expression : quals) {
        std::cout << "Column Index: " << expression.columnIdx
                  << ", Value: " << expression.value
                  << ", Compare Operation: " << expression.compareOp << std::endl;
    }

    // Implement your query logic here.
    int matches = 1;

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
                printf("Fuck");
                fflush(stdout);
                break;
        }
        if (matches == 0) {
            return 0;
        }
    }

    return int(matches * ColumnAStats->getRecords());
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
