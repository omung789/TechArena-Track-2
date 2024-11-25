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