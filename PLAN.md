# Project Plan

## Big Picture

Build a poker equity simulator in small steps, starting with clear C++ fundamentals and only introducing performance-focused complexity after we have a correct baseline.

## Milestones

1. Project basics
Create the smallest useful structure, keep the build working, and make sure we understand how files, headers, and targets fit together.

2. Card model
Implement `Suit`, `Rank`, and `Card`, and learn how to model compact value types in C++.

3. Deck model
Implement a 52-card `Deck` with reset and draw behavior, focusing on arrays, ownership, and simple memory-safe design.

4. Input parsing
Teach the CLI to read cards and simulation settings from the command line so the program becomes interactive and testable.

5. Hand evaluation
Build a first correct hand evaluator for 5-card and then 7-card poker hands, even if it is not optimized yet.

6. First Monte Carlo loop
Simulate random runouts for a basic heads-up scenario and return win/loss/tie counts.

7. Correctness pass
Add small tests and edge-case checks so we trust the simulator before optimizing it.

8. Performance pass
Measure the slow parts, reduce unnecessary work or allocations, and improve data layout intentionally.

9. Range support
Expand from exact hole cards to hand ranges so the tool becomes more useful for real poker study.

10. Polish
Improve CLI usability, output formatting, documentation, and repository organization as the project grows.

## Working Style

- One milestone at a time
- Keep each step compileable
- Prefer correct first, then fast
- Add complexity only when we understand why it helps
