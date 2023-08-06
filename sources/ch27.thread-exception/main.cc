#include <exception>
#include <stdexcept>
#include <thread>

void ThreadFunc() { throw std::runtime_error("Hello"); }

void WrapperFunc(std::exception_ptr &err) {
  try {
    ThreadFunc();
  } catch (...) {
    err = std::current_exception();
  }
}

int main() {
  std::exception_ptr err;
  std::thread t(WrapperFunc, std::ref(err));
  t.join();
  if (err) {
    std::rethrow_exception(err);
  }

  return 0;
}