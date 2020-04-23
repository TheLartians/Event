[![Actions Status](https://github.com/TheLartians/Observe/workflows/MacOS/badge.svg)](https://github.com/TheLartians/Observe/actions)
[![Actions Status](https://github.com/TheLartians/Observe/workflows/Windows/badge.svg)](https://github.com/TheLartians/Observe/actions)
[![Actions Status](https://github.com/TheLartians/Observe/workflows/Ubuntu/badge.svg)](https://github.com/TheLartians/Observe/actions)
[![Actions Status](https://github.com/TheLartians/Observe/workflows/Style/badge.svg)](https://github.com/TheLartians/Observe/actions)
[![Actions Status](https://github.com/TheLartians/Observe/workflows/Install/badge.svg)](https://github.com/TheLartians/Observe/actions)
[![codecov](https://codecov.io/gh/TheLartians/Observe/branch/master/graph/badge.svg)](https://codecov.io/gh/TheLartians/Observe)

# Observe

A thread-safe event-listener template and observable value implementation for C++17.

## Examples

Full examples can be found in the [examples directory](https://github.com/TheLartians/Observe/tree/master/examples).

### Using observe::Event

```cpp
observe::Event<float,float> onClick;
auto observer = onClick.createObserver([](auto x, auto y){ handleClick(x,y); });
onClick.emit(0,0); // emits event to all observers
observer.reset(); // removes observer from event
```

### Using observe::Value

```cpp
observe::Value a = 1;
observe::Value b = 2;
observe::DependentObservableValue sum([](auto a, auto b){ return a+b; },a,b);
sum.onChange.connect([](auto &v){ std::cout << "The result changed to " << r << std::endl; });
std::cout << "The result is " << *sum << std::endl; // -> the result is 3
a.set(3); // -> The result changed to 5
```

## Installation and usage

With [CPM](https://github.com/TheLartians/CPM), observe::Event can be used in a CMake project simply by adding the following to the project's `CMakeLists.txt`.

```cmake
CPMAddPackage(
  NAME LarsObserve
  VERSION 2.1
  GIT_REPOSITORY https://github.com/TheLartians/Observe.git
)

target_link_libraries(myProject LarsObserve)
```

Alternatively, the repository can be cloned locally and included it via `add_subdirectory`. Installing observe::Event will make it findable in CMake's `find_package`.
