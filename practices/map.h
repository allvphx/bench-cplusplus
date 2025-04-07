#ifndef PRACTICE_H
#define PRACTICE_H

#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <vector>

// 高性能服务 (---)   [客户端] ----- [服务端]
// c++ 性能优化这个问题，是很主观的
// 1. 一个场景下适用的优化在另外一个场景下不好.
// 2. 性能优化是一种权衡，90/10，90% 时间消耗在 10% 的代码上面 --- 70/30。
// which? 不是所有的代码都值得优化。

// 通用优化步骤
// O(n^2), 10000^2 ~ 10^8*a for (int i=0;i<n;++i) for(int j=i+1;j<n;++j) compare and swap. 冒泡排序。
// O(nlogn), 10000 ~ 100000. sort() --- 快速排序.
// O(n!) 2^n --.
// 100000000 * n^2  --- 1 * n^3
// 1. 时间复杂度。 F -- O(n^2) == F CPU cycle number < 100000000 * n^2
// 2. 接口设计。博客网站，搜索 + CURD 章节。 Update 章节 1. Update article。 2. Update 具体章节
// 3. 忽视掉低效用代码。
// 4. 编程的时候遵循一些常见的优化方法。for (int i=0;i<n; ++i) , /2^t >>t  / 20+ CPU --- >> (< CPU)
// 5. 分离。

// 什么该优化什么不该优化，并不能算是一门科学，而是一个经验论，一种权衡，一门艺术。

// 噪音邻居 和 资源吞噬者。
// 10% 值得我们投入时间 --- 90% 不值得我们投入大量时间优化的。
// 软件，部署在一个固定的资源。CPU-GPU-memory。
// 我们 task A docker PostgreSQL 分配了一个核心，task B 后端代码。
// --> task A 上下文切换 --> 1. 物理冲突：缓存的变化 2. 逻辑冲突：不同的 task 共享同样的 memory space。
// x ++, x ++
// R(x1) R(x2) W(x1+1) W(x2+1)
// R(x1) W(x1+1) R(x2) W(x2+1)
// task A, task B 任务. 1. 逻辑冲突 2. 物理冲突。
// task A (10%), task B PostgreSQL (90%)。
// 100 --> 100000 (in-memory)。
// 客户数：10 --> 1000 --> 100000
// 1. 我们在面临不同的应用场景和客户要求的情况下，10-90 的结果很可能是动态的。
// 2. 就算是不关键的代码，我们也不可以过度消耗资源。

// 服务器端。 ---> 加机器服务端性能更好。
// 1. scale-up: 1 核（100000 CPU cycle/s）的 E2 --> 1 核（10^8 CPU cycles/s）的 GCP compute node。（物理限制和设计限制 --> CPU 越来越贵）
// 2. scale-out: 10核的E2 1TB  --> 10 x E2 机器 10 核 1TB ---> 把我们的服务去分布到这几个机器上。
// --> 线性拓展。
// 1. 付了更多的钱，scale-up 付的钱是越来越多的。
// 2. scale-out：
//  ----》一致性
//  ----》共享组件。single point bottleneck.

// ---> 设计服务器程序的时候，本身就要去尽量优化他。
// 重构/优化部分组件。
// 1. 时间复杂度。
// 2. 时间
// 3.

class NaiveMap {
public:
    void insert(int key, int value) {
        /// O(n)
        // Check if key exists and update value
        for (auto& pair : data_) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }
        // Key doesn't exist, add new pair
        data_.emplace_back(key, value);
    }

    int get(int key) const {
        // O(n)
        // Linear search for the key
        for (const auto& pair : data_) {
            if (pair.first == key) {
                return pair.second;
            }
        }
        throw std::runtime_error("Key not found");
    }

private:
    std::vector<std::pair<int, int>> data_;
};

// 1. 算法变得更好 --> 降低时间复杂度。
// O(n)  ---> O(logn)

class OptimizedMap {
public:
    void insert(int key, int value) {
        data_[key] = value; // O(1) average time
    }

    int get(int key) const {
        auto it = data_.find(key); // O(1) average time
        if (it != data_.end()) {
            return it->second;
        }
        throw std::runtime_error("Key not found");
    }

private:
    map<int, int> data_;
};


