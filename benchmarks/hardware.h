#ifndef HARDWARE_H
#define HARDWARE_H

#include <iostream>
#include <chrono>
#include <emmintrin.h>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

auto measure_loop_overhead(int num_operations) {
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    // Empty loop, just for measuring overhead
  }
  auto end = high_resolution_clock::now();
  return duration_cast<nanoseconds>(end - start).count();
}

// ALU: Simple arithmetic operations (ADD/MOV)
// < 1 --- (2 -- 3)
auto alu_operation(int num_operations) {
  volatile int x = 1, y = 2, z = 0;
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    z = x >> 1;
    z = y << 1;
    z = y | 1;
    z = x ^ 3;
    z = ~x;
    z = x >> 1;
    z = y << 1;
    z = y | 1;
    z = x ^ 3;
    z = ~x;
  }
  auto end = high_resolution_clock::now();
  return duration_cast<nanoseconds>(end - start).count();
}

// Integer multiplication
// x86/x64 -- MUL/IMUL 1~7. (3 -- 6)
auto integer_multiplication(int num_operations) {
  volatile int x = 10, y = 20, z = 0;
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    z = x * y;  // Multiplication
    z = x * 3;
    z = x * 12;
    z = x * 213;
    z = y * 23;
    z = x * 213;
    z = x * 12;
    z = x * 213;
    z = y * 23;
    z = x * 213;
  }
  auto end = high_resolution_clock::now();
  return duration_cast<nanoseconds>(end - start).count();
}

// Integer divide
// x86/x64 --> DIV/IDIV (12 -- 44)
auto integer_divide(int num_operations) {
  volatile int x = 10, y = 20, z = 0;
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    z = x / y;  // Divide
    z = x / 3;
    z = x / 12;
    z = x / 213;
    z = y / 23;
    z = x / 213;
    z = x / 12;
    z = x / 213;
    z = y / 23;
    z = x / 2;
  }
  auto end = high_resolution_clock::now();
  return duration_cast<nanoseconds>(end - start).count();
}

// Floating-point multiplication
// int, long long,  [+/-] [010101010] --> x, 8 1000
// float, double [+/-] [01010]M [0101010101]x, +/- * x * 10^{M-128}
// FMLSS/FMLSD --> (0.5 ~ 5) --- (4 ~ 9)
auto fpu_multiplication(int num_operations) {
  volatile double x = 3.14f, y = 2.71f, z = 0.0f;
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    z = x * y;  // Multiplication
    z = x * 3.5;
    z = x * 12.4;
    z = x * 213.2;
    z = y * 23.1;
    z = x * 213.233;
    z = x * 12.111;
    z = x * 213.12;
    z = y * 23.1231;
    z = x * 213.23;
  }
  auto end = high_resolution_clock::now();
  return duration_cast<nanoseconds>(end - start).count();
}

// Floating-point divide
// FDIV --> (37 --- 44)
auto fpu_divide(int num_operations) {
  volatile double x = 3.14f, y = 2.71f, z = 0.0f;
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    z = x / y;  // Divide
    z = x / 3.5;
    z = x / 12.4;
    z = x / 213.2;
    z = y / 23.1;
    z = x / 213.233;
    z = x / 12.111;
    z = x / 213.12;
    z = y / 23.1231;
    z = x / 213.23;
  }
  auto end = high_resolution_clock::now();
  return duration_cast<nanoseconds>(end - start).count();
}

// 80 年代 --> 只需要看汇编代码->程序速度，CPU计算 = 内存访问 （4 --- 6）
// * CPU 的速度不停变快 10^3+, 内存速度相对提升较慢 10-30+
// 内存 (0x21321ddf, 0x2131289F) ---- --- a+b-(ADD) ---- exe --- c++
// 内存 (0x21321ddf, 0x2131289F) (*a=2, *(a+1)=3, ..., *(a+P)=7) ---- L3 (a=2) --- L2 寄存器 (a=?, b=?) --- ALU (a+b=?) --- CPU(result) (a=2) -- L1(a=2) ---- 寄存器 (a=?, b=?)
// 内存 (0x21321ddf, 0x2131289F) (*a=2, *(a+1)=3, ..., *(a+P)=7) ---- L3 (a=2) --- L2 (a=2) -- L1(a=2) ---- 寄存器 (a[1]=?, a[2]=? ...)
// Memory access operation
auto memory_access_adjust(int num_operations) {
  volatile static int arr[(1<<20) + 5] = {0};  // Ensure this array is not optimized away
  volatile int temp;
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    temp = arr[0] ++;   // Memory access 1
    temp = arr[1] ++;   // Memory access 2
    temp = arr[2] ++;   // Memory access 3
    temp = arr[3] ++;   // Memory access 4
    temp = arr[4] ++;   // Memory access 5
    temp = arr[5] ++;   // Memory access 6
    temp = arr[6] ++;   // Memory access 7
    temp = arr[7] ++;   // Memory access 8
    temp = arr[8] ++;   // Memory access 9
    temp = arr[9] ++;   // Memory access 10
  }
  auto end = high_resolution_clock::now();
  return duration_cast<nanoseconds>(end - start).count();
}

