# Optimization Notes

This file records the performance work from Milestone 8 in one place:

- what we changed
- why we changed it
- what effect it had
- what we learned from it

The goal is to keep the optimization history understandable instead of leaving it scattered across commits and chat context.

## Benchmark Setup

Unless noted otherwise, the numbers below refer to the same benchmark scenario:

- executable: `poker_perf`
- build style: release build
- command: `make perf PERF_ARGS=200000`
- scenario: `As Ah` vs `Ks Kh` preflop
- result metric: iterations per second and nanoseconds per iteration

Important note:

- These numbers are approximate and can move around a bit from run to run.
- The useful signal is the size and direction of the change, not a single exact number.

## Baseline

Before the Milestone 8 optimizations, the simulator was running at about:

- `487,541 iterations/sec`
- `2051 ns/iteration`

At that point, the major costs were:

- repeatedly evaluating 7-card hands
- repeated work inside the 5-card evaluator
- repeated setup work inside the simulation loop

## Optimization 1: Precomputed 7-Card Combination Table

### What changed

We replaced repeated nested-loop combination generation with a fixed compile-time table of the 21 ways to choose 5 cards from 7.

This happened in the older version of [src/hand.cpp](/Users/geonpohl/Projects/poker-monte-carlo/src/hand.cpp), before the direct 7-card evaluator replaced that whole path.

### Why we did it

The old 7-card evaluator had to check all 21 possible 5-card subsets. Even before changing the algorithm, it made sense to remove repeated loop-setup work that never changes.

The idea was:

- store the 21 combinations once
- reuse one fixed candidate buffer
- avoid rebuilding the same loop structure every call

### Result

Approximate improvement:

- from `487,541 iterations/sec`
- to `492,199 iterations/sec`

That is only a small gain, but it was low-risk and measurable.

### Lesson

This was a valid cleanup, but it was not where the big cost lived. It reduced overhead around the evaluator, not the core evaluator cost itself.

## Optimization 2: One-Pass 5-Card Rank-Shape Collection

### What changed

We reworked the 5-card evaluator so that one descending scan over rank counts collected the shape information needed later:

- quads
- trips
- pairs
- kickers
- ordered ranks present

This logic lives in [evaluate_5_card_strength() in src/hand.cpp](/Users/geonpohl/Projects/poker-monte-carlo/src/hand.cpp:301).

### Why we did it

The earlier 5-card evaluator was correct, but it did more repeated interpretation work than necessary. Since every simulation iteration eventually depends on many hand evaluations, reducing internal evaluator churn was a good next step.

The new structure made the evaluator:

- easier to branch through once
- less repetitive
- more friendly to the optimizer

### Result

Approximate improvement:

- from `492,199 iterations/sec`
- to `565,711 iterations/sec`

This was the first clearly meaningful Milestone 8 win.

### Lesson

We still had the same overall algorithm, but we reduced repeated internal work in a hot function. That mattered much more than tiny structural cleanups.

## Optimization Attempt: Smaller Count Types

### What changed

We briefly tried shrinking some counting arrays to `std::uint8_t`.

### Why we tried it

The thought was simple:

- counts are small
- smaller data might improve memory behavior
- smaller arrays sometimes help cache locality

### Result

It regressed slightly, so we reverted it.

### Lesson

Smaller types are not automatically faster. They can introduce extra conversions or less efficient generated code. This was a good reminder to measure, not guess.

## Optimization 3: Simulation Loop Cleanup

### What changed

We reduced per-iteration setup work in the simulation loop inside [src/simulator.cpp](/Users/geonpohl/Projects/poker-monte-carlo/src/simulator.cpp).

The main changes were:

- prepare the remaining-card pool once
- avoid copying a whole `Deck` object every iteration
- reuse `hero_cards` and `villain_cards` buffers
- only fill in the unknown board slots each iteration

### Why we did it

Even with a better evaluator, the simulator still runs its outer loop many times. Any unnecessary copying or rebuilding in that loop becomes expensive when multiplied by hundreds of thousands or millions of iterations.

### Result

Approximate improvement:

- from `565,711 iterations/sec`
- to `580,452 iterations/sec`

This was a modest but real gain.

### Lesson

This was a classic “hot-loop hygiene” win. It helped, but the dominant cost still remained the evaluator path.

## Optimization 4: Packed Hand Strength

### What changed

We introduced `HandStrength`, a packed integer score for hands, in [include/poker/hand.hpp](/Users/geonpohl/Projects/poker-monte-carlo/include/poker/hand.hpp) and [src/hand.cpp](/Users/geonpohl/Projects/poker-monte-carlo/src/hand.cpp).

