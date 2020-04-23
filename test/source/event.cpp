#include <doctest/doctest.h>
#include <observe/event.h>

using namespace observe;

// instantiate template for coverage
template class observe::Event<>;

TEST_CASE("Event") {
  SUBCASE("connect and observe") {
    observe::Event<> event;
    REQUIRE(event.observerCount() == 0);
    unsigned connectCount = 0, observeCount = 0;
    event.connect([&]() { connectCount++; });

    SUBCASE("reset observer") {
      observe::Event<>::Observer observer;
      observer = event.createObserver([&]() { observeCount++; });
      for (int i = 0; i < 10; ++i) {
        event.emit();
      }
      REQUIRE(event.observerCount() == 2);
      observer.reset();
      REQUIRE(event.observerCount() == 1);
      for (int i = 0; i < 10; ++i) {
        event.emit();
      }
      REQUIRE(observeCount == 10);
      REQUIRE(connectCount == 20);
    }

    SUBCASE("scoped observer") {
      SUBCASE("observe::Observer") {
        observe::Observer observer;
        observer.observe(event, [&]() { observeCount++; });
        REQUIRE(event.observerCount() == 2);
        for (int i = 0; i < 10; ++i) {
          event.emit();
        }
      }
      SUBCASE("observe::Event<>::Observer") {
        observe::Event<>::Observer observer;
        observer.observe(event, [&]() { observeCount++; });
        REQUIRE(event.observerCount() == 2);
        for (int i = 0; i < 10; ++i) {
          event.emit();
        }
      }
      REQUIRE(event.observerCount() == 1);
      for (int i = 0; i < 10; ++i) {
        event.emit();
      }
      REQUIRE(observeCount == 10);
      REQUIRE(connectCount == 20);
    }

    SUBCASE("clear observers") {
      observe::Observer observer = event.createObserver([&]() { observeCount++; });
      event.clearObservers();
      REQUIRE(event.observerCount() == 0);
      event.emit();
      REQUIRE(connectCount == 0);
    }
  }

  SUBCASE("removing observer during emit") {
    observe::Event<> event;
    observe::Event<>::Observer observer;
    unsigned count = 0;
    SUBCASE("self removing") {
      observer = event.createObserver([&]() {
        observer.reset();
        count++;
      });
      event.emit();
      REQUIRE(count == 1);
      event.emit();
      REQUIRE(count == 1);
    }
    SUBCASE("other removing") {
      event.connect([&]() { observer.reset(); });
      observer = event.createObserver([&]() { count++; });
      event.emit();
      REQUIRE(count == 0);
      event.emit();
      REQUIRE(count == 0);
    }
  }

  SUBCASE("adding observers during emit") {
    observe::Event<> event;
    std::function<void()> callback;
    callback = [&]() { event.connect(callback); };
    event.connect(callback);
    REQUIRE(event.observerCount() == 1);
    event.emit();
    REQUIRE(event.observerCount() == 2);
    event.emit();
    REQUIRE(event.observerCount() == 4);
  }

  SUBCASE("emit data") {
    observe::Event<int, int> event;
    int sum = 0;
    event.connect([&](auto a, auto b) { sum = a + b; });
    event.emit(2, 3);
    REQUIRE(sum == 5);
  }

  SUBCASE("move") {
    observe::Observer observer;
    int result = 0;
    observe::Event<int> event;
    {
      observe::Event<int> tmpEvent;
      observer = tmpEvent.createObserver([&](auto i) { result = i; });
      tmpEvent.emit(5);
      REQUIRE(result == 5);
      event = Event(std::move(tmpEvent));
      REQUIRE(tmpEvent.observerCount() == 0);
    }
    REQUIRE(event.observerCount() == 1);
    event.emit(3);
    REQUIRE(result == 3);
    observer.reset();
    REQUIRE(event.observerCount() == 0);
  }
}

TEST_CASE("EventReference") {
  observe::Event<> onA, onB;
  observe::EventReference<> onR(onA);
  unsigned aCount = 0, bCount = 0;
  onR.connect([&]() { aCount++; });
  onA.emit();
  onR = onB;
  onR.connect([&]() { bCount++; });
  onB.emit();
  REQUIRE(aCount == 1);
  REQUIRE(bCount == 1);
}