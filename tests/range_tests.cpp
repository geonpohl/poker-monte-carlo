#include "poker/range.hpp"
#include "poker/simulator.hpp"

#include <array>
#include <cstring>
#include <cstdlib>
#include <iostream>

namespace {

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

bool expect_pattern(
    const char* text,
    poker::Rank expected_high_rank,
    poker::Rank expected_low_rank,
    bool expected_suited_only,
    bool expected_offsuit_only,
    const char* message)
{
    poker::StartingHandPattern pattern{};

    if (!poker::parse_starting_hand_pattern(text, pattern)) {
        return expect(false, message);
    }

    return expect(
        pattern.high_rank == expected_high_rank && pattern.low_rank == expected_low_rank &&
            pattern.suited_only == expected_suited_only &&
            pattern.offsuit_only == expected_offsuit_only,
        message);
}

bool expect_invalid_pattern(const char* text, const char* message)
{
    poker::StartingHandPattern pattern{};
    return expect(!poker::parse_starting_hand_pattern(text, pattern), message);
}

bool expect_combo_count(const char* text, std::size_t expected_combo_count, const char* message)
{
    poker::ExpandedRangeToken range{};

    if (!poker::expand_range_token(text, range)) {
        return expect(false, message);
    }

    return expect(range.combo_count == expected_combo_count, message);
}

bool expect_no_duplicate_cards_in_range(const char* text, const char* message)
{
    poker::ExpandedRangeToken range{};

    if (!poker::expand_range_token(text, range)) {
        return expect(false, message);
    }

    for (std::size_t index = 0; index < range.combo_count; ++index) {
        if (poker::hole_cards_have_duplicate_card(range.combos[index])) {
            return expect(false, message);
        }
    }

    return true;
}

bool expect_unique_combos(const char* text, const char* message)
{
    poker::ExpandedRangeToken range{};

    if (!poker::expand_range_token(text, range)) {
        return expect(false, message);
    }

    for (std::size_t left = 0; left < range.combo_count; ++left) {
        for (std::size_t right = left + 1; right < range.combo_count; ++right) {
            if (range.combos[left] == range.combos[right]) {
                return expect(false, message);
            }
        }
    }

    return true;
}

bool expect_filtered_combo_count(
    const char* text,
    const std::array<poker::Card, poker::max_dead_cards_for_range_filter>& dead_cards,
    std::size_t dead_card_count,
    std::size_t expected_combo_count,
    const char* message)
{
    poker::ExpandedRangeToken range{};

    if (!poker::expand_range_token(text, range)) {
        return expect(false, message);
    }

    const poker::ExpandedRangeToken filtered_range =
        poker::filter_range_token_dead_cards(range, dead_cards, dead_card_count);

    return expect(filtered_range.combo_count == expected_combo_count, message);
}

bool expect_range_simulation_result(
    const poker::HeadsUpRangeVsHandSimulationInput& input,
    std::size_t expected_hero_wins,
    std::size_t expected_villain_wins,
    std::size_t expected_ties,
    const char* message)
{
    const poker::MonteCarloSimulator simulator{};
    const poker::HeadsUpSimulationResult result = simulator.simulate_heads_up_range_vs_hand(input);

    return expect(
        result.success && result.hero_wins == expected_hero_wins &&
            result.villain_wins == expected_villain_wins && result.ties == expected_ties,
        message);
}

bool expect_range_simulation_failure(
    const poker::HeadsUpRangeVsHandSimulationInput& input,
    const char* expected_error_message,
    const char* message)
{
    const poker::MonteCarloSimulator simulator{};
    const poker::HeadsUpSimulationResult result = simulator.simulate_heads_up_range_vs_hand(input);

    return expect(
        !result.success && std::strcmp(result.error_message, expected_error_message) == 0,
        message);
}

}  // namespace

