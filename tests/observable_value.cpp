#include <catch2/catch.hpp>

#include <lars/observable_value.h>

  using namespace lars;

// instantiate templates for coverage
template class lars::ObservableValue<int>;
template class lars::DependentObservableValue<int,int,int>;

TEST_CASE("ObservableValue", "[observable_value]") {
  int current = 0;
  ObservableValue value(current);
  unsigned changes = 0;
  value.onChange.connect([&](auto &v){ 
    REQUIRE(current == v); 
    changes++; 
  });
  REQUIRE(*value == 0);
  REQUIRE(static_cast<const int &>(value) == 0);
  current++;
  value.set(current);
  value.set(current);
  value.set(current);
  current++;
  value.set(current);
  value.set(current);
  REQUIRE(changes == 2);
  REQUIRE(*value == 2);
}

TEST_CASE("ObservableValue without comparison operator", "[observable_value]") {
  struct A{ };
  ObservableValue<A> value;
  unsigned changes = 0;
  value.onChange.connect([&](auto &){ changes++; });
  value.set();
  value.set();
  value.set();
  REQUIRE(changes == 3);
}

TEST_CASE("Dependent Observable Value", "[observable_value]") {
  ObservableValue a(1);
  ObservableValue b(1);
  DependentObservableValue sum([](auto a, auto b){ return a+b; },a,b);

  REQUIRE(*sum == 2);
  a.set(2);
  REQUIRE(*sum == 3);
  b.set(3);
  REQUIRE(*sum == 5);

  ObservableValue c(3);
  DependentObservableValue prod([](auto a, auto b){ return a*b; },sum,c);

  REQUIRE(*prod == 15);
  a.set(1);
  REQUIRE(*prod == 12);
  b.set(4);
  REQUIRE(*prod == 15);
  c.set(2);
  REQUIRE(*prod == 10);

  c.setSilently(3);
  REQUIRE(*prod == 10);
}

TEST_CASE("Operators", "[observable_value]") {
  using namespace lars;
  struct A { int a = 0; };
  ObservableValue<A> value;
  REQUIRE(value->a == 0);
}