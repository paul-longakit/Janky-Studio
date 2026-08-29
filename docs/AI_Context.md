# Janky Studio — AI Context

## Purpose

Janky Studio is an offline C++/Qt desktop application for creating deliberately simple, "janky" creatures, animating them, defining their behavior, creating arenas, and eventually running deterministic 2D fights.

The product goal is:

> Professional desktop workspace organization + MS Paint simplicity + intentionally janky creature creation.

The intended workflow is:

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

Janky Studio is not intended to become Photoshop, a general-purpose game engine, or an AI framework.

---

## Technology

- C++23
- Qt 6 Widgets
- CMake 3.28+
- CMake Presets
- vcpkg manifest mode
- QPainter / QImage / QPixmap for UI-side raster presentation
- Catch2 v3 for tests
- clang-format
- clang-tidy
- cppcheck
- JSON / QJsonDocument for V1 persistence
- ffmpeg subprocess for future video export
- Windows/Linux first
- Offline-first application

---

## Architecture

The fundamental dependency direction is:

```text
UI
 ↓
Application
 ↓
Feature / Domain
 ↓
Core
```

Supporting infrastructure:

```text
Persistence
    ↕
Application / Domain contracts

Rendering
    ↓
reads state
```

Core and Domain must remain Qt-free.

Painting and simulation logic should remain Qt-free where possible.

UI is responsible for interaction and presentation, not authoritative business state.

---

## Major Modules

### `core/`

Shared primitives only.

Examples:

```text
Vec2
Rect
Color
UniqueId
Result
```

Rules:

- No Qt.
- No UI.
- No file I/O.
- No domain-specific behavior.
- Do not turn Core into a dumping ground.

### `domain/`

Authoritative definitions and domain validation.

Examples:

```text
Creature
Animation
AnimationFrame
State
StateTransition
StateMachine
SimulationReadiness
ValidationError
CreatureValidator
```

Domain code must not depend on Qt.

### `application/`

User-facing use cases and orchestration.

Examples:

```text
CreateCreatureUseCase
SaveCreatureUseCase
LoadCreatureUseCase
CommitAnimationUseCase
ValidateCreatureUseCase
StartSimulationUseCase
```

Application code coordinates modules but does not implement widgets.

### `painting/`

Pure painting/editor-session logic.

Examples:

```text
PaintDocument
PaintFrame
PaintLayer
Pixel
Brush
BrushSettings
AnimationFrameConverter
```

The painting system works on raster pixel data.

It does not know about QWidget, QPainter, QImage, QPixmap, Creature, or Simulation.

### `statemachine/`

Runtime execution of the flat creature state machine.

Examples:

```text
StateContext
StateMachineRunner
TransitionEvaluator
```

This module is pure C++.

### `simulation/`

Future deterministic runtime simulation.

It must remain independent of Qt.

Planned responsibilities include:

- simulation world
- runtime entities
- fixed timestep
- movement
- gravity
- collision
- combat
- damage
- knockback
- death
- deterministic randomness
- recording

### `rendering/`

Read-only presentation of state.

Rendering may read simulation/domain state but must not own or modify authoritative state.

### `persistence/`

Versioned JSON storage, repositories, and schema migrations.

Planned formats:

```text
.jcreature
.jarena
.jrecording
```

### `ui/`

Qt Widgets and presentation.

Current page structure:

```text
MainWindow
└── QStackedWidget
    ├── MainMenuPage
    ├── CreatureEditorPage
    ├── StateEditorPage
    ├── ArenaEditorPage
    └── SimulationPage
```

---

## Authoritative State

There must be one authoritative Creature for the current editing session.

Use a thin `ProjectContext` rather than a global singleton.

Conceptually:

```text
ProjectContext
├── current creature
├── current arena
├── current file paths
└── necessary editor/session references
```

Do not make `CreatureEditorPage` and `StateEditorPage` edit separate Creature objects.

The UI displays and edits shared application state; it does not own the source of truth.

---

## Painting Rules

The Painter is a normal raster/freehand editor, not a forced logical pixel-grid editor.

The conceptual pipeline is:

```text
Mouse / Stylus
    ↓
CanvasWidget
    ↓
Canvas Coordinates
    ↓
Paint Engine
    ↓
PaintDocument / PaintLayer
    ↓
Canvas Compositor
    ↓
QImage / QPixmap
    ↓
CanvasWidget
```

The pixel buffer is the source of truth.

`QImage` and `QPixmap` are display representations.

`PaintCanvas` must not own authoritative pixel data.

Zoom is display state, not document state.

---

## Animation Boundary

Painting-session data must not directly mutate `Creature`.

The intended path is:

```text
PaintDocument
      ↓
Animation Commit
      ↓
Domain Animation
      ↓
Creature
```

The commit step composites paint frames into domain animation frames.

This is the only intended path from PaintDocument into Creature animations.

---

## State Machine

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

