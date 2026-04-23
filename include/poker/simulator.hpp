#pragma once

#include "poker/card.hpp"
#include "poker/range.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace poker {

inline constexpr std::size_t hole_card_count = 2;

struct HeadsUpSimulationInput {
    std::array<Card, hole_card_count> hero_hole{
        Card{Rank::two, Suit::clubs},
        Card{Rank::three, Suit::clubs},
    };

    std::array<Card, hole_card_count> villain_hole{
        Card{Rank::four, Suit::clubs},
        Card{Rank::five, Suit::clubs},
    };

    std::array<Card, 5> board{
        Card{Rank::two, Suit::diamonds},
        Card{Rank::three, Suit::diamonds},
        Card{Rank::four, Suit::diamonds},
        Card{Rank::five, Suit::diamonds},
        Card{Rank::six, Suit::diamonds},
    };

    std::size_t board_count{0};
    std::size_t iterations{10'000};
    std::uint32_t seed{1337};
};

struct HeadsUpSimulationResult {
    bool success{false};
    const char* error_message{"simulation not run"};
    std::size_t iterations{0};
    std::size_t hero_wins{0};
    std::size_t villain_wins{0};
    std::size_t ties{0};

    [[nodiscard]] double hero_equity() const noexcept;
    [[nodiscard]] double villain_equity() const noexcept;
};

struct HeadsUpRangeVsHandSimulationInput {
    ExpandedRangeToken hero_range{};

    std::array<Card, hole_card_count> villain_hole{
        Card{Rank::four, Suit::clubs},
        Card{Rank::five, Suit::clubs},
    };

    std::array<Card, 5> board{
        Card{Rank::two, Suit::diamonds},
        Card{Rank::three, Suit::diamonds},
        Card{Rank::four, Suit::diamonds},
        Card{Rank::five, Suit::diamonds},
        Card{Rank::six, Suit::diamonds},
    };

    std::size_t board_count{0};
    std::size_t iterations{10'000};
    std::uint32_t seed{1337};
};

class MonteCarloSimulator final {
  public:
    // Returns a short message describing the current simulator state.
    //
    // const:
    // Calling this function does not change the simulator object.
    //
    // noexcept:
    // This function promises not to throw exceptions.
    [[nodiscard]] const char* status() const noexcept;

    [[nodiscard]] HeadsUpSimulationResult simulate_heads_up(
        const HeadsUpSimulationInput& input) const noexcept;

    [[nodiscard]] HeadsUpSimulationResult simulate_heads_up_range_vs_hand(
        const HeadsUpRangeVsHandSimulationInput& input) const noexcept;
};

}  // namespace poker
