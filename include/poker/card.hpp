#pragma once

namespace poker {

// enum class creates a strongly-typed enum.
// That means Suit and Rank are their own types instead of plain integers.
enum class Suit {
    clubs,
    diamonds,
    hearts,
    spades,
};

enum class Rank {
    two = 2,
    three = 3,
    four = 4,
    five = 5,
    six = 6,
    seven = 7,
    eight = 8,
    nine = 9,
    ten = 10,
    jack = 11,
    queen = 12,
    king = 13,
    ace = 14,
};

// A struct groups related data together.
// A poker card is just a rank and a suit.
struct Card {
    Rank rank;
    Suit suit;
};

// These helper functions let the CLI turn enum values into printable text.
[[nodiscard]] const char* suit_name(Suit suit) noexcept;
[[nodiscard]] const char* rank_name(Rank rank) noexcept;
[[nodiscard]] char suit_symbol(Suit suit) noexcept;
[[nodiscard]] char rank_symbol(Rank rank) noexcept;

}  // namespace poker
