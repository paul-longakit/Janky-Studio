# Janky Creature Studio — Clean Architecture & Development Blueprint

## 1. Project Vision

**Janky Creature Studio** is an offline C++/Qt desktop application for creating small, deliberately simple creatures and putting them into a 2D fighting simulation.

The application is composed of four major tools:

1. **Creature Painter / Animator** — draw raster artwork and create frame-by-frame animations.
2. **Creature Editor** — turn artwork into a game-ready creature by defining animations, stats, states, and behavior.
3. **Arena Editor** — create the fighting environment, artwork, platforms, collision, spawn points, and other arena data.
4. **Simulator** — load creatures and arenas and run deterministic 2D fights.

The core workflow is:

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

The application is intentionally **not Photoshop, not a general game engine, and not an AI framework**.

The goal is a focused authoring tool:

> **Simple graphics tools + simple creature authoring + simple deterministic fighting simulation.**

---

# 2. Product Structure

```text
                         JANKY STUDIO
                              │
                    ┌─────────┴─────────┐
                    │      MAIN MENU    │
                    └─────────┬─────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
   CREATURE                ARENA                 OTHER
   CREATOR                 EDITOR                TOOLS
        │                     │
        │                     │
        └──────────────┬──────┘
                       ▼
                  SIMULATOR
                       │
                       ▼
                 FIGHT CREATURES
```

The main window uses a single `QMainWindow` with page switching through `QStackedWidget`.

```text
MainWindow
└── QStackedWidget
    ├── MainMenuPage
    ├── CreatureEditorPage
    ├── StateEditorPage
    ├── ArenaEditorPage
    └── SimulationPage
```

No MDI and no complicated floating-window system in V1.

---

# 3. Technology Stack

The project is deliberately centered on **modern C++ and Qt 6**.

| Area | Technology |
|---|---|
| Language | C++23 |
| Desktop UI | Qt 6.7+ |
| Build | CMake 3.28+ |
| Build configuration | CMake Presets |
| Dependencies | vcpkg manifest mode |
| 2D drawing | `QPainter` / `QImage` / `QPixmap` |
| State graph | `QGraphicsScene` / custom Qt graphics items |
| Simulation | Pure C++ |
| Physics | Custom simple 2D collision/physics for V1 |
| Serialization | JSON via `QJsonDocument` |
| Testing | Catch2 v3 |
| Formatting | clang-format |
| Static analysis | clang-tidy + cppcheck |
| Logging | Small project `Logger` wrapper |
| Video export | ffmpeg subprocess, later |
| Platform target | Offline desktop; Windows/Linux first |

### Technology philosophy

Do not add technology merely because it is popular.

The project should prefer:

- Qt Widgets over a second UI framework.
- `QPainter` over OpenGL until performance requires otherwise.
- Simple custom simulation rules over a physics engine for V1.
- JSON over binary serialization during development.
- Plain C++ domain models over QObject-heavy models.
- Small explicit modules over a large framework.
- Value types and direct data ownership over unnecessary abstractions.

---

# 4. Architectural Layers

The system uses a layered architecture with clear ownership.

```text
┌──────────────────────────────────────────────────────────────┐
│ UI                                                           │
│ Qt Widgets, pages, panels, dialogs, input                    │
├──────────────────────────────────────────────────────────────┤
│ Application                                                  │
│ User-facing use cases and orchestration                      │
├──────────────────────────────────────────────────────────────┤
│ Feature Engines                                              │
│ Painting / StateMachine / Simulation                          │
│ Pure C++ logic where possible                                 │
├──────────────────────────────────────────────────────────────┤
│ Domain                                                       │
│ Creature definitions, animations, states, validation         │
├──────────────────────────────────────────────────────────────┤
│ Persistence                                                  │
│ JSON, files, versioning, migration                            │
├──────────────────────────────────────────────────────────────┤
│ Core                                                         │
│ Vec2, Rect, Color, IDs, Result, shared primitives            │
└──────────────────────────────────────────────────────────────┘
```

Rendering is treated as a separate concern:

```text
Rendering
    ↓ reads
Editor / Domain / Simulation state
    ↓
QPainter / QImage / QWidget
```

Rendering must not become the owner of authoritative application state.

---

# 5. Dependency Rules

The dependency direction is intentional:

```text
UI
 ↓
Application
 ↓
Feature / Domain
 ↓
Core

Persistence implements storage around domain/application contracts.

Rendering reads state.
Rendering does not own authoritative state.
```

Feature modules must not directly depend on another feature module's internals.

For example:

```text
Painter ─────X────> Simulator internals
Painter ─────X────> Creature Editor UI
Simulator ───X────> Painter UI
```

Instead:

```text
Painter
   ↓
Animation data
   ↓
Creature Editor
   ↓
Creature definition
   ↓
Simulator
```

---

# 6. Core Module

## Responsibility

Shared primitives with no knowledge of the application.

```text
core/
├── Vec2
├── Rect
├── Color
├── UniqueId
├── Result
└── invariant helpers
```

