#include <iostream>
#include <chrono>

#include "allocation.h"
#include "hardware.h"
#include "function.h"
#include "practice.h"

using namespace std;
using namespace std::chrono;

// Main function
int main() {
  constexpr int num_operations = 100000;
  test_all_hardware_related(num_operations);
  measure_function_call_overhead(num_operations);
  measure_vfunc(num_operations);
  test_allocation(num_operations);
  benchmark_map_continous();
  return 0;
}
