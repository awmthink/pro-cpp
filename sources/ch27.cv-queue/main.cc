// 本示例演示了使用condition vairables来实现一个线程安全的队列

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>

template <typename T>
class CVQueue {
 public:
  void Push(const T& item) {
    std::unique_lock lock(mut_);
    queue_.push(item);
    lock.unlock();
    cv_.notify_all();
  }

  void NoJob() {
    no_job_ = true;
    // 同时需要唤醒那些阻塞在Pop上的线程
    cv_.notify_all();
  }

  std::optional<T> Pop() {
    std::unique_lock lock(mut_);
    // 在cv_上等待被唤醒，同时要求队列里有item或者没有生产了
    cv_.wait(lock, [this] { return !this->queue_.empty() || this->no_job_; });
    // 当被唤醒时，需要检查一下，是否是因为没有生产者了
    if (this->queue_.empty() && no_job_) {
      return std::nullopt;
    }
    // 如果队列中有内容，则进行pop
    auto value = queue_.front();
    queue_.pop();
    return value;
  }

 private:
  std::mutex mut_;
  std::condition_variable cv_;
  std::queue<T> queue_;
  std::atomic_bool no_job_{false};
};

int main() {
  CVQueue<std::string> q;
  // 生产5个Item
  std::thread producer = std::thread([&q] {
    for (int i{0}; i < 5; ++i) {
      q.Push("Item: " + std::to_string(i));
    }
    q.NoJob();
  });
  // 消费10次，第5次因为队列空了，同时NoJob了，会返回
  std::thread consumer = std::thread([&q] {
    for (int i{0}; i < 10; ++i) {
      auto item = q.Pop();
      if (item) {
        std::cout << item.value() << std::endl;
      } else {
        std::cout << "No more Job!" << std::endl;
        break;
      }
    }
  });

  producer.join();
  consumer.join();

  return 0;
}