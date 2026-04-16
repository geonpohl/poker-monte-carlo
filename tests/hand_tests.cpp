#include "poker/card.hpp"
#include "poker/hand.hpp"

#include <array>
#include <cstdlib>
#include <iostream>

namespace {

poker::Card make_card(poker::Rank rank, poker::Suit suit)
{
    return poker::Card{rank, suit};
}

void print_failure(const char* message)
{
    std::cerr << "test failure: " << message << '\n';
}

bool expect(bool condition, const char* message)
{
    if (!condition) {
        print_failure(message);
        return false;
    }

    return true;
}

bool expect_category(
    const std::array<poker::Card, poker::five_card_hand_size>& cards,
    poker::HandCategory expected_category,
    const char* message)
{
    const poker::EvaluatedHand hand = poker::evaluate_5_card_hand(cards);
    return expect(hand.category == expected_category, message);
}

bool expect_comparison(
    const std::array<poker::Card, poker::five_card_hand_size>& left_cards,
    const std::array<poker::Card, poker::five_card_hand_size>& right_cards,
    int expected_result,
    const char* message)
{
    const poker::EvaluatedHand left_hand = poker::evaluate_5_card_hand(left_cards);
    const poker::EvaluatedHand right_hand = poker::evaluate_5_card_hand(right_cards);
    const int comparison = poker::compare_evaluated_hands(left_hand, right_hand);
    return expect(comparison == expected_result, message);
}

bool expect_seven_card_category(
    const std::array<poker::Card, poker::seven_card_hand_size>& cards,
    poker::HandCategory expected_category,
    const char* message)
{
    const poker::EvaluatedHand hand = poker::evaluate_7_card_hand(cards);
    return expect(hand.category == expected_category, message);
}

}  // namespace

