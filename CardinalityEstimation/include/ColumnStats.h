#include <common/Expression.h>
#include <common/Root.h>

#include <climits>

#define MAX_VAL 20000000
#define BUCKET_WIDTH 100000
#define BUCKET_COUNT int(MAX_VAL / BUCKET_WIDTH) + 1

// TODO:
// - 1. Get it to compile
// - 2. Make sure imports work

class ColumnStats {
   public:
    /**
     * @brief Construct a new Column Stats object, without a list of data to create a column from. Will need to add data through ProcessNewInput().
     *
     */
    ColumnStats() {
        setMin(INT_MAX);
        setMax(INT_MIN);
        setRecords(0);
    };

    int HandleQuery(int target, CompareOp compareOp) {
        if (!InRange(target, compareOp)) {
            return 0;
        }

        if (compareOp == CompareOp::GREATER) {
            if (target <= this->getMin()) {
                return this->getRecords();
            } else {
                return this->GuessRowsGreaterThan(target);
            }
        }

        // determine probability that it is in it's bucket
        // number of rows in bucket / (bucket size) -> skewed data fucks it
        int bucketID = FindBucket(target);
        int rowsInBucket = GetBucketRows(bucketID);

        // gets lower bound of higher bucket and minuses lower bound of lower bucket
        // probability that a number in range [bucketMin, buacketMax] is in the bucket that is bound by [bucketMin, bucketMax]
        // i.e., the probability that number 5 is in bucket [0, 10]
        double p = rowsInBucket / BUCKET_WIDTH;

        return p > 0.5 ? 1 : 0;
    }

    int FindBucket(int target) {
        // return the bucket id of the target value's bucket
        if (target <= MAX_VAL) {
            return int(target / BUCKET_WIDTH);
        } else {
            return BUCKET_COUNT - 1;
        }
    }

    int GuessRowsGreaterThan(int target) {
        // get bucket data
        int BucketID = this->FindBucket(target);
        int bucketSize = this->GetBucketRows(BucketID);
        int bucketMin = BucketID * BUCKET_WIDTH;

        // caluclate how far 'into' the bucket the target is
        // will be more accurate the closer to a unifrom distribution the bucket is is
        int targetPercentage = (target - bucketMin) / BUCKET_WIDTH;

        // how many rows are less than or equal to target
        int rowsLessThanTarget = targetPercentage * bucketSize;

        // how many rows are in higher buckets
        int rowsInHigherBuckets = 0;
        for (int i = BucketID + 1; i < BUCKET_COUNT; i++) {
            rowsInHigherBuckets += this->GetBucketRows(i);
        }

        return rowsLessThanTarget + rowsInHigherBuckets;
    }

    int GetBucketRows(int bucketID) {
        // return the number of values in the bucket
        return this->buckets[bucketID];
    }

    void ProcessNewInput(int newData) {
        if (this->getRecords() == 0) {
            setMin(newData);
            setMax(newData);
        } else {
            if (newData < this->getMin()) {
                setMin(newData);
            }
            if (newData > this->getMax()) {
                setMax(newData);
            }
        }

        // add to bucket
        this->buckets[this->FindBucket(newData)]++;

        incrementRecords();
    }

    /**
     * @brief Takes query data (target and compareOP) and determines if the result will ALWAYS be 0
     *
     * @param target
     * @param compareOp
     * @return true
     * @return false
     */
    bool InRange(int target, CompareOp compareOp) {
        // if column is empty
        if (this->getRecords() == 0) {
            return false;
        }

        // if searching for something less than minimum or greater than maximum
        if (compareOp == CompareOp::EQUAL) {
            printf("%d\n", this->getMax());
            if (this->getMax() < target) {
                return false;
            }
            if (this->getMin() > target) {
                return false;
            }
        }

        if (compareOp == CompareOp::GREATER) {
            if (this->getMax() < target) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Get the minimum value in the column.
     *
     * @return The minimum value in the column.
     */
    int getMin() {
        return min;
    }

    /**
     * @brief Get the maximum value in column.
     *
     * @return The minimum value in the column.
     */
    int getMax() {
        return max;
    }

    /**
     * @brief Get the number of records in the column.
     *
     * @return The number of records in the column.
     */
    int getRecords() {
        return records;
    };

   private:
    /* @brief min value in column.
     */
    int min;

    /* @brief max value in column.
     */
    int max;

    /* Number of records in the column
     */
    int records;

    // bucketId -> number of rows in bucket
    int buckets[BUCKET_COUNT];

    /**
     * @brief Generates statistics for a given column and store them in this class's attributes.
     *
     * @param column List of tuples to analyse.
     */
    void AnalyseColumn(std::vector<int> column) {
        for (int i = 0; i < this->records; i++) {
            if (column[i] < this->min) {
                this->setMin(column[i]);
            }
            if (column[i] > this->max) {
                this->setMax(column[i]);
            }
        }
    };

    /**
     * @brief Set the minimum value in the column.
     *
     * @param min the new value to set the minimum value to.
     */
    void setMin(int min) {
        this->min = min;
    }

    /**
     * @brief Set the maximum value in the column.
     *
     * @param max the new value to set the maximum value to.
     */
    void setMax(int max) {
        this->max = max;
    }

    /**
     * @brief Increment records by 1 when a new record is added.
     */
    void incrementRecords() {
        setRecords(getRecords() + 1);
    }

    /**
     * @brief Set records to a specific value.
     *
     * @param records Value to set records to.
     */
    void setRecords(int records) {
        this->records = records;
    }
};