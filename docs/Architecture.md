# Janky Studio — Architecture

## 1. Overview

Janky Studio is an offline C++23 / Qt 6 desktop application for authoring simple creatures and eventually running them in a deterministic 2D fighting simulation.

The complete product workflow is:

```text
Draw
  ↓
Animate
  ↓
Define Creature
  ↓
Define States / Behavior
  ↓
Create Arena
  ↓
Simulate Fight
  ↓
Record / Export
```

The architecture is intentionally modular and conservative.

The project favors explicit ownership, small modules, pure C++ domain/feature logic, and a Qt UI boundary.

---

# 2. Architectural Goals

The architecture must:

1. Keep Core and Domain independent of Qt.
2. Keep painting logic separate from the Qt canvas.
3. Keep simulation logic independent of Qt.
4. Prevent UI widgets from owning authoritative application state.
5. Keep rendering read-only.
6. Keep authored Creature definitions separate from runtime simulation entities.
7. Provide one authoritative shared editing context.
8. Make important logic independently testable.
9. Keep persistence versioned and migratable.
10. Avoid unnecessary abstractions and dependencies.
11. Preserve deterministic simulation behavior.
12. Remain usable offline.

---

# 3. Layered Architecture

The primary dependency direction is:

```text
┌─────────────────────────────────────────────┐
│ UI                                          │
│ Qt Widgets, pages, panels, dialogs, input  │
└──────────────────────┬──────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────┐
│ Application                                 │
│ Use cases, orchestration, ProjectContext   │
└──────────────────────┬──────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────┐
│ Feature / Domain                            │
│ Painting, State Machine, Creature rules    │
└──────────────────────┬──────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────┐
│ Core                                        │
│ Shared Qt-free primitives                   │
└─────────────────────────────────────────────┘
```

Persistence and Rendering are supporting concerns:

```text
Persistence
    ↓
stores / loads application data

Rendering
    ↓
reads state
```

The exact implementation may use interfaces/contracts where they solve a real problem, but the dependency direction must remain clear.

---

# 4. Core

## Responsibility

Core contains application-wide primitives with no application-specific knowledge.

Current examples:

```text
Vec2
Rect
Color
UniqueId
Result
```

## Rules

Core:

- must not include Qt
- must not know about Creature
- must not know about Painting
- must not know about Simulation
- must not perform file I/O
- must not contain UI behavior
- should remain small

Do not move a type into Core merely because several files currently use it. It should be genuinely generic and stable.

---

# 5. Domain

The Domain describes what the application means.

It contains authored definitions and domain-level validation.

Conceptual model:

```text
Creature
├── id
├── name
├── animations
├── stateMachine
└── metadata
```

Animation:

```text
Animation
├── name
├── frames
├── fps
└── looping
```

Animation frame:

```text
AnimationFrame
├── frameIndex
├── imageData
└── duration
```

State:

```text
State
├── id
├── name
├── type
├── animationName
└── transitions
```

State transition:

```text
StateTransition
├── targetStateId
└── conditionTag
```

State machine:

```text
StateMachine
├── states
├── initialStateId
└── maxStates
```

V1 uses a flat finite state machine.

Typical state types:

```text
Idle
Movement
Attack
Defense
Dodge
Hit
Death
Special
```

## Domain rules

Domain code must remain Qt-free.

The domain must not contain QWidget logic, rendering code, QPainter code, or file-dialog behavior.

---

# 6. Creature Definition vs Runtime State

This is a hard boundary.

```text
Creature
    = reusable authored definition

SimulationEntity
    = runtime instance
```

A runtime entity may contain:

```text
SimulationEntity
├── creatureId
├── position
├── health
├── currentStateId
├── animationFrame
└── facingDirection
```

Runtime state must not be written into the authored Creature definition.

---

# 7. Application Layer

The Application layer coordinates user-visible operations.

Typical use cases include:

```text
CreateCreatureUseCase
SaveCreatureUseCase
LoadCreatureUseCase
CommitAnimationUseCase
ValidateCreatureUseCase
StartSimulationUseCase
RecordSimulationUseCase
CreateArenaUseCase
SaveArenaUseCase
LoadArenaUseCase
```

The Application layer:

- orchestrates operations
- coordinates shared application state
- invokes domain/feature logic
- communicates with persistence where required

It does not implement Qt widgets.

It should not become a second Domain layer.

---

# 8. ProjectContext

