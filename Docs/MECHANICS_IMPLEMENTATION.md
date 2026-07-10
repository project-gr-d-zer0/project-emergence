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