Rules:

- No Qt.
- No file I/O.
- No domain knowledge.
- No UI.
- No simulation knowledge.

Example conceptual types:

```cpp
struct Vec2;
struct Rect;
struct Color;
using UniqueId = std::uint64_t;
```

---

# 7. Domain Module

The domain describes what a creature **is**, not how the UI displays it.

## Main models

### Creature

```text
Creature
├── id
├── name
├── animations
├── stateMachine
└── metadata
```

### Animation

```text
Animation
├── name
├── frames
├── fps
└── looping
```

### AnimationFrame

```text
AnimationFrame
├── frameIndex
├── imageData
└── duration
```

### State

```text
State
├── id
├── name
├── type
├── animationName
└── transitions
```

### StateTransition

```text
StateTransition
├── targetStateId
└── conditionTag
```

### StateMachine

```text
StateMachine
├── states
├── initialStateId
└── maxStates
```

V1 uses a **flat finite state machine**.

Suggested state types:

```cpp
enum class StateType {
    Idle,
    Movement,
    Attack,
    Defense,
    Dodge,
    Hit,
    Death,
    Special
};
```

---

# 8. Creature Definition vs Runtime State

A creature file is an authored definition.

It is not the current simulation instance.

```text
Creature
    = reusable definition

SimulationEntity
    = runtime instance
```

A runtime entity can contain:

```text
SimulationEntity
├── creatureId
├── position
├── health
├── currentStateId
├── animationFrame
└── facingDirection
```

Runtime state is never written into the creature definition.

---

# 9. Paint / Animation System

The Painter is a dedicated editor-session system.

It should be a **normal raster drawing application**, not a forced pixel-grid editor.

The drawing can look rough, simple, or janky, but the artistic unit is the raster image itself.

```text
Large raster canvas
        ↓
Freehand strokes
        ↓
RGBA pixel layers
        ↓
Animation frames
```

The default canvas can be configured at creature creation time. A small working canvas is encouraged, but the architecture does not require a 32×32 or 48×48 logical-pixel grid.

The exact default size is a product setting rather than a hard architectural assumption.

---

# 10. PaintDocument

`PaintDocument` is the editor's working state.

It is deliberately separate from `Creature`.

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

Each frame contains layers:

```text
PaintFrame
├── layers
└── activeLayerIndex
```

Each layer contains:

```text
PaintLayer
├── name
├── visible
├── locked
├── opacity
├── blendMode
└── pixels
```

Pixel storage:

```text
std::vector<std::uint8_t>
```

RGBA row-major format:

```text
width × height × 4 bytes
```

The source of truth is the layer pixel buffer.

`QImage` and `QPixmap` are display representations only.

---

# 11. Painting Engine

The painting engine is pure C++ and Qt-free.

Responsibilities include:

```text
PaintEngine
├── Pencil
├── Brush
├── Eraser
├── Fill
├── Eyedropper
├── Line
├── Rectangle
├── Ellipse
└── color sampling
```

Conceptual API:

```cpp
applyPencil(...)
applyBrush(...)
applyEraser(...)
applyFill(...)
applyLine(...)
applyRect(...)
applyEllipse(...)
pickColor(...)
```

The engine modifies `PaintLayer.pixels`.

It does not know about:

- QWidget
- mouse events
- QImage
- QPixmap
- Creature
- Simulation

---

# 12. Paint Tool Model

V1 does not need a hierarchy of polymorphic tool objects.

Use a simple enum and dispatch:

```cpp
enum class PaintTool {
    Pencil,
    Brush,
    Eraser,
    Fill,
    Eyedropper,
    RectSelect,
    Lasso,
    Move,
    Line,
    Rect,
    Ellipse
};
```

The UI receives mouse input.

The canvas maps the input into canvas coordinates.

The selected tool dispatches to the painting engine.

Later, a command system can be introduced if structural editing history requires it.

---

# 13. Canvas

`CanvasWidget` is a Qt display/input component.

Responsibilities:

- receive mouse/stylus input
- map screen coordinates to canvas coordinates
- invoke paint operations
- composite the current frame for display
- handle zoom
- draw transparency checkerboard
- draw optional rulers/grid overlays

It does **not** own pixel data.

```text
Mouse Input
    ↓
CanvasWidget
    ↓
canvas coordinates
    ↓
PaintEngine
    ↓
PaintLayer.pixels
    ↓
CanvasCompositor
    ↓
QImage/QPixmap
    ↓
CanvasWidget
```

Zoom is display-only.

```text
document coordinates ≠ screen coordinates
```

The document never stores zoom.

---

# 14. Canvas Compositor

`CanvasCompositor` converts a layered frame into a flat RGBA buffer.

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

Conceptual API:

```cpp
std::vector<std::uint8_t> composite(
    const PaintFrame& frame,
    int width,
    int height);
```

Blend modes for V1:

```text
Normal
Multiply
Screen
Overlay
Add
Subtract
```

The compositor is pure C++ and testable without Qt.

Qt conversion happens at the UI/rendering boundary.

