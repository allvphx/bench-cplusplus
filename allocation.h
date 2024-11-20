#ifndef ALLOCATION_H
#define ALLOCATION_H

#include <chrono>
#include <iostream>
#include <vector>
#include <list>
#include <memory>

// 1. 优点：快速。
// 2. 缺点：1. 内存分配不灵活 2. 内存释放不灵活。
auto static_allocation_benchmark(int num_operations) {
    volatile int x = 0; // Static variable, only allocated once
    volatile static int tmp = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        tmp = i;
        x += i; // Modify the static variable
    }
    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

// Function to benchmark stack allocation
// 1, 栈分配：函数局部变量，小数组。 ---> 1. 快 2. 栈空间是有限的。
auto stack_allocation_benchmark(int num_operations) {
    volatile int x = 0;
    // RAII.
    // 进程 --> [   |^  ^] (MB)
    // int a[100];
    // int a[10000000];
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        volatile int stack_var = i; // Stack allocation
        x += stack_var;
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

// Function to benchmark heap allocation using new/delete
// 1. 优点：灵活。 8GB --- TB
// 2. 1. 因为我们涉及到了内存的具体分配和释放 --> 编程困难。 2. 很慢


// int* a = new int(1)
// delete a;
// LOOP 1000000: int* b = new int(3). b = b + 1
// cout << *a “=== 4” << endl;
auto heap_allocation_benchmark(int num_operations) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        // MALLOC --> 系统分配内存 ---> 1. 找碎片 2. 注册表里面标记。
        int* heap_var = new int(i); // Heap allocation
        *heap_var += i;
        delete heap_var;            // Heap deallocation
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

// Simple memory pool class for int allocations
class IntPool {
public:
    IntPool(size_t pool_size) : pool(pool_size) {}

    int* allocate(int value) {
        if (index < pool.size()) {
            pool[index] = value;
            return &pool[index++];
        }
        return new int(value); // Fallback if pool exhausted
    }

    void deallocate(int* ptr) {
        // No-op for pool memory, but handle new/delete for out-of-pool allocations
        if (ptr < &pool[0] || ptr >= &pool[0] + pool.size()) {
            delete ptr;
        }
    }

    void reset() { index = 0; }

private:
    std::vector<int> pool;
    size_t index = 0;
};

// Function to benchmark memory pool allocation.
auto pooled_allocation_benchmark(int num_operations) {
    auto start = std::chrono::high_resolution_clock::now();
    IntPool pool(num_operations);
    for (int i = 0; i < num_operations; ++i) {
        int* pooled_var = pool.allocate(i);
        *pooled_var += i;
        pool.deallocate(pooled_var); // Fast deallocation
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

// Function to benchmark heap allocation using smart pointers
// 内存指针计数。
// 1. thread 1 new a; (a)[1]   ---> 2. thread 2 write a; (a)[2]  ---> 3. thread 2 terminate (a)[1]
// ---> 3. thread 1 terminate.  (a)[0] ---> {a, b, c, d} 10 ms -->
// MOV ----> CAS RAII.
auto smart_pointer_allocation_benchmark(int num_operations) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        auto smart_var = std::make_unique<int>(i); // Unique pointer allocation
        *smart_var += i;
    } // Automatically deallocated here
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}
/// int(smart_var) 4 byte --->  4 8 ---- 1024 bytes | 4 byte ---> .

// Function to benchmark allocation using std::vector (contiguous memory)
// CK(x) vec[1000] new --> vec[vec.end] = new  --> O(1)
auto vector_allocation_benchmark(int num_operations) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> vec;
    // 0x0000001 --> 0x0000002 --> 0x0000003
    // vec.reserve(num_operations); // Avoid resizing during push_back
    for (int i = 0; i < num_operations; ++i) {
        vec.push_back(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

// Function to benchmark allocation using std::list (non-contiguous memory)
// O(1). tail -> next = new;
// list () --- () --- ()
// 内存局部性
auto list_allocation_benchmark(int num_operations) {
    auto start = std::chrono::high_resolution_clock::now();
    std::list<int> lst;
    for (int i = 0; i < num_operations; ++i) {
        lst.push_back(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

// Function to benchmark allocation using std::vector (contiguous memory)
auto total_vector_allocation_benchmark(int num_operations) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> vec;
    vec.reserve(num_operations); // Avoid resizing during push_back
    for (int i = 0; i < num_operations; ++i) {
        vec.push_back(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}


void test_allocation(int num_operations) {
    std::cout << "Static Allocation: " << static_allocation_benchmark(num_operations) / (num_operations/1000.0) << " ns / kalloc\n";
    std::cout << "Stack Allocation: " << stack_allocation_benchmark(num_operations) / (num_operations/1000.0) << " ns / kalloc\n";
    std::cout << "Heap Allocation (new/delete): " << heap_allocation_benchmark(num_operations) / (num_operations/1000.0) << " ns / kalloc\n";
    std::cout << "Smart Pointer Allocation (unique_ptr): " << smart_pointer_allocation_benchmark(num_operations) / (num_operations/1000.0) << " ns / kalloc\n";
    std::cout << "Pooled Allocation: " << pooled_allocation_benchmark(num_operations) / (num_operations/1000.0) << " ns / kalloc\n";

    // 内存局部性
    std::cout << "Vector Allocation: " << vector_allocation_benchmark(num_operations) / (num_operations/1000.0) << " ns / kalloc\n";
    std::cout << "List Allocation: " << list_allocation_benchmark(num_operations) / (num_operations/1000.0) << " ns / kalloc\n";
    std::cout << "Vector Reserve Allocation: " << total_vector_allocation_benchmark(num_operations) / (num_operations/1000.0) << " ns / kalloc\n";
}

#endif //ALLOCATION_H
