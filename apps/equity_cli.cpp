#include "poker/card.hpp"
#include "poker/deck.hpp"
#include "poker/hand.hpp"
#include "poker/simulator.hpp"

#include <array>
#include <cstring>
#include <cstdlib>
#include <iostream>

namespace {

void print_usage(const char* program_name)
{
    std::cout << "Usage:\n";
    std::cout << "  " << program_name << " <card1> <card2>\n";
    std::cout << "  " << program_name << " <card1> <card2> <card3> <card4> <card5>\n";
    std::cout << "  " << program_name
              << " <card1> <card2> <card3> <card4> <card5> <card6> <card7>\n";
    std::cout << "  " << program_name << " simulate <hero1> <hero2> <villain1> <villain2>\n";
    std::cout << "  " << program_name
              << " simulate <hero1> <hero2> <villain1> <villain2> <iterations>\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " As Kh\n";
    std::cout << "  " << program_name << " As Ks Qs Js Ts\n";
    std::cout << "  " << program_name << " As Ks Qs Js Ts 2d 3c\n";
    std::cout << "  " << program_name << " simulate As Ah Ks Kh 20000\n";
    std::cout << "Cards use rank+suit, such as As, Td, 10h, or 2c.\n";
}

void print_card(const poker::Card& card)
{
    std::cout << poker::rank_name(card.rank) << " of " << poker::suit_name(card.suit) << " ("
              << poker::rank_symbol(card.rank) << poker::suit_symbol(card.suit) << ")";
}

bool cards_equal(const poker::Card& left, const poker::Card& right)
{
    return left.rank == right.rank && left.suit == right.suit;
}

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

template <std::size_t N>
bool has_duplicate_cards(const std::array<poker::Card, N>& cards)
{
    for (std::size_t left = 0; left < cards.size(); ++left) {
        for (std::size_t right = left + 1; right < cards.size(); ++right) {
            if (cards_equal(cards[left], cards[right])) {
                return true;
            }
        }
    }

    return false;
}

void print_tie_break_ranks(const poker::EvaluatedHand& hand)
{
    std::cout << "Tie-break ranks:";

    if (hand.tie_break_count == 0) {
        std::cout << " none";
    }

    for (std::size_t index = 0; index < hand.tie_break_count; ++index) {
        std::cout << ' ' << poker::rank_symbol(hand.tie_break_ranks[index]);
    }

    std::cout << '\n';
}

}  // namespace

int main(int argc, char* argv[])
{
    if (argc >= 2 && std::strcmp(argv[1], "simulate") == 0) {
        if (argc != 6 && argc != 7) {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }

        poker::HeadsUpSimulationInput input{};

        if (!poker::parse_card(argv[2], input.hero_hole[0])) {
            std::cerr << "Could not parse hero card 1: " << argv[2] << '\n';
            return EXIT_FAILURE;
        }

        if (!poker::parse_card(argv[3], input.hero_hole[1])) {
            std::cerr << "Could not parse hero card 2: " << argv[3] << '\n';
            return EXIT_FAILURE;
        }

        if (!poker::parse_card(argv[4], input.villain_hole[0])) {
            std::cerr << "Could not parse villain card 1: " << argv[4] << '\n';
            return EXIT_FAILURE;
        }

        if (!poker::parse_card(argv[5], input.villain_hole[1])) {
            std::cerr << "Could not parse villain card 2: " << argv[5] << '\n';
            return EXIT_FAILURE;
        }

        if (argc == 7 && !parse_iterations(argv[6], input.iterations)) {
            std::cerr << "Could not parse iterations: " << argv[6] << '\n';
            return EXIT_FAILURE;
        }

        const poker::MonteCarloSimulator simulator{};
        const poker::HeadsUpSimulationResult result = simulator.simulate_heads_up(input);

        if (!result.success) {
            std::cerr << "Simulation error: " << result.error_message << '\n';
            return EXIT_FAILURE;
        }

        std::cout << "poker-monte-carlo heads-up simulation\n";
        std::cout << "Hero:\n";
        std::cout << "  ";
        print_card(input.hero_hole[0]);
        std::cout << '\n';
        std::cout << "  ";
        print_card(input.hero_hole[1]);
        std::cout << '\n';
        std::cout << "Villain:\n";
        std::cout << "  ";
        print_card(input.villain_hole[0]);
        std::cout << '\n';
        std::cout << "  ";
        print_card(input.villain_hole[1]);
        std::cout << '\n';
        std::cout << "Iterations: " << result.iterations << '\n';
        std::cout << "Hero wins: " << result.hero_wins << '\n';
        std::cout << "Villain wins: " << result.villain_wins << '\n';
        std::cout << "Ties: " << result.ties << '\n';
        std::cout << "Hero equity: " << result.hero_equity() << '\n';
        std::cout << "Villain equity: " << result.villain_equity() << '\n';

        return EXIT_SUCCESS;
    }

    if (argc != 3 && argc != 6 && argc != 8) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 6) {
        std::array<poker::Card, poker::five_card_hand_size> cards{
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
        };

        for (std::size_t index = 0; index < cards.size(); ++index) {
            if (!poker::parse_card(argv[static_cast<int>(index) + 1], cards[index])) {
                std::cerr << "Could not parse card " << (index + 1) << ": "
                          << argv[static_cast<int>(index) + 1] << '\n';
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
        }

        if (has_duplicate_cards(cards)) {
            std::cerr << "Duplicate cards are not allowed.\n";
            return EXIT_FAILURE;
        }

        const poker::EvaluatedHand hand = poker::evaluate_5_card_hand(cards);

        std::cout << "poker-monte-carlo 5-card evaluator\n";
        std::cout << "Cards:\n";

        for (const poker::Card& card : cards) {
            std::cout << "  ";
            print_card(card);
            std::cout << '\n';
        }

        std::cout << "Category: " << poker::hand_category_name(hand.category) << '\n';
        print_tie_break_ranks(hand);

        return EXIT_SUCCESS;
    }

    if (argc == 8) {
        std::array<poker::Card, poker::seven_card_hand_size> cards{
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
            poker::Card{poker::Rank::two, poker::Suit::clubs},
        };

        for (std::size_t index = 0; index < cards.size(); ++index) {
            if (!poker::parse_card(argv[static_cast<int>(index) + 1], cards[index])) {
                std::cerr << "Could not parse card " << (index + 1) << ": "
                          << argv[static_cast<int>(index) + 1] << '\n';
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
        }

        if (has_duplicate_cards(cards)) {
            std::cerr << "Duplicate cards are not allowed.\n";
            return EXIT_FAILURE;
        }

        const poker::EvaluatedHand hand = poker::evaluate_7_card_hand(cards);

        std::cout << "poker-monte-carlo 7-card evaluator\n";
        std::cout << "Cards:\n";

        for (const poker::Card& card : cards) {
            std::cout << "  ";
            print_card(card);
            std::cout << '\n';
        }

        std::cout << "Category: " << poker::hand_category_name(hand.category) << '\n';
        print_tie_break_ranks(hand);

        return EXIT_SUCCESS;
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

    if (cards_equal(first_card, second_card)) {
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
