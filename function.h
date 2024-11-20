#ifndef FUNCTION_H
#define FUNCTION_H

// Regular function
// push (a, b) --> address int 返回地址 -->
// jump 函数代码 regular_add --> (2xMOV, ADD) result --> push address
// 1. 内存访问，不连续
// 2. 堆栈
// cpu 25 --- 250, 参数。
// cpu 15 --- 30
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
// index hash ---> 函数位置

// Derived class inheriting from Base
class Derived : public Base {
public:
  int virtual_add(int a, int b) override {
    return a + b + 1;
  }
};

// Derived class inheriting from Base
class Derived1 : public Base {
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
// 间接调用
// push (a, b) --> address int 返回地址 --> lookup 函数指针 -->
// jump 函数代码 regular_add --> (2xMOV, ADD) result --> push address
using FunctionPtr = int(*)(int, int);
int indirect_call(FunctionPtr func, int a, int b) {
  return func(a, b);
}


void measure_function_call_overhead(int num_operations) {
    volatile int result = 0;

    // Regular function call
    auto start_regular = high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        result = regular_add(1, 2);
    }
    auto end_regular = high_resolution_clock::now();
    cout << "Regular function call: "
         << duration_cast<nanoseconds>(end_regular - start_regular).count() / static_cast<double>(num_operations/1000)
         << " ns per kcall\n";

    // Inline function call
    auto start_inline = high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        result = inline_add(1, 2);
    }
    auto end_inline = high_resolution_clock::now();
    cout << "Inline function call: "
         << duration_cast<nanoseconds>(end_inline - start_inline).count() / static_cast<double>(num_operations/1000)
         << " ns per kcall\n";

    // Always inline function call (for GCC)
    auto start_always_inline = high_resolution_clock::now();
    for (int i = 0; i < num_operations; ++i) {
        result = always_inline_add(1, 2);
    }
    auto end_always_inline = high_resolution_clock::now();
    cout << "Always inline function call (GCC): "
         << duration_cast<nanoseconds>(end_always_inline - start_always_inline).count() / static_cast<double>(num_operations/1000)
         << " ns per kcall\n";

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
       << duration_cast<nanoseconds>(end_direct - start_direct).count() / static_cast<double>(num_operations/1000)
       << " ns per kcall\n";

  // Measuring indirect call overhead
  auto start_indirect = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
     result = indirect_call(regular_add, 1, 2);
  }
  auto end_indirect = high_resolution_clock::now();
  cout << "Indirect function call: "
       << duration_cast<nanoseconds>(end_indirect - start_indirect).count() / static_cast<double>(num_operations/1000)
       << " ns per kcall\n";

  // Measuring virtual call overhead
  Base* obj = new Derived();
  auto start_virtual = high_resolution_clock::now();
  // 1. VMT ---> 函数  4 cpu  ---> 12 13 cpu. 50%, 100%.
  // 2. push a, b
  // 3. address
  for (int i = 0; i < num_operations; ++i) {
    result = obj->virtual_add(1, 2);
  }
  auto end_virtual = high_resolution_clock::now();
  cout << "Virtual function call: "
       << duration_cast<nanoseconds>(end_virtual - start_virtual).count() / static_cast<double>(num_operations/1000)
       << " ns per kcall\n";
  delete obj;

  // Measuring CRTP call overhead.
  // 奇异递归模板模式：
  CRTPDerived crtp_obj;
  auto start_crtp = high_resolution_clock::now();
  for (int i = 0; i < num_operations; ++i) {
    result = crtp_obj.crtp_function(1, 2);
    /// 2 CRTPBASE  1. P1 2 P2.
    // 1. 编译时实例化 derived 占位符 --> base。
    // 2. push a, b
    // 3. 静态类型确认 --> derived
    // 4. 函数指针 --> 派生类
  }
  auto end_crtp = high_resolution_clock::now();
  cout << "CRTP function call: "
       << duration_cast<nanoseconds>(end_crtp - start_crtp).count() / static_cast<double>(num_operations/1000)
       << " ns per kcall\n";
}


#endif //FUNCTION_H