Do not introduce hierarchical FSMs, behavior trees, utility AI, scripting, or visual programming unless the simple FSM proves insufficient.

---

## Simulation Rules

Simulation must be deterministic.

Rules:

- fixed timestep, target `1 / 60` second
- controlled seedable random engine
- no `std::rand()`
- rendering timing must not affect game results
- UI timing must not determine simulation behavior
- recordings contain value snapshots rather than references to mutable runtime objects

Given identical:

```text
Creature files
+
Arena
+
Rules
+
Random seed
```

the simulation should produce the same result.

---

## UI Rules

Choose Qt controls by semantic purpose.

```text
QPushButton → major / explicit workflow action
QToolButton → compact frequent editor action
QAction     → shared operation/menu/toolbar/shortcut
QCheckBox   → boolean state
QComboBox   → selection
QSlider     → continuous value
QSpinBox    → numeric value
```

Examples:

```text
[ Create Creature ]   → QPushButton
[ Save ]              → QPushButton or shared QAction
[ Pencil ]            → QToolButton
[ Undo ]              → QToolButton / QAction
[ Start Simulation ]  → text-bearing control
```

Icon-only controls need meaningful tooltips and semantic action text.

Do not over-iconize the UI.

UI refinement order:

```text
Functional control
    ↓
Correct Qt control
    ↓
Correct behavior
    ↓
Icon / tooltip
    ↓
Visual polish
```

---

## Testing

Important logic should be testable without QApplication whenever possible.

Relevant test areas:

```text
core
domain
painting
statemachine
simulation
persistence
```

After meaningful changes:

```bash
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

Clean build when appropriate:

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

Current application executable:

```bash
./build/app/creature_studio/janky_studio
```

---

## Current Implementation Status

As of the current project checkpoint:

### Working / established

- Core module and shared primitives
- Domain creature/state/animation foundations
- Creature validation
- Flat state-machine runtime
- State-machine tests
- Painting data model
- Brush/painting foundations
- Animation frame conversion foundations
- Qt application shell
- Main page navigation
- Creature Editor / State Editor page structure
- Paint Canvas integration
- Automated tests across the established modules

The latest development phase is focused on the real authoring pipeline.

### Current priority

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

### Not yet the immediate priority

```text
Arena
Simulation
Recording
Export
```

Do not jump to these systems merely because they are part of the final product.

---

## Development Method

### Before changing code

1. Inspect the current implementation.
2. Identify the owning module.
3. Check whether the functionality already exists.
4. Identify the smallest change that solves the task.
5. Check whether the change crosses an architectural boundary.

### Implement

1. Make the smallest sound change.
2. Keep responsibilities in the correct layer.
3. Avoid unrelated refactors.
4. Avoid new dependencies unless justified.
5. Avoid new abstractions unless there is a current concrete problem.

### Verify

1. Build.
2. Run relevant tests.
3. Run the application when UI behavior is affected.
4. Inspect the result.
5. Update this context document when a meaningful architectural or workflow decision changes.

---

## AI Rules

AI assistants working on Janky Studio must:

- Work from the actual current repository state.
- Treat the architecture document as the target architecture.
- Treat the source tree and latest verified checkpoint as current reality.
- Never assume an unimplemented feature exists.
- Never reimplement a feature that already exists.
- Preserve established module boundaries.
- Prefer small, directly executable changes.
- Explain the owning files before making multi-file changes.
- Build and test after meaningful changes.
- Diagnose actual build/test output before proposing redesigns.
- Do not invent architectural problems when the current implementation works.
- Do not replace simple working code with abstractions merely for theoretical extensibility.

When asked to implement a feature, provide:

```text
1. What changes
2. Why
3. Exact files
4. Exact code
5. Build/test commands
6. Expected result
```

---

## Do Not Do This

Do not:

- add a global singleton for application state
- put Qt into Core or Domain
- put painting logic into PaintCanvas
- put simulation logic into Qt widgets
- make rendering authoritative
- couple PaintDocument to Creature
- bake collision into arena artwork
- add a physics engine before custom V1 physics proves insufficient
- add a generic service/event/plugin framework without a concrete need
- introduce advanced AI architecture before the flat FSM proves insufficient
- redesign working architecture without a concrete problem
- polish unfinished UI at the expense of functional progress

---

## Current Task

Update this section when starting a focused development task.

```text
Phase:
Module:
Current file(s):
Task:
Expected behavior:
Constraints:
Tests to run:
```

Keep the task narrow enough that an AI can work on it without reconstructing the entire project.

---

## Important Architectural Principle

> Build the smallest correct thing that preserves the architecture and moves the actual product forward.

The project should grow inside its existing boundaries rather than accumulating layers of abstraction.

The North Star remains:

```text
DRAW
 ↓
ANIMATE
 ↓
CREATE CREATURE
 ↓
DEFINE BEHAVIOR
 ↓
BUILD ARENA
 ↓
FIGHT
 ↓
RECORD
```
