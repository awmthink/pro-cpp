// 本示例用于演示使用双重检查锁来保证资源只被初始化一次的用法
// 该版本为一个正确实现的版本，因为使用原子变量来保证内存访问的顺序

// 关于双重检查锁的原始实现问题，以及一些其他解决方法（比如call_once，magic
// static等） 可以参考：
// https://www.yuque.com/awmthink/lt2k94/gzz65ufnxls6u8gx?singleDoc#

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

void initializeSharedResources() {
  std::cout << "Shared resources initialized." << std::endl;
}

std::atomic<bool> g_initialzed{false};
std::mutex g_mutex;

void processingFunction() {
  if (!g_initialzed) {
    std::lock_guard lock{g_mutex};
    if (!g_initialzed) {
      initializeSharedResources();
      // 这里g_initialized必须为atomic的，防止这里的2条
      // 语句被编译器重排。如果重排了，那么另一个线程发现g_initialized为true了
      // 但实际上资源还没有初始化好
      g_initialzed = true;
    }
  }
  std::cout << "Use the Resources" << std::endl;
}

int main() {
  std::vector<std::thread> threads;
  for (int i{0}; i < 5; ++i) {
    threads.emplace_back(processingFunction);
  }
  for (auto &t : threads) {
    t.join();
  }
}
