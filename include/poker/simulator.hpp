#pragma once

namespace poker {

class MonteCarloSimulator final {
  public:
    // Returns a short message describing the current simulator state.
    //
    // const:
    // Calling this function does not change the simulator object.
    //
    // noexcept:
    // This function promises not to throw exceptions.
    [[nodiscard]] const char* status() const noexcept;
};

}  // namespace poker
