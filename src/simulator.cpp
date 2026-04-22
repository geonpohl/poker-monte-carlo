#include "poker/simulator.hpp"

#include "poker/deck.hpp"
#include "poker/hand.hpp"

#include <array>
#include <random>

namespace poker {

namespace {

bool card_in_used_prefix(
    const std::array<Card, 9>& used_cards,
    std::size_t used_count,
    const Card& card) noexcept
{
    for (std::size_t index = 0; index < used_count; ++index) {
        if (used_cards[index] == card) {
            return true;
        }
    }

    return false;
}

bool validate_input(const HeadsUpSimulationInput& input, const char*& error_message) noexcept
{
    if (input.iterations == 0) {
        error_message = "iterations must be greater than zero";
        return false;
    }

    if (input.board_count > 5) {
        error_message = "board_count cannot be greater than 5";
        return false;
    }

    std::array<Card, 9> used_cards{};
    std::size_t used_count = 0;

    for (const Card& card : input.hero_hole) {
        if (card_in_used_prefix(used_cards, used_count, card)) {
            error_message = "duplicate cards are not allowed";
            return false;
        }

        used_cards[used_count] = card;
        ++used_count;
    }

    for (const Card& card : input.villain_hole) {
        if (card_in_used_prefix(used_cards, used_count, card)) {
            error_message = "duplicate cards are not allowed";
            return false;
        }

        used_cards[used_count] = card;
        ++used_count;
    }

    for (std::size_t index = 0; index < input.board_count; ++index) {
        if (card_in_used_prefix(used_cards, used_count, input.board[index])) {
            error_message = "duplicate cards are not allowed";
            return false;
        }

        used_cards[used_count] = input.board[index];
        ++used_count;
    }

    error_message = "";
    return true;
}

bool prepare_base_deck(const HeadsUpSimulationInput& input, Deck& deck) noexcept
{
    deck.reset();

    for (const Card& card : input.hero_hole) {
        if (!deck.remove(card)) {
            return false;
        }
    }

    for (const Card& card : input.villain_hole) {
        if (!deck.remove(card)) {
            return false;
        }
    }

    for (std::size_t index = 0; index < input.board_count; ++index) {
        if (!deck.remove(input.board[index])) {
            return false;
        }
    }

    return true;
}

std::array<Card, seven_card_hand_size> build_seven_card_hand(
    const std::array<Card, hole_card_count>& hole_cards,
    const std::array<Card, 5>& board) noexcept
{
    return std::array<Card, seven_card_hand_size>{
        hole_cards[0],
        hole_cards[1],
        board[0],
        board[1],
        board[2],
        board[3],
        board[4],
    };
}

}  // namespace

const char* MonteCarloSimulator::status() const noexcept
{
    // This is a string literal.
    // Its memory is managed by the program itself, so we do not need to allocate anything here.
    return "heads-up monte carlo simulator ready";
}

double HeadsUpSimulationResult::hero_equity() const noexcept
{
    if (iterations == 0) {
        return 0.0;
    }

    return static_cast<double>(hero_wins) / static_cast<double>(iterations) +
           0.5 * static_cast<double>(ties) / static_cast<double>(iterations);
}

double HeadsUpSimulationResult::villain_equity() const noexcept
{
    if (iterations == 0) {
        return 0.0;
    }

    return static_cast<double>(villain_wins) / static_cast<double>(iterations) +
           0.5 * static_cast<double>(ties) / static_cast<double>(iterations);
}

HeadsUpSimulationResult MonteCarloSimulator::simulate_heads_up(
    const HeadsUpSimulationInput& input) const noexcept
{
    HeadsUpSimulationResult result{};

    const char* error_message = "";
    if (!validate_input(input, error_message)) {
        result.error_message = error_message;
        return result;
    }

    Deck base_deck{};
    if (!prepare_base_deck(input, base_deck)) {
        result.error_message = "failed to prepare the deck";
        return result;
    }

    std::mt19937 rng(input.seed);

    for (std::size_t iteration = 0; iteration < input.iterations; ++iteration) {
        Deck deck = base_deck;
        std::array<Card, 5> full_board = input.board;

        for (std::size_t board_index = input.board_count; board_index < 5; ++board_index) {
            if (!deck.draw_random(full_board[board_index], rng)) {
                result.error_message = "not enough cards left to complete the board";
                return result;
            }
        }

        const std::array<Card, seven_card_hand_size> hero_cards =
            build_seven_card_hand(input.hero_hole, full_board);
        const std::array<Card, seven_card_hand_size> villain_cards =
            build_seven_card_hand(input.villain_hole, full_board);

        const EvaluatedHand hero_hand = evaluate_7_card_hand(hero_cards);
        const EvaluatedHand villain_hand = evaluate_7_card_hand(villain_cards);
        const int comparison = compare_evaluated_hands(hero_hand, villain_hand);

        if (comparison > 0) {
            ++result.hero_wins;
        } else if (comparison < 0) {
            ++result.villain_wins;
        } else {
            ++result.ties;
        }
    }

    result.success = true;
    result.error_message = "";
    result.iterations = input.iterations;
    return result;
}

}  // namespace poker