The application needs one authoritative shared editing context.

Conceptually:

```text
ProjectContext
├── current Creature
├── current Arena
├── current file paths
└── necessary editor/session references
```

`ProjectContext` belongs to the Application layer.

It is not a global singleton.

It should remain thin.

Do not put arbitrary services, UI widgets, rendering objects, or unrelated caches into ProjectContext merely for convenience.

The critical invariant is:

```text
Creature Editor
       \
        → same authoritative Creature
       /
State Editor
```

not:

```text
Creature Editor → Creature A
State Editor    → Creature B
```

---

# 9. Painting Architecture

Painting is an editor-session system.

The source of truth is the raster pixel data.

Conceptually:

```text
PaintDocument
├── canvasWidth
├── canvasHeight
├── animationName
├── fps
├── looping
├── frames
└── activeFrameIndex
```

Each frame:

```text
PaintFrame
├── layers
└── activeLayerIndex
```

Each layer:

```text
PaintLayer
├── name
├── visible
├── locked
├── opacity
├── blendMode
└── pixels
```

Pixels are stored as a raster buffer.

The architecture describes RGBA row-major storage conceptually as:

```text
width × height × 4 bytes
```

`QImage` / `QPixmap` are display representations, not authoritative document state.

---

# 10. Paint Engine

The painting engine is pure C++ and Qt-free.

Typical operations:

```text
Pencil
Brush
Eraser
Fill
Eyedropper
Line
Rectangle
Ellipse
```

Conceptual flow:

```text
UI input
   ↓
Canvas coordinate conversion
   ↓
PaintEngine
   ↓
PaintLayer.pixels
```

The engine must not know about:

- QWidget
- mouse events
- QImage
- QPixmap
- Creature
- Simulation

---

# 11. PaintCanvas

`PaintCanvas` is a Qt UI component.

Responsibilities:

- receive mouse/stylus input
- map screen coordinates to document coordinates
- invoke painting operations
- display the current frame
- handle zoom
- display transparency checkerboard
- optionally display rulers/grid overlays

It must not own the authoritative pixel buffer.

The intended flow is:

```text
Mouse Input
    ↓
CanvasWidget
    ↓
Canvas Coordinates
    ↓
PaintEngine
    ↓
PaintDocument / PaintLayer
    ↓
CanvasCompositor
    ↓
QImage / QPixmap
    ↓
CanvasWidget
```

Document coordinates and screen coordinates are separate.

Zoom is display state, not document state.

---

# 12. Canvas Compositor

The compositor converts a layered frame into a flat RGBA representation.

```text
PaintFrame
   ↓
visible layers
   ↓
opacity
   ↓
blend modes
   ↓
flat RGBA buffer
```

The compositor should remain pure C++ and testable without Qt.

Qt conversion belongs at the UI/rendering boundary.

---

# 13. Layers

V1 layers are intentionally simple.

Supported operations:

- add
- delete
- rename
- duplicate
- move up
- move down
- hide/show
- lock/unlock
- opacity
- blend mode

Each animation frame owns its own layer stack.

```text
PaintDocument
└── Frames
    ├── Frame 1
    │   └── Layers
    ├── Frame 2
    │   └── Layers
    └── Frame 3
        └── Layers
```

---

# 14. Color System

The intended color system exposes multiple views of the same selected color.

UI concepts include:

```text
Color Field
Hue
Saturation
Lightness

RGB
Red
Green
Blue

Alpha
HEX

Current Color
Previous Color
Recent Colors
Palette
```

RGB and HSL are representations of the same color.

Changing one representation must update the other coherently.

The eyedropper should sample a canvas pixel and update the selected color.

---

# 15. Undo / Redo

Undo is document history, not merely mouse-event reversal.

The V1 approach may use snapshots.

Conceptually:

```text
User Action
    ↓
Document Change
    ↓
History Entry
```

Examples:

```text
Add layer
Draw stroke
Fill
Erase
Delete layer
```

A configurable history depth is appropriate for V1.

Important shortcuts:

```text
Ctrl+Z
Ctrl+Shift+Z
Ctrl+S
Ctrl+Shift+S
```

Do not introduce a complex command framework until the actual editor requires it.

---

# 16. Animation

Animation is frame-by-frame raster artwork.

Timeline responsibilities:

- add frame
- delete frame
- duplicate frame
- reorder frame
- select frame
- frame duration / FPS
- playback
- looping
- frame thumbnails

