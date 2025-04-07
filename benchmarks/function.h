#ifndef FUNCTION_H
#define FUNCTION_H

// Regular function
int regular_add(int a, int b) {
    return a + b;
}

// Inline function
inline int inline_add(int a, int b) {
    return a + b;
}

// Always inline function (GCC-specific)
__attribute__((always_inline)) int always_inline_add(int a, int b) {
    return a + b;
}


// Base class with a virtual function
class Base {
public:
  virtual int virtual_add(int a, int b) {
    return a + b;
  }
};

// Derived class inheriting from Base
class Derived : public Base {
public:
  int virtual_add(int a, int b) override {
    return a + b + 1;
  }
};

// CRTP Base class
template <typename DerivedClass>
class CRTPBase {
public:
  __attribute__((always_inline)) int crtp_function(int a, int b) {
    return static_cast<DerivedClass*>(this)->do_work(a, b);
  }
};

// CRTP Derived class
class CRTPDerived : public CRTPBase<CRTPDerived> {
public:
  __attribute__((always_inline)) int do_work(int a, int b) {
    return a + b;
  }
};

// Indirect function call (through function pointer)
using FunctionPtr = int(*)(int, int);
int indirect_call(FunctionPtr func, int a, int b) {
  return func(a, b);
}


void measure_function_call_overhead(int num_operations) {
    volatile int result = 0;

    // Regular function call
    auto start_regular = high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        result += regular_add(1, 2);
    }
    auto end_regular = high_resolution_clock::now();
    cout << "Regular function call: "
         << static_cast<double>(duration_cast<nanoseconds>(end_regular - start_regular).count()) / static_cast<double>(num_operations/1000.0)
         << " ns per k_call\n";

    // Inline function call
    auto start_inline = high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        result += inline_add(1, 2);
    }
    auto end_inline = high_resolution_clock::now();
    cout << "Inline function call: "
         << static_cast<double>(duration_cast<nanoseconds>(end_inline - start_inline).count()) / static_cast<double>(num_operations/1000.0)
         << " ns per k_call\n";

    // Always inline function call (for GCC)
    auto start_always_inline = high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        result += always_inline_add(1, 2);
    }
    auto end_always_inline = high_resolution_clock::now();
    cout << "Always inline function call (GCC): "
         << static_cast<double>(duration_cast<nanoseconds>(end_always_inline - start_always_inline).count())
         / static_cast<double>(num_operations/1000.0)
         << " ns per k_call\n";

}



void measure_vfunc(int num_operations) {
  // Measuring direct call overhead
  volatile int result = 0;

  auto start_direct = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    result = regular_add(1, 2);
  }
  auto end_direct = high_resolution_clock::now();
  cout << "Direct function call: "
       << static_cast<double>(duration_cast<nanoseconds>(end_direct - start_direct).count())
       / static_cast<double>(num_operations/1000)
       << " ns per k_call\n";

  // Measuring indirect call overhead
  auto start_indirect = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
     result += indirect_call(regular_add, 1, 2);
  }
  auto end_indirect = high_resolution_clock::now();
  cout << "Indirect function call: "
       << static_cast<double>(duration_cast<nanoseconds>(end_indirect - start_indirect).count())
       / static_cast<double>(num_operations/1000.0)
       << " ns per k_call\n";

  // Measuring virtual call overhead
  Base* obj = new Derived();
  auto start_virtual = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    result += obj->virtual_add(1, 2);
  }
  auto end_virtual = high_resolution_clock::now();
  cout << "Virtual function call: "
       << static_cast<double>(duration_cast<nanoseconds>(end_virtual - start_virtual).count())
       / static_cast<double>(num_operations/1000.0)
       << " ns per k_call\n";
  delete obj;

  // Measuring CRTP call overhead.
  // 奇异递归模板模式：
  CRTPDerived crtp_obj;
  auto start_crtp = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    result += crtp_obj.crtp_function(1, 2);
  }
  auto end_crtp = high_resolution_clock::now();
  cout << "CRTP function call: "
       << static_cast<double>(duration_cast<nanoseconds>(end_crtp - start_crtp).count())
       / static_cast<double>(num_operations/1000.0)
       << " ns per k_call\n";
}


#endif //FUNCTION_H