---

# 15. Layers

Layers are intentionally simple.

V1 operations:

- Add
- Delete
- Rename
- Duplicate
- Move up
- Move down
- Hide/show
- Lock/unlock
- Opacity
- Blend mode

Example:

```text
Layers

👁 🔒  Creature Body
👁 🔒  Details
👁 🔒  Outline
👁 🔒  Background
```

Each animation frame owns its own layer stack.

```text
Document
└── Frames
    ├── Frame 1
    │   └── Layers
    ├── Frame 2
    │   └── Layers
    └── Frame 3
        └── Layers
```

---

# 16. Color System

The color panel should be a real color system rather than a single HTML-style color input.

The UI should provide:

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

The internal selected color should maintain a coherent color representation.

HSL and RGB are two views of the same color.

Changing lightness must preserve the selected hue/saturation relationship.

Changing RGB must update the HSL representation.

Also support:

```text
Eyedropper
    ↓
sample canvas pixel
    ↓
selected color
    ↓
Color Panel updates
```

---

# 17. Undo / Redo

Undo is document history, not merely mouse-event reversal.

```text
User Action
    ↓
Document Change
    ↓
History Entry
```

Examples:

```text
1. Add layer
2. Draw stroke
3. Draw stroke
4. Fill
5. Erase
6. Delete layer
```

V1 can use snapshot-based history.

Recommended rule:

- snapshot the active frame
- configurable history depth
- maximum around 50 snapshots initially

Later, move to diff/command-based history only if memory or performance becomes a real problem.

Keyboard shortcuts:

```text
Ctrl+Z          Undo
Ctrl+Shift+Z    Redo
Ctrl+S          Save
Ctrl+Shift+S    Save As
```

---

# 18. Animation Timeline

Animation is frame-by-frame raster artwork.

```text
Timeline

01       02       03       04       05
[frame] [frame] [frame] [frame] [frame]
```

V1 features:

- Add frame
- Delete frame
- Duplicate frame
- Reorder frame
- Select frame
- Frame duration / FPS
- Playback
- Looping
- Frame thumbnails

Playback uses a Qt timer at approximately:

```text
1000 / fps
```

The timer is UI/application infrastructure.

Animation timing rules remain outside the paint engine.

---

# 19. Animation Commit Boundary

Painting data must not directly modify a `Creature`.

The only valid path is:

```text
PaintDocument
      ↓
AnimationCommitter
      ↓
Animation
      ↓
CommitAnimationUseCase
      ↓
Creature.animations
```

The committer:

1. composites each paint frame
2. creates an `AnimationFrame`
3. assigns frame duration
4. creates the `Animation`
5. returns the domain object

This boundary prevents editor-session data from leaking into the simulation domain.

---

# 20. Creature Editor

The Creature Editor gives game meaning to artwork.

```text
Creature Editor
├── Creature name
├── Animations
├── Combat stats
├── State machine
├── State properties
├── Hitboxes / combat data
└── Validation
```

Typical animations:

```text
Idle
Walk
Attack
Hurt
Death
```

The editor should not redraw artwork itself.

It consumes animations produced by the Painter.

---

# 21. State Editor

The State Editor represents creature behavior as a flat node graph.

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

V1 should remain intentionally simple.

Do not add:

- hierarchical FSM
- behavior trees
- utility AI
- scripting
- visual programming language

until the simple FSM has proven insufficient.

---

# 22. State Machine Runtime

The runtime evaluator is pure C++.

```text
StateMachineRunner
├── StateMachine definition
├── StateContext
├── evaluate(conditions)
├── update(dt)
└── currentState()
```

Runtime context:

```text
StateContext
├── currentStateId
└── timeInState
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

---

# 23. Simulation

The simulation is a deterministic 2D sidescroller fighting arena.

V1 target:

```text
Creature A
    vs
Creature B

        ↓

Arena
        ↓

Fixed timestep simulation
        ↓
Combat
        ↓
Winner
```

The simulation engine remains Qt-free.

---

# 24. Simulation Loop

Use a fixed timestep:

```text
dt = 1 / 60 second
```

Conceptual tick:

```text
SimulationLoop::tick()

