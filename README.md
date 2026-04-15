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
./build/poker_cli
```

## What exists right now

- A minimal CMake build
- One library target: `poker_core`
- One executable target: `poker_cli`
- A `Card` model with `Suit`, `Rank`, and `Card`
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

## Next Steps

1. Add a `Deck` type.
2. Parse a couple of hole cards from the command line.
3. Add a first simulation loop, even if it is naive.
4. Measure it, then improve memory layout and performance on purpose.
