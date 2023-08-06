#include <atomic>
#include <chrono>
#include <format>
#include <iostream>
#include <thread>

class Foo {
 private:
  int m_array[123];
};
class Bar {
 private:
  int m_int;
};

void TestLockFree() {
  std::atomic<Foo> f;
  auto fmt = std::format("[Foo] Is trivially copyable: {}, Is lock free: {}",
                         std::is_trivially_copyable_v<Foo>,
                         std::atomic<Foo>::is_always_lock_free);
  std::cout << fmt << std::endl;
  fmt = std::format("[Bar] Is trivially copyable: {}, Is lock free: {}",
                    std::is_trivially_copyable_v<Bar>,
                    std::atomic<Bar>::is_always_lock_free);
  std::cout << fmt << std::endl;
}

void AtomicNotify() {
  std::atomic_bool flag{false};
  std::thread t([&flag]() {
    std::cout << "Thread starts waiting." << std::endl;
    // wait当block当前的线程，只到其他的线程有notify_one或nofity_all
    // 而且需要wait(Oldvalue)里的OldValue产生变化，不再等于OldValue
    flag.wait(false);
    std::cout << "Thread wakes up, flag = " << flag << std::endl;
  });
  using namespace std::literals::chrono_literals;
  std::this_thread::sleep_for(100ms);
  flag = true;
  flag.notify_all();

  t.join();
}

int main() {
  TestLockFree();
  AtomicNotify();

  return 0;
}