1. Gather world conditions.
2. Evaluate each creature's FSM.
3. Resolve actions.
4. Resolve combat.
5. Apply movement / gravity / collision.
6. Update health and animation frame.
7. Check win/loss conditions.
8. Record the frame.
9. Advance tick.
```

The UI timer only drives the simulation.

The simulation itself must not depend on `QTimer`.

---

# 25. Physics Philosophy

Do not introduce a full physics engine for V1.

The game is intentionally simple.

Implement only what the fighting prototype needs:

- gravity
- horizontal movement
- vertical movement
- floor collision
- platform collision
- arena bounds
- basic hit detection
- knockback
- simple collision resolution

If future gameplay proves that custom physics is insufficient, a physics library can be introduced later.

Do not pay the complexity cost before it is necessary.

---

# 26. Combat

`CombatResolver` should be stateless.

Input:

```text
CombatAction
+
current entities
```

Output:

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

This makes combat logic easy to test.

---

# 27. Determinism

Simulation must be reproducible.

Rules:

- fixed timestep
- no `std::rand()`
- one controlled random engine
- fixed seed per simulation
- seed stored in recording data
- simulation logic does not depend on rendering timing
- UI timing does not decide game results

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

# 28. Simulation Recording

A simulation recording is a result, not a creature definition.

```text
Recording
├── id
├── creatureAId
├── creatureBId
├── mapId
├── seed
├── frames
├── winnerEntityId
└── timestamp
```

Each frame is a value snapshot.

```text
SimulationFrame
├── tick
└── entity snapshots
```

The recorder must not hold references into mutable simulation state.

---

# 29. Arena Editor

The Arena Editor creates the environment.

V1 responsibilities:

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

The Arena Editor can reuse the painting architecture for raster background/foreground artwork.

Collision remains separate data.

```text
Paint artwork
      +
Collision geometry
      +
Spawn data
      +
Arena metadata
```

Do not bake collision information into artwork.

---

# 30. Rendering

Rendering is a read-only consumer.

Main components:

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
which state transition occurs
how damage works
```

Those belong to domain/simulation logic.

---

# 31. Persistence

Use versioned JSON for V1.

Advantages:

- readable
- debuggable
- easy to inspect
- easy to migrate
- no unnecessary binary format complexity

Every persisted file contains:

```json
{
  "version": 1
}
```

Every load passes through migration logic before the data becomes active.

---

# 32. File Types

```text
.jcreature
    Complete authored creature definition.
    Animations + frames + state machine + metadata.

.jarena
    Arena definition.
    Artwork + collision + spawn data + metadata.

.jrecording
    Simulation result / replay data.

settings.json
    Application settings.
```

---

# 33. Creature File

Conceptual structure:

```json
{
  "version": 1,
  "creature": {
    "id": "...",
    "name": "Janky Goblin",
    "animations": [
      {
        "name": "idle",
        "fps": 8,
        "looping": true,
        "frames": [
          {
            "index": 0,
            "duration": 0.125,
            "imageData": "<encoded image data>"
          }
        ]
      }
    ],
    "stateMachine": {
      "initialStateId": "...",
      "states": []
    }
  }
}
```

The committed creature contains the flattened animation result.

The temporary painting layer/editing state is not part of the committed creature format in V1.

If persistent editable paint sessions become necessary later, introduce a separate `.jcpaint` format.

---

# 34. Persistence Migration

Use a migration runner:

```text
version 1
   ↓
migration
   ↓
version 2
   ↓
migration
   ↓
current version
```

Never silently reinterpret old files.

Every schema change requires an explicit migration.

---

# 35. Validation

Validation is a domain concern.

Use:

```cpp
SimulationReadiness validateForSimulation(
    const Creature& creature);
```

Validation should happen:

1. while editing
2. after loading
3. before simulation

The UI displays the result but does not implement the rules.

Example statuses:

```text
✓ Ready
⚠ Warning
✕ Error
```

Important rules include:

- at least one state exists
- initial state is valid
- state animation references resolve
- transition targets resolve
- Idle state exists
- animation frames contain valid image data

Idle is the minimum required state for V1 simulation readiness.

---

# 36. Application Layer

Use cases represent user-visible operations.

Examples:

```text
CreateCreatureUseCase
SaveCreatureUseCase
LoadCreatureUseCase
ValidateCreatureUseCase
CommitAnimationUseCase
StartSimulationUseCase
RecordSimulationUseCase
CreateArenaUseCase
SaveArenaUseCase
LoadArenaUseCase
```

The application layer orchestrates.

It does not draw widgets.

---

# 37. ProjectContext

There should be one thin shared application context.

```text
ProjectContext
├── current creature
├── current arena
├── current file paths
└── editor/session references where necessary
```

It is owned by the application layer.

It is not a global singleton.

It should not become a dumping ground for every piece of application state.

---

# 38. Recommended Project Structure

