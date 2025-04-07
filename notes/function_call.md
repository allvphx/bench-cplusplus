# Function Call Overhead Benchmark

This project provides a benchmark comparison of different C++ function call mechanisms, focusing on their performance characteristics. It helps developers understand the overhead introduced by different function types, including regular, inline, virtual, and template-based methods such as CRTP.

## 🔍 Overview

The header file `function.h` defines several kinds of function call styles in C++:

- ✅ **Regular function**: Basic function with external linkage.
- ⚡ **Inline function**: Hint to the compiler to expand the function in-place.
- 🚀 **Always inline function**: Compiler-specific (GCC) enforcement of inlining.
- 🧱 **Virtual function**: Supports runtime polymorphism with virtual table lookup.
- 🧬 **CRTP (Curiously Recurring Template Pattern)**: Compile-time polymorphism with zero runtime overhead.
- 🧭 **Indirect function call**: Calling through function pointers.

The file also provides two measurement APIs:

- `measure_function_call_overhead(int num_operations)`
- `measure_vfunc(int num_operations)`

## 🏗️ Function Types and Cost

| Function Type           | Description                             | Overhead Expected    |
|------------------------|-----------------------------------------|----------------------|
| Regular                | Direct call to a defined function       | Low                  |
| Inline                 | Compiler may replace call with code     | Lowest - Low         |
| Always Inline (GCC)    | Forced inlining                         | Lowest               |
| Indirect               | Via function pointer                    | Moderate             |
| Virtual                | Via vtable pointer                      | High                 |
| CRTP                  | Static dispatch via template trickery   | Comparable to inline |

## 🧪 Example Output

An example output might look like:

```
Regular function call: 25.3 ns per k_call
Inline function call: 14.1 ns per k_call
Always inline function call (GCC): 13.9 ns per k_call
Direct function call: 25.1 ns per k_call
Indirect function call: 35.6 ns per k_call
Virtual function call: 45.8 ns per k_call
CRTP function call: 14.3 ns per k_call
```

*(Note: Actual performance numbers vary by compiler, CPU, and optimization level.)*

## 📖 Function Call Internals & Analysis
We’ve covered low-level hardware aspects in `hardware.h`, let’s examine common software-level abstractions and how much resource they consume in typical C/C++ function calls.

### Regular Function Calls

Regular function calls are the most basic form of function invocation. The typical steps include:
- **1. Push parameters**: Arguments are passed via the stack or registers (depends on calling convention). 
- **2. Push return address**: So that control can return after function completes. 
- **3. PC Jump to function address**. 
- **4. Execute function**. 
- **5. Return and retrieve result via return register**: (e.g., eax on x86).
- **6. Pop stack and return control**: recover PC to the function call place.

```asm
# ./benchmarks/function.h:66:         result += regular_add(1, 2);
	movl	$2, %esi	# step 1
	movl	$1, %edi	# step 1
	call	_Z11regular_addii	# step 2-6
	
_Z11regular_addii:
.LFB3700:
	.cfi_startproc
	endbr64	
	pushq	%rbp	# step 2: Save old pointer as return address
	.cfi_def_cfa_offset 16 # step 3: Setup stack frame.
	.cfi_offset 6, -16
	movq	%rsp, %rbp	# step 3: Jump to func.
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)	# step 4: Retreive a
	movl	%esi, -8(%rbp)	# step 4: Retreive b
# ./benchmarks/function.h:6:     return a + b;
	movl	-4(%rbp), %edx	# step 4: Load a
	movl	-8(%rbp), %eax	# step 4: Load b
	addl	%edx, %eax	# step 4: Compute a+b
# ./benchmarks/function.h:7: }
	popq	%rbp	# step 5 skipped as result is already in %eax.
	                # step 6: pop stack and return control
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
```

This involves stack manipulation and several memory accesses. According to Efficient C++, typical cost is 25–250 CPU cycles, but in practice, calls with few parameters usually take 15–30 cycles.

### Inline Function Calls
Inline functions are expanded at compile-time, meaning function body is copied to call site, and thus:
- No actual call or jump occurs. (step 1, 3, 5, and 6) --- *better code & cache locality*.
- No stack frame is pushed/popped. (step 2, 3 and 6) --- *less commands and stack operations*.
- Only step 4 remains!

```asm
	movl	$1, -176(%rbp)	#, a
	movl	$2, -172(%rbp)	#, b
# ./benchmarks/function.h:16:     return a + b;
	movl	-176(%rbp), %edx	# a, tmp177
	movl	-172(%rbp), %eax	# b, tmp178
	addl	%eax, %edx	# tmp178, D.109316
```

