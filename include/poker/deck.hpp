#pragma once

#include "poker/card.hpp"

#include <array>
#include <cstddef>

namespace poker {

class Deck final {
  public:
    static constexpr std::size_t card_count = 52;

    Deck() noexcept;

    void reset() noexcept;

    [[nodiscard]] std::size_t remaining() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    // The Card& parameter is a reference.
    // That means draw() can write a card into the caller's variable.
    //
    // Returns true if a card was drawn.
    // Returns false if the deck is empty.
    bool draw(Card& out_card) noexcept;

  private:
    // std::array<Card, 52> stores exactly 52 Card objects directly inside the Deck.
    // No dynamic allocation is needed.
    std::array<Card, card_count> cards_{};

    // This tracks which card will be drawn next.
    std::size_t next_card_index_{0};
};

}  // namespace poker
