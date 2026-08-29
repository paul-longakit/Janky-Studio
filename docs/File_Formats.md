# Janky Studio — File Formats

## 1. Purpose

Janky Studio uses versioned JSON-based files for authored content and simulation results.

The initial file formats are:

```text
.jcreature
.jarena
.jrecording
```

Application settings use JSON as well:

```text
settings.json
```

The formats are intentionally human-readable and easy to inspect during development.

The file format is a contract.

Do not casually change the schema.

---

# 2. General Rules

Every persisted file must contain a schema version.

Minimum structure:

```json
{
  "version": 1
}
```

The version belongs to the file schema, not the application executable version.

## Loading rule

Loading should conceptually follow:

```text
File
 ↓
Parse JSON
 ↓
Read schema version
 ↓
Run migrations
 ↓
Validate structure
 ↓
Convert to domain/application objects
 ↓
Activate
```

Never silently reinterpret an old schema as though it were the current schema.

If a schema changes, add an explicit migration.

---

# 3. File Types

| Extension | Purpose |
|---|---|
| `.jcreature` | Authored creature definition |
| `.jarena` | Authored arena definition |
| `.jrecording` | Deterministic simulation result / replay |
| `.json` | Application settings and other explicitly JSON-based data |

---

# 4. `.jcreature`

## Purpose

A `.jcreature` file stores a complete authored creature definition.

It represents reusable authored data, not a runtime simulation instance.

A creature file may contain:

```text
Creature
├── identity / metadata
├── animations
├── state machine
└── other authored creature properties
```

It must not contain mutable runtime simulation state such as:

```text
current health
current position
current runtime state
runtime animation frame
runtime timers
```

---

# 5. Creature JSON Structure

The architecture defines the following conceptual structure:

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

This is a conceptual V1 structure.

The exact final serialization representation should follow the implemented domain model when persistence is introduced.

---

# 6. Creature Identity

The Creature contains an identifier.

Conceptually:

```json
{
  "id": "..."
}
```

The ID should remain stable for the authored object where the application requires stable identity.

IDs should not be used as substitutes for human-readable names.

Example:

```text
id   → stable machine identity
name → user-facing identity
```

---

# 7. Creature Name

Example:

```json
{
  "name": "Janky Goblin"
}
```

The name is user-facing metadata.

It should not be relied upon as a unique machine identifier.

---

# 8. Animations

A Creature contains authored animations.

Conceptually:

```json
{
  "animations": [
    {
      "name": "idle",
      "fps": 8,
      "looping": true,
      "frames": []
    }
  ]
}
```

Animation properties include:

```text
name
fps
looping
frames
```

---

# 9. Animation Frames

Each animation frame is a domain `AnimationFrame`.

Conceptually:

```json
{
  "index": 0,
  "duration": 0.125,
  "imageData": "<encoded image data>"
}
```

The committed Creature animation contains flattened frame output.

The temporary painting layer structure is not part of the committed Creature format in V1.

The important distinction is:

```text
PaintDocument
    = temporary/editor-session state

Creature Animation
    = committed authored asset
```

---

# 10. Paint Data and Creature Data

Painting data does not directly become a Creature.

The intended boundary is:

```text
PaintDocument
      ↓
Composite frames
      ↓
Animation Commit
      ↓
AnimationFrame
      ↓
Animation
      ↓
Creature
```

This means the `.jcreature` format represents committed animation data rather than the full editable Painter session.

If persistent editable paint sessions become necessary later, introduce a separate format such as:

```text
.jcpaint
```

Do not add that format until there is a concrete requirement.

---

# 11. State Machine

The Creature file contains its authored state machine.

Conceptually:

```json
{
  "stateMachine": {
    "initialStateId": "...",
    "states": []
  }
}
```

A state conceptually contains:

```text
id
name
type
animationName
transitions
```

A transition conceptually contains:

```text
targetStateId
conditionTag
```

The V1 state machine is flat.

---

# 12. Creature Validation

A loaded `.jcreature` should be validated before use.

Validation is domain-owned.

Important V1 validation conditions include:

