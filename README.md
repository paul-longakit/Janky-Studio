# Janky Studio

**Janky Studio** is an offline C++/Qt desktop application for creating deliberately simple creatures, animating them, defining their behavior, building arenas, and eventually putting those creatures into deterministic 2D fights.

> **Professional desktop workspace organization + MS Paint simplicity + intentionally janky creature creation.**

Janky Studio is intentionally **not Photoshop, not a general game engine, and not an AI framework**.

The goal is a focused authoring tool that makes it easy to go from a terrible little drawing to a creature that can actually fight.

---

## Project Status

Janky Studio is currently in active development.

The architectural foundation is established, with working Core and Domain foundations, a functional state-machine system, automated tests, the Qt application shell, and the beginnings of the painting/editor workflow.

The current development priority is the **authoring pipeline**:

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

The Arena, Simulation, Recording, and Export portions of the product will follow after the authoring pipeline is solid.

---

# Vision

The intended Janky Studio workflow is:

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
Record / Export Result
```

The final product consists of four major tools:

### Creature Painter / Animator

A simple raster drawing workspace for creating creature artwork and frame-by-frame animation.

### Creature Editor

Turns artwork into a game-ready creature by defining animations, metadata, combat properties, states, and behavior.

### Arena Editor

Creates the environment in which creatures fight, including artwork, platforms, collision, spawn points, and arena data.

### Simulator

Loads creatures and arenas and runs deterministic 2D fights.

---

# Technology

| Area                | Technology                  |
| ------------------- | --------------------------- |
| Language            | C++23                       |
| UI                  | Qt 6 Widgets                |
| Build               | CMake 3.28+                 |
| Build Configuration | CMake Presets               |
| Dependencies        | vcpkg manifest mode         |
| Drawing             | QPainter / QImage / QPixmap |
| Simulation          | Pure C++                    |
| Physics             | Custom simple 2D physics    |
| Persistence         | JSON / QJsonDocument        |
| Testing             | Catch2 v3                   |
| Formatting          | clang-format                |
| Static Analysis     | clang-tidy / cppcheck       |
| Video Export        | ffmpeg subprocess, planned  |
| Platforms           | Windows / Linux first       |

## Technology Philosophy

Prefer simple, explicit technology over unnecessary complexity.

In particular:

* Use Qt Widgets rather than introducing another UI framework.
* Use `QPainter` before introducing OpenGL or another rendering system.
* Use simple custom simulation rules before introducing a physics engine.
* Use JSON while the project is developing.
* Keep domain models as plain C++ types where possible.
* Prefer small explicit modules over frameworks.
* Prefer value semantics and direct ownership.
* Do not add dependencies merely because they are popular.

---

# Architecture

Janky Studio follows a layered architecture:

```text
┌─────────────────────────────────────────────┐
│ UI                                          │
│ Qt Widgets, pages, panels, dialogs, input  │
├─────────────────────────────────────────────┤
│ Application                                 │
│ User-facing use cases and orchestration    │
├─────────────────────────────────────────────┤
│ Feature / Domain                            │
│ Painting / State Machine / Creature rules  │
├─────────────────────────────────────────────┤
│ Core                                        │
│ Shared, Qt-free primitives                  │
└─────────────────────────────────────────────┘
```

Additional infrastructure surrounds these layers:

```text
Persistence
    ↓
stores / loads application data

Rendering
    ↓
reads application state and presents it
```

The dependency direction is:

```text
UI
 ↓
Application
 ↓
Feature / Domain
 ↓
Core
```

The architecture deliberately keeps Core and Domain independent of Qt. Painting and simulation logic should also remain Qt-free where possible.

---

# Core Architecture Rules

These rules are the most important rules in the repository.

## 1. Core stays Qt-free

`core/` contains shared primitives.

Examples:

```text
Vec2
Rect
Color
UniqueId
Result
```

Core must not depend on:

* Qt
* UI
* Creature-specific rules
* Painting
* Simulation
* File formats

Do not turn Core into a dumping ground.

---

## 2. Domain stays Qt-free

The Domain describes what the application means.

Examples:

```text
Creature
Animation
AnimationFrame
State
StateTransition
StateMachine
Validation
```

Domain code should not know about:

* QWidget
* QPainter
* QImage
* QPixmap
* menus
* buttons
* file dialogs

The UI presents domain state; it does not define domain rules.

---

## 3. UI does not own authoritative state

Widgets are responsible for:

* user interaction
* presentation
* input
* triggering operations
* refreshing their display

They should not become the source of truth.

The intended flow is:

```text
User
 ↓
