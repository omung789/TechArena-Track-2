//
// You should modify this file.
//
#include <common/Root.h>
#include <CardinalityEstimation.h>

void CEEngine::insertTuple(const std::vector<int>& tuple)
{
    // Implement your insert tuple logic here.
    storage.push_back(tuple);
}

void CEEngine::deleteTuple(const std::vector<int>& tuple, int tupleId)
{
    // Implement your delete tuple logic here.
    if (tupleId >= 0 && tupleId < storage.size() && storage[tupleId] == tuple) {
        storage.erase(storage.begin() + tupleId);
    }
}

int CEEngine::query(const std::vector<CompareExpression>& quals)
{
    // Implement your query logic here.
    int S1Counter = 0;
    int S2Counter = 0;
    int idx = 0;
    for (CompareExpression expression : quals) {
        switch (expression.compareOp) {
            case EQUAL:
              while (S2Counter == 0 && S2Counter < storage.size()) {
                if (storage.at(expression.columnIdx).at(idx) == expression.value) 
                {
                    S2Counter += 1;
                    break;
                }
                idx++;
              }
            
            case GREATER:
              while (S2Counter == 0 && S2Counter < storage.size()){
                if (storage.at(expression.columnIdx).at(idx) > expression.value) {
                    S1Counter += 1;
                    break;
                }
                idx++;
              }
            default:
                break;
        }
    }
    return (S1Counter && S2Counter);
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
