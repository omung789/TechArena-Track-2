//
// You should modify this file.
//
#include <common/Root.h>
#include <CardinalityEstimation.h>

void CEEngine::insertTuple(const std::vector<int>& tuple)
{
    // Implement your insert tuple logic here.
    storage.push_back(tuple);

    // I think we definitely want to store by id, as we can use binary search to find the tuple when deleting or querying, which is O(log n) instead of O(n)
    // However that would mean we need to find the correct location to insert the tuple, which obviously increases the complexity of the insert operation from O(1) to O(log n)
    // CoPilot Suggests: "I think we can just store the tuple in a vector and use the index as the id, which is O(1) for insert, O(1) for delete, and O(1) for query"
    // - Dylan 12/11/24
}

void CEEngine::deleteTuple(const std::vector<int>& tuple, int tupleId)
{
    // Implement your delete tuple logic here.
    if (tupleId >= 0 && tupleId < storage.size() && storage[tupleId] == tuple) {
        storage.erase(storage.begin() + tupleId);
    }

    // Based on my suggestion in insertTuple, we can bin search for the tuple and delete it in O(log n) time, implement this in query and call it from delete???
    // - Dylan 12/11/24
}

int CEEngine::query(const std::vector<CompareExpression>& quals)
{
    // Implement your query logic here.
    int S1Counter = 0;
    int S2Counter = 0;
    for (CompareExpression expression : quals) {
        switch (expression.compareOp) {
            // Equal
            case 0:
                if (storage.at(expression.columnIdx).at(1) == expression.value) {
                    S2Counter += 1;
                }
                break;
                // Greater Than
            case 1:
                if (storage.at(expression.columnIdx).at(0) > expression.value) {
                    S1Counter += 1;
                }
                break;
            default:
                break;
        }
    }
    if (S1Counter == 1 && S2Counter == 1) {
        return 1;
    }
    return 0;
}

void CEEngine::prepare()
{
    // Implement your prepare logic here.
    // runs before every action, i.e. prepare -> insertTuple -> prepare -> deleteTuple -> ...
}

CEEngine::CEEngine(int num, DataExecuter *dataExecuter)
{
    // Implement your constructor here.
    this->dataExecuter = dataExecuter;
}