Qt Widget / QAction
 ↓
Application
 ↓
Domain / Feature Engine
 ↓
State Change
 ↓
UI Refresh
```

Not:

```text
Button
 ↓
50 lines of business logic
 ↓
directly mutate unrelated objects
```

---

## 4. One authoritative Creature

Creature-related editors must work with the same authoritative `Creature`.

The intended relationship is:

```text
              ProjectContext
                    │
                 Creature
                /        \
               /          \
      Creature Editor   State Editor
```

Do not create independent copies of the Creature simply because two pages need access to it.

A thin `ProjectContext` is preferred over a global singleton.

---

## 5. Rendering does not decide behavior

Rendering is a read-only consumer of state.

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
which state transition fires
how damage works
```

Those decisions belong to the appropriate domain, feature, or simulation logic.

---

## 6. Painting logic stays separate from Qt

The painting engine should remain pure C++.

The conceptual flow is:

```text
Mouse / Stylus Input
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

`PaintCanvas` is a UI component.

It is not the owner of the actual painting data.

The raster pixel buffer is the source of truth.

---

## 7. Simulation stays Qt-free

The simulation must not depend on:

```text
QTimer
QWidget
QPainter
Qt event timing
```

The UI may drive the simulation, but the simulation itself must remain independent of the UI.

The intended simulation uses a fixed timestep:

```text
dt = 1 / 60 second
```

This supports deterministic results.

---

## 8. Prefer the smallest correct abstraction

Before creating a new abstraction, ask:

```text
Does this solve a current problem?

Does an existing module already own this responsibility?

Can a struct, enum, vector, or function solve it?

Does this introduce unnecessary complexity?

Will this make the project harder to understand?
```

Prefer:

```text
struct
enum
function
small class
std::vector
std::optional
```

over introducing:

```text
framework
plugin system
generic manager
event bus
service hierarchy
unnecessary inheritance
```

Do not build architecture for imaginary problems.

---

# Module Responsibilities

| Module         | Responsibility                    | Must Not Own           |
| -------------- | --------------------------------- | ---------------------- |
| `core`         | Shared primitives                 | Qt, domain rules       |
| `domain`       | Creature definitions and rules    | Qt, file I/O           |
| `application`  | Use cases and orchestration       | Widget implementation  |
| `painting`     | Raster editing logic              | Qt UI, simulation      |
| `statemachine` | FSM execution                     | UI, rendering          |
| `simulation`   | Runtime world and combat          | UI, persistence format |
| `rendering`    | Visual presentation               | Authoritative state    |
| `persistence`  | Files, JSON, migration            | Gameplay rules         |
| `ui`           | User interaction and presentation | Domain rules           |

---

# Project Structure

The repository is organized by responsibility rather than by UI screen:

```text
Janky Studio/
│
├── app/
│   └── creature_studio/
│
├── application/
│
├── core/
│
├── domain/
│
├── painting/
│
├── statemachine/
│
├── simulation/
│
├── rendering/
│
├── persistence/
│
├── ui/
│
├── tests/
│
├── docs/
│
├── assets/
│
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── .clang-format
└── .clang-tidy
```

The current repository is still being built out, so some modules contain only their initial CMake or structural foundations. The current snapshot shows Core, Domain, Painting, State Machine, UI, and tests as the primary implemented areas.

---

# Main UI Structure

Janky Studio uses a single `QMainWindow` with page switching through `QStackedWidget`.

```text
MainWindow
└── QStackedWidget
    ├── MainMenuPage
    ├── CreatureEditorPage
    ├── StateEditorPage
    ├── ArenaEditorPage
    └── SimulationPage