```text
janky-creature-studio/
│
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── .clang-format
├── .clang-tidy
│
├── app/
│   └── creature_studio/
│       ├── CMakeLists.txt
│       ├── include/
│       │   └── creature_studio/
│       │       └── main_window.hpp
│       └── src/
│           ├── main.cpp
│           └── main_window.cpp
│
├── core/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/core/
│   │   ├── vec2.hpp
│   │   ├── rect.hpp
│   │   ├── color.hpp
│   │   ├── unique_id.hpp
│   │   └── result.hpp
│   └── src/
│
├── domain/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/domain/
│   │   ├── creature.hpp
│   │   ├── animation.hpp
│   │   ├── animation_frame.hpp
│   │   ├── state.hpp
│   │   ├── state_type.hpp
│   │   ├── state_transition.hpp
│   │   ├── state_machine.hpp
│   │   ├── map.hpp
│   │   ├── simulation_readiness.hpp
│   │   └── validation_error.hpp
│   └── src/
│
├── application/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/application/
│   │   ├── project_context.hpp
│   │   ├── create_creature_use_case.hpp
│   │   ├── save_creature_use_case.hpp
│   │   ├── load_creature_use_case.hpp
│   │   ├── commit_animation_use_case.hpp
│   │   ├── validate_creature_use_case.hpp
│   │   └── start_simulation_use_case.hpp
│   └── src/
│
├── painting/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/painting/
│   │   ├── paint_document.hpp
│   │   ├── paint_frame.hpp
│   │   ├── paint_layer.hpp
│   │   ├── paint_tool.hpp
│   │   ├── paint_engine.hpp
│   │   ├── canvas_compositor.hpp
│   │   ├── mirror_tool.hpp
│   │   ├── paint_history.hpp
│   │   └── animation_committer.hpp
│   └── src/
│
├── statemachine/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/statemachine/
│   │   ├── state_machine_runner.hpp
│   │   ├── state_context.hpp
│   │   └── transition_evaluator.hpp
│   └── src/
│
├── simulation/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/simulation/
│   │   ├── simulation_world.hpp
│   │   ├── simulation_entity.hpp
│   │   ├── simulation_loop.hpp
│   │   ├── simulation_clock.hpp
│   │   ├── combat_resolver.hpp
│   │   ├── simulation_recorder.hpp
│   │   └── random_engine.hpp
│   └── src/
│
├── rendering/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/rendering/
│   │   ├── creature_renderer.hpp
│   │   ├── animation_player.hpp
│   │   ├── simulation_viewport.hpp
│   │   └── state_graph_renderer.hpp
│   └── src/
│
├── persistence/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/persistence/
│   │   ├── creature_repository.hpp
│   │   ├── arena_repository.hpp
│   │   ├── recording_repository.hpp
│   │   ├── json_serializer.hpp
│   │   ├── migration_runner.hpp
│   │   └── schema_version.hpp
│   └── src/
│
├── ui/
│   ├── CMakeLists.txt
│   ├── include/creature_studio/ui/
│   │   ├── pages/
│   │   │   ├── main_menu_page.hpp
│   │   │   ├── creature_editor_page.hpp
│   │   │   ├── state_editor_page.hpp
│   │   │   ├── arena_editor_page.hpp
│   │   │   └── simulation_page.hpp
│   │   │
│   │   ├── painting/
│   │   │   ├── canvas_widget.hpp
│   │   │   ├── color_panel_widget.hpp
│   │   │   ├── layer_panel_widget.hpp
│   │   │   └── animation_timeline_widget.hpp
│   │   │
│   │   ├── creature/
│   │   │   ├── animation_list_widget.hpp
│   │   │   ├── state_properties_widget.hpp
│   │   │   └── validation_status_widget.hpp
│   │   │
│   │   ├── state/
│   │   │   └── state_graph_widget.hpp
│   │   │
│   │   └── arena/
│   │       ├── arena_canvas_widget.hpp
│   │       ├── collision_editor_widget.hpp
│   │       └── spawn_editor_widget.hpp
│   │
│   └── src/
│
├── tests/
│   ├── core/
│   ├── domain/
│   ├── painting/
│   ├── statemachine/
│   ├── simulation/
│   └── persistence/
│
├── assets/
│   ├── icons/
│   ├── default_creatures/
│   └── default_arenas/
│
└── docs/
    ├── architecture.md
    ├── file_formats.md
    └── ai_context.md
```

---

# 39. Module Responsibility Summary

| Module | Owns | Must Not Own |
|---|---|---|
| `core` | shared primitives | Qt, domain rules |
| `domain` | creature definitions and rules | Qt, file I/O |
| `application` | use cases / orchestration | widget logic |
| `painting` | raster editing logic | Qt UI, simulation |
| `statemachine` | FSM execution | UI, rendering |
| `simulation` | runtime world/combat | UI, file format logic |
| `rendering` | visual presentation | authoritative state |
| `persistence` | files / JSON / migration | gameplay rules |
| `ui` | user interaction | domain rules |

---

# 40. UI Layout Philosophy

The application should feel like:

> **Professional desktop workspace organization + MS Paint simplicity + intentionally janky creature creation.**

Do not copy Photoshop feature-for-feature.

The Creature Painter should have:

```text
┌─────────────────────────────────────────────────────────────┐
│ File  Edit  Image  Layer  Select  View                      │
├───────┬───────────────────────────────────────┬─────────────┤
│ Tools │                                       │ Color       │
│       │                                       │             │
│ Pencil│                                       │ Color Field │
│ Brush │              CANVAS                   │ HSL         │
│ Erase │                                       │ RGB         │
│ Fill  │                                       │ HEX         │
│ Pick  │                                       │ Palette     │
│ Line  │                                       │             │
│ Rect  │                                       │             │
├───────┴───────────────────────────────────────┴─────────────┤
│ Layers                                                      │
│ Body                                                        │
│ Details                                                     │
│ Outline                                                     │
├─────────────────────────────────────────────────────────────┤
│ Timeline: [1] [2] [3] [4] [+]              FPS / Zoom       │
└─────────────────────────────────────────────────────────────┘
```

