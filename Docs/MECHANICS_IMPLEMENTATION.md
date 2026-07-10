# MECHANICS_IMPLEMENTATION.md — verified implementation playbook

Deep-research synthesis (2026-07-09) covering every remaining Project Emergence mechanic:
four research clusters, ~25 sub-agents, claims adversarially verified, ALS API claims read
directly from the Sixze/ALS-Refactored source by two independent agents. Use this as the
recipe book when implementing; PIPELINE.md covers *how* we build, this covers *what to build
and the exact APIs*.

Confidence markers: **[C]** confirmed (multi-source/official/source-read), **[L]** likely,
**[I]** inference. Unmarked statements in recipes are [C].

Canonical ALS = github.com/Sixze/ALS-Refactored (vendored in-repo). NOT the same API as
ALS-Community: overlay tag is `Rifle` not `M4`; there is no public `SetRotationMode`
(Desired-prefixed API only); no "UpperBody" montage slot exists.

---

## Cluster 1 — Firearms + bodily damage

### 1.1 Shooting architecture
- **Hybrid recommended**: ship hitscan first (Lyra pattern: ray then sphere-sweep fallback of
  `BulletTraceSweepRadius`), swap in a manager-owned struct-bullet ballistics sim later —
  a `UWorldSubsystem` owning `TArray<FBullet>{Pos,Vel,Age,AmmoDef}`, per tick integrate with
  EmergeBallistics drop/drag then one `SweepSingleByChannel(MakeSphere(0.5–2cm))` per segment.
  Insurgency-style: pre-simulate the first ~0.1s instantly so indoor fights feel hitscan.
  Tracers: ONE Niagara system fed via `NiagaraDataInterfaceArray`, never per-bullet components.
- **Folklore correction**: `UProjectileMovementComponent` does NOT tunnel on static geometry —
  it always sweeps (`bSweepCollision` default on). Substepping (`MaxSimulationTimeStep`≈0.05,
  `MaxSimulationIterations`≈8) is for curved-path accuracy, not missed hits. Real tunneling:
  SimulatePhysics without CCD, teleports, and moving targets (needs lag-comp, nothing stock).
  PMC is fine for grenades; wasteful (actor/bullet) at rifle volumes.
- **Fire loop**: one bool `IA_Fire`, NO input triggers — `ETriggerEvent::Started`→StartFire,
  `Completed`→StopFire (the `Hold` trigger is for hold-to-interact, not full-auto). Cadence in
  the weapon: accumulator in Tick — `while (TimeSince >= 60/RPM) { TimeSince -= ...; Fire(); }`
  (carries remainder; beats timers at high RPM). Burst = counter latched into same scheduler.
- **Spread**: copy Lyra's heat model wholesale (`ULyraRangedWeaponInstance`):
  `HeatToSpreadCurve` / `HeatToHeatPerShotCurve` / `HeatToCoolDownPerSecondCurve`,
  `SpreadExponent`, multipliers `_Aiming/_StandingStill/_Crouching/_JumpingOrFalling`,
  `bAllowFirstShotAccuracy`. Direction: NOT plain `VRandCone` (uniform = feels wrong);
  center-weight with `Angle *= FMath::Pow(FRand(), SpreadExponent)`.
- **Folklore correction**: **Lyra has no recoil-pattern system.** Build it: per shot push into
  a `RecoilTarget` accumulator → feed fractions through `AddControllerPitchInput/YawInput` via
  `FInterpTo` per frame (never one-frame snaps) → after no-fire delay, recover toward baseline
  minus the player's own counter-pull (cancel recovery if they compensated). Split camera
  recoil (small, authoritative) from pose recoil (violent additive montage in the anim layer).
- **ADS on ALS** (source-verified): `SetDesiredRotationMode(AlsRotationModeTags::Aiming)` +
  `SetDesiredAiming(true)`; zoom via `UAlsCameraComponent::SetFieldOfViewOverride(F)` +
  `SetFieldOfViewOverriden(true)`. Aim offsets are AUTOMATIC — `UAlsAnimationInstance`
  procedurally rotates the spine toward view (`RefreshSpine`, gated by overridable
  `IsSpineRotationAllowed()`). No aim-offset blendspace to wire.
- Sockets: "GripPoint" is FPS-template folklore, not ALS. Add plain sockets on `hand_r` (held)
  and `spine_03`/`pelvis` (holster) + virtual bones for left-hand IK. Fire the RAY from the
  camera (`CameraTowardsFocus`), cosmetics from `Muzzle` socket, and pre-trace capsule→muzzle
  to kill third-person shoot-through-cover.

### 1.2 Reload / ammo / weapon switching
- **Magazine model (Tarkov)**: mags are first-class EmergeInventoryComponent item instances
  `{AmmoType, Count}`; weapon holds a mag *reference* + its own `bChambered` (this makes N+1
  and mag-out-one-shot fall out naturally). Reload = inventory TRANSACTION (find mags by
  caliber sorted by count; old mag to grid or dropped; new attached) — never counter math.
  Two montage paths via sections + `Montage_JumpToSection`: `Reload_Tactical` (keeps chamber)
  vs `Reload_Empty` (rack, longer), branched on `bChambered`.
- Steal from Lyra the CONTAINER not the ammo design: `FGameplayTagStackContainer`
  (GameplayTagStack.h) — GAS-free, FFastArray-replicated tag→int32 counters, ideal for round
  counts/durability. Also portable: EquipmentDefinition/EquipmentInstance/EquipmentManager split.
