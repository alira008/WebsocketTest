#include "pch.hpp"
// Class to manage the memory to be used for handler-based custom allocation.
// It contains a single block of memory which may be returned for allocation
// requests. If the memory is in use when an allocation request is made, the
// allocator delegates allocation to the global heap.
class MemoryHandler {
 public:
  MemoryHandler() : in_use_(false) {}

  MemoryHandler(const MemoryHandler&) = delete;
  MemoryHandler& operator=(const MemoryHandler&) = delete;

  void* allocate(std::size_t size) {
    if (!in_use_ && size < sizeof(storage_)) {
      in_use_ = true;
      return &storage_;
    } else {
      return ::operator new(size);
    }
  }

  void deallocate(void* pointer) {
    if (pointer == &storage_) {
      in_use_ = false;
    } else {
      ::operator delete(pointer);
    }
  }

 private:
  // Storage space used for handler-based custom memory allocation.
  typename std::aligned_storage<800000>::type storage_;

  // Whether the handler-based custom allocation storage has been used.
  bool in_use_;
};

// The allocator to be associated with the handler objects. This allocator only
// needs to satisfy the C++11 minimal allocator requirements.
template <typename T>
class Allocator {
 public:
  using value_type = T;

  explicit Allocator(MemoryHandler& mem) : memory_(mem) {}

  template <typename U>
  Allocator(const Allocator<U>& other) noexcept : memory_(other.memory_) {}

  bool operator==(const Allocator& other) const noexcept {
    return &memory_ == &other.memory_;
  }

  bool operator!=(const Allocator& other) const noexcept {
    return &memory_ != &other.memory_;
  }

  T* allocate(std::size_t n) const {
    return static_cast<T*>(memory_.allocate(sizeof(T) * n));
  }

  void deallocate(T* p, std::size_t /*n*/) const {
    return memory_.deallocate(p);
  }

 private:
  template <typename>
  friend class Allocator;

  // The underlying memory.
  MemoryHandler& memory_;
};