int main()
{
    bool ok = true;

    ok = expect_category(
             {
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::jack, poker::Suit::diamonds),
                 make_card(poker::Rank::nine, poker::Suit::hearts),
                 make_card(poker::Rank::six, poker::Suit::spades),
                 make_card(poker::Rank::three, poker::Suit::clubs),
             },
             poker::HandCategory::high_card,
             "high card should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::ace, poker::Suit::diamonds),
                 make_card(poker::Rank::queen, poker::Suit::hearts),
                 make_card(poker::Rank::nine, poker::Suit::spades),
                 make_card(poker::Rank::four, poker::Suit::clubs),
             },
             poker::HandCategory::pair,
             "pair should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::king, poker::Suit::clubs),
                 make_card(poker::Rank::king, poker::Suit::diamonds),
                 make_card(poker::Rank::ten, poker::Suit::hearts),
                 make_card(poker::Rank::ten, poker::Suit::spades),
                 make_card(poker::Rank::ace, poker::Suit::clubs),
             },
             poker::HandCategory::two_pair,
             "two pair should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::queen, poker::Suit::clubs),
                 make_card(poker::Rank::queen, poker::Suit::diamonds),
                 make_card(poker::Rank::queen, poker::Suit::hearts),
                 make_card(poker::Rank::nine, poker::Suit::spades),
                 make_card(poker::Rank::four, poker::Suit::clubs),
             },
             poker::HandCategory::three_of_a_kind,
             "three of a kind should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::nine, poker::Suit::clubs),
                 make_card(poker::Rank::eight, poker::Suit::diamonds),
                 make_card(poker::Rank::seven, poker::Suit::hearts),
                 make_card(poker::Rank::six, poker::Suit::spades),
                 make_card(poker::Rank::five, poker::Suit::clubs),
             },
             poker::HandCategory::straight,
             "straight should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::two, poker::Suit::diamonds),
                 make_card(poker::Rank::three, poker::Suit::hearts),
                 make_card(poker::Rank::four, poker::Suit::spades),
                 make_card(poker::Rank::five, poker::Suit::clubs),
             },
             poker::HandCategory::straight,
             "wheel straight should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::ace, poker::Suit::hearts),
                 make_card(poker::Rank::queen, poker::Suit::hearts),
                 make_card(poker::Rank::nine, poker::Suit::hearts),
                 make_card(poker::Rank::six, poker::Suit::hearts),
                 make_card(poker::Rank::three, poker::Suit::hearts),
             },
             poker::HandCategory::flush,
             "flush should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::ace, poker::Suit::diamonds),
                 make_card(poker::Rank::ace, poker::Suit::hearts),
                 make_card(poker::Rank::king, poker::Suit::clubs),
                 make_card(poker::Rank::king, poker::Suit::diamonds),
             },
             poker::HandCategory::full_house,
             "full house should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::jack, poker::Suit::clubs),
                 make_card(poker::Rank::jack, poker::Suit::diamonds),
                 make_card(poker::Rank::jack, poker::Suit::hearts),
                 make_card(poker::Rank::jack, poker::Suit::spades),
                 make_card(poker::Rank::three, poker::Suit::clubs),
             },
             poker::HandCategory::four_of_a_kind,
             "four of a kind should be detected") &&
         ok;

    ok = expect_category(
             {
                 make_card(poker::Rank::ten, poker::Suit::spades),
                 make_card(poker::Rank::jack, poker::Suit::spades),
                 make_card(poker::Rank::queen, poker::Suit::spades),
                 make_card(poker::Rank::king, poker::Suit::spades),
                 make_card(poker::Rank::ace, poker::Suit::spades),
             },
             poker::HandCategory::straight_flush,
             "straight flush should be detected") &&
         ok;

    ok = expect_comparison(
             {
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::ace, poker::Suit::diamonds),
                 make_card(poker::Rank::queen, poker::Suit::hearts),
                 make_card(poker::Rank::nine, poker::Suit::spades),
                 make_card(poker::Rank::four, poker::Suit::clubs),
             },
             {
                 make_card(poker::Rank::king, poker::Suit::clubs),
                 make_card(poker::Rank::king, poker::Suit::diamonds),
                 make_card(poker::Rank::jack, poker::Suit::hearts),
                 make_card(poker::Rank::nine, poker::Suit::spades),
                 make_card(poker::Rank::four, poker::Suit::clubs),
             },
             1,
             "pair of aces should beat pair of kings") &&
         ok;

    ok = expect_comparison(
             {
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::king, poker::Suit::diamonds),
                 make_card(poker::Rank::queen, poker::Suit::hearts),
                 make_card(poker::Rank::jack, poker::Suit::spades),
                 make_card(poker::Rank::ten, poker::Suit::clubs),
             },
             {
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::two, poker::Suit::diamonds),
                 make_card(poker::Rank::three, poker::Suit::hearts),
                 make_card(poker::Rank::four, poker::Suit::spades),
                 make_card(poker::Rank::five, poker::Suit::clubs),
             },
             1,
             "ace-high straight should beat wheel straight") &&
         ok;

    ok = expect_comparison(
             {
                 make_card(poker::Rank::king, poker::Suit::clubs),
                 make_card(poker::Rank::king, poker::Suit::diamonds),
                 make_card(poker::Rank::ten, poker::Suit::hearts),
                 make_card(poker::Rank::ten, poker::Suit::spades),
                 make_card(poker::Rank::ace, poker::Suit::clubs),
             },
             {
                 make_card(poker::Rank::king, poker::Suit::hearts),
                 make_card(poker::Rank::king, poker::Suit::spades),
                 make_card(poker::Rank::ten, poker::Suit::clubs),
                 make_card(poker::Rank::ten, poker::Suit::diamonds),
                 make_card(poker::Rank::queen, poker::Suit::clubs),
             },
             1,
             "two pair should use kicker for comparison") &&
         ok;

    ok = expect_seven_card_category(
             {
                 make_card(poker::Rank::ace, poker::Suit::spades),
                 make_card(poker::Rank::king, poker::Suit::spades),
                 make_card(poker::Rank::queen, poker::Suit::spades),
                 make_card(poker::Rank::jack, poker::Suit::spades),
                 make_card(poker::Rank::ten, poker::Suit::spades),
                 make_card(poker::Rank::two, poker::Suit::diamonds),
                 make_card(poker::Rank::three, poker::Suit::clubs),
             },
             poker::HandCategory::straight_flush,
             "7-card evaluator should find the best straight flush") &&
         ok;

    ok = expect_seven_card_category(
             {
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::ace, poker::Suit::diamonds),
                 make_card(poker::Rank::ace, poker::Suit::hearts),
                 make_card(poker::Rank::king, poker::Suit::clubs),
                 make_card(poker::Rank::king, poker::Suit::diamonds),
                 make_card(poker::Rank::two, poker::Suit::spades),
                 make_card(poker::Rank::three, poker::Suit::hearts),
             },
             poker::HandCategory::full_house,
             "7-card evaluator should find the best full house") &&
         ok;

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
