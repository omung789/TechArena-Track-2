// TODO:
// --- CompareOP Equals ---
// 1. Implement a function that returns the number of distinct rows in a bucket
// 2. Create Bucket generator
// 3. Add to a bucket when a new value is added
// 4. Figure out a way to return 1 with a probability p and 0 with a probability 1-p in handleQuery

// -- CompareOP Greater ---
// n/a

#include <common/Root.h>

class ColumnStats {
   public:
    ColumnStats() {
        setMin(INT_MAX);
        setMax(INT_MIN);
        setRecords(0);
    };

    ColumnStats(std::vector<int> column) {
        setMin(column.at(0));
        setMax(column.at(0));
        setRecords(column.size());

        // set min and max
        AnalyseColumn(column);
    };

    int handleQuery(int target, CompareOp compareOp) {
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

        // at this point the query must be equal to and the value must be in range [min, max]

        // TODO: see if in frequent elements

        // determine probability that it is in it's bucket
        // number of rows in bucket / (bucket size) -> skewed data fucks it
        int bucketID = FindBucket(target);
        int DistinctRowsInBucket = GetBucketRows(bucketID);
        // gets lower bound of higher bucket and minuses lower bound of lower bucket
        int bucketRange = this->bucketBoundaries.find(bucketID + 1)->second - this->bucketBoundaries.find(bucketID)->second;

        // probability that a number in range [bucketMin, buacketMax] is in the bucket that is bound by [bucketMin, bucketMax]
        // i.e., the probability that number 5 is in bucket [0, 10]
        float p = DistinctRowsInBucket / bucketRange;

        // TODO: need to have a p chance to return 1 and a 1-p chance to return 0
        }

    int DistinctRowsInBucket(int BucketID) {
        // TODO
    }

    int FindBucket(int target) {
        // return the bucket id of the target value's bucket
        int result = 0;
        for (auto const& [bucketID, lower_bound] : this->bucketBoundaries) {
            if (target < lower_bound) {
                return result;
            }
            result++;
        }
    }

    int GuessRowsGreaterThan(int target) {
        // get bucket data
        int BucketID = this->FindBucket(target);
        int bucketSize = this->GetBucketRows(BucketID);
        int bucketMin = this->bucketBoundaries.find(BucketID)->second;
        int bucketMax = this->bucketBoundaries.find(BucketID + 1)->second;

        // caluclate how far 'into' the bucket the target is
        // will be more accurate the closer to a unifrom distribution the bucket is is
        int bucketRange = bucketMax - bucketMin;
        int targetPercentage = (target - bucketMin) / bucketRange;

        // how many rows are less than or equal to target
        int rowsLessThanTarget = targetPercentage * bucketSize;

        // how many rows are in higher buckets
        int rowsInHigherBuckets = 0;
        for (int i = BucketID + 1; i < this->buckets.size(); i++) {
            rowsInHigherBuckets += this->GetBucketRows(i);
        }

        return rowsLessThanTarget + rowsInHigherBuckets;
    }

    int GetBucketRows(int bucketID) {
        // return the number of rows in the bucket
        return this->buckets.find(bucketID)->second;
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
        incrementRecords();
    }

    bool InRange(int target, CompareOp compareOp) {
        // if column is empty
        if (this->getRecords() == 0) {
            return false;
        }

        if (compareOp == CompareOp::EQUAL) {
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
    std::map<int, int> buckets;

    // bucketId -> lower bound value
    std::map<int, int> bucketBoundaries;

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