#include "poker/deck.hpp"

namespace poker {

Deck::Deck() noexcept
{
    reset();
}

void Deck::reset() noexcept
{
    // size_t is preferred when the value represent a size, count, or index
    // cannot be negative, and large enough to preresent the size of objects in 
    // memory on the current platform. 
    // We use it instead of int, since int can be negative, and may not match
    // the type returned by standard library container sizes.
    std::size_t index = 0;

    // Build the deck in a predictable order:
    // all clubs, then all diamonds, then all hearts, then all spades.
    for (Suit suit : all_suits) {
        for (Rank rank : all_ranks) {
            cards_[index] = Card{rank, suit};
            ++index;
        }
    }

    next_card_index_ = 0;
}

std::size_t Deck::remaining() const noexcept
{
    return card_count - next_card_index_;
}

bool Deck::empty() const noexcept
{
    return next_card_index_ >= card_count;
}

bool Deck::draw(Card& out_card) noexcept
{
    if (empty()) {
        return false;
    }

    out_card = cards_[next_card_index_];
    ++next_card_index_;
    return true;
}

}  // namespace poker
