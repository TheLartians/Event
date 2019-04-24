[![Build Status](https://travis-ci.com/TheLartians/Event.svg?branch=master)](https://travis-ci.com/TheLartians/Event)
[![codecov](https://codecov.io/gh/TheLartians/Event/branch/master/graph/badge.svg)](https://codecov.io/gh/TheLartians/Event)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/2db87434675a4e2d8d1e5f10cd5d2387)](https://app.codacy.com/app/TheLartians/Event?utm_source=github.com&utm_medium=referral&utm_content=TheLartians/Event&utm_campaign=Badge_Grade_Dashboard)

# lars::Event

A thread-safe event-listener template and observable value implementation for C++17.

## Examples

Full examples can be found in the [examples directory](https://github.com/TheLartians/Event/tree/master/examples).

### Using lars::Event

```c++
lars::Event<float,float> onClick;
auto observer = onClick.createObserver([](auto x, auto y){ handleClick(x,y); });
onClick.emit(0,0); // emits event to all observers
observer.reset(); // removes observer from event
```

### Using lars::ObservableValue

```c++
lars::ObservableValue a = 1;
lars::ObservableValue b = 2;
lars::DependentObservableValue sum([](auto a, auto b){ return a+b; },a,b);
sum.onChange.connect([](auto &v){ std::cout << "The result changed to " << r << std::endl; });
std::cout << "The result is " << *sum << std::endl; // -> the result is 3
a.set(3); // -> The result changed to 5
```

## Installation and usage

With [CPM](https://github.com/TheLartians/CPM), lars::Event can be used in a CMake project simply by adding the following to the project's `CMakeLists.txt`.

```cmake
CPMAddPackage(
  NAME LarsEvent
  VERSION 2.0.3
  GIT_REPOSITORY https://github.com/TheLartians/Event.git
)

target_link_libraries(myProject LarsEvent)
```

Alternatively, the repository can be cloned locally and included it via `add_subdirectory`. Installing lars::Event will make it findable in CMake's `find_package`.
