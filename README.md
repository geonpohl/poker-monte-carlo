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
- One tiny simulator class with a placeholder method

## Suggested next steps

1. Add a `Card` type.
2. Add a `Deck` type.
3. Parse a couple of hole cards from the command line.
4. Add a first simulation loop, even if it is naive.
5. Measure it, then improve memory layout and performance on purpose.
