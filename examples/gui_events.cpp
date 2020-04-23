/**
 * A small example that shows how to create and use the Event class.
 */

#include <observe/event.h>

#include <iostream>

using ClickEvent = observe::Event<float, float>;

struct GuiElement {
  ClickEvent clicked;
  void mouse_down(float x, float y) { clicked.emit(x, y); }
};

int main() {
  using namespace std;

  GuiElement A, B;
  ClickEvent::Observer observer_1;
  // this observer can observe any type of event
  observe::Observer observer_2;

  observer_1.observe(A.clicked, [](float x, float y) {
    cout << "observer 1 : A clicked at " << x << ", " << y << endl;
  });

  observer_2.observe(B.clicked, [](float x, float y) {
    cout << "observer 2 : B clicked at " << x << ", " << y << endl;
  });

  // anonymous observer
  B.clicked.connect(
      [](float x, float y) { cout << "   : B clicked at " << x << ", " << y << endl; });

  do {
    auto temporary_observer = A.clicked.createObserver(
        [](float x, float y) { cout << "tmp observer: A clicked at " << x << ", " << y << endl; });

    cout << "A has two observers, B has two observers" << endl;
    A.mouse_down(1, 0);
    B.mouse_down(0, 1);
  } while (false);

  observer_2 = move(observer_1);

  cout << "A has one observer, B has one observer" << endl;
  A.mouse_down(2, 0);
  B.mouse_down(0, 2);
}
