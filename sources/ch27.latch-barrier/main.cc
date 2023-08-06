#include <barrier>
#include <chrono>
#include <format>
#include <iostream>
#include <latch>
#include <thread>

// latch 代表门锁、门闩的意思
// 它代表一种线程计数器，每一个线程到达这个点，就减1，减到0时就不会被block了
void ExampleLatch() {
  constexpr int NUM_THREADS{5};
  std::latch l(NUM_THREADS);
  for (int i{0}; i < NUM_THREADS; ++i) {
    std::thread t([&l, i]() {
      std::cout << std::format("Thread: {} reached.", i) << std::endl;
      l.count_down();
    });
    t.detach();
  }
  l.wait();
  std::cout << "All thread finished" << std::endl;
}

// barrier
// 代表一种同步屏障，在屏障位置，所有线程都被block，一直到所有线程都同步到达后
// 和latch相比，核心接口都是arrive_and_wait，但latch实际更底层和灵活一些
// barrier抽象层次更高
void ExampleBarrier() {
  constexpr int NUM_THREADS{5};
  std::barrier b(NUM_THREADS);
  std::vector<std::thread> threads(NUM_THREADS);
  for (int i{0}; i < NUM_THREADS; ++i) {
    threads[i] = std::thread([&b, i]() {
      std::cout << std::format("Thread: {} do the pre-work", i) << std::endl;
      b.arrive_and_wait();
      std::cout << std::format("Thread: {} do the mid-work", i) << std::endl;
      b.arrive_and_wait();
      std::cout << std::format("Thread: {} do the post-work", i) << std::endl;
    });
  }
  for (auto &th : threads) {
    th.join();
  }
}

int main() {
  ExampleLatch();
  ExampleBarrier();
}