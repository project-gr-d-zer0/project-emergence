# Project Emergence — The Building Pipeline (as-built, measured 2026-07-08)

Three parallel lanes, one verification oracle, one handoff tool. Every stage below is measured, not aspirational.

## The lanes
| Lane | Who | Work | Throughput |
|---|---|---|---|
| Kernel lane | emerge-builder loop (rick orchestrates, Opus/Sonnet implement, l3e7 verifies) | Single-class deterministic C++ from HUMAN-AUTHORED golden specs | ~45 s/feature, $0.10-0.50, **26/26 landed (100%)** |
| Structural lane | Claude Code by hand (batch scripts over SSH) | Multi-file C++: components, framework, plugin integration, refactors | minutes-hours, validated same oracle |
| Content lane | Claude Code via editor MCP bridge + z | Levels, Blueprints, assets, tuning, PIE testing | interactive |

## The organic circuit (proven end-to-end)
1. Editor/design work reveals a need ("falling should hurt")
2. Claude authors a trusted golden-value spec (~2-3 min) — **spec quality is the dominant lever**:
   trusted specs land 26/26; loop-self-generated specs landed 0/23. The spec contains the full
   CONTRACT (class, files, signatures, exact formulas in comments) so no planning step is needed.
3. `emerge-queue <id> "<name>" <prio> <filter> <hdr> <cpp> < spec.cpp` — registers + restarts loop (~30 s)
4. Loop: Sonnet implements -> l3e7 builds + runs FULL suite -> drift gate -> push on WIN
5. Claude pulls + Live Codes the result into the running editor (function-body changes only;
   new classes/UPROPERTYs need an editor restart)

## Verification integrity (hardened 2026-07-08)
- validate.ps1 (l3e7): hard-mirrors origin/master, kills stale test runners, LOUD fetch,
  reports the validated HEAD sha + fetch_ok in its JSON verdict
- emerge_validate.py (rick): rejects any verdict whose head != local HEAD ("STALE:" note) —
  silent-staleness is impossible
- drift gate logs its actual complaints; every non-winning candidate saved to graveyard/<id>/attemptN.txt
- Known trap fixed: git-LFS — laptop+rick carry real binaries, l3e7 runs skip-smudge (CI never opens content)

## Monitoring
- emerge-monitor.timer (15 min): service alive, stuck detection (pending>0 + no log 45 min),
  spend vs cap -> LOOP_HEALTH.json + ntfy alert (rate-limited 1/h)
- Daily spend cap in the service unit (EMERGE_DAILY_USD_CAP); emerge-cap-revert.timer resets nightly

## Git topology
- GitHub master = source of truth. rick pushes loop wins; laptop pushes via `github` remote (HEAD:master).
- l3e7 fetches rick over crosslink; laptop fetches rick (origin) or github.
- RULE: stop the loop before any hand-push from rick/laptop; restart after (loop re-baselines on start).

## Division-of-labor rules (learned, enforced by results)
- Loop gets: one class (new Emerge-prefixed OR add-method to existing), exact golden values, boundaries.
- Hand lane gets: anything touching >1 class, Build.cs, configs, plugins, engine APIs.
- Editor lane gets: anything a designer would click.
- Idle beats redundant: the auto-feeder that invented its own features is disabled on purpose.

## Ops quick reference (rick)
- queue work:   ~/bin/emerge-queue ... < spec.cpp
- health:       cat ~/work/projects/emergence/LOOP_HEALTH.json
- logs:         journalctl --user -u emerge-builder.service -n 30
- post-mortem:  ls ~/work/projects/emergence/graveyard/<feature-id>/
- validate now: python3 -c "import emerge_validate,json; print(emerge_validate.score(tests='Emergence.'))"
- pause/resume: systemctl --user stop|start emerge-builder.service   # CRITICAL: bracket pkill pattern if ever manual: pkill -9 -f "[e]merge_builder"
