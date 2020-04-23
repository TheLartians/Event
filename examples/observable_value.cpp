/**
 * A small example that shows how to create and use Value and DependentObservableValue.
 */

#include <observe/value.h>

#include <iostream>
#include <string>

int main() {
  using namespace observe;

  Value a = 1;
  a.onChange.connect([](auto v) { std::cout << "a changed to " << v << std::endl; });

  Value b = 2;
  b.onChange.connect([](auto v) { std::cout << "b changed to " << v << std::endl; });

  Value c = 3;
  c.onChange.connect([](auto v) { std::cout << "c changed to " << v << std::endl; });

  DependentObservableValue sum([](auto a, auto b) { return a + b; }, a, b);
  DependentObservableValue prod([](auto a, auto b) { return a * b; }, sum, c);

  DependentObservableValue resultString([](auto v) { return "The result is " + std::to_string(v); },
                                        prod);
  resultString.onChange.connect([](auto &r) { std::cout << r << std::endl; });

  std::cout << resultString.get() << std::endl;

  a.set(2);
  b.set(3);
  c.set(4);
}