The exact visual design can evolve without changing the architecture.

---

# 41. MVP Scope

## V1 must contain

### Application

- Main menu
- Page navigation
- Project/file open/save

### Painter

- Raster canvas
- Pencil
- Brush
- Eraser
- Fill
- Eyedropper
- Line
- Rectangle
- Ellipse
- Color panel
- HSL
- RGB
- HEX
- Alpha
- Recent colors
- Layers
- Undo/redo
- Zoom
- Animation frames
- Timeline
- Playback

### Creature Editor

- Creature name
- Animation assignment
- Basic combat stats
- State list
- State graph
- Animation-to-state assignment
- Validation

### Simulation

- Two creatures
- Simple arena
- Gravity
- Platform collision
- Movement
- Basic attacks
- Damage
- Knockback
- Death
- Flat FSM execution
- Fixed timestep
- Deterministic random seed
- Basic recording

### Persistence

- `.jcreature`
- `.jarena`
- `.jrecording`
- JSON
- version field
- migration system

---

# 42. Later Features

Only add these after the MVP proves the need:

```text
Lasso / advanced selection
Pressure-sensitive brushes
Advanced brush shapes
Onion skinning
Advanced blend modes
Canvas resize / crop
Reference image import
Persistent paint sessions (.jcpaint)
Sprite sheet export
Replay viewer
Advanced arena editor
Multiple fighters
More advanced AI conditions
Hierarchical FSM
Animation blending
Particles
Screen shake
Sound
More advanced collision
MP4 export
```

For MP4 export, the intended future architecture is:

```text
Simulation / Renderer
        ↓
RGBA frame buffers
        ↓
ffmpeg subprocess
        ↓
MP4
```

A bundled ffmpeg binary is preferred for offline usability.

---

# 43. Explicitly Avoid in V1

Do not add:

- networking
- multiplayer
- cloud services
- Lua/Python scripting
- plugin architecture
- procedural animation
- complex physics middleware
- behavior trees
- hierarchical FSM
- AI model integration
- asset marketplace
- database-backed project management

These are not part of the MVP.

---

# 44. Complexity Budget

| Feature | Complexity | Target |
|---|---:|---|
| Basic raster canvas | Medium | V1 |
| Basic tools | Low | V1 |
| Color system | Medium | V1 |
| Layers | Medium | V1 |
| Snapshot undo/redo | Medium | V1 |
| Animation timeline | Medium | V1 |
| Canvas compositor | Medium | V1 |
| Creature definition | Low | V1 |
| Flat FSM | Low | V1 |
| State graph UI | Medium | V1 |
| Basic combat | Medium | V1 |
| Basic arena | Medium | V1 |
| Deterministic recording | Medium | V1 |
| Advanced selection | Medium | Later |
| Pressure brush | Medium | Later |
| Persistent paint sessions | Medium | Later |
| Advanced arena editor | High | Later |
| Hierarchical FSM | High | Later |
| Advanced AI | High | Later |
| MP4 export | Medium | Later |

---

# 45. Architectural Rules

These rules are long-term and should not be casually broken.

1. **Domain and core never include Qt.**
2. **Simulation logic never depends on Qt.**
3. **Painting engine never depends on Qt.**
4. **Canvas widgets never own authoritative pixel data.**
5. **PaintDocument never references Creature or simulation state.**
6. **Creature data is separate from runtime SimulationEntity data.**
7. **Rendering reads state; it does not modify authoritative state.**
8. **UI does not implement domain or simulation rules.**
9. **Validation has one domain-level source of truth.**
10. **Animation commit is the only path from PaintDocument into Creature animations.**
11. **Frames are composed through CanvasCompositor, not duplicated into UI state.**
12. **Zoom is display state, not document state.**
13. **Every persisted file has a version.**
14. **Every schema change requires migration.**
15. **Simulation uses a fixed timestep.**
16. **Randomness uses a controlled seedable random engine.**
17. **Simulation recordings contain snapshots, not references to mutable runtime objects.**
18. **No global singleton for application state.**
19. **ProjectContext is the only shared mutable application context.**
20. **Do not introduce an abstraction without a current concrete reason.**
21. **Do not solve post-MVP problems inside MVP architecture unless the boundary is required.**
22. **Every module has an explicit CMake target.**
23. **Tests for core/domain/painting/state machine/simulation must be runnable without a QApplication whenever possible.**
24. **The file format is a contract; do not casually change it.**
25. **The application must remain usable offline.**

---

# 46. Coding Conventions

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

Ownership:

- prefer value semantics
- use `std::unique_ptr` for exclusive ownership
- use `std::shared_ptr` only for genuine shared ownership
- no owning raw pointers
- Qt parent ownership is acceptable inside Qt UI code

Use:

```text
std::optional
std::variant
std::vector
std::string
std::expected / project Result type
```

Prefer explicit `const` correctness.

