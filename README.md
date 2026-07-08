# Project Emergence — UE5.8 C++ (game scaffold)

Grounded survival-extraction game. Design/tech source-of-truth: `../Docs/PROJECT_EMERGENCE_SPEC.md`.
Decisions: `../DECIDED_QUESTIONS.md`. Built by the autonomous nightly loop (project pack `emergence`).

## Layout (spec 11.4)
- `Source/ProjectEmergence/` — primary runtime game module.
- `Source/EmergeCore/` — shared types/subsystems/interfaces (Public/Private split; other modules reach in only via EmergeCore).
- `Source/*.Target.cs` — Game / Editor / **Server** (dedicated Linux server; needs a **source UE5.8 build**).
- `Source/EmergeCore/Private/Tests/` — C++ **Automation Specs** = the evaluator's non-fabricable acceptance gates.
- `CI/build.sh`, `CI/test.sh` — headless build + named-test wrappers the loop invokes on the Windows UE host.

## Build/test (requires UE5.8 installed; server target requires a GitHub source build)
```
UE_ROOT=/path/to/UE_5.8 ./CI/build.sh          # compile gate
UE_ROOT=/path/to/UE_5.8 ./CI/test.sh Emergence. # acceptance gate (named specs)
```

## Phase order (spec 12 — do not reorder). Phase 0 = core-feel slice; each phase ends playable + passes exit criteria.