- at least one state exists
- initial state is valid
- Idle state exists
- state animation references resolve
- transition targets resolve
- animation frames contain valid image data
- animation frame timing is valid

The UI displays validation results.

It does not implement the validation rules.

---

# 13. `.jarena`

## Purpose

A `.jarena` file stores an authored arena.

Conceptually:

```text
Arena
├── background
├── foreground
├── platforms
├── collision
├── spawn points
├── arena bounds
├── decorations
└── metadata
```

The exact serialized schema should follow the implemented Arena domain model when persistence is added.

---

# 14. Arena Artwork vs Collision

Arena artwork and collision data are separate.

Conceptually:

```text
Arena
├── Paint artwork
├── Collision geometry
├── Spawn data
└── Metadata
```

Collision must not be inferred from or baked permanently into raster artwork.

This allows visual changes without unintentionally changing gameplay collision.

---

# 15. Arena Collision

Collision geometry should represent gameplay-relevant geometry separately from artwork.

Potential V1 concepts include:

```text
floor
platforms
arena bounds
```

The exact geometry representation is an implementation detail of the Arena domain/simulation system and should not be invented in the file format before the corresponding model exists.

---

# 16. Arena Spawn Data

Spawn points are authored arena data.

Conceptually:

```json
{
  "spawnPoints": [
    {
      "id": "...",
      "position": {
        "x": 100,
        "y": 200
      }
    }
  ]
}
```

The exact representation should match the Core/Domain geometry types used by the implemented Arena model.

---

# 17. `.jrecording`

## Purpose

A `.jrecording` file stores the result of a deterministic simulation.

A recording is not an editable Creature.

It is a simulation result/replay.

Conceptually:

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

---

# 18. Recording Determinism

A recording should preserve the information needed to identify/reproduce the simulation.

At minimum, the architecture requires a controlled random seed.

Conceptually:

```json
{
  "version": 1,
  "recording": {
    "seed": 12345
  }
}
```

The simulation itself uses:

```text
fixed timestep
+
controlled random engine
+
seed
```

The simulation result must not depend on rendering timing.

---

# 19. Recording Frames

Each recording frame is a value snapshot.

Conceptually:

```text
SimulationFrame
├── tick
└── entity snapshots
```

A recording must not retain references to mutable simulation objects.

Conceptual JSON:

```json
{
  "tick": 120,
  "entities": [
    {
      "entityId": "...",
      "position": {
        "x": 400,
        "y": 300
      },
      "health": 75,
      "stateId": "..."
    }
  ]
}
```

The exact entity snapshot fields should follow the implemented SimulationEntity model.

Do not add fields to the persisted schema merely because they might be useful later.

---

# 20. Recording Metadata

A recording conceptually identifies:

```text
recording ID
creature A
creature B
arena
random seed
winner
timestamp
```

References should use stable IDs where appropriate.

The recording should not become a second copy of the complete Creature or Arena definitions unless a concrete reproducibility requirement later demands embedded snapshots.

---

# 21. `settings.json`

Application settings are separate from authored project content.

Examples may include:

```text
window state
UI preferences
recent files
editor preferences
```

Settings are not part of:

```text
.jcreature
.jarena
.jrecording
```

Do not place gameplay state or authored creature data into settings.

---

# 22. Schema Versioning

Every persisted format has a version.

Example:

```json
{
  "version": 1
}
```

When changing the schema:

```text
Version 1
   ↓
Migration 1 → 2
   ↓
Version 2
```

For multiple versions:

```text
Version 1
   ↓
Migration 1 → 2
   ↓
Migration 2 → 3
   ↓
Current Version
```

Do not skip explicit migration logic by silently accepting old fields with new meanings.

---

# 23. Migration Runner

Persistence should provide a migration mechanism conceptually like:

```text
MigrationRunner
├── currentSchemaVersion
├── migration 1 → 2
├── migration 2 → 3
└── ...
```

Loading should:

1. parse the JSON
2. read the version
3. identify the current schema
4. execute required migrations in order
5. validate the migrated representation
6. deserialize into application/domain objects

Migrations should be deterministic and testable.

---

# 24. Serialization Boundary

Persistence should translate between files and application/domain objects.

Conceptually:

