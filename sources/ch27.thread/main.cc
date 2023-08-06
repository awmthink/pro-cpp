#include <format>
#include <iostream>
#include <thread>

void CounterFunction(int id, int num_iterations) {
  for (int i{}; i < num_iterations; ++i) {
    std::cout << std::format("Counter {} has value {}\n", id, i) << std::flush;
  }
}

void CreateThreadWithFunctionPointer() {
  std::thread t{CounterFunction, 1, 2};
  t.join();
}

class CounterFunctor {
 public:
  CounterFunctor(int id, int num_iterations)
      : id_(id), num_iterations_(num_iterations) {}
  void operator()() const {
    for (int i{}; i < num_iterations_; ++i) {
      std::cout << std::format("Counter {} has value {}\n", id_, i)
                << std::flush;
    }
  }

 private:
  int id_;
  int num_iterations_;
};

void CreateThreadWithFunctionObject() {
  std::thread t{CounterFunctor{1, 6}};
  t.join();
}

void CreateThreadWithLambda() {
  int id{1};
  int num_iterations{5};
  auto counter = [id, num_iterations]() {
    for (int i{}; i < num_iterations; ++i) {
      std::cout << std::format("Counter {} has value {}\n", id, i)
                << std::flush;
    }
  };
  std::thread t{counter};
  t.join();
}

class Widget {
 public:
  Widget(int id, int num_iterations)
      : id_(id), num_iterations_(num_iterations) {}
  void Counter() const {
    for (int i{}; i < num_iterations_; ++i) {
      std::cout << std::format("Counter {} has value {}\n", id_, i)
                << std::flush;
    }
  }

 private:
  int id_;
  int num_iterations_;
};

void CreateThreadWithMemberFunction() {
  Widget w{1, 5};
  std::thread t{&Widget::Counter, &w};
  t.join();
}

int main() {
  CreateThreadWithFunctionPointer();
  CreateThreadWithFunctionObject();
  CreateThreadWithLambda();
}