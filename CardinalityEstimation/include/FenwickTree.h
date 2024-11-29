#include "common/Expression.h"
#include "common/Root.h"

class FenwickTree {
   private:
    std::vector<int> tree;
    int size;

   public:
    FenwickTree(int n) : size(n) {
        tree.resize(n + 1, 0);
    }

    ~FenwickTree() = default;

    void update(int index, int delta) {
        while (index <= size) {
            tree[index] += delta;
            index += index & -index;
        }
    };

    int query(int index) const {
        int sum = 0;
        while (index > 0) {
            sum += tree[index];
            index -= index & -index;
        }
        return sum;
    }

    int rangeQuery(int left, int right) const {
        return query(right) - query(left - 1);
    }
};