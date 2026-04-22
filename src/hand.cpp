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

void push_tie_break_rank(EvaluatedHand& hand, Rank rank) noexcept
{
    if (hand.tie_break_count < hand.tie_break_ranks.size()) {
        hand.tie_break_ranks[hand.tie_break_count] = rank;
        ++hand.tie_break_count;
    }
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

EvaluatedHand evaluate_5_card_hand(const std::array<Card, five_card_hand_size>& cards) noexcept
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

    EvaluatedHand hand{};

    if (straight_info.is_straight && flush) {
        hand.category = HandCategory::straight_flush;
        push_tie_break_rank(hand, straight_info.high_rank);
        return hand;
    }

    if (four_of_a_kind_index != -1) {
        hand.category = HandCategory::four_of_a_kind;
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(four_of_a_kind_index)));
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(single_indices[0])));
        return hand;
    }

    if (three_of_a_kind_index != -1 && pair_count >= 1) {
        hand.category = HandCategory::full_house;
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(three_of_a_kind_index)));
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(pair_indices[0])));
        return hand;
    }

    if (flush) {
        hand.category = HandCategory::flush;
        for (std::size_t index = 0; index < present_rank_count; ++index) {
            push_tie_break_rank(
                hand, rank_from_index(static_cast<std::size_t>(present_rank_indices[index])));
        }
        return hand;
    }

    if (straight_info.is_straight) {
        hand.category = HandCategory::straight;
        push_tie_break_rank(hand, straight_info.high_rank);
        return hand;
    }

    if (three_of_a_kind_index != -1) {
        hand.category = HandCategory::three_of_a_kind;
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(three_of_a_kind_index)));

        for (std::size_t index = 0; index < single_count; ++index) {
            push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(single_indices[index])));
        }

        return hand;
    }

    if (pair_count == 2) {
        hand.category = HandCategory::two_pair;
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(pair_indices[0])));
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(pair_indices[1])));
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(single_indices[0])));
        return hand;
    }

    if (pair_count == 1) {
        hand.category = HandCategory::pair;
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(pair_indices[0])));

        for (std::size_t index = 0; index < single_count; ++index) {
            push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(single_indices[index])));
        }

        return hand;
    }

    hand.category = HandCategory::high_card;
    for (std::size_t index = 0; index < present_rank_count; ++index) {
        push_tie_break_rank(
            hand, rank_from_index(static_cast<std::size_t>(present_rank_indices[index])));
    }
    return hand;
}

EvaluatedHand evaluate_7_card_hand(const std::array<Card, seven_card_hand_size>& cards) noexcept
{
    std::array<Card, five_card_hand_size> candidate_cards{};

    // Seed best_hand from the first combination so we can avoid a branch inside the loop.
    for (std::size_t card_index = 0; card_index < five_card_hand_size; ++card_index) {
        candidate_cards[card_index] = cards[kSevenChooseFiveCombinations[0][card_index]];
    }

    EvaluatedHand best_hand = evaluate_5_card_hand(candidate_cards);

    for (std::size_t combination_index = 1; combination_index < kSevenChooseFiveCombinations.size();
         ++combination_index) {
        const auto& combination = kSevenChooseFiveCombinations[combination_index];

        for (std::size_t card_index = 0; card_index < five_card_hand_size; ++card_index) {
            candidate_cards[card_index] = cards[combination[card_index]];
        }

        const EvaluatedHand candidate_hand = evaluate_5_card_hand(candidate_cards);

        if (compare_evaluated_hands(candidate_hand, best_hand) > 0) {
            best_hand = candidate_hand;
        }
    }

    return best_hand;
}

int compare_evaluated_hands(const EvaluatedHand& left, const EvaluatedHand& right) noexcept
{
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
