#include <observe/event.h>

#include <iostream>
#include <string>

int main() {
  // events can be valueless
  observe::Event<> eventA;

  // or have arguments
  observe::Event<std::string, float> eventB;

  // connect will always trigger when an event is triggered
  eventA.connect([]() { std::cout << "A triggered" << std::endl; });

  // observers will remove themselves from the event on destroy or reset
  observe::Observer observer = eventB.createObserver([](const std::string &str, float v) {
    std::cout << "B triggered with " << str << " and " << v << std::endl;
  });

  // call emit to trigger all observers
  eventA.emit();
  eventB.emit("meaning of life", 42);

  // `observe::Observer` can store any type of observer
  observer.observe(eventA, []() { std::cout << "I am now observing A" << std::endl; });

  // to remove an observer without destroying the object, call reset
  observer.reset();
}