```

V1 does not use an MDI architecture or a complicated floating-window system.

---

# Painting Philosophy

The Painter is a **raster/freehand drawing application**, not a forced pixel-grid editor.

The artwork is represented internally as raster pixels, but pixels are implementation details rather than the artistic unit.

The intended experience is:

```text
Large Raster Canvas
        ↓
Freehand Drawing
        ↓
RGBA Layers
        ↓
Animation Frames
```

The Painter should provide simple tools such as:

```text
Pencil
Brush
Eraser
Fill
Eyedropper
Line
Rectangle
Ellipse
Selection
```

along with:

```text
Layers
Undo / Redo
Colors
Zoom
Timeline
Animation Playback
```

The goal is:

> **Photoshop's workspace organization + MS Paint's simplicity + Janky Studio's personality.**

---

# Animation

Animation is frame-by-frame raster artwork.

The intended workflow is:

```text
Draw Frame 1
      ↓
Duplicate / Create Frame
      ↓
Modify Frame
      ↓
Repeat
      ↓
Timeline
      ↓
Playback
      ↓
Commit Animation
```

Animation timing is part of the application/animation system, not the painting engine.

When painting data becomes a domain animation, it crosses an explicit commit boundary:

```text
PaintDocument
      ↓
Animation Commit
      ↓
Domain Animation
      ↓
Creature
```

This prevents temporary editor state from leaking directly into the creature definition.

---

# Creature Model

A Creature is an authored definition.

Conceptually:

```text
Creature
├── ID
├── Name
├── Animations
├── State Machine
└── Metadata
```

A creature definition is different from a runtime simulation entity.

```text
Creature
    = reusable authored definition

SimulationEntity
    = runtime instance
```

Runtime state such as health, position, current state, animation frame, and facing direction belongs to the simulation entity rather than the creature definition.

---

# State Machine

V1 uses a **flat finite state machine**.

Typical state types include:

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

Conceptually:

```text
[Idle]
   │
   ├── enemy_in_range ──> [Attack]
   │
   └── enemy_seen ──────> [Movement]

[Attack]
   │
   └── attack_finished ─> [Idle]
```

Do not introduce hierarchical state machines, behavior trees, utility AI, scripting systems, or visual programming languages unless the existing simple FSM proves insufficient.

---

# Simulation Philosophy

The Simulator is intentionally simple.

V1 targets:

```text
Creature A
     vs
Creature B
     ↓
   Arena
     ↓
Fixed timestep
     ↓
Combat
     ↓
Winner
```

The simulation should implement only the mechanics required for the prototype:

* gravity
* horizontal movement
* vertical movement
* floor/platform collision
* arena bounds
* basic hit detection
* damage
* knockback
* death
* win/loss conditions

A full physics engine is not part of the initial design.

---

# Determinism

Simulation results should be reproducible.

The simulation uses:

* fixed timestep
* controlled random generation
* explicit random seed
* simulation-independent rendering timing

Given the same:

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

# Persistence

The intended persistence format is versioned JSON.

Planned file types:

```text
.jcreature
.jarena
.jrecording
```

Persisted data should include a schema version:

```json
{
  "version": 1
}
```

Schema changes should use explicit migrations.

Old files should never be silently reinterpreted.

---

# Testing

Important logic should be testable without Qt whenever possible.

Tests are organized by module:

```text
tests/
├── core/
├── domain/
├── painting/
├── statemachine/
├── simulation/
└── persistence/
```

The project currently has tests covering areas including:

* Creature validation
* Painting
* Paint documents
* Animation frame conversion
* State-machine execution

The development workflow expects builds and tests to remain green after meaningful changes.

---

# Coding Conventions

Use modern C++ consistently.

```text
Classes / structs      PascalCase
Methods                camelCase
Members                m_memberName
Constants              k_constantName
Enums                  PascalCase
Files                  snake_case.hpp / snake_case.cpp
Namespaces             creature_studio::...
```

Prefer:

* value semantics
* `std::unique_ptr` for exclusive ownership
* `std::shared_ptr` only for genuine shared ownership
* `std::optional`
* `std::variant`
* `std::vector`
* explicit `const` correctness

Avoid owning raw pointers.

Qt parent ownership is acceptable inside Qt UI code.

Core, Domain, and simulation logic should not use exceptions unless a concrete infrastructure requirement justifies them.

---

# UI Control Standard

UI controls are chosen according to their purpose.

### `QPushButton`

Use for major or explicit workflow actions:

```text
Create Creature
Open
Save
Start Simulation
Create Arena
Cancel
Apply
```

### `QToolButton`

Use for compact, frequent editor operations:

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

### `QAction`

Use when the same operation appears in multiple places.

For example:

```text
Save
 ├── File menu
 ├── Toolbar
 └── Ctrl+S
