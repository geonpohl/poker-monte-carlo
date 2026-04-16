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

## Milestone 5 Breakdown

1. Hand categories
Define the poker hand categories we need to rank: high card, pair, two pair, three of a kind, straight, flush, full house, four of a kind, straight flush.

2. Evaluation result type
Create a small result type that stores both the hand category and the tie-break information needed to compare two hands.

3. 5-card evaluator input shape
Decide how a 5-card hand will be passed into the evaluator and add the function signature.

4. Rank and suit counting
Count ranks and suits in a 5-card hand so we can detect pairs, trips, quads, and flushes.

5. Straight detection
Add straight detection, including the wheel straight: A-2-3-4-5.

6. 5-card category detection
Use the counts plus straight/flush checks to classify the hand correctly.

7. 5-card comparison
Implement comparison logic so two evaluated 5-card hands can be ordered correctly.

8. CLI/demo integration
Add a small way to exercise the evaluator from the CLI so we can see real hands being evaluated.

9. 7-card evaluation
Extend to 7-card hands by checking all 5-card combinations and choosing the best result.

10. Confidence pass
Add a focused set of examples to verify each hand category and a few important tie-break cases.
