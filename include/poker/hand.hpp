#pragma once

#include "poker/card.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace poker {

// These are the standard poker hand categories from weakest to strongest.
// Keeping them in strength order will make later comparison logic easier to write.
enum class HandCategory {
    high_card,
    pair,
    two_pair,
    three_of_a_kind,
    straight,
    flush,
    full_house,
    four_of_a_kind,
    straight_flush,
};

using HandStrength = std::uint32_t;

// This helper gives us readable text for printing and debugging.
[[nodiscard]] const char* hand_category_name(HandCategory category) noexcept;

// This struct will be the output of our evaluator.
//
// category:
// The broad hand type, such as flush or full house.
//
// tie_break_ranks:
// Extra ranks used to compare two hands in the same category.
// Examples:
// - Pair of aces with K-Q-J kickers
// - Two pair, kings and tens, with ace kicker
//
// tie_break_count:
// How many entries inside tie_break_ranks are meaningful for this hand.
struct EvaluatedHand {
    HandCategory category{HandCategory::high_card};

    // Packed score form of this hand.
    // We keep it alongside the readable fields so the simulator can compare
    // hands quickly without rebuilding tie-break logic on every iteration.
    HandStrength strength{0};

    // A fixed-size array keeps the result simple and avoids dynamic allocation.
    // Five is enough because a 5-card hand can only contribute up to five ordered ranks.
    std::array<Rank, 5> tie_break_ranks{
        Rank::two,
        Rank::two,
        Rank::two,
        Rank::two,
        Rank::two,
    };

    std::size_t tie_break_count{0};
};

// The evaluator will start with exactly 5 cards.
// Using std::array<Card, 5> makes that size requirement explicit in the type itself.
inline constexpr std::size_t five_card_hand_size = 5;
inline constexpr std::size_t seven_card_hand_size = 7;

// This struct stores the raw counts we will use in later steps.
//
// rank_counts[0] -> number of twos
// rank_counts[1] -> number of threes
// ...
// rank_counts[12] -> number of aces
//
// suit_counts[0] -> clubs
// suit_counts[1] -> diamonds
// suit_counts[2] -> hearts
// suit_counts[3] -> spades
struct HandCounts {
    std::array<int, 13> rank_counts{};
    std::array<int, 4> suit_counts{};
};

// Straight detection needs to answer two questions:
// 1. Is this hand a straight?
// 2. If so, what is the highest card in that straight?
//
// Example:
// - T-J-Q-K-A -> is_straight = true, high_rank = ace
// - A-2-3-4-5 -> is_straight = true, high_rank = five
struct StraightInfo {
    bool is_straight{false};
    Rank high_rank{Rank::two};
};

// Count how many times each rank and suit appears in a 5-card hand.
[[nodiscard]] HandCounts count_ranks_and_suits(
    const std::array<Card, five_card_hand_size>& cards) noexcept;

// Detect whether a 5-card hand forms a straight.
[[nodiscard]] StraightInfo detect_straight(const HandCounts& counts) noexcept;

// Evaluate a 5-card poker hand into a comparable result.
[[nodiscard]] EvaluatedHand evaluate_5_card_hand(
    const std::array<Card, five_card_hand_size>& cards) noexcept;

// Fast packed score form used for hot-path comparisons.
// Higher values always mean stronger hands.
[[nodiscard]] HandStrength evaluate_5_card_strength(
    const std::array<Card, five_card_hand_size>& cards) noexcept;

// Evaluate 7 cards by choosing the best possible 5-card hand.
[[nodiscard]] EvaluatedHand evaluate_7_card_hand(
    const std::array<Card, seven_card_hand_size>& cards) noexcept;

[[nodiscard]] HandStrength evaluate_7_card_strength(
    const std::array<Card, seven_card_hand_size>& cards) noexcept;

// Compare two evaluated hands.
// Returns:
// -1 if left is weaker
//  0 if they are equal
//  1 if left is stronger
[[nodiscard]] int compare_evaluated_hands(
    const EvaluatedHand& left,
    const EvaluatedHand& right) noexcept;

}  // namespace poker
