# ifth::types

This module contains the code of basic auxiliary types that are missing in the current standard library.

The box is memory control primitive. It has functionality similar to unique_ptr, but it doesn't call a delete, instead of this it calls destroy_at at destructor. It is useful with manually memory placed objects. [box](include/types/box.h)

```cpp
namespace ihft::types
{

template<typename Type>
class box final
```

The type of delegate for calling a function over an interface without ownership. It provides a similar call price, but significantly benefits in the speed of construction, because it doesn't pack an extra data in a local buffer or heap. [function_ref](include/types/function_ref.h)

```cpp
namespace ihft::types
{

template<typename Ret, typename ... Params>
class function_ref<Ret(Params...)> final
```

The type of function result. It contains result type or error type and provides cozy interface. [result](include/types/result.h)

```cpp
namespace ihft::types
{

template<typename Type, typename Error>
class result final
```

## Benchmarks

[function_ref benchmark](benchmark/benchmark_function_ref.cpp)
