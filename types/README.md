# ifth::types

This module contains the code of basic auxiliary types that are missing in the current standard library.

Type of function result. It contains result type or error type and provides cozy interface.

```
namespace ihft::types
{

template<typename TYPE, typename ERROR>
class result final
```

The type of delegate for calling a function over an interface without ownership. It provides a similar call price, but significantly benefits in the speed of construction, because it doesn't pack an extra data in a local buffer or heap.

```
namespace ihft::types
{

template<typename Ret, typename ... Params>
class function_ref<Ret(Params...)> final
```
