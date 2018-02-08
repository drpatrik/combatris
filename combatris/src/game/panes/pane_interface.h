#pragma once

class PaneInterface {
 public:
  virtual ~PaneInterface() noexcept {}
  virtual void Render() const = 0;
  virtual void Reset() = 0;
};