```

These should preferably share one logical action.

### Other controls

Use the Qt control appropriate to the value:

```text
QCheckBox  → Boolean state
QComboBox  → Selection
QSlider    → Continuous value
QSpinBox   → Numeric value
```

Icon-only controls should provide a meaningful tooltip and semantic action text.

The UI standard exists to keep the application understandable rather than simply maximizing icon usage.

---

# UI Development Rule

New UI features should normally progress in this order:

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

Do not spend significant time polishing unfinished functionality.

Major workflow actions should remain discoverable through text when necessary.

---

# Development Workflow

Janky Studio is developed incrementally.

The preferred loop is:

```text
Understand current code
        ↓
Identify owning module
        ↓
Make smallest sound change
        ↓
Build
        ↓
Test
        ↓
Run / verify UI
        ↓
Commit
```

## Before changing code

Ask:

1. Which module owns this responsibility?
2. Does the functionality already exist?
3. Is the requested behavior already partially implemented?
4. Does the change cross an architectural boundary?
5. Can the existing architecture support it without modification?

Avoid redesigning working code without a concrete reason.

---

# Build

From the repository root:

```bash
cmake --build build -j$(nproc)
```

For a clean build:

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j$(nproc)
```

---

# Tests

Run:

```bash
ctest --test-dir build --output-on-failure
```

A meaningful change should normally finish with:

```text
Build successful
Tests passing
```

If a change affects a specific module, run the relevant tests during development as appropriate.

---

# Running Janky Studio

The current application executable is:

```bash
./build/app/creature_studio/janky_studio
```

---

# Code Quality

When appropriate, use:

```text
clang-format
clang-tidy
cppcheck
```

Formatting and static analysis should support the architecture rather than become a source of unnecessary ceremony.

Do not perform broad unrelated formatting changes while implementing a feature.

---

# Collaboration Guidelines

Janky Studio is intentionally being developed as a small, understandable codebase.

Contributions should preserve that quality.

## 1. Work within the existing architecture

Before adding a new module or abstraction, determine whether an existing module already owns the responsibility.

Prefer:

```text
existing boundary
+
small extension
```

over:

```text
new manager
+
new service
+
new abstraction
+
new dependency
```

---

## 2. Keep changes focused

A pull request should ideally answer one question:

> **What specific problem or feature does this change address?**

Avoid mixing:

```text
feature implementation
+
unrelated refactor
+
UI redesign
+
formatting the entire project
```

unless there is a concrete reason.

---

## 3. Do not break architectural boundaries for convenience

For example, do not move painting logic into `PaintCanvas` simply because it is easier.

Do not move domain validation into a widget.

Do not make simulation logic depend on Qt.

Do not let rendering become the owner of game state.

Fix the problem at the appropriate layer.

---

## 4. Verify before committing

Before committing a meaningful change:

```bash
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

For larger build-system changes, use a clean configuration/build when appropriate.

---

## 5. Keep commits understandable

Prefer focused commits such as:

```text
Implement state machine transition evaluation
Add creature validation tests
Add paint document frame support
Add brush painting to canvas
Add layer controls
```

Avoid commits such as:

```text
stuff
changes
fix
more stuff
```

A commit should communicate what changed.

---

# Pull Requests

A useful pull request should include:

### What changed?

Briefly describe the feature or fix.

### Why?

Explain the concrete problem being solved.

### Architecture

Mention any relevant ownership or dependency changes.

### Verification

Include the build/test result.

Example:

```text
Build:
cmake --build build -j$(nproc)

