#ifndef CARDINALITYESTIMATION_ENGINE
#define CARDINALITYESTIMATION_ENGINE
//
// You should modify this file.
//
#include "ColumnStats.h"
#include "FenwickTree.h"
#include "common/Expression.h"
#include "common/Root.h"
#include "executer/DataExecuter.h"

class CEEngine {
   public:
    /**
     * Insert a tuple, indicating that the tuple is inserted and appended to the end of the disk.
     * @param tuple Inserted tuple.
     */
    void insertTuple(const std::vector<int> &tuple);
    /**
     * Deletion function. Pass a tuple and tupleId, indicating that the tuple at the tupleId position is deleted.
     * @param tuple Deleted tuple.
     * @param tupleId Location of the deleted tuple.
     */
    void deleteTuple(const std::vector<int> &tuple, int tupleId);
    /**
     * Query function, pass in expression, return estimated cardinality result.
     * @param quals expression.
     * @return return estimated cardinality result.
     */
    int query(const std::vector<CompareExpression> &quals);
    /**
     * Preprocessing function of the cardinality estimation algorithm. This function is executed before each operation
     * is called.
     */
    void prepare();
    /**
     * The constructor function of cardinality estimation.
     * @param num Size of the initial data set.
     * @param dataExecuter Interfaces for datasets.
     */
    CEEngine(int num, DataExecuter *dataExecuter);
    ~CEEngine() = default;

   private:
    DataExecuter *dataExecuter;

    static const int MAX_VALUE = 20000000;  // Maximum value range for A and B
    static const int BUCKET_SIZE = 1000;    // Size of each bucket in the histogram

    FenwickTree fenwickA;
    FenwickTree fenwickB;

    std::vector<int> frequencyA;  // Cumulative frequencies for column A
    std::vector<int> frequencyB;  // Cumulative frequencies for column B
    int totalRows;

    void updateHistogram(std::vector<int> &histogram, std::vector<int> &cumulative, int value, int delta) {
        if (value < 0 || value > MAX_VALUE) {
            std::cerr << "Value out of bounds: " << value << std::endl;
            return;  // Ignore invalid values
        }
        int bucketIdx = value / BUCKET_SIZE;
        histogram[bucketIdx] += delta;
        if (histogram[bucketIdx] < 0) histogram[bucketIdx] = 0;  // Prevent negative frequencies

        // Update cumulative frequencies safely
        std::partial_sum(histogram.begin(), histogram.end(), cumulative.begin());
    }

    int estimateCardinality(const std::vector<int> &histogram, const std::vector<int> &cumulative,
                            CompareExpression expr) {
        if (expr.value < 0 || expr.value > MAX_VALUE) {
            std::cerr << "Query value out of bounds: " << expr.value << std::endl;
            return 0;  // Invalid query
        }
        int bucketIdx = expr.value / BUCKET_SIZE;
        if (expr.compareOp == EQUAL) {
            return histogram[bucketIdx];
        } else if (expr.compareOp == GREATER) {
            return cumulative.back() - cumulative[bucketIdx];
        }
        return 0;
    }

    int valueToIndex(int value) const {
        if (value < 0 || value > MAX_VALUE) {
            return -1;  // Return an invalid index to handle gracefully
        }
        return value / BUCKET_SIZE + 1;
    }
};

#endif