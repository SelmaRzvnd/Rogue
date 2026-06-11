# Rogue-like Dungeon Crawler (C + ncurses)

A terminal-based, roguelike dungeon crawler written in **C** using **ncurses**. Explore procedurally generated floors, collect gold, weapons and spells, survive traps and monsters, and reach the Treasure Room on floor 4 to win.

---

## Main features

* Procedural dungeon generation (rooms, corridors, doors).
* Persistent user accounts (Sign Up / Login) and Guest play.
* Regex-based input validation for signup (username, email, password).
* 8-direction support + advanced movement options.
* Items: gold (regular & rare black gold), food, weapons, spells, keys, traps.
* Enemies with distinct stats and simple pursuit AI.
* Save / Load multiple games (preserves explored map state).
* Treasure Room on floor 4: final challenge and scoring.

---

## Accounts & Authentication

Players can enter the game in three ways:

1. **Sign Up** — create a persistent account (username, email, password).

   * Input is validated with regular expressions:

     * **Username**: allowed characters and length constraints.
     * **Email**: must match a standard email regex (e.g. `local@domain`).
     * **Password**: minimum length (≥ 7), must include at least one digit, one uppercase and one lowercase letter.
   * On validation failure the game shows a clear error message explaining which rule failed.

2. **Login** — enter username (or email) and password; credentials checked against stored user files.

3. **Guest** — play immediately without creating an account; guest progress is **not** persisted.

---

## Main menus

* **New Game** — starts a new dungeon run using current pre-game settings.
* **Continue Last Game** — resume a previously saved game.
* **Leaderboard** — shows top players with: rank, username, total score (gold), saved gold, completed runs, experience (time since first run). The current player row is highlighted.
* **Settings** — difficulty, hero color, and background music selection.
* **Profile** — view account details and saved games.

---

## Map & Level generation

Each floor is a self-contained dungeon level composed of randomly sized rooms connected by corridors.

**Core rules**:

* At least **6 rooms** should exist on the visible page at any time.
* Minimum room size: **4×4**.
* Rooms are placed randomly with corridors (direct/maze/multi-branch) to ensure full accessibility (all rooms reachable).
* Exactly **one staircase** per floor (to go down/up). The corresponding stair on the next floor lines up logically so returning is possible when appropriate.
* Visibility (fog of war): you cannot see more than 5 cells into a corridor unless you have already explored that corridor cell.
* When exploring: rooms adjacent to the current room become visible only after entering them at least once.

**Tiles & symbols** (suggested):

* Wall: `|` or `_`
* Floor: `.`
* Door: `+`
* Corridor: `#`
* Pillar: `O`
* Window: `=` (shows view into adjacent room but cannot pass through)
* Stairs: `>`
* Discovered trap: `^`
* Discovered secret-door: `?`

---

## Special map elements

### Traps

* Hidden traps are placed on room floors.

### Secret doors

* Appear in dead-end rooms.
* Must be discovered by attempting to move into the adjacent cell facing where the door is.

### Password-locked doors

* Locked doors are shown as `@` (red when locked, green when open).
* A button `&` appears in the room (in a corner). When the player steps on it a **4-digit code** is generated and displayed for **30 seconds**; only the last generated code in that room is valid.
* To open the door the player must stand next to it and move toward it — a prompt asks for the code. The player has **3 attempts**:

  * 1st wrong attempt → yellow warning
  * 2nd wrong attempt → orange warning
  * 3rd wrong attempt → activates room security and locks the door

### Key Ancient (Key Master)

* Symbol: `△` (gold) — one per floor.
* Single-use: opens any password door without code, but has a **10% chance to break** on use.
* Two broken keys can be combined to attempt to craft a new key (optional mechanic).

---

## Rooms & themes

Rooms are assigned themes that affect contents and visuals:

* **Treasure Room (rare)** — contains many traps and large gold rewards; only appears on the final floor. Entering it and clearing enemies ends the game.
* **Enchant Room** — no enemies; contains many spells/potions. Staying too long may have negative side effects (design choice).
* **Nightmare Room (optional/bonus)** — limited visibility (5×5 window centered on player), no gold/food; items are illusions.
* **Regular Room** — most common (~50%), may contain gold, food, spells, ammo, traps or standard enemies.

Visuals: color and characters for each theme should differ for clarity.

---

## Movement & exploration

* **8-directional movement**: ↖ (Y), ↑ (J), ↗ (U), → (L), ↘ (N), ↓ (K), ↙ (B), ← (H)
* **Blocked movement**: moving into walls is not allowed — the hero remains in place.
* **Door/corridor reveal**: stepping into a door reveals connecting corridor; stepping into a corridor reveals the destination room (if not locked).

**Advanced movement**:

* `f` + direction → run in that direction until you hit a wall or an interactable object.
* `g` + direction → move to the next cell containing an item without automatically picking it up.
* `s` while standing on a map cell → scan neighboring cells for traps/secret doors.

**Map reveal**:

* Press `M` to toggle full-map reveal (shows unexplored areas temporarily). Press again to return to normal fog-of-war.

**Controls note**: For simplicity you may accept `f` or `g` pressed first, then direction key.