Tests:
ctest --test-dir build --output-on-failure

Result:
25/25 tests passed
```

### UI changes

If the change affects the UI, describe what was manually verified.

---

# AI-Assisted Development

AI tools may be used during development, but they should work from the **actual current repository state**.

The preferred workflow is:

```text
Architecture
     ↓
Current code
     ↓
Current task
     ↓
Small implementation
     ↓
Build
     ↓
Test
     ↓
Verify
```

AI-generated changes must follow the same architectural rules as manually written code.

Do not accept a suggested abstraction merely because it sounds architecturally sophisticated.

The key questions remain:

```text
Does this solve a current problem?

Does it belong in an existing module?

Does it preserve dependency direction?

Does it introduce unnecessary complexity?

Does it work with the current code?
```

The project maintains `docs/ai_context.md` for concise development context rather than repeatedly supplying the entire architecture to an AI assistant.

---

# Development Priorities

The project is intentionally being developed in stages.

## Current priority

### Authoring Pipeline

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

## After that

```text
Arena
   ↓
Simulation
   ↓
Recording
   ↓
Export
```

Do not jump ahead simply because a later system is more exciting.

A complete, usable authoring pipeline is more valuable than five unfinished subsystems.

---

# MVP Scope

The V1 product is intended to contain:

### Application

* Main menu
* Page navigation
* Project/file open/save

### Painter

* Raster canvas
* Pencil
* Brush
* Eraser
* Fill
* Eyedropper
* Line
* Rectangle
* Ellipse
* Color system
* Layers
* Undo/redo
* Zoom
* Animation frames
* Timeline
* Playback

### Creature Editor

* Creature name
* Animation assignment
* Basic combat stats
* State list
* State graph
* Animation-to-state assignment
* Validation

### Simulation

* Two creatures
* Simple arena
* Gravity
* Platform collision
* Movement
* Basic attacks
* Damage
* Knockback
* Death
* Flat FSM execution
* Fixed timestep
* Deterministic random seed
* Basic recording

### Persistence

* `.jcreature`
* `.jarena`
* `.jrecording`
* JSON
* Version field
* Migration system

---

# What Janky Studio Is Not

Janky Studio deliberately avoids becoming:

* Photoshop
* Unity
* Godot
* a general-purpose game engine
* an AI framework
* a visual programming environment
* a complex physics sandbox
* an enterprise architecture exercise

If a feature does not help the core workflow, it should be questioned before being added.

---

# Design Principle

The project should remain understandable to one developer opening it months later.

That means:

```text
Simple
   ↓
Explicit
   ↓
Testable
   ↓
Maintainable
```

rather than:

```text
Complex
   ↓
Abstract
   ↓
Framework-heavy
   ↓
Hard to understand
```

The project should grow by adding capabilities inside existing boundaries.

Good:

```text
painting
    ↓
better painting tools

statemachine
    ↓
better state behavior

simulation
    ↓
better combat

arena
    ↓
better environments
```

Bad:

```text
new feature
    ↓
new manager
    ↓
new service
    ↓
new event bus
    ↓
new framework
```

---

# Documentation

Additional project documentation belongs under:

```text
docs/
```

Current/planned documentation includes:

```text
docs/
├── architecture.md
├── file_formats.md
└── ai_context.md
```

The README provides the project's orientation and contribution rules.

Detailed architectural specifications should remain in the dedicated architecture documentation rather than duplicating the entire specification here.

---

# License

> **License: TBD**

A project license should be added before the repository is distributed publicly or accepts external contributions.

---

# The North Star

Janky Studio exists to make this possible:

```text
        DRAW SOMETHING STUPID
                 │
                 ▼
             ANIMATE IT
                 │
                 ▼
          MAKE IT A CREATURE
                 │
                 ▼
          GIVE IT BEHAVIOR
                 │
                 ▼
          BUILD AN ARENA
                 │
                 ▼
       LET THE CREATURES FIGHT
                 │
                 ▼
            RECORD IT
```

**Draw → Animate → Create → Fight.**

Keep it simple.

Keep it understandable.

Keep it janky.
