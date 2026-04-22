#include "poker/card.hpp"
#include "poker/hand.hpp"
#include "poker/simulator.hpp"

#include <array>
#include <cmath>
#include <cstring>
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

bool expect_strength_comparison(
    const std::array<poker::Card, poker::five_card_hand_size>& left_cards,
    const std::array<poker::Card, poker::five_card_hand_size>& right_cards,
    int expected_result,
    const char* message)
{
    const poker::HandStrength left_strength = poker::evaluate_5_card_strength(left_cards);
    const poker::HandStrength right_strength = poker::evaluate_5_card_strength(right_cards);

    int comparison = 0;
    if (left_strength < right_strength) {
        comparison = -1;
    } else if (left_strength > right_strength) {
        comparison = 1;
    }

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

bool expect_simulation_result(
    const poker::HeadsUpSimulationInput& input,
    std::size_t expected_hero_wins,
    std::size_t expected_villain_wins,
    std::size_t expected_ties,
    const char* message)
{
    const poker::MonteCarloSimulator simulator{};
    const poker::HeadsUpSimulationResult result = simulator.simulate_heads_up(input);

    return expect(
        result.success && result.hero_wins == expected_hero_wins &&
            result.villain_wins == expected_villain_wins && result.ties == expected_ties,
        message);
}

bool expect_simulation_failure(
    const poker::HeadsUpSimulationInput& input,
    const char* expected_error_message,
    const char* message)
{
    const poker::MonteCarloSimulator simulator{};
    const poker::HeadsUpSimulationResult result = simulator.simulate_heads_up(input);

    return expect(
        !result.success && std::strcmp(result.error_message, expected_error_message) == 0,
        message);
}

bool expect_approximately_equal(double left, double right, double tolerance, const char* message)
{
    return expect(std::fabs(left - right) <= tolerance, message);
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

    ok = expect_strength_comparison(
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
             "packed strength should rank pair of aces above pair of kings") &&
         ok;

    ok = expect_strength_comparison(
             {
                 make_card(poker::Rank::ten, poker::Suit::spades),
                 make_card(poker::Rank::jack, poker::Suit::spades),
                 make_card(poker::Rank::queen, poker::Suit::spades),
                 make_card(poker::Rank::king, poker::Suit::spades),
                 make_card(poker::Rank::ace, poker::Suit::spades),
             },
             {
                 make_card(poker::Rank::ace, poker::Suit::hearts),
                 make_card(poker::Rank::queen, poker::Suit::hearts),
                 make_card(poker::Rank::nine, poker::Suit::hearts),
                 make_card(poker::Rank::six, poker::Suit::hearts),
                 make_card(poker::Rank::three, poker::Suit::hearts),
             },
             1,
             "packed strength should rank straight flush above flush") &&
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

    ok = expect_seven_card_category(
             {
                 make_card(poker::Rank::two, poker::Suit::hearts),
                 make_card(poker::Rank::four, poker::Suit::hearts),
                 make_card(poker::Rank::six, poker::Suit::hearts),
                 make_card(poker::Rank::eight, poker::Suit::hearts),
                 make_card(poker::Rank::ace, poker::Suit::hearts),
                 make_card(poker::Rank::ace, poker::Suit::clubs),
                 make_card(poker::Rank::ace, poker::Suit::diamonds),
             },
             poker::HandCategory::flush,
             "7-card evaluator should choose the best flush over a weaker trip hand") &&
         ok;

    poker::HeadsUpSimulationInput hero_wins_input{};
    hero_wins_input.hero_hole = {
        make_card(poker::Rank::ace, poker::Suit::clubs),
        make_card(poker::Rank::ace, poker::Suit::diamonds),
    };
    hero_wins_input.villain_hole = {
        make_card(poker::Rank::king, poker::Suit::clubs),
        make_card(poker::Rank::king, poker::Suit::diamonds),
    };
    hero_wins_input.board = {
        make_card(poker::Rank::two, poker::Suit::spades),
        make_card(poker::Rank::three, poker::Suit::hearts),
        make_card(poker::Rank::four, poker::Suit::clubs),
        make_card(poker::Rank::five, poker::Suit::diamonds),
        make_card(poker::Rank::seven, poker::Suit::spades),
    };
    hero_wins_input.board_count = 5;
    hero_wins_input.iterations = 10;
    ok = expect_simulation_result(
             hero_wins_input,
             10,
             0,
             0,
             "simulation should give hero all wins when board is complete and hero is ahead") &&
         ok;

    poker::HeadsUpSimulationInput tie_input{};
    tie_input.hero_hole = {
        make_card(poker::Rank::ace, poker::Suit::clubs),
        make_card(poker::Rank::king, poker::Suit::diamonds),
    };
    tie_input.villain_hole = {
        make_card(poker::Rank::ace, poker::Suit::hearts),
        make_card(poker::Rank::king, poker::Suit::spades),
    };
    tie_input.board = {
        make_card(poker::Rank::queen, poker::Suit::clubs),
        make_card(poker::Rank::jack, poker::Suit::diamonds),
        make_card(poker::Rank::ten, poker::Suit::hearts),
        make_card(poker::Rank::two, poker::Suit::clubs),
        make_card(poker::Rank::three, poker::Suit::spades),
    };
    tie_input.board_count = 5;
    tie_input.iterations = 10;
    ok = expect_simulation_result(
             tie_input,
             0,
             0,
             10,
             "simulation should count ties when both players share the same best hand") &&
         ok;

    poker::HeadsUpSimulationInput invalid_iterations_input{};
    invalid_iterations_input.hero_hole = {
        make_card(poker::Rank::ace, poker::Suit::clubs),
        make_card(poker::Rank::king, poker::Suit::clubs),
    };
    invalid_iterations_input.villain_hole = {
        make_card(poker::Rank::queen, poker::Suit::clubs),
        make_card(poker::Rank::jack, poker::Suit::clubs),
    };
    invalid_iterations_input.iterations = 0;
    ok = expect_simulation_failure(
             invalid_iterations_input,
             "iterations must be greater than zero",
             "simulation should reject zero iterations") &&
         ok;

    poker::HeadsUpSimulationInput invalid_board_count_input{};
    invalid_board_count_input.hero_hole = {
        make_card(poker::Rank::ace, poker::Suit::clubs),
        make_card(poker::Rank::king, poker::Suit::clubs),
    };
    invalid_board_count_input.villain_hole = {
        make_card(poker::Rank::queen, poker::Suit::clubs),
        make_card(poker::Rank::jack, poker::Suit::clubs),
    };
    invalid_board_count_input.board_count = 6;
    ok = expect_simulation_failure(
             invalid_board_count_input,
             "board_count cannot be greater than 5",
             "simulation should reject board_count values above 5") &&
         ok;

    poker::HeadsUpSimulationInput duplicate_card_input{};
    duplicate_card_input.hero_hole = {
        make_card(poker::Rank::ace, poker::Suit::clubs),
        make_card(poker::Rank::king, poker::Suit::clubs),
    };
    duplicate_card_input.villain_hole = {
        make_card(poker::Rank::ace, poker::Suit::clubs),
        make_card(poker::Rank::queen, poker::Suit::clubs),
    };
    ok = expect_simulation_failure(
             duplicate_card_input,
             "duplicate cards are not allowed",
             "simulation should reject duplicate cards across both players") &&
         ok;

    poker::HeadsUpSimulationInput duplicate_board_input{};
    duplicate_board_input.hero_hole = {
        make_card(poker::Rank::ace, poker::Suit::clubs),
        make_card(poker::Rank::king, poker::Suit::clubs),
    };
    duplicate_board_input.villain_hole = {
        make_card(poker::Rank::queen, poker::Suit::clubs),
        make_card(poker::Rank::jack, poker::Suit::clubs),
    };
    duplicate_board_input.board = {
        make_card(poker::Rank::ace, poker::Suit::clubs),
        make_card(poker::Rank::two, poker::Suit::diamonds),
        make_card(poker::Rank::three, poker::Suit::hearts),
        make_card(poker::Rank::four, poker::Suit::spades),
        make_card(poker::Rank::five, poker::Suit::clubs),
    };
    duplicate_board_input.board_count = 1;
    ok = expect_simulation_failure(
             duplicate_board_input,
             "duplicate cards are not allowed",
             "simulation should reject duplicate cards between hole cards and the known board") &&
         ok;

    const poker::MonteCarloSimulator simulator{};
    poker::HeadsUpSimulationInput preflop_input{};
    preflop_input.hero_hole = {
        make_card(poker::Rank::ace, poker::Suit::spades),
        make_card(poker::Rank::ace, poker::Suit::hearts),
    };
    preflop_input.villain_hole = {
        make_card(poker::Rank::king, poker::Suit::spades),
        make_card(poker::Rank::king, poker::Suit::hearts),
    };
    preflop_input.iterations = 50;
    preflop_input.seed = 7;

    const poker::HeadsUpSimulationResult preflop_result = simulator.simulate_heads_up(preflop_input);
    ok = expect(
             preflop_result.success &&
                 preflop_result.hero_wins + preflop_result.villain_wins + preflop_result.ties ==
                     preflop_result.iterations,
             "simulation counts should sum to the total number of iterations") &&
         ok;

    ok = expect_approximately_equal(
             preflop_result.hero_equity() + preflop_result.villain_equity(),
             1.0,
             1e-12,
             "hero and villain equity should add up to 1.0") &&
         ok;

    poker::HeadsUpSimulationInput seeded_input{};
    seeded_input.hero_hole = {
        make_card(poker::Rank::ace, poker::Suit::spades),
        make_card(poker::Rank::queen, poker::Suit::spades),
    };
    seeded_input.villain_hole = {
        make_card(poker::Rank::king, poker::Suit::hearts),
        make_card(poker::Rank::jack, poker::Suit::hearts),
    };
    seeded_input.board = {
        make_card(poker::Rank::two, poker::Suit::clubs),
        make_card(poker::Rank::seven, poker::Suit::diamonds),
        make_card(poker::Rank::nine, poker::Suit::spades),
        make_card(poker::Rank::three, poker::Suit::hearts),
        make_card(poker::Rank::four, poker::Suit::clubs),
    };
    seeded_input.board_count = 3;
    seeded_input.iterations = 100;
    seeded_input.seed = 42;

    const poker::HeadsUpSimulationResult first_seeded_run = simulator.simulate_heads_up(seeded_input);
    const poker::HeadsUpSimulationResult second_seeded_run = simulator.simulate_heads_up(seeded_input);

    ok = expect(
             first_seeded_run.success && second_seeded_run.success &&
                 first_seeded_run.hero_wins == second_seeded_run.hero_wins &&
                 first_seeded_run.villain_wins == second_seeded_run.villain_wins &&
                 first_seeded_run.ties == second_seeded_run.ties,
             "simulation should be deterministic when the seed and input are the same") &&
         ok;

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
