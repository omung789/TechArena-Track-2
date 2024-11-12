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
    /* 
    prerequisite C and C++ knowledge: 
    - push_back() is a function that adds an element to the end of a vector
    - rand() is a function that generates a random integer
    - std::vector is a standard library container that stores elements in a dynamic array

    The generateInsert function in the DataExecuterDemo class is designed to generate a new tuple of random integers and insert it into a collection. 
    The function begins by creating an empty std::vector<int> named tuple. This vector will hold the random integers that make up the tuple.

    Next, the function uses the push_back method of the std::vector class to add two random integers to the tuple. 
    The rand() function generates these random integers. The push_back method appends each generated integer to the end of the tuple vector.

    After populating the tuple with two random integers, the function adds this tuple to another collection named set using the push_back method. 
    The set is presumably a member variable of the DataExecuterDemo class, likely a std::vector of vectors, which stores all the generated tuples.

    The function then increments the end variable by one. 
    The end variable is likely a member variable of the DataExecuterDemo class that keeps track of the number of tuples stored in the set.

    Finally, the function returns the newly created tuple. This allows the caller of the function to receive and potentially use the generated tuple of random integers.
    
    Generated by COPILOT Version 21.12.1 on 2022-01-26
    */

    std::vector<int> tuple;
    tuple.push_back(rand());
    tuple.push_back(rand());
    set.push_back(tuple);
    end++;
    return tuple;
}

int DataExecuterDemo::generateDelete()
{
    /*
    The generateDelete function in the DataExecuterDemo class is designed to generate a unique random index within a specified range and mark it as visited. 
    This function is useful in scenarios where you need to ensure that each generated index is unique and has not been used before.

    The function begins by generating a random integer x using the rand() function. This random integer is then taken modulo end to ensure that the value of x falls within the range [0, end-1]. 
    The end variable is presumably a member variable of the DataExecuterDemo class that defines the upper limit for the range of indices.

    The function then enters a while loop that checks if the index x has already been visited by examining the vis map at position x. 
    The vis map is a std::unordered_map<int, bool> that keeps track of which indices have already been generated.
    If vis[x] is true, it means that the index x has already been generated, and the function will generate a new random index x and check again. 
    This process repeats until an index x is found that has not been visited.

    Once an unused index x is found, the function marks this index as visited by setting vis[x] to true. This ensures that the index will not be generated again in future calls to generateDelete.

    Finally, the function returns the unique random index x. 
    This allows the caller of the function to receive a unique index that has not been previously generated, ensuring that each call to generateDelete returns a distinct index within the specified range.

    Generated by COPILOT Version 21.12.1 on 2022-01-26
    */
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
