#pragma once

#include "poker/card.hpp"

#include <array>
#include <cstddef>
#include <random>
#include <utility>

namespace poker {

class Deck final {
  public:
    static constexpr std::size_t card_count = 52;

    Deck() noexcept;

    void reset() noexcept;

    [[nodiscard]] std::size_t remaining() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    // remove() marks a specific card as already used.
    // We will use this in the simulator to remove known hole cards and known board cards.
    bool remove(const Card& card) noexcept;

    // The Card& parameter is a reference.
    // That means draw() can write a card into the caller's variable.
    //
    // Returns true if a card was drawn.
    // Returns false if the deck is empty.
    bool draw(Card& out_card) noexcept;

    template <typename RNG>
    bool draw_random(Card& out_card, RNG& rng) noexcept
    {
        if (empty()) {
            return false;
        }

        // Pick one random card from the remaining range, swap it to the front
        // of that range, then draw it by advancing next_card_index_.
        std::uniform_int_distribution<std::size_t> distribution(next_card_index_, card_count - 1);
        const std::size_t random_index = distribution(rng);

        std::swap(cards_[next_card_index_], cards_[random_index]);
        out_card = cards_[next_card_index_];
        ++next_card_index_;

        return true;
    }

  private:
    // std::array<Card, 52> stores exactly 52 Card objects directly inside the Deck.
    // No dynamic allocation is needed.
    std::array<Card, card_count> cards_{};

    // This tracks which card will be drawn next.
    std::size_t next_card_index_{0};
};

}  // namespace poker
