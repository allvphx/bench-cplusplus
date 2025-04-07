#include "./benchmarks/allocation.h"
#include "./benchmarks/hardware.h"
#include "./benchmarks/function.h"
#include "./practices/map.h"

using namespace std;
using namespace std::chrono;

// Main function
int main() {
  constexpr int num_operations = 100000000;
//  test_all_hardware_related(num_operations);
  measure_function_call_overhead(num_operations);
//  test_allocation(num_operations);
//  benchmark_map();
  return 0;
}
