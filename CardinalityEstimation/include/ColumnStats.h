#include <climits>

#include "common/Expression.h"
#include "common/Root.h"

#define MAX_VAL 20000000
#define BUCKET_WIDTH 1000
#define BUCKET_COUNT int(MAX_VAL / BUCKET_WIDTH)
#define ARRAY_SIZE BUCKET_COUNT + 1

/**
 * @brief Stands for index of different columns
 *
 */
enum ColumnIdx { COLUMN_A = 0,
                 COLUMN_B = 1 };

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
        this->occurancesOfMin = 0;
        this->occurancesOfMax = 0;
    };

    /**
     * @brief Returns the estimated number of rows that will be returned by the query.
     *
     * @param target Query target
     * @param compareOp Query operator
     * @return `int` The estimated number of rows that will be returned by the query.
     */
    int HandleQuery(int target, CompareOp compareOp) {
        // target not in column
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

        if (target == this->getMin()) {
            return this->occurancesOfMin;
        }

        if (target == this->getMax()) {
            return this->occurancesOfMax;
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

    /**
     * @brief Deletes a value from the column and updates the column statistics.
     *
     * @param target The value to be delted from the column
     */
    void ProcessDelete(int target) {
        // remove from bucket
        int bucketID = this->FindBucket(target);
        this->buckets[bucketID] = std::min(this->buckets[bucketID] - 1, 0);

        if (target >= getMax()) {
            // find new max
            this->occurancesOfMax--;
            if (this->occurancesOfMax == 0) {
                setMax(findNewMax());
            }
        } else if (target <= getMin()) {
            // find new min
            this->occurancesOfMin--;
            if (this->occurancesOfMin == 0) {
                setMin(findNewMin());
            }
        }

        decrementRecords();
    }

    /**
     * @brief Processes new input data and updates the column statistics.
     *
     * @param newData The new data to add to the column.
     */
    void ProcessNewInput(int newData) {
        if (this->getRecords() == 0) {
            setMin(newData);
            setMax(newData);
        } else {
            if (newData == this->getMin()) {
                occurancesOfMin++;
            } else if (newData < this->getMin()) {
                setMin(newData);
            }

            if (newData == this->getMax()) {
                occurancesOfMax++;
            } else if (newData > this->getMax()) {
                setMax(newData);
            }
        }

        // add to bucket
        this->buckets[this->FindBucket(newData)]++;

        incrementRecords();
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

    /* @brief Number of records in the column
     */
    int records;

    /* @brief Number of values in each bucket
     */
    int buckets[ARRAY_SIZE];

    int occurancesOfMin;
    int occurancesOfMax;

    /**
     * @brief Set the minimum value in the column.
     *
     * @param min the new value to set the minimum value to.
     */
    void setMin(int min) {
        this->min = min;
        occurancesOfMin = 1;
    }

    /**
     * @brief Set the maximum value in the column.
     *
     * @param max the new value to set the maximum value to.
     */
    void setMax(int max) {
        this->max = max;
        occurancesOfMax = 1;
    }

    /**
     * @brief Set records to a specific value.
     *
     * @param records Value to set records to.
     */
    void setRecords(int records) {
        this->records = records;
    }

    /**
     * @brief Increment records by 1 when a new record is added.
     */
    void incrementRecords() {
        setRecords(getRecords() + 1);
    }

    /**
     * @brief Decrement records by 1 when a record is deleted.
     */
    void decrementRecords() {
        setRecords(std::min(getRecords() - 1, 0));
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

        // if searching for something greater than maximum
        // will be false whether > or =
        if (this->getMax() < target) {
            return false;
        }

        // if searching for something less than minimum
        if (compareOp == CompareOp::EQUAL && this->getMin() > target) {
            return false;
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
     * @brief Finds a value's bucket.
     *
     * @param target The value which we are trying to find the bucket of.
     * @return `int` The bucket index of the bucket that the target value is in, or -1.
     */
    int FindBucket(int target) {
        // return the bucket id of the target value's bucket
        if (target <= MAX_VAL) {
            return int(target / BUCKET_WIDTH);
        } else {
            return BUCKET_COUNT;
        }
    }

    /**
     * @brief Performs histogram calculations to determine how many rows are greater than the target value.
     *
     * @param target Target Value
     * @return `int` The estimated number of rows that are greater than the target
     */
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
        for (int i = BucketID + 1; i < ARRAY_SIZE; i++) {
            rowsInHigherBuckets += this->GetBucketRows(i);
        }

        return rowsLessThanTarget + rowsInHigherBuckets;
    }

    /**
     * @brief Returns the midpoint of the lowest occupied bucket (or 0).
     *
     * @return `int` The midpoint of the lowest occupied bucket (or 0).
     */
    int findNewMin() {
        // perform a binary search to find the midpoint of the lowest occupied bucket
        int l = 0;
        int r = BUCKET_COUNT - 1;
        int res = -1;

        while (l <= r) {
            int m = l + (r - l) / 2;
            if (this->GetBucketRows(m) > 0) {
                res = m;  // could be anser, check if there is a lower one
                r = m - 1;
            } else {
                l = m + 1;
            }
        }

        return res != -1 ? (res + 0.5) * BUCKET_WIDTH : 0;
    }

    /**
     * @brief Returns the midpoint of the highest occupied bucket (or 0).
     *
     * @return `int` The midpoint of the highest occupied bucket (or 0).
     */
    int findNewMax() {
        // perform a binary search to find the midpoint of the highest occupied bucket
        int l = 0;
        int r = BUCKET_COUNT;
        int res = -1;

        while (l < r) {
            int m = l + (r - l) / 2;
            if (this->GetBucketRows(m) > 0) {
                l = m + 1;
                res = m;
            } else {
                r = m - 1;
            }
        }

        return res != -1 ? (res + 0.5) * BUCKET_WIDTH : 0;
    }

    /**
     * @brief Get the number of rows in a bucket.
     *
     * @param bucketID The bucket index of the bucket we want to get the number of rows of.
     * @return `int` The number of rows in the bucket.
     */
    int GetBucketRows(int bucketID) {
        // return the number of values in the bucket
        return this->buckets[bucketID];
    }
};