Playback timing is UI/application infrastructure.

The painting engine does not own Qt timers.

---

# 17. Animation Commit Boundary

Painting-session data must not directly modify a Creature.

The intended boundary is:

```text
PaintDocument
      ↓
AnimationCommitter / CommitAnimationUseCase
      ↓
Animation
      ↓
Creature.animations
```

The commit operation:

1. composites each paint frame
2. creates a domain `AnimationFrame`
3. assigns timing
4. creates the domain `Animation`
5. commits it to the Creature

This boundary prevents temporary editor state from leaking into the simulation domain.

---

# 18. State Machine

V1 uses a flat finite state machine.

Example:

```text
[Idle]
   │
   ├── enemy_in_range ──> [Attack]
   │
   └── enemy_seen ──────> [Movement]

[Attack]
   │
   └── attack_finished ─> [Idle]

[Movement]
   │
   └── enemy_in_range ──> [Attack]

[Hit]
   └── recovery_done ───> [Idle]

[Death]
```

Runtime components:

```text
StateMachineRunner
StateContext
TransitionEvaluator
```

Per simulation tick:

```text
1. Gather conditions.
2. Evaluate transitions.
3. Change state if a transition fires.
4. Reset time in state.
5. Otherwise advance state time.
6. Update animation selection.
```

The runtime evaluator must remain pure C++.

Do not introduce hierarchical FSMs, behavior trees, utility AI, scripting, or visual programming in V1.

---

# 19. Simulation Architecture

The simulation is a deterministic 2D fighting environment.

The simulation must remain Qt-free.

Conceptual structure:

```text
SimulationWorld
├── SimulationEntity[]
├── Arena
├── simulation clock
└── rules
```

Runtime entity:

```text
SimulationEntity
├── creatureId
├── position
├── health
├── currentStateId
├── animationFrame
└── facingDirection
```

---

# 20. Simulation Loop

The target is a fixed timestep:

```text
dt = 1 / 60 second
```

Conceptual tick:

```text
SimulationLoop::tick()

1. Gather world conditions.
2. Evaluate creature FSMs.
3. Resolve actions.
4. Resolve combat.
5. Apply movement / gravity / collision.
6. Update health and animation frame.
7. Check win/loss conditions.
8. Record the frame.
9. Advance tick.
```

A Qt timer may drive the simulation from the UI, but the simulation itself must not depend on QTimer.

---

# 21. Physics

V1 uses simple custom 2D physics.

Required concepts:

- gravity
- horizontal movement
- vertical movement
- floor collision
- platform collision
- arena bounds
- basic hit detection
- knockback
- simple collision resolution

Do not introduce a full physics engine unless real gameplay demonstrates that custom physics is insufficient.

---

# 22. Combat

`CombatResolver` should be stateless.

Conceptual input:

```text
CombatAction
+
current entities
```

Conceptual output:

```text
CombatResult
```

Possible results:

```text
damage
knockback
state change
hit confirmation
death
```

This keeps combat rules independently testable.

---

# 23. Determinism

The simulation must be reproducible.

Rules:

- fixed timestep
- no `std::rand()`
- one controlled random engine
- fixed seed per simulation
- seed stored in recording data
- simulation logic does not depend on rendering timing
- UI timing does not decide game results

Same:

```text
Creature files
+
Arena
+
Rules
+
Random seed
```

should produce the same simulation result.

---

# 24. Simulation Recording

A recording is a result, not a Creature definition.

Conceptual model:

```text
Recording
├── id
├── creatureAId
├── creatureBId
├── arenaId
├── seed
├── frames
├── winnerEntityId
└── timestamp
```

A frame contains value snapshots:

```text
SimulationFrame
├── tick
└── entity snapshots
```

The recorder must not retain references into mutable simulation state.

---

# 25. Arena Architecture

The Arena Editor is responsible for:

```text
Arena
├── Background
├── Foreground
├── Platforms
├── Collision
├── Spawn points
├── Arena bounds
└── Decorations
```

Raster artwork and collision are separate:

```text
Paint artwork
      +
Collision geometry
      +
Spawn data
      +
Arena metadata
```

Collision information must not be baked into artwork.

---

# 26. Rendering

Rendering is a read-only consumer.

Conceptual components:

```text
CreatureRenderer
AnimationPlayer
SimulationViewport
StateGraphRenderer
```

