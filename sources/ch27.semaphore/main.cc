// 本示例用于演示semaphore的用法
// semaphores 信号量，是一种更加轻量的底层同步原语，它可以用于构建
// mutex,latch,barrier等
// semaphores里管理的就是一定数量的资源slot，当我们require时，就取出一个资源，如果没有则block
// 当release则释放一个或多个资源，并通知并block的线程

#include <chrono>
#include <iostream>
#include <semaphore>
#include <string>
#include <thread>

using namespace std::literals::chrono_literals;

// 使用semaphore来控制最大启动的线程数量
void ExampleCounintSemaphores() {
  constexpr std::ptrdiff_t SEM_COUNT = 4;
  std::counting_semaphore sem(SEM_COUNT);
  for (int i{0}; i < 10; ++i) {
    std::thread t([&sem, i]() {
      sem.acquire();
      std::cout << "Thread " + std::to_string(i) + " do work" << std::endl;
      std::this_thread::sleep_for(200ms);
      sem.release();
    });
    t.detach();
  }

  // 预期是每个分割线前，做有4个线程完成了工作（每4个线程属于一批次）
  for (int i{0}; i < 3; ++i) {
    std::cout << "==================" << std::endl;
    std::this_thread::sleep_for(200ms);
  }

  std::this_thread::sleep_for(1s);
}

int main() { ExampleCounintSemaphores(); }