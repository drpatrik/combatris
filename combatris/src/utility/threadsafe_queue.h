#pragma once

#include <atomic>
#include <queue>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() : abort_(false), size_(0) {}

  ThreadSafeQueue(const ThreadSafeQueue&) = delete;

  ~ThreadSafeQueue() noexcept { Cancel(); }

  void Push(T new_value) {
    std::unique_lock<std::mutex> lock(mutex_);

    queue_.push(std::move(new_value));
    size_ = queue_.size();
    lock.unlock();
    event_.notify_one();
  }

  bool Pop(T& value) {
    value = std::move(Pop());

    return (is_cancelled()) ? false : true;
  }

  T Pop() {
    std::unique_lock<std::mutex> lock(mutex_);

    event_.wait(lock, [this] { return !queue_.empty() || abort_; });

    if (is_cancelled()) {
      return T();
    }
    auto value(std::move(queue_.front()));

    queue_.pop();
    size_ = queue_.size();

    return value;
  }

  void Cancel() noexcept {
    std::unique_lock<std::mutex> lock(mutex_);

    if (is_cancelled()) {
      return;
    }
    abort_.store(true, std::memory_order_release);
    lock.unlock();
    event_.notify_all();
  }

  inline bool is_cancelled() const { return abort_.load(std::memory_order_acquire); }

  inline size_t size() const { return size_; }

  inline bool empty() const { return 0 == size_; }

 private:
  mutable std::mutex mutex_;
  std::condition_variable event_;
  std::queue<T> queue_;
  std::atomic<bool> abort_;
  size_t size_;
};