No exceptions in core/domain/simulation unless a concrete infrastructure requirement justifies them.

---

# 47. Testing Strategy

The most important logic must be testable without Qt.

## Core

Test:

- vector operations
- rectangle operations
- color conversions

## Domain

Test:

- creature invariants
- animation validity
- state validation
- simulation readiness

## Painting

Test:

- pencil
- eraser
- fill
- line
- rectangle
- ellipse
- color picking
- compositing
- opacity
- blend modes
- mirror operations
- history

## State Machine

Test:

- valid transition
- invalid target
- condition matching
- state timing
- initial state

## Simulation

Test:

- movement
- collision
- combat
- damage
- knockback
- death
- deterministic outcomes
- recording

The UI should mostly require integration/manual testing rather than containing the important rules itself.

---

# 48. Recommended Development Order

## Phase 1 — Project Foundation

1. CMake
2. CMake Presets
3. Qt 6
4. vcpkg
5. clang-format
6. clang-tidy
7. test infrastructure
8. core module
9. empty UI shell

**Deliverable:**

A clean application that builds and launches.

---

## Phase 2 — Paint Foundation

1. `PaintDocument`
2. `PaintFrame`
3. `PaintLayer`
4. raster pixel buffer
5. `PaintEngine`
6. `CanvasCompositor`
7. `CanvasWidget`
8. pencil
9. brush
10. eraser
11. eyedropper
12. fill
13. color system

**Deliverable:**

Open the application and draw a creature.

---

## Phase 3 — Paint Editing

1. layers
2. opacity
3. blend modes
4. layer reorder
5. undo
6. redo
7. zoom
8. rulers/grid overlays
9. save/load working data where appropriate

**Deliverable:**

A genuinely usable creature painting workspace.

---

## Phase 4 — Animation

1. `PaintFrame`
2. timeline
3. add/delete/duplicate
4. frame selection
5. frame thumbnails
6. FPS
7. looping
8. playback
9. `AnimationCommitter`

**Deliverable:**

Create an animated creature from hand-drawn frames.

---

## Phase 5 — Creature Editor

1. Creature model
2. animation list
3. creature metadata
4. combat stats
5. state model
6. state properties
7. state graph
8. validation
9. `.jcreature` save/load

**Deliverable:**

Turn artwork into a complete creature definition.

---

## Phase 6 — Simulation

1. SimulationWorld
2. SimulationEntity
3. fixed timestep
4. gravity
5. movement
6. collision
7. FSM runtime
8. combat
9. damage
10. knockback
11. death
12. win condition
13. simulation viewport

**Deliverable:**

Two authored creatures can fight.

---

## Phase 7 — Arena Editor

1. arena document
2. background painting
3. foreground painting
4. platforms
5. collision editing
6. spawn points
7. bounds
8. `.jarena`

**Deliverable:**

Create custom fighting arenas.

---

## Phase 8 — Recording / Polish

1. simulation recorder
2. `.jrecording`
3. replay viewer
4. better validation UX
5. settings
6. packaging
7. ffmpeg export
8. polish

**Deliverable:**

A complete offline creature-creation and fighting workflow.

---

# 49. Complete End-to-End Workflow

```text
                 ┌──────────────────┐
                 │    MAIN MENU     │
                 └────────┬─────────┘
                          │
              ┌───────────┴───────────┐
              ▼                       ▼
         PAINTER                  ARENA EDITOR
              │                       │
              ▼                       ▼
        PaintDocument             Arena
              │                       │
              ▼                       │
        Animation                  Collision
              │                       │
              ▼                       │
       CREATURE EDITOR                │
              │                       │
       ┌──────┴──────┐                │
       ▼             ▼                ▼
   Animations      States          Arena Data
       │             │                │
       └──────┬──────┘                │
              ▼                       │
          Creature ───────────────────┘
              │
              ▼
          SIMULATOR
              │
              ▼
       SimulationWorld
              │
              ▼
          FSM + Combat
              │
              ▼
          Recording
              │
              ▼
       Replay / Export
```

---

# 50. AI / Chat Context Workflow

The project should be designed so work can move between ChatGPT, Claude, Cursor, Copilot, or another coding AI without rebuilding the entire context every session.

Keep:

```text
docs/ai_context.md
```

with:

```markdown
# Janky Creature Studio — AI Context

## Project
Short description.

## Architecture
Short architecture summary.

## Technology
C++23 + Qt 6 + CMake.

## Current Phase
Example: Phase 3 — Paint Editing.

## Current Module
Example: painting.

## Current File
Example: painting/src/paint_engine.cpp.

## Working
- Pencil works.
- Eraser works.
- Layers work.

## Broken
- Fill leaks through transparent boundary.

## Decisions
- Qt 6 Widgets.
- Pure C++ painting engine.
- PaintDocument separate from Creature.
- Flat FSM.
- Fixed timestep simulation.
- JSON persistence.
- No unnecessary abstractions.

## Current Task
One specific task.

## Constraints
Anything the AI must not change.
```

