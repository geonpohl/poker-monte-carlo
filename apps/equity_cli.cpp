#include "poker/card.hpp"
#include "poker/deck.hpp"
#include "poker/simulator.hpp"

#include <cstdlib>
#include <iostream>

namespace {

void print_card(const poker::Card& card)
{
    std::cout << poker::rank_name(card.rank) << " of " << poker::suit_name(card.suit) << " ("
              << poker::rank_symbol(card.rank) << poker::suit_symbol(card.suit) << ")";
}

}  // namespace

int main()
{
    // Create a couple of example cards using aggregate initialization:
    // Card{rank, suit}
    const poker::Card ace_of_spades{poker::Rank::ace, poker::Suit::spades};
    const poker::Card ten_of_hearts{poker::Rank::ten, poker::Suit::hearts};

    // Create a full 52-card deck.
    poker::Deck deck{};

    // Create one simulator object from the library code.
    const poker::MonteCarloSimulator simulator{};

    // This executable is intentionally small.
    // Its job is to call into the library and print results for the user.
    std::cout << "poker-monte-carlo scaffold\n";
    std::cout << "Example cards:\n";
    std::cout << "  ";
    print_card(ace_of_spades);
    std::cout << '\n';
    std::cout << "  ";
    print_card(ten_of_hearts);
    std::cout << '\n';

    std::cout << "Deck example:\n";
    std::cout << "  Fresh deck has " << deck.remaining() << " cards\n";

    // We create a variable first, then ask draw() to write into it.
    poker::Card drawn_card{poker::Rank::two, poker::Suit::clubs};

    if (deck.draw(drawn_card)) {
        std::cout << "  First draw: ";
        print_card(drawn_card);
        std::cout << '\n';
    }

    if (deck.draw(drawn_card)) {
        std::cout << "  Second draw: ";
        print_card(drawn_card);
        std::cout << '\n';
    }

    std::cout << "  Cards remaining after two draws: " << deck.remaining() << '\n';
    std::cout << simulator.status() << '\n';

    return EXIT_SUCCESS;
}
