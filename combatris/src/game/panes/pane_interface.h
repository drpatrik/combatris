#pragma once

class PaneInterface {
 public:
  virtual ~PaneInterface() noexcept {}
  virtual void Render(double) = 0;
  virtual void Reset() = 0;
};
