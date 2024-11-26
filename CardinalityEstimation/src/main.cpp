#include <CardinalityEstimation.h>
#include <executer/DataExecuterDemo.h>

int main(int argc, char *argv[])
{
    int initSize = 100000; // Initial data size.
    int opSize = 10000; // Number of operations.
    double score = 0;
    int cnt = 0;
    
    DataExecuterDemo dataExecuter(initSize - 1, opSize);
    CEEngine ceEngine(initSize, &dataExecuter);
    Action action = dataExecuter.getNextAction();

    printf("Starting main loop\n");
    fflush(stdout);

    while (action.actionType != NONE) {
        ceEngine.prepare();
        if (action.actionType == INSERT) {
            printf("Inserting tuple\n");
            fflush(stdout);
            ceEngine.insertTuple(action.actionTuple);
        } else if (action.actionType == DELETE) {
            printf("Deleting tuple\n");
            fflush(stdout);
            ceEngine.deleteTuple(action.actionTuple, action.tupleId);
        } else if (action.actionType == QUERY) {
            printf("Querying\n");
            fflush(stdout);
            int ans = ceEngine.query(action.quals);
            score += dataExecuter.answer(ans);
            cnt++;
        }
        action = dataExecuter.getNextAction();
    }
    std::cout << score / cnt << std::endl;
}