// 2. 第二重要：非阻塞。
// 3. 第三重要：硬件，代码细节。


struct alignas(64) KeyValue {
    int key, value;
    bool operator<(const KeyValue& other) const {
        return key < other.key;
    }
};


class CacheFriendlyMap {
public:
    CacheFriendlyMap(size_t expected_size = 0) {
        if (expected_size > 0) {
            keys_.reserve(expected_size);
            values_.reserve(expected_size);
        }
    }

    // Batch insertions to optimize insertion time
    void bulk_insert(const std::vector<int>& keys, const std::vector<int>& values) {
        if (keys.size() != values.size()) {
            throw std::invalid_argument("Keys and values must have the same size");
        }

        size_t total_size = keys_.size() + keys.size();
        keys_.reserve(total_size);
        values_.reserve(total_size);

        // Append new keys and values
        keys_.insert(keys_.end(), keys.begin(), keys.end());
        values_.insert(values_.end(), values.begin(), values.end());

        // Create index vector for sorting
        std::vector<size_t> indices(keys_.size());
        for (size_t i = 0; i < indices.size(); ++i) {
            indices[i] = i;
        }

        // Sort indices based on keys
        std::sort(indices.begin(), indices.end(),
                  [this](size_t lhs, size_t rhs) { return keys_[lhs] < keys_[rhs]; });

        // Apply sorted order to keys and values
        std::vector<int> sorted_keys(keys_.size());
        std::vector<int> sorted_values(values_.size());

        for (size_t i = 0; i < indices.size(); ++i) {
            sorted_keys[i] = keys_[indices[i]];
            sorted_values[i] = values_[indices[i]];
        }

        keys_.swap(sorted_keys);
        values_.swap(sorted_values);
    }

    // Single insertion (less efficient than bulk_insert)
    __attribute__((always_inline)) void insert(int key, int value) {
        auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
        size_t index = it - keys_.begin();
        if (__builtin_expect(it != keys_.end() && *it == key, 1)) {
            values_[index] = value; // Update existing key
        } else {
            keys_.insert(it, key);
            values_.insert(values_.begin() + index, value);
        }
    }

     __attribute__((always_inline)) int get(int key) const {
        auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
        size_t index = it - keys_.begin();
        if (__builtin_expect(it != keys_.end() && *it == key, 1)) {
            return values_[index]; // Key found
        }
        throw std::runtime_error("Key not found");
    }

private:
    std::vector<int> keys_;
    std::vector<int> values_;
};

// Helper function to generate random integers
std::vector<int> generate_random_ints(size_t count, int min, int max) {
    std::vector<int> data(count);
    std::random_device rd;
    std::mt19937 mt(rd());
    uniform_int_distribution<int> dist(min, max);

    for (size_t i = 0; i < count; ++i) {
        data[i] = dist(mt);
    }
    return data;
}

std::vector<int> generate_continous_ints(size_t count) {
    std::vector<int> data(count);
    for (size_t i = 0; i < count; ++i) {
        data[i] = i;
    }
    return data;
}