Rendering may read:

```text
Creature
Animation
SimulationEntity
SimulationWorld
```

Rendering must not decide:

```text
who attacks
who wins
which transition fires
how damage works
```

Those decisions belong to domain/state-machine/simulation logic.

---

# 27. Persistence

V1 uses versioned JSON.

Goals:

- readable
- debuggable
- easy to inspect
- easy to migrate
- no unnecessary binary complexity

Every persisted file includes a version:

```json
{
  "version": 1
}
```

Loading should pass through migration before data becomes active.

Never silently reinterpret an old schema.

---

# 28. Validation

Validation is a domain concern and has one authoritative source of truth.

Conceptually:

```cpp
SimulationReadiness validateForSimulation(
    const Creature& creature);
```

Validation should occur:

1. while editing
2. after loading
3. before simulation

The UI displays validation results but does not implement validation rules.

Important V1 readiness rules include:

- at least one state exists
- initial state is valid
- state animation references resolve
- transition targets resolve
- Idle exists
- animation frames contain valid image data

---

# 29. UI Architecture

The application uses one `QMainWindow` with page switching through `QStackedWidget`.

```text
MainWindow
└── QStackedWidget
    ├── MainMenuPage
    ├── CreatureEditorPage
    ├── StateEditorPage
    ├── ArenaEditorPage
    └── SimulationPage
```

V1 does not require MDI or a complicated floating-window architecture.

---

# 30. UI Control Rules

Use controls according to semantic purpose:

```text
QPushButton
    major / explicit workflow action

QToolButton
    compact frequent editor action

QAction
    shared operation, menu, toolbar, shortcut

QCheckBox
    boolean state

QComboBox
    selection

QSlider
    continuous value

QSpinBox
    numeric value
```

Major actions should remain discoverable:

```text
Create Creature
Open
Save
Start Simulation
Create Arena
```

Frequent editor actions can be icon-first:

```text
Pencil
Brush
Eraser
Fill
Eyedropper
Undo
Redo
Delete
Move Up
Move Down
```

Icon-only controls need:

- tooltip
- meaningful semantic action text
- accessible naming where appropriate

If the same operation appears in a menu, toolbar, and shortcut, prefer one `QAction`.

---

# 31. Icon and Asset Rules

Application-owned icons should be packaged through Qt's resource system.

Preferred hierarchy:

```text
Qt/system theme icon
        ↓
bundled SVG application icon
        ↓
open-source SVG collection
        ↓
additional icon framework only if justified
```

Runtime icons must never require an internet connection.

SVG is preferred for application-owned icons where practical.

Do not mix unrelated icon styles without a deliberate reason.

Icons represent actions or state; they are not decoration.

---

# 32. Persistence Boundaries

Persistence translates between durable file representations and application/domain objects.

It should not become the owner of gameplay behavior.

Conceptually:

```text
Application / Domain
        ↓
Repository / Serializer
        ↓
JSON
        ↓
File
```

Schema migration belongs to persistence.

Domain validation remains domain-owned.

---

# 33. CMake Module Boundaries

Each major module has an explicit CMake target.

Expected module structure:

```text
core
domain
application
painting
statemachine
simulation
rendering
persistence
ui
app
tests
```

Dependencies should be visible in CMake.

Avoid hiding cross-module dependencies through global include paths or accidental transitive dependencies.

---

# 34. Testing Architecture

Important logic should be testable without `QApplication` whenever possible.

Test areas:

### Core

- vector operations
- rectangle operations
- color behavior

### Domain

- creature invariants
- animation validity
- state validation
- simulation readiness

### Painting

- brush/pencil operations
- eraser
- fill
- shape tools
- color sampling
- compositing
- opacity
- blend modes
- history

### State Machine

- initial state
- transition matching
- invalid targets
- unknown conditions
- state timing
- transition reset

### Simulation

- movement
- collision
- combat
- damage
- knockback
- death
- determinism
- recording

### Persistence

- serialization
- deserialization
- schema version
- migration

---

# 35. Coding Conventions

Use:

```text
Classes / structs      PascalCase
Methods                camelCase
Members                m_memberName
Constants              k_constantName
Enums                  PascalCase
Files                  snake_case.hpp / snake_case.cpp
Namespaces             creature_studio::...
```

Ownership:

