#pragma once

#include <utility>

// https://dev.krzaq.cc/post/you-dont-need-a-stateful-deleter-in-your-unique_ptr-usually/

template <typename T, T *func> struct function_caller {
  template <typename... Args> auto operator()(Args &&... arg) const {
    return func(std::forward<Args...>(arg...));
  }
};