```text
Domain / Application Object
        ↓
Serializer
        ↓
JSON Object
        ↓
File
```

and:

```text
File
 ↓
JSON
 ↓
Migration
 ↓
Deserializer
 ↓
Domain / Application Object
```

Serialization code must not silently introduce gameplay behavior.

Domain validation remains domain-owned.

---

# 25. Human Readability

V1 uses JSON because it is:

- readable
- inspectable
- debuggable
- easy to migrate
- easy to diff during development

Prefer clear field names over compressed or cryptic representations.

Do not optimize the file format for theoretical size requirements before actual file sizes demonstrate a problem.

---

# 26. Image Data

Creature animation frames ultimately contain image data.

The architecture specifies conceptual encoded image data in the Creature format:

```json
{
  "imageData": "<encoded image data>"
}
```

The exact encoding should be selected when the persistence implementation is built.

The persistence design must preserve the distinction between:

```text
PaintLayer pixel buffers
        ↓
Composited frame
        ↓
Committed AnimationFrame image data
```

Do not make the file format depend directly on Qt UI objects.

---

# 27. File Format and Qt

The persisted schema should represent application/domain data, not Qt object graphs.

Do not serialize:

```text
QWidget
QMainWindow
QImage object state
QPixmap object state
QObject ownership trees
```

A Qt-specific conversion layer may translate image/pixel representations at the boundary, but the file format should remain an application-level contract.

---

# 28. Backward Compatibility

Old files should remain loadable when practical through explicit migrations.

When a breaking format change is required:

```text
old version
    ↓
migration
    ↓
current version
```

If a file cannot be migrated safely, loading should fail explicitly rather than silently producing incorrect content.

---

# 29. Forward Compatibility

A newer application may contain fields unknown to an older application.

V1 does not require full forward compatibility.

Do not design a complex compatibility protocol prematurely.

The important V1 rule is:

> Known schema versions must have explicit interpretation and migration rules.

---

# 30. File Extensions

The extensions are part of the product contract:

```text
.jcreature
.jarena
.jrecording
```

Do not rename extensions casually.

If a future format is materially different, introduce a new version/format deliberately rather than changing the meaning of an existing extension invisibly.

---

# 31. Validation at Load Time

Loading is not complete merely because JSON parsed successfully.

The pipeline is:

```text
JSON syntax valid
      ↓
Schema version valid
      ↓
Migration successful
      ↓
Required fields valid
      ↓
Domain conversion successful
      ↓
Domain validation
      ↓
Object becomes active
```

Malformed or invalid authored data should not be allowed to silently enter active application state.

---

# 32. Testing File Formats

Persistence tests should eventually cover:

### Serialization

```text
domain object
    ↓
JSON
```

### Deserialization

```text
JSON
    ↓
domain object
```

### Round trip

```text
domain
 ↓
serialize
 ↓
deserialize
 ↓
equivalent domain
```

### Migration

```text
old JSON
 ↓
migration
 ↓
current JSON
 ↓
domain
```

### Invalid files

Test:

- missing version
- unsupported version
- malformed JSON
- missing required fields
- invalid IDs
- invalid references
- invalid animation data
- invalid state transitions

---

# 33. V1 Scope

The initial persistence scope is intentionally small.

Required:

```text
.jcreature
.jarena
.jrecording
```

with:

```text
JSON
version field
migration system
```

Do not add:

```text
binary formats
database storage
cloud synchronization
asset servers
package managers for project files
```

unless the actual product demonstrates a need.

---

# 34. Future `.jcpaint`

A separate editable Painter document format may eventually be useful:

```text
.jcpaint
```

This would potentially preserve:

```text
canvas size
frames
layers
pixel buffers
active layer
editing metadata
history or other editor-session data
```

However:

> `.jcpaint` is not part of the current required V1 persistence scope.

Do not add it until persistent editable paint sessions become a concrete product requirement.

---

# 35. File Format Principle

The file format should be:

```text
explicit
  ↓
versioned
  ↓
readable
  ↓
migratable
  ↓
testable
```

It should not become a second architecture.

> Persist the authoritative authored data and the information required to reproduce a recording—nothing more than the product currently needs.
