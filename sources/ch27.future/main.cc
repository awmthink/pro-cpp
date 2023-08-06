#include <future>
#include <iostream>
#include <vector>

template <typename F, typename... Args>
void doWork(std::promise<int> thePromise, F f, Args... args) {
  try {
    auto value = std::forward<F &>(f)(std::forward<Args &>(args)...);
    thePromise.set_value(value);
  } catch (...) {
    thePromise.set_exception(std::current_exception());
  }
}

template <typename F, typename... Args>
std::promise<int> PackTask(F f, Args... args) {
  std::promise<int> thePromise;
  try {
    auto value = std::forward<F &>(f)(std::forward<Args &>(args)...);
    thePromise.set_value(value);
  } catch (...) {
    thePromise.set_exception(std::current_exception());
  }
  return thePromise;
}

template <typename F, typename... Args>
std::future<int> MyLaunch(F f, Args... args) {
  std::promise<int> thePromise;
  std::thread(doWork, std::move(thePromise), std::forward<F &>(f),
              std::forward<Args &>(args)...);
  return thePromise.get_future();
}

int DoAsyncWork(int i) {
  if (i == 42) {
    throw std::runtime_error("42");
  }
  return i;
}

void GetReturnValue() {
  int input = 2333;
  auto fut = std::async(DoAsyncWork, input);
  if (fut.get() != input) {
    std::cout << "Got unexpected value from thread!" << std::endl;
  }
  std::cout << "=> GetReturnValue: pass" << std::endl;
}

// 如果线程中发生了异常，get接口里对异常进行了rethrow
// 底层其实也是通过rethrow_exception了exception_ptr来实现的
void CatchExceptionFromThread() {
  int input = 42;
  auto fut = std::async(DoAsyncWork, input);
  fut.wait();
  try {
    fut.get();
  } catch (std::runtime_error &e) {
    std::cout << "=> CatchExceptionFromThread: pass" << std::endl;
  }
}

void DeferedTask() {
  auto task = []() { return std::this_thread::get_id(); };
  auto fut = std::async(std::launch::deferred, task);
  auto thread_id = fut.get();
  if (thread_id != std::this_thread::get_id()) {
    std::cout << "=> DeferedTask: error" << std::endl;
  }
  std::cout << "=> DeferedTask: pass" << std::endl;
}

void OutOfMaxUserProcesses() {
  using namespace std::literals::chrono_literals;
  int num_threads = 100000;
  std::vector<std::thread> threads(num_threads);
  try {
    for (auto &&thrd : threads) {
      thrd = std::thread([]() { std::this_thread::sleep_for(500ms); });
    }
  } catch (std::system_error &err) {
    // std::cout << err.what() << std::endl; // Resource temporarily unavailable
    std::cout << "=> OutOfMaxUserProcesses: pass" << std::endl;
  }
  for (auto &&thrd : threads) {
    if (thrd.joinable()) thrd.join();
  }
}

// 通过wait_for可以获取future当前的状态
// timeout（结果未返回，超时了）、ready（结果已经返回）、defereed
void TestFutureStatus() {
  using namespace std::literals::chrono_literals;
  auto f = []() { std::this_thread::sleep_for(500ms); };
  auto fut = std::async(std::launch::async, f);
  auto status = fut.wait_for(50ms);
  if (status != std::future_status::timeout) {
    return;
  }
  status = fut.wait_for(1s);
  if (status != std::future_status::ready) {
    return;
  }
  auto fut_defered = std::async(std::launch::deferred, f);
  if (fut_defered.wait_for(0s) != std::future_status::deferred) {
    return;
  }
  std::cout << "=> TestFutureStatus: pass" << std::endl;
}

// 通过std::async启动异步任务返回的future，关联了一个
// 分配在堆上的共享状态 SharedState，对于这样的future在
// 析构时会阻塞以等待任务完成，也就是会隐匿的join底层的线程
void FutureAutoDetor() {
  using namespace std::literals::chrono_literals;
  auto f = []() {
    std::this_thread::sleep_for(1s);
    std::cout << "=> FutureAutoDetor: pass" << std::endl;
  };
  auto fut = std::async(std::launch::async, f);
  // fut析构时，会自动等待线程执行完毕
}

void UseFutureToNotifyOnce() {
  using namespace std::literals::chrono_literals;
  std::promise<void> p;
  auto thrd = std::thread([&p]() {
    std::this_thread::sleep_for(500ms);
    p.set_value();
  });
  p.get_future().wait();
  thrd.join();
  std::cout << "=> UseFutureToNotifyOnce: pass" << std::endl;
}

int main() {
  GetReturnValue();
  CatchExceptionFromThread();
  DeferedTask();
  OutOfMaxUserProcesses();
  TestFutureStatus();
  FutureAutoDetor();
  UseFutureToNotifyOnce();
  return 0;
}