void benchmark_map() {
    const size_t NUM_OPERATIONS = 10000;

    // Generate random keys and values
    std::vector<int> keys = generate_continous_ints(NUM_OPERATIONS);
    std::vector<int> values = generate_random_ints(NUM_OPERATIONS, 1, NUM_OPERATIONS * 10);

    // Benchmark Naive (Code 1)
    {
        NaiveMap map;

        // Measure insertion time
        auto start_insert = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            map.insert(keys[i], values[i]);
        }
        auto end_insert = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> insert_duration = end_insert - start_insert;

        // Measure retrieval time
        auto start_get = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            try {
                int value = map.get(keys[i]);
            } catch (const std::runtime_error&) {
                // Key not found (should not happen in this test)
            }
        }
        auto end_get = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> get_duration = end_get - start_get;

        std::cout << "OptimizedMap (O(N^2)):\n";
        std::cout << "Insertion time: " << insert_duration.count() << " seconds\n";
        std::cout << "Retrieval time: " << get_duration.count() << " seconds\n\n";
    }

    // Benchmark OptimizedMap (Code 2)
    {
        OptimizedMap map;

        // Measure insertion time
        auto start_insert = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            map.insert(keys[i], values[i]);
        }
        auto end_insert = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> insert_duration = end_insert - start_insert;

        // Measure retrieval time
        auto start_get = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            try {
                int value = map.get(keys[i]);
            } catch (const std::runtime_error&) {
                // Key not found (should not happen in this test)
            }
        }
        auto end_get = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> get_duration = end_get - start_get;

        std::cout << "OptimizedMap (std::map O(nlogn)):\n";
        std::cout << "Insertion time: " << insert_duration.count() << " seconds\n";
        std::cout << "Retrieval time: " << get_duration.count() << " seconds\n\n";
    }

    // Benchmark CacheFriendlyMap (Code 3)
    {
        CacheFriendlyMap map(NUM_OPERATIONS+2);

        // Measure insertion time
        auto start_insert = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            map.insert(keys[i], values[i]);
        }
        auto end_insert = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> insert_duration = end_insert - start_insert;

        // Measure retrieval time
        auto start_get = std::chrono::high_resolution_clock::now();
        int value;
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            value = map.get(keys[i]);
        }
        auto end_get = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> get_duration = end_get - start_get;

        std::cout << "CacheFriendlyMap:\n";
        std::cout << "Insertion time: " << insert_duration.count() << " seconds\n";
        std::cout << "Retrieval time: " << get_duration.count() << " seconds\n\n";
    }
}


void benchmark_map_random() {
    const size_t NUM_OPERATIONS = 10000;

    // Generate random keys and values
    std::vector<int> keys = generate_random_ints(NUM_OPERATIONS, 1, NUM_OPERATIONS * 10);
    std::vector<int> values = generate_random_ints(NUM_OPERATIONS, 1, NUM_OPERATIONS * 10);

    // Benchmark Naive (Code 1)
    {
        NaiveMap map;

        // Measure insertion time
        auto start_insert = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            map.insert(keys[i], values[i]);
        }
        auto end_insert = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> insert_duration = end_insert - start_insert;

        // Measure retrieval time
        auto start_get = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            try {
                int value = map.get(keys[i]);
            } catch (const std::runtime_error&) {
                // Key not found (should not happen in this test)
            }
        }
        auto end_get = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> get_duration = end_get - start_get;

        std::cout << "OptimizedMap (O(N^2)):\n";
        std::cout << "Insertion time: " << insert_duration.count() << " seconds\n";
        std::cout << "Retrieval time: " << get_duration.count() << " seconds\n\n";
    }

    // Benchmark OptimizedMap (Code 2)
    {
        OptimizedMap map;

        // Measure insertion time
        auto start_insert = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            map.insert(keys[i], values[i]);
        }
        auto end_insert = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> insert_duration = end_insert - start_insert;

        // Measure retrieval time
        auto start_get = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            try {
                int value = map.get(keys[i]);
            } catch (const std::runtime_error&) {
                // Key not found (should not happen in this test)
            }
        }
        auto end_get = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> get_duration = end_get - start_get;

        std::cout << "OptimizedMap (std::map O(nlogn)):\n";
        std::cout << "Insertion time: " << insert_duration.count() << " seconds\n";
        std::cout << "Retrieval time: " << get_duration.count() << " seconds\n\n";
    }

    // Benchmark CacheFriendlyMap (Code 3)
    {
        CacheFriendlyMap map(NUM_OPERATIONS+2);

        // Measure insertion time
        auto start_insert = std::chrono::high_resolution_clock::now();
        map.bulk_insert(keys, values);
        auto end_insert = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> insert_duration = end_insert - start_insert;

        // Measure retrieval time
        int value;
        auto start_get = std::chrono::high_resolution_clock::now();
        for (register size_t i = 0; i < NUM_OPERATIONS; ++i) {
            value = map.get(keys[i]);
        }
        auto end_get = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> get_duration = end_get - start_get;

        std::cout << "CacheFriendlyMap:\n";
        std::cout << "Insertion time: " << insert_duration.count() << " seconds\n";
        std::cout << "Retrieval time: " << get_duration.count() << " seconds\n\n";
    }
}

#endif //PRACTICE_H