This reduces overhead and improves performance, especially for small and frequently used functions.
However, the inline keyword is just a hint. For critical code paths, use compiler directives:
- GCC: `__attribute__((always_inline))`
- MSVC: `__forceinline`

Inlining can also enable better instruction reordering and improved cache locality.

### Indirect Function Calls

Involves calling through a function pointer, which introduces:
- Pointer dereference overhead. 
- Loss of branch prediction efficiency if the target varies.

Despite CPU predictors improving, misprediction costs can be 10–30 cycles. Overhead comes from:
- Runtime resolution of function address. 
- Jump via pointer. 
- Break in instruction pipeline.

```asm
_Z13indirect_callPFiiiEii:
.LFB3707:
	.cfi_startproc
	.... # the same step 2 - 3 as 
	.cfi_def_cfa_register 6
	subq	$16, %rsp	#,
	movq	%rdi, -8(%rbp)	# func, func
	movl	%esi, -12(%rbp)	# a, a
	movl	%edx, -16(%rbp)	# b, b
# ./benchmarks/function.h:56:   return func(a, b);
	movl	-16(%rbp), %edx	# b, tmp84
	movl	-12(%rbp), %eax	# a, tmp85
	movq	-8(%rbp), %rcx	# func, tmp86
	movl	%edx, %esi	# tmp84,
	movl	%eax, %edi	# tmp85,
	call	*%rcx	# tmp86
# ./benchmarks/function.h:57: }
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
```

### Virtual Function Calls
Used in OOP with inheritance and polymorphism. Key characteristics:
- Each object has a vtable pointer. 
- Function resolution happens at runtime through the vtable. 
- Cost includes vtable lookup + indirect jump.

Measured cost can be 2× that of a direct function call for small functions. Virtual functions have:
- **Additional memory read**: vtable pointer lookup.
- **Branch misprediction risk**: due to vtable, the branch prediction is harder.
- **Poorer inlining opportunities**: since a function code can direct to multiple possible function implementations.


### CRTP (Curiously Recurring Template Pattern)

A compile-time polymorphism technique where a base class is templated on the derived class:
```c++
template <typename Derived>
class Base {
   int do_work() {
       return static_cast<Derived*>(this)->impl();
   }
};
```
#### Benefits: 
- **Zero overhead polymorphism**: during compilation, `Base<Derived>` is instantiated. Thus no need for vtable lookup, etc. As the function resolution is static, there is no runtime dispatch overhead.
- **Inlining-friendly**: can use inline as the code is static cast `static_cast<Derived*>`. 
- **Better cache and optimization potential**: as the code is static, the branch prediction etc. is easier.

#### Limitations:
- **No runtime polymorphism**:  If you need to choose behavior dynamically (based on some runtime condition or input), CRTP won’t work. You can’t, for example, store different CRTP-derived types in the same container and call a common interface method unless you fall back to traditional inheritance or add type erasure.  Bellow is an example:
```c++
template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->impl();
    }
};

class DerivedA : public Base<DerivedA> {
public:
    void impl() { std::cout << "A\n"; }
};

class DerivedB : public Base<DerivedB> {
public:
    void impl() { std::cout << "B\n"; }
};

// This won't work:
std::vector<Base<?>>> list;  // CRTP has no common base you can use like this.
```
You can’t store `DerivedA` and `DerivedB` in a single vector of `Base<?>` like you would with virtual functions.

- **Increased code complexity**: the code less readable compared to those implemented with `virtual`.
- **Risk of code bloat due to template instantiations**: every unique `Base<Derived>` generates a new version of `Base`'s code at compile time. This can make the built binary significantly large.

CRTP is widely used in libraries requiring high performance without runtime cost, such as numeric computation or serialization frameworks.



## 🛠️ How to Use

1. Include `function.h` in your project.
2. In your `main.cpp`, call:

```cpp
#include "function.h"

int main() {
    measure_function_call_overhead(10000000);
    measure_vfunc(10000000);
    return 0;
}
```

3. Compile with optimization flags for meaningful performance data:

```bash
g++ -O3 -std=c++17 main.cpp -o benchmark
./benchmark
```

## 📒 Notes

- The `__attribute__((always_inline))` is specific to GCC/Clang. For MSVC, use `__forceinline`.
- The `volatile` keyword prevents the compiler from optimizing away function calls. We use it to reduce the influence of compiler optimizations during our benchmarking.
- The CRTP pattern achieves zero-cost abstraction by resolving function calls at compile time.

## 📚 References

- [C++ Function Inlining](https://en.cppreference.com/w/cpp/language/inline)
- [Virtual Functions and VTables](https://en.cppreference.com/w/cpp/language/virtual)
- [Function Pointer Basics](https://en.cppreference.com/w/cpp/language/pointer)
- [CRTP Pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)

---
