#include "poker/range.hpp"

#include <cstring>

namespace poker {

namespace {

int rank_value(Rank rank) noexcept
{
    return static_cast<int>(rank);
}

HoleCards make_ordered_hole_cards(Card first_card, Card second_card) noexcept
{
    if (rank_value(first_card.rank) < rank_value(second_card.rank) ||
        (first_card.rank == second_card.rank &&
         static_cast<int>(first_card.suit) > static_cast<int>(second_card.suit))) {
        return HoleCards{{second_card, first_card}};
    }

    return HoleCards{{first_card, second_card}};
}

void push_combo(ExpandedRangeToken& range, const HoleCards& combo) noexcept
{
    if (range.combo_count < range.combos.size()) {
        range.combos[range.combo_count] = combo;
        ++range.combo_count;
    }
}

}  // namespace

bool hole_cards_have_duplicate_card(const HoleCards& hole_cards) noexcept
{
    return hole_cards.cards[0] == hole_cards.cards[1];
}

bool starting_hand_pattern_is_pair(const StartingHandPattern& pattern) noexcept
{
    return pattern.high_rank == pattern.low_rank;
}

bool hole_cards_overlap_dead_cards(
    const HoleCards& hole_cards,
    const std::array<Card, max_dead_cards_for_range_filter>& dead_cards,
    std::size_t dead_card_count) noexcept
{
    for (std::size_t dead_index = 0; dead_index < dead_card_count; ++dead_index) {
        if (hole_cards.cards[0] == dead_cards[dead_index] ||
            hole_cards.cards[1] == dead_cards[dead_index]) {
            return true;
        }
    }

    return false;
}

bool parse_starting_hand_pattern(const char* text, StartingHandPattern& out_pattern) noexcept
{
    if (text == nullptr) {
        return false;
    }

    const std::size_t length = std::strlen(text);
    if (length != 2 && length != 3) {
        return false;
    }

    char first_rank_text[2]{text[0], '\0'};
    char second_rank_text[2]{text[1], '\0'};

    Rank first_rank{};
    Rank second_rank{};

    if (!parse_rank(first_rank_text, first_rank) || !parse_rank(second_rank_text, second_rank)) {
        return false;
    }

    // Normalize so the higher rank always comes first.
    Rank high_rank = first_rank;
    Rank low_rank = second_rank;
    if (rank_value(high_rank) < rank_value(low_rank)) {
        high_rank = second_rank;
        low_rank = first_rank;
    }

    StartingHandPattern pattern{};
    pattern.high_rank = high_rank;
    pattern.low_rank = low_rank;

    if (length == 2) {
        if (!starting_hand_pattern_is_pair(pattern)) {
            return false;
        }

        out_pattern = pattern;
        return true;
    }

    if (starting_hand_pattern_is_pair(pattern)) {
        return false;
    }

    if (text[2] == 's' || text[2] == 'S') {
        pattern.suited_only = true;
    } else if (text[2] == 'o' || text[2] == 'O') {
        pattern.offsuit_only = true;
    } else {
        return false;
    }

    out_pattern = pattern;
    return true;
}

ExpandedRangeToken expand_starting_hand_pattern(const StartingHandPattern& pattern) noexcept
{
    ExpandedRangeToken range{};

    if (starting_hand_pattern_is_pair(pattern)) {
        for (std::size_t first_suit = 0; first_suit < all_suits.size(); ++first_suit) {
            for (std::size_t second_suit = first_suit + 1; second_suit < all_suits.size();
                 ++second_suit) {
                push_combo(
                    range,
                    make_ordered_hole_cards(
                        Card{pattern.high_rank, all_suits[first_suit]},
                        Card{pattern.low_rank, all_suits[second_suit]}));
            }
        }

        return range;
    }

    for (Suit first_suit : all_suits) {
        for (Suit second_suit : all_suits) {
            const bool suited = first_suit == second_suit;

            if (pattern.suited_only && !suited) {
                continue;
            }

            if (pattern.offsuit_only && suited) {
                continue;
            }

            push_combo(
                range,
                make_ordered_hole_cards(
                    Card{pattern.high_rank, first_suit},
                    Card{pattern.low_rank, second_suit}));
        }
    }

    return range;
}

bool expand_range_token(const char* text, ExpandedRangeToken& out_range) noexcept
{
    StartingHandPattern pattern{};

    if (!parse_starting_hand_pattern(text, pattern)) {
        return false;
    }

    out_range = expand_starting_hand_pattern(pattern);
    return true;
}

ExpandedRangeToken filter_range_token_dead_cards(
    const ExpandedRangeToken& range,
    const std::array<Card, max_dead_cards_for_range_filter>& dead_cards,
    std::size_t dead_card_count) noexcept
{
    ExpandedRangeToken filtered_range{};

    for (std::size_t combo_index = 0; combo_index < range.combo_count; ++combo_index) {
        if (!hole_cards_overlap_dead_cards(range.combos[combo_index], dead_cards, dead_card_count)) {
            push_combo(filtered_range, range.combos[combo_index]);
        }
    }

    return filtered_range;
}

}  // namespace poker
