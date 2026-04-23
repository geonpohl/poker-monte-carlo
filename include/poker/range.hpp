#pragma once

#include "poker/card.hpp"

#include <array>
#include <cstddef>

namespace poker {

inline constexpr std::size_t hole_cards_per_combo = 2;
inline constexpr std::size_t max_combos_per_range_token = 16;
inline constexpr std::size_t max_dead_cards_for_range_filter = 9;

// A concrete two-card starting hand.
// We keep this separate from simulation input because ranges will eventually
// need to hold many possible hole-card combinations.
struct HoleCards {
    std::array<Card, hole_cards_per_combo> cards{
        Card{Rank::two, Suit::clubs},
        Card{Rank::three, Suit::clubs},
    };

    bool operator==(const HoleCards&) const = default;
};

// One parsed starting-hand token such as:
// - QQ
// - AKs
// - AQo
struct StartingHandPattern {
    Rank high_rank{Rank::ace};
    Rank low_rank{Rank::king};
    bool suited_only{false};
    bool offsuit_only{false};
};

// A fixed-size result keeps this learning step simple and allocation-free.
// The largest combo count we need for a single basic token right now is 12
// (for offsuit hands), so 16 gives us a small amount of headroom.
struct ExpandedRangeToken {
    std::array<HoleCards, max_combos_per_range_token> combos{};
    std::size_t combo_count{0};
};

[[nodiscard]] bool hole_cards_have_duplicate_card(const HoleCards& hole_cards) noexcept;
[[nodiscard]] bool starting_hand_pattern_is_pair(const StartingHandPattern& pattern) noexcept;
[[nodiscard]] bool hole_cards_overlap_dead_cards(
    const HoleCards& hole_cards,
    const std::array<Card, max_dead_cards_for_range_filter>& dead_cards,
    std::size_t dead_card_count) noexcept;

// Parse a single starting-hand token into a normalized pattern.
// Current supported forms:
// - QQ
// - AKs
// - AQo
bool parse_starting_hand_pattern(const char* text, StartingHandPattern& out_pattern) noexcept;

// Expand one parsed pattern into its concrete two-card combinations.
[[nodiscard]] ExpandedRangeToken expand_starting_hand_pattern(
    const StartingHandPattern& pattern) noexcept;

// Convenience helper that parses and expands in one call.
bool expand_range_token(const char* text, ExpandedRangeToken& out_range) noexcept;

// Remove any concrete combos blocked by known cards such as villain hole cards
// or known board cards.
[[nodiscard]] ExpandedRangeToken filter_range_token_dead_cards(
    const ExpandedRangeToken& range,
    const std::array<Card, max_dead_cards_for_range_filter>& dead_cards,
    std::size_t dead_card_count) noexcept;

}  // namespace poker
