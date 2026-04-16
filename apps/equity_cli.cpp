#include "poker/card.hpp"
#include "poker/deck.hpp"
#include "poker/simulator.hpp"

#include <cstdlib>
#include <iostream>

namespace {

void print_usage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " <card1> <card2>\n";
    std::cout << "Example: " << program_name << " As Kh\n";
    std::cout << "Cards use rank+suit, such as As, Td, 10h, or 2c.\n";
}

void print_card(const poker::Card& card)
{
    std::cout << poker::rank_name(card.rank) << " of " << poker::suit_name(card.suit) << " ("
              << poker::rank_symbol(card.rank) << poker::suit_symbol(card.suit) << ")";
}

}  // namespace

int main(int argc, char* argv[])
{
    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    poker::Card first_card{poker::Rank::two, poker::Suit::clubs};
    poker::Card second_card{poker::Rank::two, poker::Suit::clubs};

    if (!poker::parse_card(argv[1], first_card)) {
        std::cerr << "Could not parse first card: " << argv[1] << '\n';
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!poker::parse_card(argv[2], second_card)) {
        std::cerr << "Could not parse second card: " << argv[2] << '\n';
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (first_card.rank == second_card.rank && first_card.suit == second_card.suit) {
        std::cerr << "Duplicate cards are not allowed.\n";
        return EXIT_FAILURE;
    }

    // Create a full 52-card deck.
    poker::Deck deck{};

    // Create one simulator object from the library code.
    const poker::MonteCarloSimulator simulator{};

    // This executable is intentionally small.
    // Its job is to call into the library and print results for the user.
    std::cout << "poker-monte-carlo scaffold\n";
    std::cout << "Parsed cards:\n";
    std::cout << "  ";
    print_card(first_card);
    std::cout << '\n';
    std::cout << "  ";
    print_card(second_card);
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
