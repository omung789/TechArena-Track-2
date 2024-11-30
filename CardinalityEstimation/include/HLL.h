#include "common/Expression.h"
#include "common/Root.h"

class HyperLogLog {
   private:
    std::vector<uint8_t> registers;  // Registers to store the maximum leading zero counts
    int numRegisters;                // Number of registers (2^p)
    double alphaMM;                  // Alpha * m^2, a correction factor

    // Helper function to calculate the hash of a value
    uint32_t hash(int value) const {
        std::hash<int> hasher;
        return hasher(value);
    }

    // Count the number of leading zeros in a binary number
    int countLeadingZeros(uint32_t x) const {
        int count = 1;
        while (x >>= 1) ++count;
        return 32 - count + 1;
    }

   public:
    // Constructor: initialize registers and calculate alphaMM
    HyperLogLog(int p) : numRegisters(1 << p), registers(1 << p, 0) {
        double alpha;
        if (numRegisters == 16)
            alpha = 0.673;
        else if (numRegisters == 32)
            alpha = 0.697;
        else if (numRegisters == 64)
            alpha = 0.709;
        else
            alpha = 0.7213 / (1 + 1.079 / numRegisters);
        alphaMM = alpha * numRegisters * numRegisters;
    }

    // Add a value to the HLL
    void add(int value) {
        uint32_t hashValue = hash(value);
        int registerIndex = hashValue % numRegisters;
        int leadingZeros = countLeadingZeros(hashValue / numRegisters);
        registers[registerIndex] = std::max(registers[registerIndex], static_cast<uint8_t>(leadingZeros));
    }

    // Estimate the cardinality
    double estimate() const {
        double sum = 0.0;
        for (int i = 0; i < numRegisters; ++i) {
            sum += 1.0 / (1 << registers[i]);
        }
        double rawEstimate = alphaMM / sum;

        // Apply bias correction for small cardinalities
        if (rawEstimate <= 2.5 * numRegisters) {
            int zeroRegisters = std::count(registers.begin(), registers.end(), 0);
            if (zeroRegisters > 0) {
                rawEstimate = numRegisters * std::log(static_cast<double>(numRegisters) / zeroRegisters);
            }
        }
        return rawEstimate;
    }
};