### Food

* Represented by an icon on the map; appears randomly in rooms.
* Food is necessary to avoid starvation: max **5 units** carried.
* Eating (`E`) restores HP; if the player does not eat for a long time HP gradually decreases.
* Food types (phase 2/bonus): Normal, Premium (restores more + temporary buff), Magical (restores + temporary speed), Rotten (harmful).
* Food degrades over time (premium → normal → rotten).

### Gold

* Regular gold appears commonly; **Black gold** is rare and worth significantly more.
* Stepping on a gold bag auto-collects it and shows a message with the amount picked up.
* Gold contributes directly to final score and to saved wealth.

### Weapons & Ammo

* Starting weapon: **Mace** (default melee).
* Other weapons spawn on the ground and are collected into the inventory. Inventory shown with `i`.
* Sample weapons:

  * **Mace** — melee, 5 damage, hits all adjacent tiles. (default)
  * **Dagger** — 12 damage, throwable up to 5 tiles; pickup grants 10 daggers.
  * **Magic Wand** — 15 damage, throwable up to 10 tiles; stuns enemies on hit; pickup grants 8.
  * **Arrow** — 5 damage, throwable 5 tiles; pickups grant 20 arrows.
  * **Sword** — 10 damage, melee (one-time pickup grants permanent sword).

**Weapon rules**:

* Press `i` to open weapon menu. Weapons grouped by melee / ranged.
* Press the weapon's letter to equip it as the default weapon (if available).
* Press `space` to attack. For ranged weapons, select direction; `a` repeats the previous shot/action.
* Throwing consumes one unit of that ammo. If a thrown projectile lands on the ground and is picked up, it increases inventory by **1** (not the original pickup batch).

---

## Spells / Potions

Three active spell types (temporary buffs):

* **Health** — doubles HP regeneration speed for **10 moves**.
* **Speed** — grants double movement (e.g., two steps per turn or jump behavior) for **10 moves**.
* **Damage** — doubles weapon damage for **10 moves**.

Spells are collected on the floor and stored in inventory; use them from the spells menu. Duration counts down per player action.

---

## Enemies

Enemies are represented by letters and have HP and simple pursuit behaviors:

| Enemy        | Symbol |  HP | Behavior summary                                                    |
| ------------ | :----: | :-: | ------------------------------------------------------------------- |
| Deamon       |    D   |  5  | Weak. Attacks when adjacent.                                        |
| Fire Monster |    F   |  10 | Moderate. Attacks adjacent.                                         |
| Giant        |    G   |  15 | Stronger. Pursues hero up to 5 steps.                               |
| Snake        |    S   |  20 | Persistent: once it detects the hero it keeps chasing until killed. |
| Undeed       |    U   |  30 | Strongest: chases up to 5 steps when adjacent or alerted.           |

**Movement & turns**:

* Enemy movement is **local**: only enemies inside the room containing the hero move. No enemies spawn or move in corridors.
* Each player move constitutes a turn; enemies move after the player. Enemy movement simply selects a step that reduces distance to the hero (no advanced pathfinding required). Some enemy types may pause or change behavior after several steps.

**Combat**:

* When an enemy is adjacent and the hero moves into its cell (or attacks) they exchange damage according to weapon/enemy stats. Attack events are logged into the in-game message area.

---

## Save & Load

* The player can save multiple games with custom names. Saved data includes the player state and explored map (fog-of-war) so returning preserves what was seen.
* If the game exits unexpectedly, saved runs can still be resumed from the menu.

> Implementation tip: store per-user save files in a `saves/` folder and reference them in the Continue menu.

---

## Victory & Defeat

* **Victory**: reach the Treasure Room on floor 4, clear its enemies and traps — final score is computed from collected gold and bonuses; show victory screen and return to main menu.
* **Defeat**: if the player dies before or inside the Treasure Room, show defeat screen with final stats and return to main menu. In both cases the last save slot should be considered finished and Continue should not resume it.

---

## Controls (default)

| Action                          |                                                 Key(s) |
| ------------------------------- | -----------------------------------------------------: |
| Move (8 directions)             | ↖ (Y), ↑ (J), ↗ (U), → (L), ↘ (N), ↓ (K), ↙ (B), ← (H)|
| Run until obstacle              |                                     `f` then direction |
| Move to next object             |                                     `g` then direction |
| Attack / Use weapon             |                                                `Space` |
| Repeat attack / action          |                                                    `a` |
| Open inventory (weapons/spells) |                                                    `i` |
| Consume food / open food menu   |                                                    `e` |
| Descend stairs                  |                                                    `<` |
| Ascend stairs                   |                                                    `>` |
| Toggle full-map reveal          |                                                    `M` |

---

## Compilation

Compile the program using:
gcc main.c -o rogue -lncursesw -lm -lSDL2 -lSDL2_mixer -g

---

#Sample User Data

For testing and demonstration purposes, this repository includes a small set of pre-generated user accounts inside the Users' Info/ directory.
These accounts were randomly created to simulate leaderboard activity and show example statistics such as usernames, scores, and progress.


## Screenshots

[Click here to view the Gameplay Screenshots (PDF)](Rogue.pdf)
