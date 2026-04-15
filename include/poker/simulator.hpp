#pragma once

#include <string_view>

namespace poker {

class MonteCarloSimulator final {
  public:
    [[nodiscard]] std::string_view status() const noexcept;
};

}  // namespace poker
