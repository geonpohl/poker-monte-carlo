#include "poker/hand.hpp"

namespace poker {

namespace {

// There are exactly 21 ways to choose 5 cards from 7.
// Keeping them in one table lets the evaluator loop over fixed data instead of
// rebuilding the same nested-loop structure on every call.
constexpr std::array<std::array<std::size_t, five_card_hand_size>, 21>
    kSevenChooseFiveCombinations{{
        {{0, 1, 2, 3, 4}},
        {{0, 1, 2, 3, 5}},
        {{0, 1, 2, 3, 6}},
        {{0, 1, 2, 4, 5}},
        {{0, 1, 2, 4, 6}},
        {{0, 1, 2, 5, 6}},
        {{0, 1, 3, 4, 5}},
        {{0, 1, 3, 4, 6}},
        {{0, 1, 3, 5, 6}},
        {{0, 1, 4, 5, 6}},
        {{0, 2, 3, 4, 5}},
        {{0, 2, 3, 4, 6}},
        {{0, 2, 3, 5, 6}},
        {{0, 2, 4, 5, 6}},
        {{0, 3, 4, 5, 6}},
        {{1, 2, 3, 4, 5}},
        {{1, 2, 3, 4, 6}},
        {{1, 2, 3, 5, 6}},
        {{1, 2, 4, 5, 6}},
        {{1, 3, 4, 5, 6}},
        {{2, 3, 4, 5, 6}},
    }};

int rank_value(Rank rank) noexcept
{
    return static_cast<int>(rank);
}

int category_value(HandCategory category) noexcept
{
    return static_cast<int>(category);
}

int rank_value_from_index(int index) noexcept
{
    return index + static_cast<int>(Rank::two);
}

std::size_t tie_break_count_for_category(HandCategory category) noexcept
{
    switch (category) {
    case HandCategory::high_card:
        return 5;
    case HandCategory::pair:
        return 4;
    case HandCategory::two_pair:
        return 3;
    case HandCategory::three_of_a_kind:
        return 3;
    case HandCategory::straight:
        return 1;
    case HandCategory::flush:
        return 5;
    case HandCategory::full_house:
        return 2;
    case HandCategory::four_of_a_kind:
        return 2;
    case HandCategory::straight_flush:
        return 1;
    }

    return 0;
}

HandStrength pack_strength(
    HandCategory category,
    int rank0 = 0,
    int rank1 = 0,
    int rank2 = 0,
    int rank3 = 0,
    int rank4 = 0) noexcept
{
    // Bit layout:
    // [ category | rank0 | rank1 | rank2 | rank3 | rank4 ]
    //
    // Each field uses 4 bits. That is enough because:
    // - categories fit in 0..8
    // - ranks fit in 2..14
    //
    // Higher-strength information is placed in higher bits so plain integer
    // comparison matches poker hand ordering.
    HandStrength strength = static_cast<HandStrength>(category_value(category));
    strength = (strength << 4) | static_cast<HandStrength>(rank0);
    strength = (strength << 4) | static_cast<HandStrength>(rank1);
    strength = (strength << 4) | static_cast<HandStrength>(rank2);
    strength = (strength << 4) | static_cast<HandStrength>(rank3);
    strength = (strength << 4) | static_cast<HandStrength>(rank4);
    return strength;
}

EvaluatedHand decode_strength(HandStrength strength) noexcept
{
    EvaluatedHand hand{};
    hand.strength = strength;

    const HandCategory category =
        static_cast<HandCategory>((strength >> 20U) & static_cast<HandStrength>(0xF));
    hand.category = category;
    hand.tie_break_count = tie_break_count_for_category(category);

    for (std::size_t index = 0; index < hand.tie_break_count; ++index) {
        const unsigned shift = static_cast<unsigned>(16U - (index * 4U));
        const HandStrength rank_bits = (strength >> shift) & static_cast<HandStrength>(0xF);
        hand.tie_break_ranks[index] = static_cast<Rank>(rank_bits);
    }

    return hand;
}

std::size_t rank_index(Rank rank) noexcept
{
    // Rank::two has the numeric value 2, so subtracting Rank::two makes
    // the indexes line up with our rank_counts array:
    // two -> 0, three -> 1, ..., ace -> 12
    return static_cast<std::size_t>(static_cast<int>(rank) - static_cast<int>(Rank::two));
}

std::size_t suit_index(Suit suit) noexcept
{
    return static_cast<std::size_t>(suit);
}

Rank rank_from_index(std::size_t index) noexcept
{
    return static_cast<Rank>(static_cast<int>(Rank::two) + static_cast<int>(index));
}

bool is_flush(const HandCounts& counts) noexcept
{
    for (int suit_count : counts.suit_counts) {
        if (suit_count == 5) {
            return true;
        }
    }

    return false;
}

}  // namespace

const char* hand_category_name(HandCategory category) noexcept
{
    switch (category) {
    case HandCategory::high_card:
        return "high card";
    case HandCategory::pair:
        return "pair";
    case HandCategory::two_pair:
        return "two pair";
    case HandCategory::three_of_a_kind:
        return "three of a kind";
    case HandCategory::straight:
        return "straight";
    case HandCategory::flush:
        return "flush";
    case HandCategory::full_house:
        return "full house";
    case HandCategory::four_of_a_kind:
        return "four of a kind";
    case HandCategory::straight_flush:
        return "straight flush";
    }

    return "unknown hand category";
}

HandCounts count_ranks_and_suits(const std::array<Card, five_card_hand_size>& cards) noexcept
{
    HandCounts counts{};

    for (const Card& card : cards) {
        ++counts.rank_counts[rank_index(card.rank)];
        ++counts.suit_counts[suit_index(card.suit)];
    }

    return counts;
}

StraightInfo detect_straight(const HandCounts& counts) noexcept
{
    // Check normal straights from ace-high down to five-high.
    // Example windows:
    // A-K-Q-J-T
    // K-Q-J-T-9
    // ...
    // 6-5-4-3-2
    for (std::size_t high_index = 12; high_index >= 4; --high_index) {
        bool all_present = true;

        for (std::size_t offset = 0; offset < 5; ++offset) {
            if (counts.rank_counts[high_index - offset] == 0) {
                all_present = false;
                break;
            }
        }

        if (all_present) {
            return StraightInfo{
                .is_straight = true,
                .high_rank = rank_from_index(high_index),
            };
        }
    }

    // Special case: A-2-3-4-5, called the wheel.
    if (counts.rank_counts[12] > 0 && counts.rank_counts[0] > 0 && counts.rank_counts[1] > 0 &&
        counts.rank_counts[2] > 0 && counts.rank_counts[3] > 0) {
        return StraightInfo{
            .is_straight = true,
            .high_rank = Rank::five,
        };
    }

    return StraightInfo{};
}

HandStrength evaluate_5_card_strength(const std::array<Card, five_card_hand_size>& cards) noexcept
{
    const HandCounts counts = count_ranks_and_suits(cards);
    const StraightInfo straight_info = detect_straight(counts);
    const bool flush = is_flush(counts);

    int four_of_a_kind_index = -1;
    int three_of_a_kind_index = -1;
    std::array<int, 2> pair_indices{-1, -1};
    std::size_t pair_count = 0;
    std::array<int, 5> single_indices{-1, -1, -1, -1, -1};
    std::size_t single_count = 0;
    std::array<int, 5> present_rank_indices{-1, -1, -1, -1, -1};
    std::size_t present_rank_count = 0;

    // Scan from ace down to two once and collect all the rank-shape information
    // the later category checks will need.
    for (int index = 12; index >= 0; --index) {
        const int count = counts.rank_counts[static_cast<std::size_t>(index)];

        if (count > 0) {
            present_rank_indices[present_rank_count] = index;
            ++present_rank_count;
        }

        if (count == 4) {
            four_of_a_kind_index = index;
        } else if (count == 3) {
            three_of_a_kind_index = index;
        } else if (count == 2) {
            if (pair_count < pair_indices.size()) {
                pair_indices[pair_count] = index;
                ++pair_count;
            }
        } else if (count == 1) {
            if (single_count < single_indices.size()) {
                single_indices[single_count] = index;
                ++single_count;
            }
        }
    }

    if (straight_info.is_straight && flush) {
        return pack_strength(
            HandCategory::straight_flush, rank_value(straight_info.high_rank), 0, 0, 0, 0);
    }

    if (four_of_a_kind_index != -1) {
        return pack_strength(
            HandCategory::four_of_a_kind,
            rank_value_from_index(four_of_a_kind_index),
            rank_value_from_index(single_indices[0]),
            0,
            0,
            0);
    }

    if (three_of_a_kind_index != -1 && pair_count >= 1) {
        return pack_strength(
            HandCategory::full_house,
            rank_value_from_index(three_of_a_kind_index),
            rank_value_from_index(pair_indices[0]),
            0,
            0,
            0);
    }

    if (flush) {
        return pack_strength(
            HandCategory::flush,
            rank_value_from_index(present_rank_indices[0]),
            rank_value_from_index(present_rank_indices[1]),
            rank_value_from_index(present_rank_indices[2]),
            rank_value_from_index(present_rank_indices[3]),
            rank_value_from_index(present_rank_indices[4]));
    }

    if (straight_info.is_straight) {
        return pack_strength(
            HandCategory::straight, rank_value(straight_info.high_rank), 0, 0, 0, 0);
    }

    if (three_of_a_kind_index != -1) {
        return pack_strength(
            HandCategory::three_of_a_kind,
            rank_value_from_index(three_of_a_kind_index),
            rank_value_from_index(single_indices[0]),
            rank_value_from_index(single_indices[1]),
            0,
            0);
    }

    if (pair_count == 2) {
        return pack_strength(
            HandCategory::two_pair,
            rank_value_from_index(pair_indices[0]),
            rank_value_from_index(pair_indices[1]),
            rank_value_from_index(single_indices[0]),
            0,
            0);
    }

    if (pair_count == 1) {
        return pack_strength(
            HandCategory::pair,
            rank_value_from_index(pair_indices[0]),
            rank_value_from_index(single_indices[0]),
            rank_value_from_index(single_indices[1]),
            rank_value_from_index(single_indices[2]),
            0);
    }

    return pack_strength(
        HandCategory::high_card,
        rank_value_from_index(present_rank_indices[0]),
        rank_value_from_index(present_rank_indices[1]),
        rank_value_from_index(present_rank_indices[2]),
        rank_value_from_index(present_rank_indices[3]),
        rank_value_from_index(present_rank_indices[4]));
}

EvaluatedHand evaluate_5_card_hand(const std::array<Card, five_card_hand_size>& cards) noexcept
{
    return decode_strength(evaluate_5_card_strength(cards));
}

HandStrength evaluate_7_card_strength(const std::array<Card, seven_card_hand_size>& cards) noexcept
{
    std::array<Card, five_card_hand_size> candidate_cards{};

    // Seed best_hand from the first combination so we can avoid a branch inside the loop.
    for (std::size_t card_index = 0; card_index < five_card_hand_size; ++card_index) {
        candidate_cards[card_index] = cards[kSevenChooseFiveCombinations[0][card_index]];
    }

    HandStrength best_strength = evaluate_5_card_strength(candidate_cards);

    for (std::size_t combination_index = 1; combination_index < kSevenChooseFiveCombinations.size();
         ++combination_index) {
        const auto& combination = kSevenChooseFiveCombinations[combination_index];

        for (std::size_t card_index = 0; card_index < five_card_hand_size; ++card_index) {
            candidate_cards[card_index] = cards[combination[card_index]];
        }

        const HandStrength candidate_strength = evaluate_5_card_strength(candidate_cards);

        if (candidate_strength > best_strength) {
            best_strength = candidate_strength;
        }
    }

    return best_strength;
}

EvaluatedHand evaluate_7_card_hand(const std::array<Card, seven_card_hand_size>& cards) noexcept
{
    return decode_strength(evaluate_7_card_strength(cards));
}

int compare_evaluated_hands(const EvaluatedHand& left, const EvaluatedHand& right) noexcept
{
    // Fast path for hands that came from the evaluator.
    if (left.strength != 0 || right.strength != 0) {
        if (left.strength < right.strength) {
            return -1;
        }

        if (left.strength > right.strength) {
            return 1;
        }

        return 0;
    }

    if (left.category != right.category) {
        return category_value(left.category) < category_value(right.category) ? -1 : 1;
    }

    const std::size_t max_count =
        left.tie_break_count > right.tie_break_count ? left.tie_break_count : right.tie_break_count;

    for (std::size_t index = 0; index < max_count; ++index) {
        const int left_value =
            index < left.tie_break_count ? rank_value(left.tie_break_ranks[index]) : 0;
        const int right_value =
            index < right.tie_break_count ? rank_value(right.tie_break_ranks[index]) : 0;

        if (left_value < right_value) {
            return -1;
        }

        if (left_value > right_value) {
            return 1;
        }
    }

    return 0;
}

}  // namespace poker