int main()
{
    bool ok = true;

    ok = expect_pattern(
             "QQ",
             poker::Rank::queen,
             poker::Rank::queen,
             false,
             false,
             "QQ should parse as a pair pattern") &&
         ok;

    ok = expect_pattern(
             "AKs",
             poker::Rank::ace,
             poker::Rank::king,
             true,
             false,
             "AKs should parse as suited-only") &&
         ok;

    ok = expect_pattern(
             "AQo",
             poker::Rank::ace,
             poker::Rank::queen,
             false,
             true,
             "AQo should parse as offsuit-only") &&
         ok;

    ok = expect_pattern(
             "kAs",
             poker::Rank::ace,
             poker::Rank::king,
             true,
             false,
             "parser should normalize rank order") &&
         ok;

    ok = expect_invalid_pattern("AK", "non-pair token without suitedness should be rejected") && ok;
    ok = expect_invalid_pattern("QQs", "pair token with suited suffix should be rejected") && ok;
    ok = expect_invalid_pattern("Axs", "invalid rank should be rejected") && ok;

    ok = expect_combo_count("QQ", 6, "QQ should expand to 6 pocket-pair combos") && ok;
    ok = expect_combo_count("AKs", 4, "AKs should expand to 4 suited combos") && ok;
    ok = expect_combo_count("AQo", 12, "AQo should expand to 12 offsuit combos") && ok;

    ok = expect_no_duplicate_cards_in_range(
             "QQ", "expanded pair combos should not contain duplicate cards") &&
         ok;
    ok = expect_no_duplicate_cards_in_range(
             "AKs", "expanded suited combos should not contain duplicate cards") &&
         ok;
    ok = expect_no_duplicate_cards_in_range(
             "AQo", "expanded offsuit combos should not contain duplicate cards") &&
         ok;

    ok = expect_unique_combos("QQ", "expanded pair combos should be unique") && ok;
    ok = expect_unique_combos("AKs", "expanded suited combos should be unique") && ok;
    ok = expect_unique_combos("AQo", "expanded offsuit combos should be unique") && ok;

    ok = expect_filtered_combo_count(
             "QQ",
             std::array<poker::Card, poker::max_dead_cards_for_range_filter>{
                 poker::Card{poker::Rank::queen, poker::Suit::clubs},
             },
             1,
             3,
             "dead-card filtering should remove blocked QQ combos") &&
         ok;

    ok = expect_filtered_combo_count(
             "AKs",
             std::array<poker::Card, poker::max_dead_cards_for_range_filter>{
                 poker::Card{poker::Rank::ace, poker::Suit::clubs},
             },
             1,
             3,
             "dead-card filtering should remove blocked AKs combos") &&
         ok;

    ok = expect_filtered_combo_count(
             "AQo",
             std::array<poker::Card, poker::max_dead_cards_for_range_filter>{
                 poker::Card{poker::Rank::ace, poker::Suit::hearts},
             },
             1,
             9,
             "dead-card filtering should remove blocked AQo combos") &&
         ok;

    poker::HeadsUpRangeVsHandSimulationInput hero_range_wins_input{};
    ok = expect(poker::expand_range_token("AKs", hero_range_wins_input.hero_range), "AKs should parse for range simulation") &&
         ok;
    hero_range_wins_input.villain_hole = {
        poker::Card{poker::Rank::ace, poker::Suit::clubs},
        poker::Card{poker::Rank::ace, poker::Suit::diamonds},
    };
    hero_range_wins_input.board = {
        poker::Card{poker::Rank::queen, poker::Suit::spades},
        poker::Card{poker::Rank::jack, poker::Suit::spades},
        poker::Card{poker::Rank::ten, poker::Suit::spades},
        poker::Card{poker::Rank::two, poker::Suit::diamonds},
        poker::Card{poker::Rank::three, poker::Suit::clubs},
    };
    hero_range_wins_input.board_count = 5;
    hero_range_wins_input.iterations = 10;

    ok = expect_range_simulation_result(
             hero_range_wins_input,
             10,
             0,
             0,
             "range-vs-hand simulation should use valid AKs combos and count hero wins") &&
         ok;

    poker::HeadsUpRangeVsHandSimulationInput blocked_range_input{};
    ok = expect(poker::expand_range_token("QQ", blocked_range_input.hero_range), "QQ should parse for blocked range test") &&
         ok;
    blocked_range_input.villain_hole = {
        poker::Card{poker::Rank::queen, poker::Suit::clubs},
        poker::Card{poker::Rank::queen, poker::Suit::diamonds},
    };
    blocked_range_input.board = {
        poker::Card{poker::Rank::queen, poker::Suit::hearts},
        poker::Card{poker::Rank::two, poker::Suit::clubs},
        poker::Card{poker::Rank::three, poker::Suit::diamonds},
        poker::Card{poker::Rank::four, poker::Suit::hearts},
        poker::Card{poker::Rank::five, poker::Suit::spades},
    };
    blocked_range_input.board_count = 1;

    ok = expect_range_simulation_failure(
             blocked_range_input,
             "range has no valid combos after dead-card filtering",
             "range-vs-hand simulation should reject fully blocked ranges") &&
         ok;

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