### AI session rule

Give the AI only:

1. the architecture context
2. the current file(s)
3. the current task
4. the current error/output
5. explicit constraints

Do not repeatedly paste the entire project.

---

# 51. AI Coding Workflow

## New feature

```text
1. Read architecture.
2. Identify owning module.
3. Identify affected files.
4. Decide whether domain/application/UI boundaries change.
5. Implement the smallest correct change.
6. Build.
7. Run relevant tests.
8. Update AI context.
```

## Bug

```text
1. Reproduce.
2. Identify layer.
3. Trace data ownership.
4. Fix the root cause.
5. Do not move logic into UI just to make the symptom disappear.
6. Build.
7. Test.
8. Record the architectural lesson if needed.
```

## Refactor

```text
1. State the problem.
2. Identify current ownership.
3. Define the desired ownership.
4. Move responsibilities.
5. Keep behavior unchanged.
6. Build and test.
```

## New architectural idea

Do not immediately implement it.

First ask:

```text
Does this solve a current problem?
Does it belong to an existing module?
Does it introduce a new dependency?
Does it increase complexity?
Can the current architecture already support it?
```

Only then add it.

---

# 52. Long-Term Architecture Direction

The architecture should grow by adding capabilities inside existing boundaries.

Good growth:

```text
painting
    ↓
better tools

simulation
    ↓
better combat

statemachine
    ↓
more conditions

arena
    ↓
more environment features
```

Bad growth:

```text
every new feature
    ↓
new global manager
    ↓
new event bus
    ↓
new service layer
    ↓
new abstraction
    ↓
architecture becomes harder than the game
```

The project should remain understandable to one developer opening it months later.

---

# 53. Final Architecture

```text
┌──────────────────────────────────────────────────────────────────┐
│                         Qt 6 UI                                  │
│                                                                  │
│ Main Menu | Creature Editor | State Editor | Arena | Simulator   │
│                                                                  │
│ Canvas | Color | Layers | Timeline | State Graph | Viewports     │
└──────────────────────────────┬───────────────────────────────────┘
                               │
                               ▼
┌──────────────────────────────────────────────────────────────────┐
│                       Application                                │
│                                                                  │
│ Use Cases + ProjectContext + Orchestration                       │
└───────────────┬───────────────────────┬──────────────────────────┘
                │                       │
                ▼                       ▼
┌────────────────────────┐   ┌─────────────────────────────────────┐
│        Domain          │   │             Engines                 │
│                        │   │                                     │
│ Creature               │   │ Painting       StateMachine         │
│ Animation              │   │ Simulation                         │
│ StateMachine           │   │                                     │
│ Validation             │   │ Pure C++ wherever possible          │
└────────────┬───────────┘   └──────────────────┬──────────────────┘
             │                                  │
             └────────────────┬─────────────────┘
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│                           Core                                   │
│                                                                  │
│ Vec2 | Rect | Color | UniqueId | Result | Shared Primitives      │
└──────────────────────────────────────────────────────────────────┘

                    ┌───────────────────────┐
                    │     Persistence       │
                    │                       │
                    │ JSON | Repositories   │
                    │ Versioning | Migration│
                    └───────────────────────┘

                    ┌───────────────────────┐
                    │      Rendering        │
                    │                       │
                    │ QPainter | QImage     │
                    │ Read-only presentation│
                    └───────────────────────┘
```

---

# 54. Architecture Decision Summary

| Area | Decision |
|---|---|
| Application type | Offline desktop application |
| Language | C++23 |
| UI | Qt 6 Widgets |
| Build | CMake + Presets |
| Package management | vcpkg |
| Graphics | QPainter / QImage |
| Architecture | Layered modular architecture |
| Painter | Pure C++ raster engine + Qt UI |
| Paint model | PaintDocument → PaintFrame → PaintLayer |
| Canvas | Freehand raster, not forced pixel-grid art |
| Animation | Frame-by-frame |
| Creature behavior | Flat FSM |
| Simulation | Deterministic fixed timestep |
| Physics | Simple custom 2D physics for V1 |
| Persistence | Versioned JSON |
| Creature format | `.jcreature` |
| Arena format | `.jarena` |
| Recording format | `.jrecording` |
| Undo | Snapshot-based V1 |
| Testing | Catch2 |
| Rendering | Read-only |
| AI workflow | Small persistent architecture/context document |
| Complexity strategy | MVP first, abstraction only when justified |

---

# 55. The Core Principle

The most important rule for the entire project is:

> **Keep the editor simple, keep the simulation deterministic, and keep the boundaries obvious.**

A creature should be easy to make:

```text
Draw → Animate → Define → Fight
```

The architecture should be strong enough to survive years of additions without turning the project into an unnecessary framework.

Build the smallest version that proves the entire loop:

```text
DRAW CREATURE
      ↓
ANIMATE
      ↓
CREATE CREATURE
      ↓
CREATE SIMPLE ARENA
      ↓
FIGHT
```

Once that complete loop works, expand each subsystem deliberately.
