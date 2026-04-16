# poker-monte-carlo

A small C++ starting point for building a Monte Carlo poker equity simulator step by step.

This version is intentionally light on scaffolding so the interesting parts of the codebase are things we implement together rather than things that already exist.

## Layout

```text
.
|-- apps/          # small executable entry points
|-- include/       # header files
|-- src/           # implementation files
|-- CMakeLists.txt
```

## Quick start

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/poker_cli As Kh
./build/poker_cli As Ks Qs Js Ts
./build/poker_cli As Ks Qs Js Ts 2d 3c
```

Or with the convenience `Makefile`:

```bash
make test
make run ARGS="As Kh"
make run ARGS="As Ks Qs Js Ts"
make run ARGS="As Ks Qs Js Ts 2d 3c"
```

## What exists right now

- A minimal CMake build
- One library target: `poker_core`
- One executable target: `poker_cli`
- A `Card` model with `Suit`, `Rank`, and `Card`
- A `Deck` model with reset and draw behavior
- CLI parsing for two hole cards
- A `HandCategory` type for poker hand strength vocabulary
- An `EvaluatedHand` result type for future hand comparisons
- A 5-card evaluator entry point plus rank/suit counting helpers
- Straight detection, including the wheel straight
- 5-card hand classification and hand-to-hand comparison
- 7-card evaluation by choosing the best 5-card combination
- A focused test suite for hand-ranking confidence checks
- One tiny simulator class with a placeholder method

## Milestone 1: Project Basics

The goal of this milestone is not poker logic yet. The goal is to understand how a simple C++ project is split up.

- `CMakeLists.txt`
  Describes how the project is built.
- `include/poker/simulator.hpp`
  Declares the `MonteCarloSimulator` class.
- `src/simulator.cpp`
  Defines the class function bodies.
- `apps/equity_cli.cpp`
  Contains `main()`, which is the program entry point.

The current flow is:

1. CMake builds `poker_core` from `src/simulator.cpp`.
2. CMake builds `poker_cli` from `apps/equity_cli.cpp`.
3. `poker_cli` links against `poker_core`.
4. `main()` creates a `MonteCarloSimulator` and calls `status()`.

This is a good starter shape because it teaches one important C++ habit early:
put reusable logic in library code, and keep `main()` small.

## Milestone 2: Card Model

This milestone introduces the first real poker domain type.

- `Suit`
  An enum class for clubs, diamonds, hearts, and spades.
- `Rank`
  An enum class for two through ace.
- `Card`
  A small struct that stores one `Rank` and one `Suit`.

We also added helper functions that turn suits and ranks into printable names and symbols.

This milestone is useful because it teaches:

- `enum class`
- `struct`
- header declarations vs source-file definitions
- passing small value types around without dynamic allocation

## Milestone 3: Deck Model

This milestone adds a `Deck` class that owns exactly 52 cards.

- `Deck`
  Stores cards in a `std::array<Card, 52>`.
- `reset()`
  Rebuilds the deck in a known order.
- `draw(Card& out_card)`
  Copies the next card into a variable supplied by the caller.
- `remaining()`
  Reports how many cards are left.

This milestone is useful because it teaches:

- `std::array`
- constructors
- class member variables
- tracking object state with an index
- passing output values by reference
- keeping ownership simple and local

## Milestone 4: Input Parsing

This milestone makes the CLI accept two cards from the command line.

- `parse_rank()`
  Converts text like `A`, `K`, `T`, or `10` into a `Rank`.
- `parse_suit()`
  Converts text like `s` or `h` into a `Suit`.
- `parse_card()`
  Converts text like `As` or `10h` into a `Card`.
- `main(int argc, char* argv[])`
  Reads the command-line arguments and validates them.

This milestone is useful because it teaches:

- command-line arguments
- C-style strings with `const char*`
- simple validation
- returning `bool` for success or failure
- separating parsing logic from CLI printing logic

## Milestone 5.1: Hand Categories

This step does not evaluate hands yet. It gives us the vocabulary we will use when we do.

- `HandCategory`
  Lists the standard poker hand categories from weakest to strongest.
- `hand_category_name()`
  Converts a category into readable text.

This step is useful because it teaches:

- creating a new domain type before writing logic around it
- choosing enum order intentionally
- separating poker terminology from evaluation code

## Milestone 5.2: Evaluation Result Type

This step defines the shape of the answer our evaluator will eventually return.

- `EvaluatedHand`
  Stores the hand category plus ordered tie-break ranks.
- `tie_break_ranks`
  A fixed-size array that will hold the ranks used to compare equal categories.
- `tie_break_count`
  Tells us how many of those ranks are actually meaningful.

This step is useful because it teaches:

- designing result types before implementing algorithms
- fixed-size storage with `std::array`
- planning comparison data separately from evaluation logic

## Milestone 5.3 and 5.4: 5-Card Input Shape and Counting

This step gives the evaluator a real 5-card function signature and adds the counting data it will use later.

- `five_card_hand_size`
  A named constant for the size of a 5-card hand.
- `evaluate_5_card_hand()`
  The entry point for evaluating exactly five cards.
- `HandCounts`
  Stores rank counts and suit counts for one 5-card hand.
- `count_ranks_and_suits()`
  Fills a `HandCounts` result from the cards in the hand.

This step is useful because it teaches:

- using `std::array<Card, 5>` to encode size in the type
- preprocessing data before writing the full algorithm
- converting enum values into array indexes carefully

## Milestone 5.5: Straight Detection

This step adds the logic for detecting whether a 5-card hand is a straight.

- `StraightInfo`
  Reports whether the hand is a straight and, if it is, what the high card is.
- `detect_straight()`
  Uses rank counts to test for five consecutive ranks.
- Wheel support
  Special handling for `A-2-3-4-5`, where the straight high card is `five`.

This step is useful because it teaches:

- scanning ordered data for consecutive patterns
- representing special-case poker rules explicitly
- separating one hand rule from the full evaluator

## Milestone 5.6 and 5.7: Classification and Comparison

This step turns the 5-card evaluator into a real classifier and adds comparison logic for evaluated hands.

- `evaluate_5_card_hand()`
  Now classifies all standard 5-card poker hands.
- `compare_evaluated_hands()`
  Compares two evaluated hands by category first, then by tie-break ranks.
- Tie-break ordering
  The evaluator fills `tie_break_ranks` in strength order so comparison can be simple and predictable.

This step is useful because it teaches:

- building an algorithm in clear rule order
- encoding poker tie-break rules into data
- separating evaluation from comparison

## Milestone 5.8: CLI and Demo Integration

This step exposes the 5-card evaluator through the CLI so we can try real hands from the terminal.

- Two-card mode
  Still works as the earlier parsing/demo mode.
- Five-card mode
  Evaluates a full 5-card poker hand from command-line input.
- CLI output
  Prints the hand category and the tie-break ranks used for comparisons.

This step is useful because it teaches:

- connecting library code to a user-facing entry point
- keeping demo logic in `main()` while evaluator logic stays in the library
- validating larger groups of input cards

## Milestone 5.9: 7-Card Evaluation

This step adds true Hold'em-style evaluation for 7 available cards.

- `evaluate_7_card_hand()`
  Tries every 5-card subset from the 7 cards and keeps the strongest result.
- 21 combinations
  A 7-card hand has exactly 21 distinct 5-card combinations.
- CLI support
  The command-line tool now accepts 7 cards and prints the best evaluated hand.

This step is useful because it teaches:

- solving a larger problem by reusing a smaller correct function
- brute-force enumeration as a clear first implementation
- moving closer to real Hold'em hand evaluation

## Milestone 5.10: Confidence Pass

This step adds a focused set of automated checks around the evaluator.

- Category coverage
  Tests all main 5-card hand categories.
- Tie-break checks
  Verifies important comparisons such as better pairs and better straights.
- 7-card checks
  Confirms the 7-card evaluator chooses the best 5-card combination.

This step is useful because it teaches:

- turning manual examples into repeatable tests
- checking both classification and comparison behavior
- building confidence before moving on to simulation

## Next Steps

1. Add a first simulation loop, even if it is naive.
2. Measure it, then improve memory layout and performance on purpose.
