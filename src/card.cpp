#include "poker/card.hpp"

namespace poker {

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

}  // namespace poker