- prefer value semantics
- use `std::unique_ptr` for exclusive ownership
- use `std::shared_ptr` only for genuine shared ownership
- avoid owning raw pointers
- Qt parent ownership is acceptable in UI code

Prefer explicit `const` correctness.

Do not use exceptions in Core, Domain, or Simulation unless a concrete infrastructure requirement justifies them.

---

# 36. Architectural Rules

These are long-term rules and should not be casually broken.

1. Domain and Core never include Qt.
2. Simulation logic never depends on Qt.
3. Painting engine never depends on Qt.
4. Canvas widgets never own authoritative pixel data.
5. PaintDocument never references Creature or simulation state.
6. Creature definitions are separate from runtime SimulationEntity state.
7. Rendering reads state; it does not modify authoritative state.
8. UI does not implement domain or simulation rules.
9. Validation has one domain-level source of truth.
10. Animation commit is the path from PaintDocument into Creature animations.
11. Frames are composed through the compositor rather than duplicated into UI state.
12. Zoom is display state, not document state.
13. Every persisted file has a version.
14. Every schema change requires explicit migration.
15. Simulation uses a fixed timestep.
16. Randomness uses a controlled seedable engine.
17. Recordings contain snapshots, not references to mutable runtime objects.
18. There is no global singleton for application state.
19. ProjectContext is the shared mutable application context.
20. Do not introduce abstractions without a current concrete reason.
21. Do not solve post-MVP problems inside MVP architecture unless the boundary is required.
22. Every module has an explicit CMake target.
23. Core/domain/painting/state-machine/simulation tests should run without QApplication whenever possible.
24. File formats are contracts and should not be changed casually.
25. The application remains usable offline.

---

# 37. Development Order

The architecture supports this intended sequence.

## Phase 1 — Foundation

- CMake
- presets
- Qt
- vcpkg
- formatting
- static analysis
- tests
- Core
- UI shell

## Phase 2 — Paint Foundation

- PaintDocument
- PaintFrame
- PaintLayer
- pixel buffer
- PaintEngine
- CanvasCompositor
- CanvasWidget
- basic tools
- color system

## Phase 3 — Paint Editing

- layers
- opacity
- blend modes
- undo/redo
- zoom
- additional tools
- editor controls

## Phase 4 — Animation

- timeline
- frame management
- FPS
- looping
- playback
- animation commit

## Phase 5 — Creature Authoring

- creature metadata
- animation assignment
- combat stats
- states
- state graph
- validation
- `.jcreature`

## Phase 6 — Arena

- arena data
- artwork
- collision
- spawn points
- bounds
- `.jarena`

## Phase 7 — Simulation

- SimulationWorld
- SimulationEntity
- fixed timestep
- movement
- physics
- combat
- FSM runtime integration
- deterministic randomness

## Phase 8 — Recording / Export

- simulation recordings
- replay
- export
- future video output

---

# 38. Current Product Priority

The immediate product milestone is:

```text
Create Creature
      ↓
Painter
      ↓
Draw
      ↓
Animate
      ↓
Commit Animation
      ↓
Creature Editor
      ↓
State Editor
      ↓
Validate
```

Only after this authoring pipeline is solid should the project move aggressively into:

```text
Arena
  ↓
Simulation
  ↓
Recording
  ↓
Export
```

This keeps the project focused and avoids accumulating unfinished subsystems.

---

# 39. Complexity Policy

A feature should not automatically create a new abstraction.

Before introducing a manager, service, framework, event bus, plugin system, or inheritance hierarchy, ask:

```text
Does this solve a current problem?
Does an existing module already own this responsibility?
Can a struct, enum, vector, or function solve it?
Does this introduce unnecessary coupling?
Will this make the project harder to understand?
```

The preferred architecture is:

```text
simple
  ↓
explicit
  ↓
testable
  ↓
maintainable
```

not:

```text
abstract
  ↓
framework-heavy
  ↓
indirect
  ↓
hard to understand
```

---

# 40. Final Architectural Principle

Janky Studio should grow by adding capabilities inside clear boundaries.

Good growth:

```text
painting
    ↓
better painting

statemachine
    ↓
better behavior

simulation
    ↓
better combat

arena
    ↓
better environments
```

Bad growth:

```text
feature
 ↓
new manager
 ↓
new service
 ↓
new event bus
 ↓
new framework
```

The architecture exists to support the product, not to become the product.

> Keep the editor simple, keep the simulation deterministic, and keep the boundaries obvious.
