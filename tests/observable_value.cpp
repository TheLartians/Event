#include <catch2/catch.hpp>

#include <lars/observable_value.h>

// instantiate templates for coverage
template class lars::ObservableValue<int>;
template class lars::DependentObservableValue<int,int,int>;

TEST_CASE("Observable Value", "[observable_value]") {
  using namespace lars;

  ObservableValue value(0);

  unsigned total = 0;
  value.onChange.connect([&](auto &v){ total+=v; });
  REQUIRE(*value == 0);
  value.set(1);
  value.set(2);
  REQUIRE(total == 3);
  REQUIRE(*value == 2);
}

TEST_CASE("Dependent Observable Value", "[observable_value]") {
  using namespace lars;

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