// Memory access operation
auto memory_access_random(int num_operations) {
  volatile static int arr[(1<<20) + 5] = {0};  // Ensure this array is not optimized away
  volatile int temp;
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    temp = arr[0] ++;       // Memory access 1
    temp = arr[1<<10] ++;   // Memory access 2
    temp = arr[1<<11] ++;   // Memory access 3
    temp = arr[1<<12] ++;   // Memory access 4
    temp = arr[1<<13] ++;   // Memory access 5
    temp = arr[1<<11] ++;   // Memory access 6
    temp = arr[1<<14] ++;   // Memory access 7
    temp = arr[1<<20] ++;   // Memory access 8
    temp = arr[1<<3] ++;   // Memory access 9
    temp = arr[1<<18] ++;   // Memory access 10
  }
  auto end = high_resolution_clock::now();
  return duration_cast<nanoseconds>(end - start).count();
}


// SIMD 单指令多数据 --> Intel (SSE AVX), ARM (ARM Neon)
// 0 ~ 2^32 uint, 0 ~ 2^64 uint64, ()
// [ ABA count (64-48) | pointer (48) | hdr (64)] ---> 128
// struct { uint64 x, y; alignment. } T;
// T ---> offset --> T.x ---> ptr ---> value of T.x
//                |-> T.y --> ptr ---> value of T.y
// SIMD: 向量运算
// (x, y).
// int, uint x --> 从内存找出 x --> 直接把 x 的值送到 ALU
// Int x --> 从内存找出 x 的类的位置 --> 从类的位置上取出 x 的值 --> x 送到 ALU
auto simd_addition_128bit(int num_operations) {
  __m128i a = _mm_set_epi32(4, 3, 2, 1);  // [4, 3, 2, 1]
  __m128i b = _mm_set_epi32(8, 7, 6, 5);  // [8, 7, 6, 5]
  __m128i result;

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
    result = _mm_add_epi32(a, b);
  }
  auto end = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}


// 旁路延迟：
// int x = 3; double y = 4.5;
// auto z = x * y;
// int x = 1,  [+/-]0000000001
// double y = 1, [+/-][power]0000001, y = 1 [+][128][00000000001], y = 0.01 [+][128-2][00000001]
// base * 10^n, (0, 1)
// 10001 ---> 1/2 + 0 + 0 + 0 + 1/2^5
// double x = 3, y = 4.5; z = x*y;
// MOV(x) ---> MOV(y) ---> ALU_MUL(x, y)->result ---> IO
// MOV(x) ---> MOV(y) ---> ALU_MUL()/ error --> x transform type ---> MOV(x') --->  ALU_MUL(x, y)->result ---> IO
// x86 Intel ---> 0 ~ 3 CPU 周期
auto bypass_delay_benchmark(int num_operations) {
  volatile int int_x = 10, int_y = 20, int_z = 0;
  volatile float float_x = 3.14f, float_y = 2.71f, float_z = 0.0f;

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    int_z = float_x * int_y;
    int_z = float_x * 3;
    int_z = float_x * 12;
    int_z = float_x * 213;
    int_z = float_y * 23;

    // Floating-point multiplication
    float_z = int_x * float_y;
    float_z = int_x * 2.5f;
    float_z = int_y * 1.8f;
    float_z = int_x * 4.2f;
    float_z = int_y * 3.7f;
  }
  auto end = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

// [MOV] [MOV] [MOV] [SET] [IF] [ADD] [JUMP] .... [条件 1] .... [条件 2]
// 【 】 --- L1 L2
// [MOV] [MOV] [MOV] [SET] [IF] [ADD] [JUMP] [条件 1] ....  .... [条件 2]
// 【 】 --- L1 L2