That let the simulator compare hands with integer comparisons instead of rebuilding richer comparison logic on every hot-path call.

Related pieces:

- `pack_strength()`
- `decode_strength()`
- `evaluate_5_card_strength()`
- `evaluate_7_card_strength()`

### Why we did it

The simulator mostly cares about one question:

- is hero stronger, weaker, or tied?

For that use case, a packed score is ideal. It preserves ordering while making the hot-path comparison much cheaper.

We kept `EvaluatedHand` around for:

- CLI output
- debugging
- tests

So the code stayed teachable while the hot path got faster.

### Result

Approximate improvement:

- from `580k iterations/sec`
- to about `660k-670k iterations/sec`

### Lesson

This was a good example of separating:

- readable API surface
- fast internal representation

That split made the simulator faster without throwing away the beginner-friendly interface entirely.

## Optimization 5: Direct 7-Card Evaluator

### What changed

This was the big one.

We replaced the old “best of 21 five-card hands” 7-card evaluation approach with a direct evaluator in [evaluate_7_card_strength() in src/hand.cpp](/Users/geonpohl/Projects/poker-monte-carlo/src/hand.cpp:427).

The new path:

1. summarizes the 7 cards once
2. builds rank counts, suit counts, and bit masks
3. checks categories directly in poker-strength order

Supporting helpers:

- `SevenCardSummary`
- `summarize_seven_cards()`
- `highest_straight_high_rank_from_mask()`
- `collect_top_rank_values_from_mask()`
- `highest_other_rank_index()`
- `collect_highest_other_rank_indices()`

### Why we did it

The old algorithm was correct, but expensive:

- 21 different 5-card subsets per 7-card hand
- each subset rebuilt counts
- each subset ran full 5-card evaluation
- the simulator does this twice per Monte Carlo iteration

So one simulation iteration effectively did a large pile of repeated evaluator work.

The direct evaluator avoids that by recognizing the best 7-card result from one summary pass.

### Result

Approximate improvement:

- from about `660k iterations/sec`
- to about `5.5M iterations/sec` on the `200000` iteration benchmark

A longer `1000000` iteration benchmark run landed around:

- `6.8M iterations/sec`

This was a major algorithmic improvement, not a micro-optimization.

### Why the gain was so large

Because we removed the dominant repeated work.

Old model:

- build and evaluate 21 candidate 5-card hands

New model:

- summarize 7 cards once
- choose the best category directly

That changes the cost structure of the hottest path in the entire simulator.

### How we validated it

Since this optimization was much more complex than the earlier ones, we added stronger correctness checks in [tests/hand_tests.cpp](/Users/geonpohl/Projects/poker-monte-carlo/tests/hand_tests.cpp):

- specific known 7-card hands
- tricky cases like double-trip full houses
- flush-heavy hands
- 5000 deterministic random 7-card samples

Each of those checks compares the new direct evaluator against a slower reference implementation that still does the old “best of 21 five-card hands” approach.

That gave us a safety net strong enough to keep the optimization with confidence.

## Overall Performance Story

Approximate progression:

- baseline: `487k iterations/sec`
- precomputed combination table: `492k iterations/sec`
- one-pass 5-card evaluator cleanup: `566k iterations/sec`
- simulation-loop cleanup: `580k iterations/sec`
- packed hand strength: `660k-670k iterations/sec`
- direct 7-card evaluator: `5.5M+ iterations/sec`

The biggest lesson is clear:

- micro-optimizations helped a little
- internal hot-function cleanup helped more
- the truly large win came from changing the algorithm

## What We Rejected

Not every optimization was kept.

Rejected or reverted ideas:

- shrinking some count arrays to `std::uint8_t`
  It regressed slightly.
- keeping all performance notes inline in `README.md`
  It made the main project docs too noisy.

This matters because good optimization work is not just about finding wins. It is also about measuring honestly and reverting ideas that do not pay for their complexity.

## What This Means For Future Work

Milestone 8 changed the shape of the project:

- the simulator now has a much faster evaluator core
- the hot path uses compact score comparison
- correctness checks are stronger than before

That gives us a better foundation for future features like:

- partial-board simulation improvements
- range-vs-range support
- multithreading
- more advanced benchmarking

## Commands Used During Milestone 8

Useful commands for revisiting this work:

```bash
make perf PERF_ARGS=200000
make perf PERF_ARGS=1000000
make test
ctest --test-dir build --output-on-failure
```
