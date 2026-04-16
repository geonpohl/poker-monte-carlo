#include "poker/hand.hpp"

namespace poker {

namespace {

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

void push_all_present_ranks_descending(EvaluatedHand& hand, const HandCounts& counts) noexcept
{
    // We use int here instead of size_t because this loop needs to count downward to -1.
    for (int index = 12; index >= 0; --index) {
        if (counts.rank_counts[static_cast<std::size_t>(index)] > 0) {
            push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(index)));
        }
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
    int high_pair_index = -1;
    int low_pair_index = -1;
    int kicker_index = -1;

    // Scan from ace down to two so tie-break ranks naturally come out in strength order.
    for (int index = 12; index >= 0; --index) {
        const int count = counts.rank_counts[static_cast<std::size_t>(index)];

        if (count == 4) {
            four_of_a_kind_index = index;
        } else if (count == 3) {
            three_of_a_kind_index = index;
        } else if (count == 2) {
            if (high_pair_index == -1) {
                high_pair_index = index;
            } else {
                low_pair_index = index;
            }
        } else if (count == 1) {
            if (kicker_index == -1) {
                kicker_index = index;
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
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(kicker_index)));
        return hand;
    }

    if (three_of_a_kind_index != -1 && high_pair_index != -1) {
        hand.category = HandCategory::full_house;
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(three_of_a_kind_index)));
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(high_pair_index)));
        return hand;
    }

    if (flush) {
        hand.category = HandCategory::flush;
        push_all_present_ranks_descending(hand, counts);
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

        for (int index = 12; index >= 0; --index) {
            if (counts.rank_counts[static_cast<std::size_t>(index)] == 1) {
                push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(index)));
            }
        }

        return hand;
    }

    if (high_pair_index != -1 && low_pair_index != -1) {
        hand.category = HandCategory::two_pair;
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(high_pair_index)));
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(low_pair_index)));
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(kicker_index)));
        return hand;
    }

    if (high_pair_index != -1) {
        hand.category = HandCategory::pair;
        push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(high_pair_index)));

        for (int index = 12; index >= 0; --index) {
            if (counts.rank_counts[static_cast<std::size_t>(index)] == 1) {
                push_tie_break_rank(hand, rank_from_index(static_cast<std::size_t>(index)));
            }
        }

        return hand;
    }

    hand.category = HandCategory::high_card;
    push_all_present_ranks_descending(hand, counts);
    return hand;
}

EvaluatedHand evaluate_7_card_hand(const std::array<Card, seven_card_hand_size>& cards) noexcept
{
    EvaluatedHand best_hand{};
    bool has_best_hand = false;

    // A 7-card hand has 21 different 5-card combinations.
    // We evaluate each one and keep the strongest result.
    for (std::size_t first = 0; first < cards.size() - 4; ++first) {
        for (std::size_t second = first + 1; second < cards.size() - 3; ++second) {
            for (std::size_t third = second + 1; third < cards.size() - 2; ++third) {
                for (std::size_t fourth = third + 1; fourth < cards.size() - 1; ++fourth) {
                    for (std::size_t fifth = fourth + 1; fifth < cards.size(); ++fifth) {
                        const std::array<Card, five_card_hand_size> candidate_cards{
                            cards[first],
                            cards[second],
                            cards[third],
                            cards[fourth],
                            cards[fifth],
                        };

                        const EvaluatedHand candidate_hand = evaluate_5_card_hand(candidate_cards);

                        if (!has_best_hand ||
                            compare_evaluated_hands(candidate_hand, best_hand) > 0) {
                            best_hand = candidate_hand;
                            has_best_hand = true;
                        }
                    }
                }
            }
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
