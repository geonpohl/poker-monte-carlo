#include "poker/simulator.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

namespace {

bool parse_iterations(const char* text, std::size_t& out_iterations)
{
    char* end = nullptr;
    const unsigned long long parsed_value = std::strtoull(text, &end, 10);

    if (text == end || *end != '\0' || parsed_value == 0) {
        return false;
    }

    out_iterations = static_cast<std::size_t>(parsed_value);
    return true;
}

}  // namespace

int main(int argc, char* argv[])
{
    std::size_t iterations = 200'000;

    if (argc == 2 && !parse_iterations(argv[1], iterations)) {
        std::cerr << "Could not parse iterations: " << argv[1] << '\n';
        return EXIT_FAILURE;
    }

    poker::HeadsUpSimulationInput input{};
    input.hero_hole = {
        poker::Card{poker::Rank::ace, poker::Suit::spades},
        poker::Card{poker::Rank::ace, poker::Suit::hearts},
    };
    input.villain_hole = {
        poker::Card{poker::Rank::king, poker::Suit::spades},
        poker::Card{poker::Rank::king, poker::Suit::hearts},
    };
    input.iterations = iterations;
    input.seed = 1337;

    const poker::MonteCarloSimulator simulator{};

    const auto start = std::chrono::steady_clock::now();
    const poker::HeadsUpSimulationResult result = simulator.simulate_heads_up(input);
    const auto finish = std::chrono::steady_clock::now();

    if (!result.success) {
        std::cerr << "Benchmark simulation failed: " << result.error_message << '\n';
        return EXIT_FAILURE;
    }

    const auto elapsed_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
    const double elapsed_seconds =
        static_cast<double>(elapsed_ns) / 1'000'000'000.0;
    const double iterations_per_second =
        static_cast<double>(result.iterations) / elapsed_seconds;
    const double nanoseconds_per_iteration =
        static_cast<double>(elapsed_ns) / static_cast<double>(result.iterations);

    std::cout << "poker-monte-carlo performance benchmark\n";
    std::cout << "Scenario: As Ah vs Ks Kh preflop\n";
    std::cout << "Iterations: " << result.iterations << '\n';
    std::cout << "Elapsed seconds: " << elapsed_seconds << '\n';
    std::cout << "Iterations per second: " << iterations_per_second << '\n';
    std::cout << "Nanoseconds per iteration: " << nanoseconds_per_iteration << '\n';
    std::cout << "Hero equity: " << result.hero_equity() << '\n';

    return EXIT_SUCCESS;
}