- **THE reload gotcha on ALS-Refactored**: no "UpperBody" slot (`AlsConstants.h` slots:
  `Transition`, `TurnInPlaceStanding`, `TurnInPlaceCrouching` only). Intended mechanism =
  Layering slots in `AB_Als_Layering` blended by `Layer*Slot` curves; a plain montage does NOT
  work there — the source sequence must carry the layering curves, baked with the shipped
  `UAlsAnimationModifier_CreateLayeringCurves` (modifiers can't apply to montages). **Pragmatic
  ship path (maintainer-endorsed, Discussion #439)**: add our own `Layered Blend Per Bone`
  (root `spine_03`) after ALS's locomotion output feeding a custom slot — plain upper-body
  reload montages then just work while moving. Use this. [C]
- Caveat: montage notifies fire even when the montage isn't visually contributing — "notify
  fired, no anim" means slot/curve setup, not the notify.
- **Weapon switch**: holster montage → mid-montage AnimNotify reattaches to holster socket →
  `SetOverlayMode(Default)` → equip montage → notify attaches to hand → `SetOverlayMode(New)`.
  Switch the overlay tag AT the notify, not at input time.

### 1.3 Hit application → limbs
- Traces vs skeletal meshes resolve against **Physics Asset bodies**, never render tris.
  Folklore corrections: `bTraceComplex` does nothing on skeletal meshes; `Hit.PhysMaterial`
  is null unless `FCollisionQueryParams::bReturnPhysicalMaterial = true` (top cause of
  "headshots don't register"); the phys mat returned is the physics-asset body's; projectile
  overlap SweepResults give flaky BoneName — on impact re-trace a short line along velocity.
- Collision: custom `Weapon` trace channel; capsule **Ignores** it, mesh **Blocks** it
  (or the capsule eats every shot); disable query on twist/ik bodies in the PhysicsAsset.
- **Bone→limb**: ~15-entry `TMap<FName, ELimb>` of anchor bones (`head/neck_01`→Head,
  `thigh_l`→LegL, `clavicle_l/upperarm_l`→ArmL, spine splits…), resolve ANY hit bone by
  walking parents (`USkinnedMeshComponent::GetParentBone`) — twist/finger bones resolve free.
- **Ordering (Tarkov)**: resolve limb → armor layer (coverage on EmergeArmorWear, pen roll vs
  durability, degrade, blunt leak) → limb/headshot multiplier (owned by weapon/ammo def, Lyra
  phys-mat-tags style) → EmergeLimbHealthComponent + vitals rule (head/thorax 0 = death;
  limb 0 = crippled).
- Plumbing: skip legacy `FDamageEvent`/`UDamageType`; custom
  `FWeaponDamageParams{BaseDamage,Pen,Multipliers,FHitResult,Instigator}` into
  `EmergeDamageComponent::ProcessHit`. Thin `TakeDamage` wrapper only for AIPerception damage sense.

### 1.4 Locomotion consequences (leg/arm damage) — headline mechanic
- **Leg damage → speed, source-verified**: `UAlsCharacterMovementComponent` keeps a VALUE-COPY
  cache (`FAlsMovementGaitSettings GaitSettings`), refreshed only by private
  `RefreshGaitSettings()` via `SetMovementSettings()` / `SetStance()` / `SetRotationMode()` /
  `MoveAutonomous()`. **Mutating the data asset in place does NOTHING.** Recipe: at BeginPlay,
  `DuplicateObject<UAlsMovementSettings>(Default, this)` per character (we already do this on
  the zombie — same pattern) + `SetMovementSettings(RuntimeCopy)`. On leg hit: scale
  Walk/Run/Sprint fields from cached BASE values (idempotent multipliers), then **re-call
  `SetMovementSettings(RuntimeCopy)`** — the only public cache-refresh path. Immediate effect.
- Tiers (Tarkov): bruised = multiplier; fractured = clamp gait to Walking via desired-gait API
  (no sprint + limp); blacked = walk-only. MP note: replicate the injury factor and apply both
  sides or CMC prediction rubber-bands.
- **Arm damage → sway**: two layers scaled by `ArmInjury01` — true-aim noise (per-tick
  controller offsets from `FMath::PerlinNoise1D`, coherent unlike RandRange) + larger cosmetic
  anim sway (Spring Controller / Transform-Bone on the hand IK bone in our linked layer). [L]
- **Hit reacts, escalation ladder** (EmergeStaggerComponent): light = additive directional
  montage (4-way dot product); medium = root-motion stumble + custom `LocomotionAction` tag
  (ALS's own pattern for rolling/mantling) to suppress other actions; heavy = knockdown.
  Partial-body physics layer: `UPhysicalAnimationComponent` →
  `ApplyPhysicalAnimationSettingsBelow(Bone, D)` (OrientationStrength≈1000 etc.) +
  `SetAllBodiesBelowSimulatePhysics/BlendWeight(0.3–1.0)` + `AddImpulseToAllBodiesBelow`,
  ramp weight to 0 over 0.2–0.5s. **Critical ALS rule (source-verified)**: ALS ragdolling
  assumes exclusive ownership of mesh physics (`StartRagdollingImplementation` resets all
  bodies) — force-cancel any partial hit-react the instant ragdolling starts; re-apply the
  profile after `StopRagdolling`; gate on `LocomotionAction != Ragdolling`. Consider
  Vaei/ProcHitReact (MIT) before hand-rolling.
- **Knockdown→ragdoll→get-up** (source-verified, `AlsCharacter_Actions.cpp`):
  `StartRagdolling()` (requires physics bodies on `pelvis` AND `spine_03` — ENSUREs) →
  `Mesh->AddImpulse...` → timer/health → `StopRagdolling()` (pose snapshot, computes
  `bRagdollFacingUpward`, plays `SelectGetUpMontage_Implementation(bool)` — override for
  injured variants). Remote sync built in; AI works out of the box (server-local controllers).
  Bonus: `Settings->Ragdolling.bStartRagdollingOnLand` already exists.

### 1.5 Dismemberment lite (stretch)
1. `Mesh->HideBoneByName(Bone, PBO_Term)` (irreversible physics below; remaining ragdoll OK)
2. spawn pre-authored STATIC limb mesh at `GetSocketTransform(Bone)`, SimulatePhysics +
   `AddImpulseAtLocation`, ~10s lifespan
3. gore-cap static mesh attached to parent-bone socket
4. optional non-fatal dangle: `BreakConstraint` (never visually severs — dangle only)

Severable: head, lowerarm_l/r, calf_l/r. 100%-weight severable verts to their bone or
candy-wrapper seams. Bone hiding is post-anim-eval → orthogonal to ALS. Skip runtime slicing.
Reference: Lim-Young/SkeletalMeshDestruction.

### Cluster-1 build order
trace→limb pipeline → fire loop + hitscan → leg-damage speed + gait clamp → stagger ladder →
ADS/overlay wiring (parallel OK) → magazine inventory + reloads (biggest content lift) →
ballistics manager swap-in → dismemberment last.

---

## Cluster 2 — Survival systems wiring + UI

### 2.1 Survival simulation ticking
- **Central `UEmergeSurvivalSubsystem : UTickableWorldSubsystem`** (Sea of Thieves
  tick-aggregation pattern). Vitals/Infection/Temperature/Medical components register in
  BeginPlay, unregister in EndPlay, `PrimaryComponentTick.bCanEverTick = false`.
- Folklore: "component ticks are catastrophic" is overstated at our scale — centralize for
  deterministic order, one pause/dilation point, per-effect rate tuning, one save point.
- **Rates (DayZ decompiled ground truth)**: per-effect accumulator intervals — bleed 0.5–1s,
  temperature/infection 3–10s, slower when dormant; always pass real delta so math is
  rate-independent. Accumulator-in-Tick beats `FTimerManager` loops (timers burst-fire after
  hitches, give no delta, ignore per-actor CustomTimeDilation).
- **Thresholds**: per-vital `ECurrentSeverity`; broadcast ONLY on transition `(Old, New)`;
  asymmetric hysteresis (enter Critical ≤20, exit ≥25) in an `FVitalThreshold` struct —
  kills flapping when regen fights bleed.
- Save: `UPROPERTY(SaveGame)` + `FMemoryWriter`/`FObjectAndNameAsStringProxyArchive`
  (`ArIsSaveGame=true`). Components are NOT in `Actor->Serialize` — serialize each into its
  own byte array keyed by name. Absolute sim-clock timestamps (own persistent double), plain
  int version header.

### 2.2 Grid inventory UI (Tarkov-style)
- **One root `UCanvasPanel`, one widget per ITEM (never per cell)**, grid lines + drop
  preview in `NativeOnPaint`. `UUniformGridPanel` can't do multi-cell items.
- Drag-drop (fully C++, folklore says BP-only — false):
  1. `NativeOnMouseButtonDown` → `DetectDragIfPressed(...).NativeReply` (the gotcha).
     Use Border/Image in slots, not UButton (eats events).
  2. `NativeOnDragDetected` → `NewObject<UEmergeItemDragOp>()` with TYPED fields (item
     handle, source container, grabbed-cell offset, bRotated) — not the generic Payload.
     `DefaultDragVisual` + `Pivot = EDragPivot::MouseDown`.
  3. `NativeOnDragOver` = footprint fit-check + highlight; `NativeOnDrop` = commit;
     `NativeOnDragCancelled` = return or drop-to-world.
- **Rotation gotcha (R while dragging)**: keyboard does NOT route to widgets during a Slate
  drag. Register an `IInputProcessor` via `FSlateApplication::RegisterInputPreProcessor` for
  the drag lifetime; flip `bRotated`, `SetRenderTransformAngle(90)` on the visual, swap
  footprint `FIntPoint(Y,X)`, re-fit.
- Perf: the real cost is construction spike on open (build once, reuse, event-driven updates)
  and per-frame bindings, not paint. Widgets via `WidgetTree->ConstructWidget<T>()`.
- Nested containers: container items as UObject-based instance children; on drop, walk the
  target's ownership chain upward and reject if the dragged container appears (no bag-in-itself).
- Controller later: CommonUI `FCommonAnalogCursor` drives mouse drag-drop verbatim — build
  mouse-first, keep the screen ActivatableWidget-friendly.
- Thin WBP: `UCLASS(Abstract)` C++ base + `meta=(BindWidget)` (compile-enforced),
  `BindWidgetAnim` + Transient; widgets valid from `NativeOnInitialized`.
- **Lyra has NO spatial grid UI and broken stacking** — mine only its item data layer
  (fragments + stat tags). References: DevAddict spatial-inventory series +
  TDKArkham/SpatialInventory; VenomousXIA/TarkovInventory (no license — read-only).

### 2.3 World items + interaction
- Pickups: plain tickless `AActor` (bCanEverTick=false) + World Partition streaming is fine
  into the low thousands (folklore "every actor = draw call + tick" is false for tickless
  material-sharing actors; real costs = memory + spawn/GC). Pooled spawner subsystem for
  runtime drops. Don't use Mass for loot. WP gotcha: unload destroys actors — own a
  persistence layer re-applying looted/moved state on cell reload.
- **Interaction = Lyra's dual-trace**: throttled sphere `OverlapMultiByChannel` (~0.1s, own
  trace channel) for candidates + per-tick camera line trace for focus; skip tracing when the
  overlap set is empty. `IInteractableTarget`-style UINTERFACE implementable by actor OR
  component (Lyra's key trick).
- Highlight: Custom Depth "Enabled with Stencil"; `SetRenderCustomDepth(true)` +
  `SetCustomDepthStencilValue(N)` (N per rarity) + post-process outline. Hold-to-interact:
  Enhanced Input `UInputTriggerHold`. Per-item `UWidgetComponent` does NOT scale (render
  target each) — screen-space prompt, or one shared component moved to focus.
- Containers: SAME `EmergeInventoryComponent` on container actors as on the player — one code
  path. Lazy loot (table handle + seed, roll on first open) is a valid SP approximation
  (Tarkov/DayZ actually pre-roll — matters only if MP).
- **Equipping on ALS (source-verified)**: override BlueprintNativeEvent
  `OnOverlayModeChanged(FGameplayTag Previous)` to attach/detach meshes in sync with the pose.
  Overlay tags are TAGS not enums — new overlays (Melee, Flashlight) = register tag + linked
  anim layer, no plugin edits. Item definition carries `{overlay tag, attach socket, anim
  layer class}`; EquipmentComponent applies in OnOverlayModeChanged. Holster sockets: author
  our own on spine/pelvis (ALS provides none). Known pain: off-hand IK per weapon shape.

### 2.4 Using items (meds, consumables, hotbar)
- **Channeled use**: `UEmergeChanneledActionComponent`: `Montage_Play` →
  `Montage_SetBlendingOutDelegate` (earliest interrupt, bInterrupted=true) +
  `Montage_SetEndDelegate` (bInterrupted=false = completed) → **apply the effect in the
  completion path, NEVER in an anim notify** (notifies are silently skipped if interrupted
  before reaching them; NotifyEnd DOES fire on interrupt but can't distinguish — folklore
  corrected both ways). Notifies = cosmetics only. All cancel triggers (damage, movement)
  funnel through `Montage_Stop()` → one interrupted path drives gameplay + UI.
- GAS verdict: not worth retrofitting onto working bespoke components for item use. Keep bespoke.
- Consumables: charges on EmergeItemInstance as tag→int stats; med-kit = one HP pool int.
  Food/drink-over-time = EmergeStatusEffectComponent `{Tag,Magnitude,Period,TimeRemaining}`
  stepped by the survival subsystem.
- **Hotbar**: QuickBar on the PlayerController, slots store item handles/FGuids resolved
  through inventory each use (never raw pointers) + subscribe to item-removed delegate to
  null slots. Nine `IA_Quickslot1..9`, `BindAction(IA, Started, this, &X::OnSlot, Index)`.
- Progress UI: radial via `UImage` + MID, one scalar 0–1; show only after ~0.2s. One widget
  ticking is fine (folklore "widget tick is evil" doesn't apply to one widget).

### 2.5 HUD composition
- CommonUI `UPrimaryGameLayout` + tag-named `UCommonActivatableWidgetStack` layers
  (`UI.Layer.Game/GameMenu/Menu/Modal`); push via `PushWidgetToLayerStack<T>` — input
  auto-routes to topmost, falls back on deactivate. Since 5.5 PushWidget activates internally —
  don't double-activate.
- Updates: delegate-driven ONLY — no UMG property bindings (poll per frame), no HUD Tick;
  `UInvalidationBox` on static regions, `Collapsed` for hidden.
- Status icons: `UHorizontalBox` + small pre-created pool (show/collapse/re-sort on effect
  delegates); pulse via material Time-sine (zero game-thread cost). Steal DayZ trend arrows
  (show the derivative, not just level). Philosophy: chronic = tiny persistent glyphs;
  acute = transient icons (healthy HUD is quiet); full detail gated behind inventory screen.
- Damage direction: one centered `UImage` + radial-mask material; yaw =
  `(DamageDir.Rotation() - CameraRotation).Yaw`, re-evaluate while shown as the player turns;
  pair with a center hit-flash (edge-only rings get missed by ~half of players).

### Cluster-2 build order
survival subsystem wiring → HUD skeleton (proves event pipeline) → interaction + world items →
grid inventory UI (biggest chunk) → equipment on ALS → item use + hotbar → polish
(direction indicator, trend arrows, save pass).

---

## Cluster 3 — Extraction game loop, director, noise, persistence

### 3.1 Raid flow architecture
- `ARaidGameMode : AGameModeBase` (NOT AGameMode — MatchState is UT-legacy; Lyra abandoned it)
  = server-only adjudication (Extracted/KIA/MIA, settlement, spawn control).
  `ARaidGameState : AGameStateBase` = replicated shared state (`RaidEndServerTime`, phase tag,
  extract flags).
- **Phase machine = `URaidPhaseSubsystem : UWorldSubsystem`** with NESTED gameplay tags
  (`Raid.Insert → Raid.Active → Raid.Active.ExtractsOpen → Raid.Ending → Raid.Complete.X`);
  starting a phase ends all non-ancestor phases; observers get
  `WhenPhaseStartsOrIsActive/WhenPhaseEnds`. This is Lyra's `ULyraGamePhaseSubsystem` minus
  GAS — a minimal port is current-tag + ancestor rule + multicast delegates. World-scoped =
  fresh raid state free.
- `UGameInstanceSubsystem` (`UProfileSubsystem`) = cross-raid ONLY (stash, profile,
  settlement; EmergeEconomy/Reputation/Skill snapshots). Survives level travel.
- **`AExtractionPoint : AActor`**: BoxComponent overlap begin/end (authority only); gate
  `CanExtract(Pawn)` (item/cost/window); `SetTimer(Handle, this, &Complete, HoldSeconds)`;
  clear on end-overlap. HUD progress: store `ExtractStartServerTime` once, HUD computes
  locally per frame — never tick a delegate. Steal from Hunt: Showdown — extract timer
  PAUSES while an enemy is in the radius (extraction becomes a combat magnet).
- **Death = pure data settlement (Tarkov ruleset)**: (1) snapshot loadout to profile at raid
  START (also defeats Alt+F4), (2) per-item flags `bBroughtIn / bFoundInRaid(pending) /
  bInSecureContainer` (parent chain rooted at secure slot), (3) ONE settlement function at
  raid end: Survived → merge + promote FiR; Died/MIA → restore secure subtree + always-kept
  slots; rest → insurance queue. No raid-world persistence needed in SP.
- **Raid timer**: server sets `RaidEndServerTime = GetServerWorldTimeSeconds() + Duration`
  ONCE (replicated float on GameState); clients derive remaining per frame. Never replicate a
  decrementing int. Discrete events: OnRep + dynamic multicast delegates; the
  GameplayMessageRouter plugin (NOT stock engine — copy from Lyra) is the scale-up.

### 3.2 AI director / spawn pacing (Booth's actual AIIDE 2009 numbers)
- **Intensity** per survivor, director uses MAX not average: +damage taken, +incap, +ledge
  pull, +nearby infected death (inverse distance); decays over time but **decay freezes while
  infected actively engage**. Booth: it "adjusts pacing, not difficulty — amplitude is not
  changed, frequency is." → keep the EmergeWorldDifficulty scalar SEPARATE from pacing:
  difficulty scales amplitude (mob size, ticket pool), intensity only gates when the budget
  is spendable.
- **State machine**: Build Up (full population until intensity peaks) → Sustain Peak (3–5s) →
  Peak Fade (minimal population until intensity decays below peak) → Relax (30–45s or
  distance traveled). Published constants: mob interval 90–180s, mob size 20–30 ramping
  min→max after each mob. Bosses exempt by design.
- **"75% of mobs from behind" is REAL** (slide 70), scoped to mobs. Extraction-map adaptation
  (no linear flow): keep ~10s of position history → heading → weight spawn candidates by
  `dot(candidateDir, -heading)`; tune so ~75% land in the rear hemisphere emergently.
- Spawning: Active Area Set around the team; wanderer counts per area **zeroed the moment an
  area becomes visible**; despawn on AAS exit. ("Removed if stared at" is folklore distortion.)
- Population: global `MaxAliveZombies` + per-category ticket budgets (wanderer/mob/special)
  modulated by director state; death refunds tickets. Director on a 0.5–1s timer, never per-tick.
- **Sampling**: `UNavigationSystemV1::GetRandomReachablePointInRadius` (NOT
  GetRandomPointInNavigableRadius — returns disconnected islands/rooftops). It's
  pathfind-priced — batch. Always `ProjectPointToNavigation` with generous vertical extent;
  `SpawnActorDeferred` + `AdjustIfPossibleButAlwaysSpawn`; lift Z by capsule half-height.
  Hybrid: pick direction from bias distribution → annulus sample → project → visibility gate
  → retry N → authored ATargetPoint fallback.
- **No-spawn-in-view gate (cheapest first)**: distance band → frustum dot
  (`Dot(CamForward, ToCand) > Cos(HalfFOV+10°)` using `APlayerCameraManager`, not pawn eyes)
  → LOS trace to camera only for in-frustum candidates (blocked = behind wall = OK).
  Same gate for despawns. **`WasRecentlyRendered` is NOT a spawn-visibility test** (shadow
  passes count as rendered, lags occlusion, 5.5 LastRenderTime regression) — despawn hint only.
- **Influence-grid integration** (Dave Mark, Game AI Pro 2 ch.30): layer the existing grid —
  `Threat` (noise stamps: gunshot +1.0, explosion +2.0, sprint step +0.3, exp decay half-life
  10–20s), `PlayerPresence` (negative stamp of frustums + recently-seen cells = Booth's PVS
  discretized), `BehindBias` (+along -heading). Spawn score =
  `w1*Threat + w2*BehindBias − w3*Presence`, weighted roulette over projected annulus cells.
  Noise-attracts-spawns falls out of the Threat layer free.
- Days Gone GDC 2018 = the horde LOD-AI precedent: hordes 50–500, 8 character models with
  0.9–1.1 scaling, cluster-level group AI with individual promotion near the player — exactly
  our influence grid + Mass plan.

### 3.3 Noise / hearing (EmergeNoise wiring)
- **Engine formula** (`UAISense_Hearing::Update`, verified):
  heard iff `DistSq <= HearingRangeSq * Loudness²` AND
  `(MaxRange <= 0 || DistSq <= (MaxRange * Loudness)²)`. Pure Euclidean, binary, NO occlusion.
- Folklore corrections: `MaxRange` MULTIPLIES by Loudness (not an absolute cap); Loudness is
  unclamped above 1 (2 = double range — Epic-confirmed); `bUseLoSHearing`/`LoSHearingRange`
  never did anything (dead config); `AActor::MakeNoise` IS rerouted into AIPerception (the
  dead path is calling `UPawnNoiseEmitterComponent::MakeNoise` directly) — in C++ just call
  `UAISense_Hearing::ReportNoiseEvent`; hearing does NOT need a StimuliSourceComponent on the
  noise maker. Real "AI won't hear" culprits: **affiliation filtering** (player resolves
  Friendly without `IGenericTeamAgentInterface` → implement `GetGenericTeamId` or enable
  `bDetectFriendlies`), perception component not on the AIController, null Instigator.
- **Convention: Loudness=1 always; put EmergeNoise radii in MaxRange per event**; listener
  `HearingRange` high (~6000+) so the listener gate never surprises. Tiers [I]: crouch 250 /
  walk 700 / sprint 1800 (drive from ALS gait via footstep AnimNotify so cadence matches
  animation); suppressed shot ~4000; unsuppressed 15000+ or MaxRange=0.
- Expiry: `HearingConfig->SetMaxAge(t)` + Project Settings → AI System → **Forget Stale
  Actors** = true, or `OnTargetPerceptionForgotten` never fires. Workhorse delegate:
  `OnTargetPerceptionUpdated(AActor*, FAIStimulus)` (read Tag, Strength, StimulusLocation,
  WasSuccessfullySensed). Payload channel is the single FName Tag — encode tiers
  (`Footstep.Sprint`).
- **Occlusion is 100% DIY — sweet spot = line-trace attenuation (what TLOU shipped)**: rays
  listener→source, damp ~0.4 per blocking hit, re-test vs radius. (Correction: TLOU hearing
  was raycast occlusion, not nav-distance.) Upgrade path: navmesh path-distance blend
  (`0.5*Euclid + 0.5*PathLen`, doubles as the investigate path).
- **Route ALL noise through one wrapper**: `EmergeNoise::Report(Loc, Radius, Tag)` →
  (a) `ReportNoiseEvent` for per-zombie reflexes, (b) influence-grid stamp (Gaussian weighted
  by loudness, `V *= exp(-dt/τ)` τ≈10–30s, clear cells in confirmed zombie vision → emergent
  group search — Third Eye Crime occupancy-map pattern). At horde scale skip per-agent
  perception entirely; sample only the grid. (Epic: AIPerception is in maintenance mode —
  fine for elites, grid is the horde-scale source of truth.)

### 3.4 Persistence
- Folklore corrections: `UPROPERTY(SaveGame)` does NOTHING with `SaveGameToSlot` (it
  serializes ALL UPROPERTYs; the specifier only matters with `ArIsSaveGame=true` DIY
  archives). Async save only moves file I/O off-thread — reflection serialization still
  hitches the game thread (keep payloads lean, save at pauses). **No built-in atomicity**:
  DIY tmp-write→rename, CRC in payload, .bak rotation, check `Ar.IsError()` on load.
  Raw UObject* degrade to path strings → nullptr. Lyra's inventory has no disk persistence.
- **Three slots**: `Profile.sav` (version int, XP/skills/rep), `Stash.sav` (grid inventory),
  `World.sav` (loot respawn timestamps). Independent versioning, less corruption blast radius.
- Item records: definition = `UPrimaryDataAsset` saved as `FPrimaryAssetId`; instance = plain
  USTRUCT `{Def; Stack; Durability; GridX,GridY; bRotated; FGuid Id; TArray<FInstancedStruct>
  Extras}`. Rebuild runtime objects on load. Versioning: `int32 SaveVersion` + chained
  `MigrateVn→Vn+1` (tagged-property serialization already tolerates add/remove). Renaming the
  USaveGame class breaks old files without Core Redirects.
- **Timing**: commit at raid boundaries only — raid start (loadout deducted), extraction,
  death (**save the death BEFORE showing the death screen** — anti-scum), stash menu.
  `AsyncSaveGameToSlot` for mid-gameplay.
- Loot respawn: per-container `{FGuid; FDateTime LastLootedUtc}`; lazily compare on raid load
  and reroll — never tick timers. Everything else resets per raid (genre norm).

### 3.5 Session / level flow
- Folklore corrections: GameInstance + subsystems SURVIVE every travel type (OpenLevel does
  not reset them); OpenLevel is ALWAYS hard travel (`bUseSeamlessTravel` needs ServerTravel);
  GameMode/GameState are rebuilt even in seamless travel; "UMG widget then OpenLevel" loading
  screens don't work (widget dies with the PC, game thread blocks); `bShouldBlockOnLoad` is
  unreliable with World Partition.
- **SP verdict: `OpenLevel` + GameInstanceSubsystem carrier.** Co-op insulation: route every
  transition through one `URaidFlowSubsystem::StartRaid(FRaidConfig)` (today OpenLevel, later
  ServerTravel branch); keep carried data OUT of GameMode. Config split: map + insert tag +
  `?Game=` in the Options string (read in InitGame via `ParseOption` — identical with
  ServerTravel later); loadout/result structs in the subsystem.
- Loading screen: copy Lyra's **CommonLoadingScreen** plugin — `ULoadingScreenManager` hooks
  `PreLoadMapWithContext/PostLoadMapWithWorld` and `ILoadingProcessInterface::
  ShouldShowLoadingScreen()` holds the screen until streaming finishes.
- World Partition spawn trap: distant insert before cells stream = fall-through. Mitigations:
  `WorldPartitionStreamingSource` at the insert, poll `IsStreamingCompleted()` (buggy with
  Data Layers), plus the pragmatic gate: downward line trace at the insert each tick behind
  the loading screen; spawn when it hits. Wire as an ILoadingProcessInterface holder.
- Spawn selection: tag `APlayerStart`s (`Insert_NE`…), override `ChoosePlayerStart` to pick
  among matching tags; override `HandleStartingNewPlayer` to defer `RestartPlayer` until the
  streaming gate passes.

### Cluster-3 build order
raid skeleton (GameMode/State + phase subsystem + timer) → level flow (RaidFlowSubsystem +
loading screen + spawn gate) → persistence (3 slots + settlement) → extraction points →
noise wiring (mostly plumbing; radii math exists) → director last (needs 1, 2, 5 feeding it;
ship a dumb constant-budget spawner meanwhile).

---

## Cluster 4 — Mass Entity horde (Phase B, 200–500 agents)

### 4.0 Go/no-go
**Mass in 5.8 is a safe bet for this exact use case** (SP, 200–500 agents, custom steering):
Beta on Epic's roadmap, ships under LEGO Fortnite and 5.8 MetaHuman Crowds. NOT safe: Mass
replication (Epic: "build custom if you need it now"), ISM animation (officially
experimental), MassNavMeshNavigation (thin track record). At 200–500 agents we're 10–100×
below Mass's design scale — **animation/render strategy decides frame rate, not the ECS**.
Budget ~1–2 days of API fixups per engine upgrade. 5.8 gotcha: new **MassCore** module split
from MassGameplay — check Build.cs deps; also sparse/virtual fragments, off-game-thread
entity creation, Simplified Query API (`UE::Mass::FQueryExecutor`).

### 4.1 Fundamentals
- Fragments (`FMassFragment`), tags (`FMassTag`), shared fragments, traits
  (`UMassEntityTraitBase::BuildTemplate`), processors (`UMassProcessor`), config assets
  (`UMassEntityConfigAsset`). Modern signatures: `ConfigureQueries(const
  TSharedRef<FMassEntityManager>&)`, `Query.ForEachEntityChunk(Context, lambda)`.
- Folklore: "MassEntity deprecated in 5.5" is FALSE (SEO-blog noise).
- Spawning waves: `UMassSpawnerSubsystem::SpawnEntities(TemplateID, Num,
  FMassTransformsSpawnData, UMassSpawnLocationProcessor::StaticClass(), OutHandles)`.
  **#1 pitfall: any synchronous create/destroy during Mass processing asserts — ALWAYS route
  runtime spawns/mutations through `EntityManager.Defer()`.**
- Per-entity state: `FZombieAwarenessFragment{float Awareness; FVector TargetLocation;}`.
  Bulk access via `GetMutableFragmentView<T>()`; random access via
  `FMassEntityView(Manager, Handle).GetFragmentDataPtr<T>()` — value writes safe,
  composition changes deferred.
- Best refs: Megafunk/MassSample (tracks releases through 5.7), x157.github.io/UE5/Mass,
  vrealmatic.com/unreal-engine/mass.

### 4.2 Movement — decision: custom steering from the influence grid
- **ZoneGraph: NO.** Epic (verbatim): "maintenance mode for critical bug fixes with no plans
  for further development." Also design-wrong for pursuit (lane flow, goals are lane
  locations, not a moving player). CitySample crowds ARE ZoneGraph — that's why its MOVEMENT
  layer is not our template (its representation layer is).
- **MassNavMeshNavigation: partial use only** ("very experimental" per Epic; known StateTree
  drift bug). Worth taking: `UMassNavMeshNavigationBoundaryProcessor` feeds navmesh boundary
  edges into avoidance. **Confirmed: no nav-link traversal anywhere in Mass through 5.8 —
  Mass agents are surface-walkers** (matches our prior finding; scripted traversal is ours to
  build).
- **The recipe (proven precedent — stopthem/MassZombies: 5,000 zombies converging on the
  player at 25–30fps on a 3060 Ti)**: `UZombieSteeringProcessor` samples the
  `UEmergeInfluenceGrid` gradient per entity → writes `FMassForceFragment`; stock
  `UMassApplyMovementProcessor` integrates force→velocity→transform. Street funneling comes
  FREE from obstacle-masked diffusion in the grid (rasterize navmesh/blocking geo into it) —
  no ZoneGraph needed. Boids layering precedent: PiotrJezyna/MassFlock.
- **Avoidance**: stock `UMassObstacleAvoidanceTrait` → Moving/Standing avoidance processors —
  force-based (not ORCA), queries `FNavigationObstacleHashGrid2D`. Pushy/jittery in dense
  clumps — aesthetically CORRECT for zombies. Cost at 500: noise. Gotcha: disable the Off-LOD
  behavior that teleports far entities to their goal.
- **Traces from processors**: synchronous line traces from worker threads are FINE (Chaos
  scene queries are read-thread-safe; MassSample's projectile processor proves it — no
  `bRequiresGameThreadExecution`). For hundreds: `UWorld::AsyncLineTraceByChannel`
  fire-frame/consume-next + stagger 1/N entities per frame. Low walls: prefer avoidance
  edges; trace only the actual climb-affordance check on blocked entities → climb VAT clip or
  promote-to-traverse.

### 4.3 Representation / LOD
- Stock `UMassVisualizationTrait` per-LOD `EMassRepresentationType`: `HighResSpawnedActor` /
  `LowResSpawnedActor` / `StaticMeshInstance` / `None`. MassLOD: High/Med/Low/Off +
  `LODSignificance` 0–3, **`LODMaxCount` = the hard actor budget**,
  `BufferHysteresisOnDistancePercentage` (default 10%) vs oscillation, `bKeepActorExtraFrame`
  vs popping. Actors POOLED via `IMassActorPoolableInterface` — **fully reset state in
  PrepareForGame** (stale-health-from-previous-zombie bugs are documented).
- **VAT: AnimToTexture plugin ships in-engine since 5.1** (disabled by default, in-editor
  baking, no Houdini; API changed at 5.4 — pre-5.3 tutorials broken). Workflow: static mesh
  from skel mesh → `UAnimToTextureDataAsset` → bake bone-mode textures → material functions +
  PerInstanceCustomData. Variation: CitySample layout = 8 custom floats (anim start-time /
  state-index / play-rate + tint); `PerInstanceRandom` = free phase desync.
- **Watch: UE 5.6+ `UInstancedSkinnedMeshComponent` + Animation Banks** (GPU-instanced
  skinned crowds, no bake; 5.8 MetaHuman Crowds is built on it, works on non-MetaHuman
  meshes) — but Epic says the GPU path "isn't meant to be used yet in production" and anim
  blending missed 5.8. **Ship Phase B on VAT; timebox an ISKM/AnimBank spike as the
  migration target.**
- CitySample wiring to copy: `FCrowdAnimationFragment` (AnimToTextureData, GlobalStartTime,
  AnimationStateIndex, PlayRate), `UMassCrowdUpdateISMVertexAnimationProcessor`, anim state
  persisting ACROSS actor↔ISM swaps (their anti-pop trick).
- Hit reactions at ISM tier (proven blocks, unproven combo): (a) per-instance custom-data
  flinch — `RepresentationSubsystem->GetMutableInstancedStaticMeshInfos()` → element
  `FMassInstancedStaticMeshInfo::AddBatchedCustomData` AFTER the representation phase (raw
  `SetCustomDataValue` conflicts with batched writes); (b) swap anim-state float to a baked
  flinch clip; (c) promote to actor.
- **Promotion (entity → elite ALS)**: it's just representation LOD High —
  subclass `UMassCrowdRepresentationActorManagement`, override `OnPostActorSpawn` (copy
  transform/velocity/health fragments → ACharacter, possess with EmergeEnemyController), set
  `RepresentationActorManagementClass` on the trait; copy state back in `PrepareForPooling`.
  ONE movement authority while promoted (CMC drives; translator syncs transform back to
  fragments — MassActors ships capsule/CMC translators). Gameplay "graduate to elite" =
  force LOD High for that entity. [L — assembled recipe, no canonical tutorial]

### 4.4 Combat interop
- **Folklore correction: do NOT trace the ISM and map `FHitResult.Item`→entity.** Mass
  rebuilds instance batches every frame as LOD shifts — index↔entity is not stable, and the
  runtime ISMs typically have no collision. **Use hash-grid hit resolution**: trace world
  geometry for the ray, then segment-query a spatial hash for candidate entities
  (`FNavigationObstacleHashGrid2D` — we get it free with the avoidance trait — or our own),
  cheap capsule test, nearest wins. Near tier is free: promoted actors have real capsules.
  Cautionary: MassZombies found actor collision on EVERYTHING halved FPS.
- **Damage/death chain (canonical)**: weapon writes damage queue +
  `UMassSignalSubsystem::SignalEntities("Damaged", Hits)` → `UMassSignalProcessorBase`
  (processes only signaled entities) subtracts health fragment →
  `Context.Defer().AddTag<FDead>()` → `UMassObserverProcessor` on FDead-add spawns death
  presentation → `Defer().DestroyEntity()` or mutate to corpse archetype. Signals carry NO
  payload (name + handles; damage lives in fragments/queues). Signals = repeatable events;
  observers = one-shot composition changes.
- Death at scale: default = VAT death clips (swap anim-index float, leave corpse/fade). Real
  ragdolls only for near-tier deaths, capped low-double-digits (Epic bench: 64 ragdolls ≈
  8.2ms, dated but order-of-magnitude right).
- **Influence-grid thread safety**: processors run multithreaded.
  `Query.AddSubsystemRequirement<UEmergeInfluenceGrid>(EMassFragmentAccess::ReadOnly)` +
  specialize `TMassExternalSubsystemTraits<UEmergeInfluenceGrid>{ThreadSafeRead=true}` —
  valid ONLY if the grid is stable during the Mass sim phase (decay/writes on game thread
  outside processor execution). If not guaranteed: leave false (Mass serializes access) or
  `bRequiresGameThreadExecution=true` on the steering processor (affordable at 500).

### 4.5 Perf budget
| Layer | Cost @200–500 | Evidence |
|---|---|---|
| Mass sim (steer+avoid+LOD) | ~0.5–1ms (noise) | MassZombies 5k@3060Ti; MassFlock 15k/16ms |
| VAT/ISM far tier | ~zero CPU anim; instanced draws | community consensus |
| Skeletal near tier | THE cost: ~0.05–0.15ms/agent | crossover ~200–500 skeletal even w/ ABA |
| Ragdolls | ms each aggregate; cap 5–15 | Epic Chaos bench |

Animation Budget Allocator: use on the near/elite tier only (needs
`SkeletalMeshComponentBudgeted`; known auto-register/CVar-init warts). WWZ Swarm Engine
(verified deck): 500 on-screen on PS4-class, CPU-bound on draw submission; behavior LOD =
horde flows as one unit far away, individuals detach near players, clusters of ~10–15 share
one path solution behind a leader — **validates our exact tiering**. Same engine powers
Space Marine 2.

### Phase-B architecture + build order
Tiers: (1) elite 1–5 = full ALS + awareness FSM (built); (2) near ~15–40 LODMaxCount-capped
promoted pooled actors (light ABP + ABA, real capsules, montage reacts, budgeted ragdolls);
(3) far ≤500 = ISM + VAT (walk/lurch/flinch/climb/death states, per-instance variation);
(4) off = None + MassSimulationLOD throttling.

Modules: MassEntity(/MassCore), MassCommon, MassSpawner, MassMovement, MassNavigation,
MassRepresentation, MassLOD, MassActors, MassSignals. Skip: ZoneGraph/MassCrowd lanes,
MassTraffic, MassReplication.

Build order: (1) walking skeleton — zombie EntityConfig + spawner + gradient-descent steering
processor + debug draw (proves grid→horde with cubes, 1–2 days); (2) avoidance + grid
funneling; (3) VAT far tier ("500 zombies shamble at me" milestone); (4) combat — hash-grid
hitscan + Damaged signal → FDead observer → VAT deaths + flinch; (5) promotion tier +
graduate-to-elite; (6) traversal + MassSimulationLOD + MassInsights profiling; (7) timeboxed
ISKM/AnimBank spike.

Risk register: MassCore Build.cs renames on upgrade; ISM-anim experimental (CitySample-proven
path + spike as mitigation); pooled-actor stale state (hard reset in PrepareForGame); never
sync-spawn/destroy during processing (Defer); never trust ISM hit indices (hash grid); Mass
replication off the table if MP ever appears.

---

## Cross-cutting sequencing (whole game)

Near-term (already queued as task #17): pause menu → health bar → zombie melee → pursuit
polish — recipes banked in memory (emergence-npc-ai.md).

Then, dependency-ordered across clusters:
1. **Raid skeleton + level flow** (C3) — everything else hangs off phases + the raid loop.
2. **Survival subsystem wiring + HUD skeleton** (C2) — pure wiring of already-landed kernels
   (EmergeVitals/Infection/Temperature/StatusTier/SprintGate); proves the event pipeline.
3. **Trace→limb pipeline + fire loop** (C1) — unblocks all combat; hitscan first.
4. **Interaction + world items → grid inventory UI** (C2) — biggest UI chunk.
5. **Leg-damage speed + stagger ladder + ADS/overlays** (C1) — headline feel mechanics.
6. **Persistence + extraction points + noise wiring** (C3) — the loop gets stakes; EmergeNoise
   radii math already exists, this is plumbing.
7. **Magazine inventory + reloads** (C1) + **item use/hotbar** (C2).
8. **Director** (C3) — needs raid skeleton, spawning, and noise feeding it.
9. **Phase B Mass horde** (C4) — walking skeleton can start any time after the influence grid
   is being stamped by noise (step 6); full tier build follows the C4 order.
10. Stretch: dismemberment lite, ballistics manager swap-in, ISKM/AnimBank migration.

Shared foundations to build ONCE, used by many clusters: the `EmergeNoise::Report` wrapper
(C3 hearing + C3 director Threat layer + C4 horde stimulus), the influence grid layers
(chase FSM + director + Mass steering), item handle/FGuid + FInstancedStruct records
(C2 inventory + C3 persistence + C1 magazines), CommonUI PrimaryGameLayout (C2 HUD + pause
menu + C3 loading screen).

---
---

# ROUND 2 (2026-07-10): gap clusters 5–10

Second research pass covering everything round 1 didn't: melee/stealth, animation polish,
audio/VFX, weapon modding + economy, city environment + performance, menus/settings/UX.
Same verification standard; ALS claims source-read.

## Cluster 5 — Melee combat + stealth

### 5.1 Zombie melee (task #17 material)
- **Attack executor**: in-place montage ON THE MAIN ANIM INSTANCE (never a linked layer —
  Epic tech note: root motion is not extracted from montages on linked instances; motion
  warping inherits the same trap, so a warped montage on an ALS layer will NOT warp) +
  soft-rotation windup (`RInterpTo` yaw toward target during the windup notify window,
  FREEZE at active frames so dodges work) + contact-frame radial re-check (sphere/cone
  overlap re-validating range + facing dot) → EmergeDamageComponent. Data-drive
  windup/active/recovery + SEPARATE hit-vs-whiff recovery (L4D exposes exactly this).
  Published pacing anchor: melee enemies attack every ~2–3s average, staggered.
- **Distance-closing lunge**: `FRootMotionSource_MoveToForce` — the SAME mechanism ALS uses
  for mantling (folklore killed: ALS does NOT use Motion Warping for mantles), network-clean,
  no montage root motion needed. Telegraph rule: stronger attack = bigger tell; gate lunges
  on on-camera presence (Killzone rule — off-screen hits feel unfair) or loud audio.
- **Attack tokens (Kung-Fu Circle, Game AI Pro ch.28 — full published spec)**: centralized
  stage manager; 8-slot world-aligned ring per target; TWO budgets — grid capacity (who may
  engage) + attack capacity (concurrent attacks); slots released the moment an attack
  LAUNCHES (attackers rotate); unassigned zombies hover outside the ring → free flanking;
  difficulty = raise the two capacities, never rebalance weights. Category rule (Aztez):
  never two of the same category simultaneously (≤1 active grab, ever). DOOM 2016 twist:
  better-positioned demons STEAL tokens.
- **Grab/pin**: for solo play use Dying Light's mash-QTE self-escape, not L4D hard pins
  (those are co-op pressure tools). Hand-rolled paired montages + attach + shared-anchor
  warp; Epic's ContextualAnimation plugin is purpose-built but still Experimental — adopt
  only if variants multiply. Grab montages on the main instance (same linked-layer trap).
- **Hit detection**: zombies = contact-frame radial check (O(1), immune to update-rate
  optimization on distant AI); socket sweeps (prev→current socket sphere traces in a notify
  window) reserved for PLAYER weapons where geometric fairness is visible. Already-hit
  `TSet<AActor*>` lives on the combat component keyed per swing, NEVER on the notify
  (instances shared — our banked constraint, reconfirmed).

### 5.2 Player melee + shove
- New overlay tags `Als.OverlayMode.Melee1H/2H` (there is NO stock melee tag — verified in
  AlsGameplayTags.h) + linked anim layer. Combos in pure C++: one montage with named
  sections, `Montage_JumpToSection`/`Montage_SetNextSection`, input buffered as `bComboQueued`
  consumed at BranchingPoint notifies (tick-synchronous — Queued notifies can fire late/drop).
- **Shove (L4D verified numbers)**: cone overlap → EmergeStagger event +
  `LaunchCharacter(Vel, true, false)` (sets velocity — deterministic; AddImpulse is
  mass-scaled and friction-resisted); shove FATIGUE = ~7 free consecutive shoves then ~1s
  forced delay (the anti-spam that made L4D melee balanced).
- **Hitstop**: `CustomTimeDilation ≈ 0.01` on attacker+victim only (never global — freezes
  particles/camera), restore via normal timer (timers ignore per-actor dilation — that's the
  clean combo). 3–6 frames light, 8–12 heavy/kill (fighting-game norms; GoW/MonHun numbers
  are folklore). Store pre-hitstop dilation once / clamp restore to 1.0.
- Melee stamina anchor (Project Zomboid, best-documented): cost ∝ weapon weight; 15–30
  swings fresh; heavy = 2–3× light.

### 5.3 Stealth model upgrade path (each step shippable)
1. **`IAISightTargetInterface::CanBeSeenFrom` on the player** returning fractional
   `OutSightStrength` — the single hook everything plugs into; keeps AIPerception as the
   cheap LOS/candidate filter (community consensus: exactly our current shape).
2. Stance + motion multipliers into strength: stance ~1.0/0.6/0.35 (stand/crouch/prone),
   motion 1–2×.
3. Custom `StealthVisibility` trace channel (Project Settings → AI System → Default Sight
   Collision Channel is configurable): bushes block it, stay non-blocking on Visibility.
   3–5 socket samples → cover fraction; but present ONE moving primary sample point
   (TLoU shipped this after trying multi-joint weighting — players couldn't predict safety;
   legibility beats accuracy).
4. Light term: analytic light aggregation @10Hz cached (directional shadow-traced +
   skylight + point/spot cone/falloff math) as a 0.2–1.0 multiplier, NEVER a gate. UE has no
   luminance-at-point API (confirmed); render-capture readback (Dark Mod lightgem pattern,
   `FRHIGPUTextureReadback` async, 8×8, accept 1–3 frame latency) only if Lumen GI must
   matter. Design precedent: coarse/binary beats accurate (Thief, MGSV).
5. Kernel hardening: Blacklist-style gates (min visible fraction ~0.25 to even start
   filling) + per-FSM-state thresholds (Unaware ≫ Suspicious ≫ Chasing) + vision cone angle
   INVERSELY proportional to distance (wide near — fixes point-blank blindness; TLoU).
6. Takedowns: dual montages + MotionWarping SkewWarp to an anchor (this one CAN warp — it
   plays on the main instance); eligibility = dot-behind + distance + awareness ∈
   {Unaware, Suspicious} (numbers are folklore everywhere — tune ours); witnesses see the
   event (no i-frames — that's folklore too); kill noise = small MaxRange (~300–800uu) +
   influence-grid stamp; corpse = `UAIPerceptionStimuliSourceComponent` registered for Sight
   (and UNREGISTER the live-pawn registration on death or zombies keep "seeing" it).
7. Archetype presets (published precedent maps to our FSM + grid): shambler 1500uu/70°
   hear×1 persist 30s · runner 2500/90° ×1.5 fill×2 60s · clicker sight 0 hear×4 fill×3 15s ·
   screamer 2000/110° + new `Alarming` FSM state (root, telegraph 1–2s kill window, grid
   stamp over 60–100m). **Skip UAISense_Team** — `ReportTeamEvent` doesn't exist (folklore);
   the real API is a payload-less radius broadcast; our influence grid is strictly superior
   and matches shipped practice (WWZ/Days Gone/L4D all use group-memory, not agent events).
   Use `IGenericTeamAgentInterface` + affiliation only so zombies don't perceive zombies.

## Cluster 6 — Animation polish + content pipeline

### 6.1 Foot-slide fix (our quantified 375-authored @ 560 chase, ratio 1.493)
- **The answer: Lyra-style split — play rate × stride warp.** (1) Play rate ~1.2 on the run
  sequence (or `SetPlayrateToMatchSpeed` clamped 1.0–1.2). (2) **Stride Warping node in
  MANUAL mode** (Graph mode reads root-motion delta = silently does nothing on ALS's
  in-place anims — the #1 trap): `StrideScale = Speed/(375 × PlayRate)` ≈ 1.24, clamp
  (0.9, 1.35), stride direction = component-space velocity. 1.2 × 1.24 = 1.488 ≈ 1.493 →
  residual skate ~0; verify with footSlideUu telemetry. Zombie bias: hotter rate (1.3) /
  lighter stride reads fine — frantic zombie legs are genre-tolerated.
- Node setup: plugin Animation Warping; wrap `LocalToComponent → StrideWarping →
  ComponentToLocal`; Foot Definitions per leg {ik_foot_l, foot_l, thigh_l} + ik_foot_root +
  pelvis; Pelvis IK solver on; Clamp IK Using FK Limits. Place AFTER locomotion/layering,
  BEFORE ALS's `CR_Als` Control Rig foot node (ALS foot IK/lock is C++-computed and applied
  by Control Rig at graph end). Watch: ALS foot LOCK pins planted feet in world space —
  residual slide becomes leg stretch/pop; dampen FootLeftLock/FootRightLock curves for the
  chase gait if seen. Debug: `a.AnimNode.StrideWarping.Debug 1`. No prior art in the ALS
  repo — we're first; no reported blockers either.
- Distance Matching fixes starts/stops/pivots (transients), NOT steady-state loop mismatch —
  wrong tool for this bug.
- **Parallel content path (~$45–100)**: MoCap Online Zombie packs (Basic 119 anims $44.99 /
  Pro 265 $99.99) — locomotion in BOTH root-motion and in-place variants WITH documented
  cm/sec travel speeds (directly solves speed matching); UE4-template skeleton = closest to
  SK_Als (ALS-Refactored is NOT on the UE5 Manny skeleton — folklore). Pick the chase clip
  nearest 560 → stride warp shrinks to a trim. "Frank Zombie"/Kubold zombie packs don't exist.

### 6.2 ALS rotation + wander + gait delivery
- **CHECK OUR VENDORED TAG FIRST**: `RotationInterpolationSpeedCurve` semantics INVERTED
  between ALS versions — ≤4.15: interp speed (bigger = faster); ≥4.17/main: HALF-LIFE in
  seconds (bigger = SLOWER), via exact damper. Curve sampled at GaitAmount clamped ≥1 —
  the 0–1 region is never read; author on X ∈ [1,3]. ALS ships three authored archetypes:
  `CF_Als_RotationInterpolationSpeed_Normal/_Responsive/_Sluggish` — start zombie from
  Sluggish, player from Normal/Responsive; author `CF_Zombie_*` next to the zombie movement
  settings asset (preferred over runtime DuplicateObject; reserve duplication for
  per-instance randomized lurch). View-yaw multiplier ≈1 for AI → the curve alone dominates
  NPC turning, and the player gets snappier when whipping the camera for free.
- **Speed wander is trivial (folklore killed)**: `UCrowdManager` re-reads
  `GetCrowdAgentMaxSpeed()` = `GetMaxSpeed()` EVERY crowd tick — per-tick
  `MaxWalkSpeed = Base * (1 + Amp * PerlinNoise1D(Time*Freq))` propagates automatically; no
  `UpdateCrowdAgentParams` call, no re-MoveTo. Optional direction-preserving layer: subclass
  `UCrowdFollowingComponent`, override 4-param `ApplyCrowdAgentVelocity`, scale magnitude,
  call Super; install via `SetDefaultSubobjectClass` in the controller ctor. Re-issuing
  MoveTo for wander = strictly worse. **NavCorridor: skip** — no following component exists
  (folklore); it's a math library only, no docs, no sample.
- **Zombie gait delivery (the maintainer-built mechanism)**: every Linked Anim Graph node in
  AB_Als has a TAG (release 4.14) → override with `LinkAnimGraphByTag()` — make a zombie
  Grounded-layer AB with zombie clips, link it on the zombie class only; player stays stock;
  vendored plugin stays upgradeable. Bought clips lack ALS's baked curves (foot IK/lock,
  GroundedMovement) — re-author curves or disable those features for zombies.
- Retarget pipeline: `unreal.IKRetargetBatchOperation.duplicate_and_retarget(assets, srcMesh,
  tgtMesh, retargeter, search, replace, prefix, suffix, remap)` (no output-folder param —
  move after); Retarget Root = Pelvis; `root` must be its OWN mapped chain (Globally Scaled)
  or root motion silently doesn't transfer; keep separate retargeter configs for RM vs
  in-place batches; known bugs: fingers mis-posed, IK bones not retargeted (ALS "Setup Als
  Skeleton" scripted action mitigates), compressed-data corruption fixed by FBX round-trip.
- **GASP/Motion Matching verdict: don't migrate the player now** (GASP has no overlay/weapon
  layering, rougher netcode, and is mid-pivot to Mover/UAF; ALS maintenance-mode is the one
  counter-argument — wait, don't chase a moving target). Zombies on MM: feasible but
  pointless — clips + stride warping is cheaper and more art-directable.
- Near-tier polish: head-look is FREE (`AIController->SetFocus(Player)` — ALS HeadState
  tracks view rotation, head-only by default since spine needs Aiming mode); twitch =
  looping mesh-space additive clip via Layered-blend-per-bone on spine_01+ with randomized
  alpha/phase (cheapest, primary), Spring Controller on neck for loose-neck overshoot
  (LOD-thresholded); death = authored anim 0.2–0.5s → partial ragdoll blend below pelvis →
  Chaos sleep → freeze at ~10s (`SavePoseSnapshot` FIRST or SetSimulatePhysics(false) snaps
  to ref pose); prefer Animation Budget Allocator over URO (ALS is curve-heavy and curves
  misbehave under URO).

## Cluster 7 — Audio + VFX

### 7.1 Weapons (MetaSounds)
- Layered gunshot: 3 Wave Players (mech/body/tail) → Mixer; `UE.Attenuation` interface's
  auto Distance input → Map Range → Crossfade to distant-tail layer. MetaSounds has NO
  per-layer attenuation (SoundCue's one legit surviving advantage); overall rolloff on the
  SoundAttenuation asset. SoundCues are NOT deprecated (folklore) — but MetaSounds-first.
- **Full-auto = ONE persistent looping MetaSound per shooter** (`Trigger Repeat` is
  sample-accurate), exposed rate + explicit Stop trigger, tail as one-shot on stop — one
  voice instead of ~15/sec; Lyra's own rifle bugs (fires ~1s after empty, 5ms-poll drift)
  are the cautionary tale. One-shot-per-shot stays fine for semi-auto.
- Reload foley: exposed triggers (MagOut/MagIn/BoltRelease) fired from AnimNotifies via
  `ExecuteTriggerParameter` → `Random Get` arrays with No Repeats + **Enable Shared State**
  (otherwise 40 instances repeat independently — critical for zombies too).
- Concurrency (verified names): `USoundConcurrency` MaxCount/Retrigger Time/Voice Steal
  Release Time (20–50ms on gunshots vs clicks); rules PreventNew/StopOldest/StopFarthest…;
  gunfire group MaxCount 4–8 StopOldest; default engine ceiling 32 voices
  (`FAudioPlatformSettings::MaxChannels`). PlayWhenSilent still CONSUMES a voice. Debug:
  `au.Debug.Sounds 1`.

### 7.2 Footsteps + zombie vocals
- **ALS already ships the footstep system**: `UAlsAnimNotify_FootstepEffects` +
  `UAlsFootstepEffectsSettings` DataAsset — `TMap<EPhysicalSurface, {Sound, Decal,
  Particle}>`, foot-bone trace with `bReturnPhysicalMaterial=true`, `FootstepSoundBlock`
  anim curve suppresses blend spam. Extend the DataAsset with our surfaces. Its spawn
  functions are PRIVATE/non-virtual and it's cosmetic-only (`bTriggerOnDedicatedServer =
  false`) → report EmergeNoise from movement/gameplay code, not the notify (also immune to
  URO mistimed notifies + off-screen `OnlyTickPoseWhenRendered`).
- Vocals budget: near tier 8–12 concurrent one-shots (concurrency group MaxCount ~10,
  StopFarthestThenOldest, Volume Scale Mode = Distance); far tier = **3–5 cluster emitters**
  playing looping horde-walla beds with headcount/aggro params, attached to influence-grid
  cluster centroids (the Planet Coaster/DL2 pattern — our grid already computes it).
  Bark manager subsystem on FSM transitions with global semantic caps ("one first-detection
  scream per 2s"). L4D lesson: every special owns a frequency band + spawn sting
  (distance-coded), always played identically — audio as fairness contract.
- Horror spatialization: LogReverse/custom falloff (faintly-there-then-blooms), Air
  Absorption LPF = cheapest dread lever, built-in occlusion (1 async trace per source) on
  the ~20 near voices only, Non-Spatialized Radius on contact-range vocals. Interiors:
  Audio Gameplay Volumes (Attenuation + Filter components = the cheap doorway/muffle trick).
  Mix states: Audio Modulation control buses (`CB_AmbienceVol/CB_MusicVol/CB_SFX_LPF`) +
  mixes CBM_Stealth/Combat/Pause — activated mixes are ref-counted and OUTLIVE their
  activator (always deactivate in EndPlay). Same LPF bus doubles as critical-health muffle.

### 7.3 VFX at horde scale
- Folklore killed: GPU sim is NOT cheaper for tiny emitters (Epic: CPU for low counts; GPU
  pays off ~1000+); muzzle flash + shells = CPU sim, one system, Light Renderer (no shadows
  → dodges VSM), never the Component Renderer.
- **One-system-as-a-service** (Epic's own recommendation) for impacts: single parameterized
  burst system + surface→tag map; bullet holes = persistent CPU emitter with the **Decal
  Renderer** (5.2+; per-decal cost ≈ Decal Actor but far cheaper at volume, lifetime = fade).
- **Horde deaths = Niagara Data Channels**: one persistent consumer system; batch-write the
  frame's deaths via `CreateDataChannelWriter` → WritePosition/etc.; use the **GameplayBurst**
  access context (5.7+ — Islands is Legacy now). CRITICAL: `SearchParameters.Location`
  defaults to ORIGIN — unset, every write is bounds-culled at (0,0,0). GPU emitters can read
  but not write NDCs. Get transforms from Mass fragments, not GetInstanceTransform.
- Blood: 3–5 layers (droplet flipbook, chunks, mist, decal); ribbons for arterial; floor
  gore at scale = Grid2D splat-mask render target written by ONE global system (Let Them
  Come pattern); blood-on-camera = post-process material + MPC scalar (not a camera sprite).
  Gibs: physics actors only for 1–4 hero limbs; rest = mesh-renderer particles; stump-cap
  mesh over the HideBoneByName joint (scale-to-0 stretches skin).
- Tracers: velocity-stretched sprites (NOT ribbons — CPU-serial ordering flickers at bullet
  speed); array-DI recipe needs Persistent IDs + reactivation per burst → NDC is the
  productized version once multiple shooters exist.
- Budgets: total Niagara tick <2–3ms; overdraw kills GPU long before counts; system INSTANCE
  count is the CPU constraint (hence one-system architectures); raise Effect-Type Max
  Distance (some templates default 1500uu!). The "0.5–1.5ms GPU VFX budget" quote is
  sourceless folklore.
- Screen feedback: split vignette (post-process blendable, `AddOrUpdateBlendable` once +
  drive MID scalar) from directional wedge (UMG radial-mask material, angle from
  `Atan2(Dot(ToAttacker,Right), Dot(ToAttacker,Fwd))` off GetControlRotation, RE-EVALUATE
  while displayed as camera turns); heartbeat = MetaSound `BPM To Seconds` → Trigger Repeat,
  BPM param from vitals delegate with hysteresis; one health fraction drives BPM + LPF bus +
  vignette together.

## Cluster 8 — Weapon modding + loot economy

### 8.1 Modular weapons (Tarkov's actual architecture, read from SPT server source)
- **Template/instance split**: templates = immutable catalog assets where every item has
  `Slots[]` ({SlotName, AttachSocket, bRequired, RequiredTags/BlockedTags, explicit
  Allowed/Excluded lists}) — recursion is implicit (a handguard is just a template with its
  own slots); tree-global `ConflictingItems` + `BlocksSlotNames` (integrated stock blocks
  mod_stock). **Instances = a FLAT array** `{InstanceId, Template, ParentInstanceId,
  SlotName, FInstancedStruct State}` — the tree is reconstructed by joining parent/slot;
  trivially serializable; maps perfectly onto our FInstancedStruct records. Nothing
  published implements the recursive version in UE (Lyra has NO attachment system) — we
  synthesize. Tag gotchas: `HasAll(empty)==true` (accepts anything — desirable);
  `HasTagExact` for calibers.
- Mesh composition: sockets all the way down — static meshes support sockets too
  (`UStaticMeshSocket` on a rail exposes where the scope goes); attach with
  `SnapToTargetNotIncludingScale` (IncludingScale snaps scale — folklore trap); runtime
  components need `RegisterComponent()`. Leader Pose is NOT needed (that's for modular
  characters); skip SkeletalMeshMerge.
- **Stat aggregation (verified against 3 codebases)**: ergo = FLAT SUM; recoil = additive
  percentage points applied ONCE to base (`base × (1 + Σmod%/100)`) — NOT multiplicative
  compounding (folklore); nesting depth is irrelevant (tree is flattened). Aggregate on
  attach/detach via DFS, cache, feed EmergeWeapon/EmergeBallistics.
- **ADS recalibration**: `S_Aim` socket ON EACH OPTIC's mesh (X along sight line); solve
  weapon transform so the socket sits on the view axis at eye relief:
  `WeaponADS = AimRel.Inverse() * DesiredAimWorld`. Swapping optics re-zeros automatically;
  compose through EVERY intermediate socket (rail→mount→optic — rotated intermediates skew
  the chain). Canted sights = second socket rolled −45°. Zeroing = angle the launch
  direction at muzzle toward SightForward × ZeroDistance (height-over-bore emerges).
  Magnified: post-process scopes; render-target PiP only for one hero sniper tier.
- Inspect screen: hidden studio actor + `USceneCaptureComponent2D` → RT → UImage;
  RTF_RGBA8_SRGB, SCS_FinalToneCurveHDR, PRM_UseShowOnlyList + `ShowOnlyActorComponents`
  (MUST re-call after every attach/detach — parts invisible otherwise), manual exposure,
  `CaptureSceneDeferred()` on demand only (always-on = second scene render), Lighting
  Channels to isolate studio lighting (ShowOnlyList does NOT filter lights).
- Ammo: full Tarkov field set banked (Damage/Pen/ArmorDamage%/Frag/InitialSpeed/BC/tracer/
  DurabilityBurn/Malf chances); pen-vs-armor curve `A=(121−5000/(45+2d))·class·0.1`, pen
  chance piecewise at P vs A; **pen<20 forces frag to 0** (the real rule — "frag<20" is
  folklore). Mags feed LIFO (tracers at the BOTTOM as low-ammo alarm — player trick worth
  keeping); mag payload = run-length `TArray<FAmmoRun>{AmmoType, Count}`, chambered round a
  separate slot; store asset IDs not copied stats.
- **Malfunctions verdict: YES for SP, but the Far Cry 2 version**: hard safe threshold
  (~93% durability — a maintained gun never betrays you), misfire + failure-to-feed only,
  the clear ANIMATION is the feature, NPCs jam too, difficulty toggle. Gives looted
  zombie-city guns identity and feeds hideout maintenance.

### 8.2 Loot + traders + progression
- Three orthogonal systems (Tarkov, verified): loose-loot spawnpoints (position +
  probability + own mini-table; count = mult × NormalRandom(mean,std)), container tables
  (count distribution THEN weighted picks), staticAmmo (caliber → weighted ammo for found
  mags). Tables as UPrimaryDataAssets with SOFT refs (hard refs pull the whole catalog);
  entry = weight + (Item XOR NestedTable XOR null), recursion cap + IsDataValid cycle check;
  pity/bad-luck state in the runtime resolver, never in assets.
- **DayZ CLE borrows**: usage/tier TAG INTERSECTION (locations tagged Military/Medic/etc ×
  Tier1-4 zones × item tags = generation rule — the single best borrow); **proto/pos split**
  (spawn points authored once per building CLASS, stamped per instance — our city-scale
  scaling law); condition + random-attachment spawn presets (a found AKM at 0.45–0.85
  condition FEELS found); one bounded fill pass at raid start.
- Authoring: marker `USceneComponent`s with GameplayTags inside furniture/building
  Blueprints (positions travel with the asset; NEVER ChildActorComponents); **buildings as
  Level Instances NOT Packed Level Actors** (PLAs strip non-mesh components — Epic-staff
  confirmed; markers can't live in them); bake a registry at cook time (commandlet sweep →
  per-cell {GUID, transform, tags}); loot director UWorldSubsystem fills via tag
  intersection + SoD2 quota decks ("of ten restaurant sites, exactly seven have food");
  persist looted-state by GUID.
- Loot psychology: every item ≥2 uses (vendor-only = self-declared trash); vendor price
  below use-value; the best items UNBUYABLE (Tarkov's FIR lesson); smart-loot bias toward
  current quests/next hideout module; hidden pity on progress-gating items; keys = the
  genre's perfect rare; run 2–3 parallel reward loops (money/craft/collection).
- Traders on EmergeEconomy/Reputation: loyalty tiers on rep + CUMULATIVE SPEND (drop the
  level axis); LLs gate STOCK not prices (folklore); per-offer stock caps; payRate bands
  0.45–0.63 by specialization + one Fence archetype at 0.30; 10–30 curated barters
  (cash-as-barter unifies the mechanism — SPT schema); **restock per N raids** (never
  wall-clock — kills log-off-and-wait).
- Skills: do NOT copy Tarkov use-based leveling (BSG's own patch notes admit the abuse;
  the failure is incentive misalignment, fully reproduced in SP) → raid-XP-spend (our
  EmergeSkill curve fits) or SoD2 capped-use hybrid with a specialization choice at cap;
  SoD2's best borrow: individual skills gate BASE FACILITIES (textbooks become loot as
  exciting as weapons).
- Hideout at Zero Sievert scale: 5–7 stations × 3 tiers (Stash/Workbench/Medstation/Power/
  +flavor), junk-loot requirements, raid-count craft timers, stash expansion as flagship
  reward; skip fuel micromanagement + passive income (daily-login mechanics).
- Quests = the content spine: `UQuestDefinition : UPrimaryDataAsset` (objective rows: type
  enum + EventTag + FGameplayTagQuery + count), GameplayMessageRouter broadcasts from
  kill/pickup/extract emitters (they never know quests exist), `UQuestSubsystem :
  UGameInstanceSubsystem` matches tags + evaluates queries. Reference: SUQS (MIT). SP
  adjustments: relax FIR, conditioned kills as optional challenge mods, always flag quest
  items in loot UI.
- Tuning workflow: definitions = PrimaryDataAssets + AssetManager (AlwaysCook); bulk numbers
  = DataTables with C++ FTableRowBase ONLY (BP structs corrupt tables — UE-40033);
  progression curves = CurveTables; feel curves = inline FRuntimeFloatCurve on definitions
  (what Lyra actually does — Lyra is NOT DataTable-driven, folklore); CSV → Auto Reimport;
  CVars work in Shipping (folklore says otherwise); `UCheatManager` exec `ReloadBalance()`
  via `CreateTableFromCSVString` for packaged playtests; runtime writes to definition assets
  PERSIST in PIE — always copy stats into runtime instances at spawn.

## Cluster 9 — City environment + performance

### 9.1 Map architecture (Downtown West → 1km² raid map)
- Pack prep FIRST: bulk-enable Nanite on all opaque meshes via property matrix (it's a 2020
  UE4 pack, NOT Nanite-authored — and non-Nanite geometry is the expensive path in VSM);
  texture LOD bias 4K→2K (texture pool is the real memory pressure); vertex-painted meshes
  lose paint when batched into ISM/PLA — keep painted heroes as plain actors.
- **World Partition map with `Enable Streaming = false`** — 1km² doesn't need streaming
  (consensus: <4km² single-persistent; City Sample is 16km²/64GB min-spec — we're 1/16th;
  Nanite + textures stream from disk regardless of WP), but the WP map keeps One File Per
  Actor, Data Layers (work with streaming off), HLODs, and the flip-it-later option.
- Structure: Level Instances for authoring units (building → block → district), Packed
  Level Actors only for the heaviest repeated visual-only groups (remember: PLAs strip
  non-mesh — loot markers live in LIs), ISM for Nanite meshes (HISM only for non-Nanite —
  "HISM clustering is irrelevant under Nanite", Epic staff), 5.5+ Runtime Cell Transformers /
  5.6 FastGeo do the merging automatically.
- Interiors: author as Level Instances in-map; runtime Data Layers ONLY for logic-gated
  content (each runtime DL × grid combo spawns extra streaming levels — keep few/coarse);
  non-enterable windows = City Sample's interior-mapping shader (two tris + cubemap, free).
- HLOD0 = Instancing layer type for the skyline (cheap to build); HLOD proxies have NO
  collision by default — never AI/physics ground.
- Nanite kitbash caveat: interpenetrating modules = overdraw (Nanite's real weakness) —
  remesh the most-repeated stacked assemblies; masked materials ≈ 2× raster cost; clamp Max
  WPO Displacement. If streaming ever on: cell 64–128m, loading range ~256m; hard refs
  between distant actors merge their cells (soft refs across the map; debug
  `wp.Editor.DumpStreamingGenerationLog`).

### 9.2 GPU budgets (our hardware) + the horde rendering risk
- Budget table (triangulated, MED confidence): 4090 @1440p native 70–100fps comfortable;
  4090 @4K + DLSS Quality = the 4K target; 3090 @1440p + upscale Quality = its target
  (native 1440p won't hold 60); 3090 @4K native not viable. Lumen GI+reflections target
  4ms@60fps (Epic's own number, 1080p console) — 5.8 adds **Lumen Lite** (~2× faster than
  High) as the natural mid preset; skip HWRT hit lighting.
- **#1 rendering risk: VSM invalidation from the crowd.** Skeletal/WPO geometry invalidates
  cached shadow pages EVERY frame; Nanite skinning does NOT fix it (UE-328823 open).
  Mitigations in order: cast-shadow ONLY on nearest ~20–30 zombies; capsule/contact shadows
  for the rest; `r.Shadow.Virtual.ResolutionLodBiasDirectional` (Fortnite shipped 60fps by
  ~halving effective sun-shadow res); if the sun moves continuously anyway,
  `r.Shadow.Virtual.Cache.ForceInvalidateDirectional`. Don't fall back to CSM in a Nanite
  city — "cheaper VSM" beats it.
- City Sample was CPU-bound on a 12900K — at 500 agents OUR bottleneck is likely CPU:
  MassLOD ladder + Animation Budget Allocator are mandatory, not optional.
- Night city: moon = the one VSM directional; big soft emissive façades (never small bare
  bulbs — noise; and `MaxRayIntensity` default dropped 40→10 in 5.6, silently dimming neon
  GI); **MegaLights (Production-Ready 5.8)** for many shadowed local lights; flashlight =
  spot + IES profile, MegaLights-driven, own component with `RInterpTo` lag (never
  hard-attach to camera/hand — decouples ALS anim noise), beam = analytic stimulus into the
  awareness kernel (Dying Light precedent: light attracts).
- Sky: DIY SunSky stack (Sun Position Calculator + SkyAtmosphere + volumetric clouds + EHF);
  SkyLight Real Time Capture is time-sliced ≤0.2ms (cheap — folklore killed) but never call
  `RecaptureSky()` for time-of-day (≥120ms hitch). UDS purchase = weather turnkey only,
  giant Blueprint, hard to drive from C++ — skip. Volumetric fog only for weather states
  (GridPixelSize 8; 4 ≈ 4ms@1080p). Rain = camera-attached GPU Niagara + depth collision +
  top-down rain-mask RT for under-roof; wetness = one MPC scalar in master materials.
- Time-of-day → senses: subsystem multiplier on a 0.5–1s tick; per-agent
  `UAISenseConfig_Sight` radius updates require `RequestStimuliListenerUpdate()` (nothing
  applies without it); horde reads the same multiplier via our kernel directly.

### 9.3 Navmesh + streaming×AI + packaging
- Navmesh: DMO (banked, correct), CellSize 32 streets (Nav Mesh Resolution Params for
  Low=64 open areas), TileSizeUU ~2048–3072, **thin street-level bounds volumes with tight
  Z** (rooftops never voxelize — cheapest exclusion; walkable slabs at every height cost
  tile layers, "height is free" is folklore), Fixed Tile Pool SIZED (exhausted pool =
  silent gaps) — but disabling it is also the recurring packaged-build workaround: TEST
  PACKAGED EARLY. Scale anchor: a 5.5 WP city reported ~400MB total nav memory. WP-
  partitioned navmesh still Experimental + Epic-stated stale — one resident navmesh, matches
  our streaming-off verdict. NO nav invokers (they require full Dynamic generation — the
  most expensive mode; 500 invokers is an anti-pattern).
- Streaming × AI: **verified — runtime-spawned actors are NOT WP-managed** (persist in the
  persistent level; never unload) → our Director's Active Area Set is the ONLY despawner;
  Mass entities aren't WP-streamed either (MassSpawner ACTOR is — uncheck Is Spatially
  Loaded, the classic "City Sample has no crowds" fix). With streaming off the whole
  interaction class disappears. L4D's AAS (Booth) is the canonical pattern.
- Packaging: cook-only-referenced is default — the killers are directory-list settings and
  demo maps seeding the cook graph (`DirectoriesToNeverCook`, explicit MapsToCook); Oodle
  Kraken already default; `bSkipEditorContent=True`; expect 40–60% size cut, textures
  dominating. CI on l3e7: `RunUAT BuildCookRun -platform=Win64 -clientconfig=Development
  -build -cook -stage -pak -archive`; incremental cook 5.6+ needs
  `+IncrementalClassScriptPackageAllowList=Allow,<ProjectRoot>` in DefaultEditor.ini (24min →
  5.5min on a big project); cold cook 1–3h (shader-dominated), warm 5–20min; persistent DDC
  on NVMe; run rendering tests in an interactive session, not Session 0 (same class of
  problem as Plex on that box).
- PIE-vs-packaged traps (AI/nav): RuntimeGeneration mismatch on the RecastNavMesh actor vs
  Project Settings; non-integer CellSize silently breaks tile loading;
  `bAllowClientSideNavigation=false` nulls NavigationSystem on net clients; GameplayDebugger
  doesn't exist in Shipping (gate the module in Build.cs or it fails to link).
- **RemoteControl verified at source: packages into Shipping fine, runtime-gated** — outside
  the editor the web server needs the `-RCWebControlEnable` launch flag. Our autonomous
  harness therefore works against packaged DEVELOPMENT builds with that flag; Shipping
  (console compiled out) is a separate harness-free validation lane. For release hygiene:
  per-plugin `"TargetAllowList": ["Editor"]` in .uproject.

## Cluster 10 — Menus, settings, UX

### 10.1 Front end + settings
- Flow = the CommonUI stack itself: push MainMenu → Settings → etc. onto `UI.Layer.Menu`;
  back-handling via `bIsBackHandler` + `NativeOnHandleBackAction` (the "never fires" threads
  are all misconfiguration: needs CommonGameViewportClient, CommonUIInputData Confirm/Back
  rows, DefaultGamepadName "Generic"). Implement `GetDesiredFocusTarget()` on EVERY screen;
  do setup in `NativeOnActivated` (fires each activation), never NativeConstruct.
  **CRITICAL: deactivating a widget does NOT restore the previous input config** — keep a
  persistent base activatable on `UI.Layer.Game` (the HUD) returning the Game input config,
  or menus strand input state. Copy `ULyraActivatableWidget`'s ~30-line pattern (input-mode
  property + GetDesiredInputConfig override); skip Lyra's Experience/ControlFlow machinery —
  our RaidFlowSubsystem survives travel and owns the flow.
- Front end = dedicated `L_FrontEnd` diorama map (hideout/staging vignette) + CineCamera +
  SetViewTarget, UMG on top; every raid starts from a deterministically fresh world. AVOID
  SceneCapture2D fullscreen backdrops (second scene render); its legit use = inventory
  character/weapon preview PiP. Menu music on a GameInstance-subsystem-owned component
  survives travel.
- Settings: `UEmergeGameUserSettings : UGameUserSettings` registered via
  `GameUserSettingsClassName` in DefaultEngine.ini; custom fields = UPROPERTY(Config);
  override `ApplyNonResolutionSettings()` (call Super) to push into cvars/subsystems;
  `RunHardwareBenchmark()` + `ApplyHardwareBenchmarkResults()` on first boot. Resolution:
  apply → 15s countdown → `ConfirmVideoMode()` (it applies NOTHING — only stamps "last
  confirmed"; the countdown/revert choreography is ours). Use the GameUserSettings quality
  setters (persist under [ScalabilityGroups]) — console-set sg.* bypasses persistence.
- Upscalers: ship TSR-only on 5.8 behind our own {Upscaler, Quality} enum pair — vendor
  plugins lag engine releases by months; DLSS = `UDLSSLibrary::EnableDLSS` + we set
  r.ScreenPercentage ourselves (SetDLSSMode deprecated); FSR = unified "AMD FSR Upscaling"
  plugin (`r.FidelityFX.FSR.*` — its QualityMode DOES own screen percentage, opposite of
  DLSS); frame-gen providers can't swap at runtime (gate behind restart).
- Audio sliders: classic SoundClass tree (SC_Master → Music/SFX/Voice) + one SM_UserVolumes
  mix — `PushSoundMixModifier` ONCE at GameInstance init (SetSoundMixClassOverride silently
  no-ops without the push), then override per slider; Config floats re-applied in
  ApplyNonResolutionSettings (AudioDevice may not exist yet — re-apply on first world).
  Audio Modulation buses only if we're already modulation-heavy (we will be — see C7 mix
  states; can migrate then).
- **FOV × ALS (source-verified)**: `UAlsCameraComponent::TickCamera` recomputes FOV every
  tick from `UAlsCameraSettings` — `PlayerCameraManager->SetFOV()` gets STOMPED. Write user
  FOV into a per-instance duplicate of the camera settings asset (preserves TP/FP lerp);
  re-apply on possess. Sensitivity: custom `UInputModifier` on the Look IA reading
  settings × ADS multiplier.
- Input remapping: Enhanced Input User Settings (stable since 5.5) —
  `bEnableUserSettings=True` in DefaultInput.ini; `UPlayerMappableKeySettings` on each IA
  with UNIQUE Names (rows keyed by name — shared names clobber);
  **`RegisterInputMappingContexts()` at SetupInputComponent is what loads+applies saved
  rebinds** (the #1 "rebinds don't persist" cause — not a bug); `MapPlayerKey`/`UnMapPlayerKey`;
  conflict detection is DIY (~30 lines via `GetMappingNamesForKey` filtered by device →
  Block-or-Swap prompt). KBM full rebind + gamepad presets = the ship floor. Never mutate
  IMCs at runtime (destructive pre-5.3 folklore).

### 10.2 Save UX, juice, accessibility
- Save slots: flat PROFILE list (name/timestamp/playtime/preview via a tiny per-slot META
  save — stock UE has no header-only read; never deserialize full saves on the load screen);
  Continue = most recent, default-focused. Raid-in-progress: suspend-save on quit
  (delete-on-resume), **crash = resume not death** (Zero Sievert's buggy resume = endless
  forum rage), menu-abandon = MIA with explicit confirm. Corruption: atomic writes
  (tmp→flush→rename), 3 rotating backups tried newest→oldest automatically, non-blocking
  toast on silent restore, modal only for total loss; our per-file split means a corrupt
  World.sav rebuilds while Profile+Stash survive.
- **Hit markers: none — the zombie IS the hit marker** (Tarkov/DayZ/Insurgency precedent;
  SP needs no netcode disambiguation): directional flinch (primary), L4D2-style persistent
  wounds (GDC 2010 = the canonical zombie reference), blood mist sized for range, distinct
  flesh audio; kill confirm = ragdoll; down-vs-dead ambiguity is a tension FEATURE.
- **Camera shake works with ALS out of the box (folklore killed, source-verified)**:
  ALS-Refactored feeds the stock PlayerCameraManager via CalcCamera; modifiers/shakes apply
  after; the plugin even ships `AlsAnimNotify_CameraShake`. 5.8 note: shake pattern classes
  moved to the EngineCameras plugin (Build.cs dependency `EngineCameras`). Recipes: fire =
  0.1–0.2s rotation-only pitch 0.2–1°, `bSingleInstance=true` (restart-not-stack keeps
  autofire sane); damage = 0.2–0.4s Perlin; explosion = CameraShakeSource with attenuation.
- Rumble: NO "Enhanced Input haptics" (folklore — EI is input-only); `UForceFeedbackEffect`
  assets via `ClientPlayForceFeedback(Effect, {Tag})` — same Tag REPLACES (the anti-stacking
  tool); DualSense adaptive triggers on PC need a plugin (FabulousDualSense — same author
  as ALS).
- Raid summary: accumulate FRaidStats in RaidFlowSubsystem; Tarkov status banner incl.
  Run-through + Marauders-style "what killed you" ("Killed by: Screamer, bite, neck" —
  steal this); staggered row count-up, skip-input snaps to finals (mandatory).
- Accessibility: subtitles ≥46px @1080p, ≤2 lines, letterboxed, ON by default (60–80% usage;
  5.6+ engine subtitle plugin is Beta — thin DIY bark queue is the pragmatic path);
  rarity ladder needs a LUMINANCE ramp + shape/border redundancy (never color alone; 5th
  tier teal not green); hold/toggle for ADS/crouch/sprint = one IA per verb branching on the
  setting in the handler (no duplicate IMCs — folklore); difficulty = per-subsystem
  UCurveFloats in one DataAsset mapping the EmergeWorldDifficulty scalar (tooltip derives
  its concrete-change list from the same asset so the UI never lies); gamma = MPC scalar in
  a post-process material (UI unaffected), never auto-exposure.

---

## REVISED master build order (all 10 clusters)

Near-term (task #17, unchanged but now research-complete): **pause menu → health bar →
zombie melee → pursuit polish** — melee now uses C5's executor/token recipes; pursuit
polish now uses C6's play-rate+stride-warp split, Sluggish rotation curve, Perlin MaxSpeed
wander, SetFocus head-look.

Then (revised with round-2 dependencies):
1. Raid skeleton + level flow (C3) + `L_FrontEnd` shell + `UEmergeGameUserSettings`
   foundation (C10.1 — engine-side, no UI needed to test).
2. Survival wiring + HUD skeleton (C2) + settings row widgets reachable from the pause menu.
3. Trace→limb + fire loop (C1) + footsteps/weapon MetaSounds v1 + impact system (C7).
4. Interaction + world items → grid inventory (C2) + loot kernel + marker/registry
   authoring pipeline (C8 — start early, gates level-art workflow).
5. Leg-damage + stagger ladder + ADS (C1) + stealth steps 1–3 (C5: CanBeSeenFrom hook,
   stance/motion, cover channel) + zombie vocals/bark manager (C7).
6. Persistence + extraction + noise wrapper (C3) + save-slot UX/suspend-save (C10.2).
7. Item templates → weapon assembly → magazines (C8; C1 reloads land here) + inspect screen.
8. Director (C3) + archetype presets + screamer Alarming state (C5) + input remapping
   screen (C10).
9. City map build (C9 order: pack prep → WP-no-streaming blockout → navmesh + packaged test
   WEEK ONE → horde port → sky/lighting → kitbash by district → HLODs → CI packaging lane).
10. Phase B Mass horde (C4) + horde-death NDC VFX + cluster-emitter walla (C7) + traders/
    hideout-lite/quests (C8) + raid summary + accessibility pass (C10).

Builder-loop kernel backlog derived from this research (queue as specs): director
intensity/decay + state machine, spawn scoring + behind-bias, stealth fill multipliers +
visibility gate, limb damage/leg-state tiers, recoil kick/recovery, fire cadence
accumulator, extract-hold stepping, vital-tier hysteresis, weighted loot roll, stride/
play-rate solver, melee range/facing/token gates.
