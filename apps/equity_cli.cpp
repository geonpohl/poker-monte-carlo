#include "poker/card.hpp"
#include "poker/simulator.hpp"

#include <cstdlib>
#include <iostream>

int main()
{
    // Create a couple of example cards using aggregate initialization:
    // Card{rank, suit}
    const poker::Card ace_of_spades{poker::Rank::ace, poker::Suit::spades};
    const poker::Card ten_of_hearts{poker::Rank::ten, poker::Suit::hearts};

    // Create one simulator object from the library code.
    const poker::MonteCarloSimulator simulator{};

    // This executable is intentionally small.
    // Its job is to call into the library and print results for the user.
    std::cout << "poker-monte-carlo scaffold\n";
    std::cout << "Example cards:\n";
    std::cout << "  " << poker::rank_name(ace_of_spades.rank) << " of "
              << poker::suit_name(ace_of_spades.suit) << " (" << poker::rank_symbol(ace_of_spades.rank)
              << poker::suit_symbol(ace_of_spades.suit) << ")\n";
    std::cout << "  " << poker::rank_name(ten_of_hearts.rank) << " of "
              << poker::suit_name(ten_of_hearts.suit) << " (" << poker::rank_symbol(ten_of_hearts.rank)
              << poker::suit_symbol(ten_of_hearts.suit) << ")\n";
    std::cout << simulator.status() << '\n';

    return EXIT_SUCCESS;
}
