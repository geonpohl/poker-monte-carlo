#include "poker/simulator.hpp"

#include "poker/deck.hpp"
#include "poker/hand.hpp"
#include "poker/range.hpp"

#include <algorithm>
#include <array>
#include <random>

namespace poker {

namespace {

bool card_in_used_prefix(
    const std::array<Card, max_dead_cards_for_range_filter>& used_cards,
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

bool validate_exact_input(const HeadsUpSimulationInput& input, const char*& error_message) noexcept
{
    if (input.iterations == 0) {
        error_message = "iterations must be greater than zero";
        return false;
    }

    if (input.board_count > 5) {
        error_message = "board_count cannot be greater than 5";
        return false;
    }

    std::array<Card, max_dead_cards_for_range_filter> used_cards{};
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

bool validate_range_vs_hand_input(
    const HeadsUpRangeVsHandSimulationInput& input,
    const char*& error_message) noexcept
{
    if (input.iterations == 0) {
        error_message = "iterations must be greater than zero";
        return false;
    }

    if (input.board_count > 5) {
        error_message = "board_count cannot be greater than 5";
        return false;
    }

    if (input.hero_range.combo_count == 0) {
        error_message = "range must contain at least one combo";
        return false;
    }

    std::array<Card, max_dead_cards_for_range_filter> used_cards{};
    std::size_t used_count = 0;

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

    for (std::size_t combo_index = 0; combo_index < input.hero_range.combo_count; ++combo_index) {
        if (hole_cards_have_duplicate_card(input.hero_range.combos[combo_index])) {
            error_message = "range contains an invalid combo";
            return false;
        }
    }

    error_message = "";
    return true;
}

void collect_exact_dead_cards(
    const HeadsUpSimulationInput& input,
    std::array<Card, max_dead_cards_for_range_filter>& dead_cards,
    std::size_t& dead_card_count) noexcept
{
    dead_card_count = 0;

    for (const Card& card : input.hero_hole) {
        dead_cards[dead_card_count] = card;
        ++dead_card_count;
    }

    for (const Card& card : input.villain_hole) {
        dead_cards[dead_card_count] = card;
        ++dead_card_count;
    }

    for (std::size_t index = 0; index < input.board_count; ++index) {
        dead_cards[dead_card_count] = input.board[index];
        ++dead_card_count;
    }
}

void collect_range_vs_hand_dead_cards(
    const HeadsUpRangeVsHandSimulationInput& input,
    std::array<Card, max_dead_cards_for_range_filter>& dead_cards,
    std::size_t& dead_card_count) noexcept
{
    dead_card_count = 0;

    for (const Card& card : input.villain_hole) {
        dead_cards[dead_card_count] = card;
        ++dead_card_count;
    }

    for (std::size_t index = 0; index < input.board_count; ++index) {
        dead_cards[dead_card_count] = input.board[index];
        ++dead_card_count;
    }
}

bool prepare_base_remaining_cards(
    const std::array<Card, max_dead_cards_for_range_filter>& dead_cards,
    std::size_t dead_card_count,
    std::array<Card, Deck::card_count>& remaining_cards,
    std::size_t& remaining_count) noexcept
{
    Deck deck{};
    deck.reset();

    for (std::size_t index = 0; index < dead_card_count; ++index) {
        if (!deck.remove(dead_cards[index])) {
            return false;
        }
    }

    deck.copy_remaining_cards(remaining_cards, remaining_count);
    return true;
}

bool remove_card_from_remaining_cards(
    std::array<Card, Deck::card_count>& remaining_cards,
    std::size_t& remaining_count,
    const Card& card) noexcept
{
    for (std::size_t index = 0; index < remaining_count; ++index) {
        if (remaining_cards[index] == card) {
            --remaining_count;
            std::swap(remaining_cards[index], remaining_cards[remaining_count]);
            return true;
        }
    }

    return false;
}

}  // namespace

const char* MonteCarloSimulator::status() const noexcept
{
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
    if (!validate_exact_input(input, error_message)) {
        result.error_message = error_message;
        return result;
    }

    std::array<Card, max_dead_cards_for_range_filter> dead_cards{};
    std::size_t dead_card_count = 0;
    collect_exact_dead_cards(input, dead_cards, dead_card_count);

    std::array<Card, Deck::card_count> base_remaining_cards{};
    std::size_t base_remaining_count = 0;

    if (!prepare_base_remaining_cards(
            dead_cards, dead_card_count, base_remaining_cards, base_remaining_count)) {
        result.error_message = "failed to prepare the deck";
        return result;
    }

    std::mt19937 rng(input.seed);
    std::uniform_int_distribution<std::size_t> distribution{};

    std::array<Card, seven_card_hand_size> hero_cards{
        input.hero_hole[0],
        input.hero_hole[1],
        input.board[0],
        input.board[1],
        input.board[2],
        input.board[3],
        input.board[4],
    };
    std::array<Card, seven_card_hand_size> villain_cards{
        input.villain_hole[0],
        input.villain_hole[1],
        input.board[0],
        input.board[1],
        input.board[2],
        input.board[3],
        input.board[4],
    };

    for (std::size_t iteration = 0; iteration < input.iterations; ++iteration) {
        std::array<Card, Deck::card_count> remaining_cards{};
        std::size_t remaining_count = base_remaining_count;

        if (input.board_count < 5) {
            std::copy_n(base_remaining_cards.data(), base_remaining_count, remaining_cards.data());
        }

        for (std::size_t board_index = input.board_count; board_index < 5; ++board_index) {
            if (remaining_count == 0) {
                result.error_message = "not enough cards left to complete the board";
                return result;
            }

            distribution.param(
                std::uniform_int_distribution<std::size_t>::param_type(0, remaining_count - 1));
            const std::size_t random_index = distribution(rng);
            const Card drawn_card = remaining_cards[random_index];

            hero_cards[board_index + 2] = drawn_card;
            villain_cards[board_index + 2] = drawn_card;

            --remaining_count;
            std::swap(remaining_cards[random_index], remaining_cards[remaining_count]);
        }

        const HandStrength hero_strength = evaluate_7_card_strength(hero_cards);
        const HandStrength villain_strength = evaluate_7_card_strength(villain_cards);

        if (hero_strength > villain_strength) {
            ++result.hero_wins;
        } else if (hero_strength < villain_strength) {
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

HeadsUpSimulationResult MonteCarloSimulator::simulate_heads_up_range_vs_hand(
    const HeadsUpRangeVsHandSimulationInput& input) const noexcept
{
    HeadsUpSimulationResult result{};

    const char* error_message = "";
    if (!validate_range_vs_hand_input(input, error_message)) {
        result.error_message = error_message;
        return result;
    }

    std::array<Card, max_dead_cards_for_range_filter> dead_cards{};
    std::size_t dead_card_count = 0;
    collect_range_vs_hand_dead_cards(input, dead_cards, dead_card_count);

    const ExpandedRangeToken valid_hero_range =
        filter_range_token_dead_cards(input.hero_range, dead_cards, dead_card_count);

    if (valid_hero_range.combo_count == 0) {
        result.error_message = "range has no valid combos after dead-card filtering";
        return result;
    }

    std::array<Card, Deck::card_count> base_remaining_cards{};
    std::size_t base_remaining_count = 0;

    if (!prepare_base_remaining_cards(
            dead_cards, dead_card_count, base_remaining_cards, base_remaining_count)) {
        result.error_message = "failed to prepare the deck";
        return result;
    }

    std::mt19937 rng(input.seed);
    std::uniform_int_distribution<std::size_t> combo_distribution{};
    std::uniform_int_distribution<std::size_t> board_distribution{};

    std::array<Card, seven_card_hand_size> villain_cards{
        input.villain_hole[0],
        input.villain_hole[1],
        input.board[0],
        input.board[1],
        input.board[2],
        input.board[3],
        input.board[4],
    };

    for (std::size_t iteration = 0; iteration < input.iterations; ++iteration) {
        combo_distribution.param(
            std::uniform_int_distribution<std::size_t>::param_type(0, valid_hero_range.combo_count - 1));
        const HoleCards hero_hole = valid_hero_range.combos[combo_distribution(rng)];

        std::array<Card, seven_card_hand_size> hero_cards{
            hero_hole.cards[0],
            hero_hole.cards[1],
            input.board[0],
            input.board[1],
            input.board[2],
            input.board[3],
            input.board[4],
        };

        std::array<Card, Deck::card_count> remaining_cards{};
        std::size_t remaining_count = base_remaining_count;
        std::copy_n(base_remaining_cards.data(), base_remaining_count, remaining_cards.data());

        if (!remove_card_from_remaining_cards(remaining_cards, remaining_count, hero_hole.cards[0]) ||
            !remove_card_from_remaining_cards(remaining_cards, remaining_count, hero_hole.cards[1])) {
            result.error_message = "failed to remove sampled range cards from the deck";
            return result;
        }

        for (std::size_t board_index = input.board_count; board_index < 5; ++board_index) {
            if (remaining_count == 0) {
                result.error_message = "not enough cards left to complete the board";
                return result;
            }

            board_distribution.param(
                std::uniform_int_distribution<std::size_t>::param_type(0, remaining_count - 1));
            const std::size_t random_index = board_distribution(rng);
            const Card drawn_card = remaining_cards[random_index];

            hero_cards[board_index + 2] = drawn_card;
            villain_cards[board_index + 2] = drawn_card;

            --remaining_count;
            std::swap(remaining_cards[random_index], remaining_cards[remaining_count]);
        }

        const HandStrength hero_strength = evaluate_7_card_strength(hero_cards);
        const HandStrength villain_strength = evaluate_7_card_strength(villain_cards);

        if (hero_strength > villain_strength) {
            ++result.hero_wins;
        } else if (hero_strength < villain_strength) {
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
