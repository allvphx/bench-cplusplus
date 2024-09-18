#include <iostream>
#include <chrono>

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

// Memory access operation
auto memory_access_adjust(int num_operations) {
  volatile static int arr[(1<<20) + 5] = {0};  // Ensure this array is not optimized away
  volatile int temp;
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    temp = arr[0] ++;       // Memory access 1
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

// Main function
int main() {
  const int num_operations = 10000000;  // Unified number of operations

  auto base = measure_loop_overhead(num_operations) / 100000.0;
  cout << "ALU operation (ADD) time: " << alu_operation(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Integer multiplication time: " << integer_multiplication(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Integer divide time: " << integer_divide(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Floating-point multiplication time: " << fpu_multiplication(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Floating-point divide time: " << fpu_divide(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Memory access adjust time: " << memory_access_adjust(num_operations) / 100000.0 -  base << " ns/kops\n";
  cout << "Memory access random time: " << memory_access_random(num_operations) / 100000.0 -  base << " ns/kops\n";
  return 0;
}
