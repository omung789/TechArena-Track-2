//
// Demo data generator for local debugging. You can implement your own data generator for debugging based on this class.
//

#include <executer/DataExecuterDemo.h>

std::unordered_map<int, bool> vis;
DataExecuterDemo::DataExecuterDemo(int end, int count) : DataExecuter()
{
    /*
    prerequisite C and C++ knowledge:
    - std::vector is a standard library container that stores elements in a dynamic array
    - rand() is a function that generates a random integer
    - std::unordered_map is a standard library container that stores key-value pairs in a hash table
    
    The provided code snippet defines the constructor for the DataExecuterDemo class, which is designed to initialize an instance of the class with a specified number of random tuples. 
    The constructor takes two parameters: end and count. 
    The end parameter likely represents the upper limit for the range of indices or the number of tuples to generate, while count might represent the number of elements or some other relevant count for the class.

    The constructor begins by initializing the member variables end and count with the values passed as arguments. 
    This is done using the this pointer to distinguish between the member variables and the parameters with the same names.

    Next, the constructor enters a for loop that iterates from 0 to end (inclusive). In each iteration, it creates an empty std::vector<int> named tuple. 
    This vector is intended to hold a pair of random integers.

    Within the loop, the rand() function is called twice to generate two random integers. These integers are added to the tuple vector using the push_back method, which appends each integer to the end of the vector.

    After populating the tuple with two random integers, the constructor adds this tuple to a collection named set using the push_back method. 
    The set is presumably a member variable of the DataExecuterDemo class, likely a std::vector of vectors, which stores all the generated tuples.

    Additionally, there is a global std::unordered_map<int, bool> named vis declared outside the class. 
    This map is likely used elsewhere in the class to keep track of visited indices or some other state related to the generated tuples.

    Generated by COPILOT Version 21.12.1 on 2022-01-26
    */

    this->end = end;
    this->count = count;
    for (int i = 0; i <= end; ++i) {
        std::vector<int> tuple;
        tuple.push_back(rand());
        tuple.push_back(rand());
        set.push_back(tuple);
    }
}

std::vector<int> DataExecuterDemo::generateInsert()
{
    std::vector<int> tuple;
    tuple.push_back(rand());
    tuple.push_back(rand());
    set.push_back(tuple);
    end++;
    return tuple;
}

int DataExecuterDemo::generateDelete()
{
    int x = (rand()) % end;
    while (vis[x]) {
        x = (rand()) % end;
    }
    vis[x] = true;
    return x;
}

void DataExecuterDemo::readTuples(int start, int offset, std::vector<std::vector<int>> &vec)
{
    for (int i = start; i < start + offset; ++i) {
        if (!vis[i]) {
            vec.push_back(set[i]);
        }
    }
    return;
};

Action DataExecuterDemo::getNextAction()
{
    Action action;
    if (count == 0) {
        action.actionType = NONE;
        return action;
    }
    if (count % 100 == 99) {
        action.actionType = QUERY;
        CompareExpression expr = {rand() % 2, CompareOp(rand() % 2), rand()};
        action.quals.push_back(expr);
    } else if (count % 100 < 90) {
        action.actionType = INSERT;
        action.actionTuple = generateInsert();
    } else {
        action.actionType = DELETE;
        action.tupleId = generateDelete();
        action.actionTuple = set[action.tupleId];
    }
    count--;
    curAction = action;
    return action;
};

double DataExecuterDemo::answer(int ans)
{
    int cnt = 0;
    for (int i = 0; i <= end; ++i) {
        if (vis[i])
            continue;
        bool flag = true;
        for (int j = 0; j < curAction.quals.size(); ++j) {
            CompareExpression &expr = curAction.quals[j];
            if (expr.compareOp == GREATER && set[i][expr.columnIdx] <= expr.value) {
                flag = false;
                break;
            }
            if (expr.compareOp == EQUAL && set[i][expr.columnIdx] != expr.value) {
                flag = false;
                break;
            }
        }
        if (flag)
            cnt++;
    }
    double error = fabs(std::log((ans + 1) * 1.0 / (cnt + 1)));
    return error;
};