// [PC +1 -> PC'] ---> [MOV ADD SAN MUL JUMP .... INC ..... DEC ]
// IF (x = y)
// [PC +1 -> PC'] ---> [MOV ADD SAN MUL JUMP INC ..... DEC ]
// 正确预测 1-2 CPU 错误预测 10 - 20 (branchpredictor) ---> CPU 2018 (15 - 20)
auto naive_branching(int num_operations) {
  volatile int x = 0;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < num_operations; ++i) {
    // Random branching outcome with 50% chance
    if (rand() % 10 == 0) { x++; } else { x--; }
    if (rand() % 20 == 0) { x += 2; } else { x -= 2; }
    if (rand() % 50 == 0) { x += 3; } else { x -= 3; }
    if (rand() % 100 == 0) { x += 4; } else { x -= 4; }
    if (rand() % 200 == 0) { x += 5; } else { x -= 5; }
    if (rand() % 500 == 0) { x += 6; } else { x -= 6; }
    if (rand() % 1000 == 0) { x += 7; } else { x -= 7; }
    if (rand() % 2000 == 0) { x += 8; } else { x -= 8; }
    if (rand() % 5000 == 0) { x += 9; } else { x -= 9; }
    if (rand() % 10000 == 0) { x += 10; } else { x -= 10; }
  }

  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

// CPU ---> 内部算法，帮你写好 __builtin_expect --->
auto optimized_branching(int num_operations) {
  volatile int x = 0;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < num_operations; ++i) {
    // Here we expect the true branch to occur 90% of the time
    if (__builtin_expect(i % 10 != 0, 1)) {  // 90% likely
      x++;
    } else {  // 10% likely
      x--;
    }

    // Vary the likelihood of each operation using larger modulus
    if (__builtin_expect(i % 20 != 0, 1)) {  // 95% likely
      x += 2;
    } else {  // 5% likely
      x -= 2;
    }

    if (__builtin_expect(i % 50 != 0, 1)) {  // 98% likely
      x += 3;
    } else {  // 2% likely
      x -= 3;
    }

    if (__builtin_expect(i % 100 != 0, 1)) {  // 99% likely
      x += 4;
    } else {  // 1% likely
      x -= 4;
    }

    if (__builtin_expect(i % 200 != 0, 1)) {  // 99.5% likely
      x += 5;
    } else {  // 0.5% likely
      x -= 5;
    }

    if (__builtin_expect(i % 500 != 0, 1)) {  // 99.8% likely
      x += 6;
    } else {  // 0.2% likely
      x -= 6;
    }

    if (__builtin_expect(i % 1000 != 0, 1)) {  // 99.9% likely
      x += 7;
    } else {  // 0.1% likely
      x -= 7;
    }

    if (__builtin_expect(i % 2000 != 0, 1)) {  // 99.95% likely
      x += 8;
    } else {  // 0.05% likely
      x -= 8;
    }

    if (__builtin_expect(i % 5000 != 0, 1)) {  // 99.98% likely
      x += 9;
    } else {  // 0.02% likely
      x -= 9;
    }

    if (__builtin_expect(i % 10000 != 0, 1)) {  // 99.99% likely
      x += 10;
    } else {  // 0.01% likely
      x -= 10;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}


// CAS ---> x86 IBM ARM  15 --- 30 CPU cycle.
// thread ---> [x=1] ---> [x=3] thread2
// volatile  -->
// CAS ---> [x=1] ---> [x=3]


// TLB -->  10 process [0x0000000 ------  0x32131cxzF]
// TLB map (process id, + 虚拟内存) ---> 物理内存
// I/O ---- Page size, memory alignment -->
// TLB map 很大，TLB cache。
// 内存局部性


void test_all_hardware_related(int num_operations) {
  auto base = measure_loop_overhead(num_operations) / 100000.0;
  cout << "ALU operation (ADD) time: " << alu_operation(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Integer multiplication time: " << integer_multiplication(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Integer divide time: " << integer_divide(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Floating-point multiplication time: " << fpu_multiplication(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Floating-point divide time: " << fpu_divide(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Memory access adjust time: " << memory_access_adjust(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Memory access random time: " << memory_access_random(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "SIMD add: " << simd_addition_128bit(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Bypassing delay: " << bypass_delay_benchmark(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Naive branching: " << naive_branching(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Optimized branching: " << optimized_branching(num_operations) / 100000.0 -  base << " ns/kops\n";
}

#endif //HARDWARE_H
