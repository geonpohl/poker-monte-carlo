#include "poker/card.hpp"

#include <cctype>
#include <cstring>

namespace poker {

namespace {

char lowercase_char(char value) noexcept
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(value)));
}

}  // namespace

const char* suit_name(Suit suit) noexcept
{
    switch (suit) {
    case Suit::clubs:
        return "clubs";
    case Suit::diamonds:
        return "diamonds";
    case Suit::hearts:
        return "hearts";
    case Suit::spades:
        return "spades";
    }

    return "unknown suit";
}

const char* rank_name(Rank rank) noexcept
{
    switch (rank) {
    case Rank::two:
        return "two";
    case Rank::three:
        return "three";
    case Rank::four:
        return "four";
    case Rank::five:
        return "five";
    case Rank::six:
        return "six";
    case Rank::seven:
        return "seven";
    case Rank::eight:
        return "eight";
    case Rank::nine:
        return "nine";
    case Rank::ten:
        return "ten";
    case Rank::jack:
        return "jack";
    case Rank::queen:
        return "queen";
    case Rank::king:
        return "king";
    case Rank::ace:
        return "ace";
    }

    return "unknown rank";
}

char suit_symbol(Suit suit) noexcept
{
    switch (suit) {
    case Suit::clubs:
        return 'c';
    case Suit::diamonds:
        return 'd';
    case Suit::hearts:
        return 'h';
    case Suit::spades:
        return 's';
    }

    return '?';
}

char rank_symbol(Rank rank) noexcept
{
    switch (rank) {
    case Rank::two:
        return '2';
    case Rank::three:
        return '3';
    case Rank::four:
        return '4';
    case Rank::five:
        return '5';
    case Rank::six:
        return '6';
    case Rank::seven:
        return '7';
    case Rank::eight:
        return '8';
    case Rank::nine:
        return '9';
    case Rank::ten:
        return 'T';
    case Rank::jack:
        return 'J';
    case Rank::queen:
        return 'Q';
    case Rank::king:
        return 'K';
    case Rank::ace:
        return 'A';
    }

    return '?';
}

bool parse_suit(const char* text, Suit& out_suit) noexcept
{
    if (text == nullptr || text[0] == '\0' || text[1] != '\0') {
        return false;
    }

    switch (lowercase_char(text[0])) {
    case 'c':
        out_suit = Suit::clubs;
        return true;
    case 'd':
        out_suit = Suit::diamonds;
        return true;
    case 'h':
        out_suit = Suit::hearts;
        return true;
    case 's':
        out_suit = Suit::spades;
        return true;
    default:
        return false;
    }
}

bool parse_rank(const char* text, Rank& out_rank) noexcept
{
    if (text == nullptr || text[0] == '\0') {
        return false;
    }

    // Accept both "T" and "10" for ten.
    if (text[0] == '1' && text[1] == '0' && text[2] == '\0') {
        out_rank = Rank::ten;
        return true;
    }

    if (text[1] != '\0') {
        return false;
    }

    switch (lowercase_char(text[0])) {
    case '2':
        out_rank = Rank::two;
        return true;
    case '3':
        out_rank = Rank::three;
        return true;
    case '4':
        out_rank = Rank::four;
        return true;
    case '5':
        out_rank = Rank::five;
        return true;
    case '6':
        out_rank = Rank::six;
        return true;
    case '7':
        out_rank = Rank::seven;
        return true;
    case '8':
        out_rank = Rank::eight;
        return true;
    case '9':
        out_rank = Rank::nine;
        return true;
    case 't':
        out_rank = Rank::ten;
        return true;
    case 'j':
        out_rank = Rank::jack;
        return true;
    case 'q':
        out_rank = Rank::queen;
        return true;
    case 'k':
        out_rank = Rank::king;
        return true;
    case 'a':
        out_rank = Rank::ace;
        return true;
    default:
        return false;
    }
}

bool parse_card(const char* text, Card& out_card) noexcept
{
    if (text == nullptr) {
        return false;
    }

    const std::size_t length = std::strlen(text);

    // We accept:
    // "As"  -> ace of spades
    // "10h" -> ten of hearts
    if (length != 2 && length != 3) {
        return false;
    }

    Suit parsed_suit{};
    if (!parse_suit(text + length - 1, parsed_suit)) {
        return false;
    }

    Rank parsed_rank{};

    if (length == 2) {
        char rank_text[2]{text[0], '\0'};
        if (!parse_rank(rank_text, parsed_rank)) {
            return false;
        }
    } else {
        char rank_text[3]{text[0], text[1], '\0'};
        if (!parse_rank(rank_text, parsed_rank)) {
            return false;
        }
    }

    out_card = Card{parsed_rank, parsed_suit};
    return true;
}

}  // namespace poker
