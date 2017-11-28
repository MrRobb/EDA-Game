# EDA-Game

## Game Rules

### Map

Square board made of cells of type:

- Water: you can't walk through water
- Grass
- Forest
- Sand
- City
- Path

The board is surrounded with water.

Cities are rectangles of cells of type CITY.

Path are sequences of cells of type PATH.

> Paths connect cities and never cross each other.

### Objective

At each round you have to command your orks.

If you try to give > 1000 instructions ---> game over

### Orks

#### Habilities

Orks can:

- Remain still
- Move one cell (N, S, W, E)

If an ork receive more than one instruction, only the first one matters.

If you walk ---> your health decreases.

If health negative ---> ork dies and you have to regenerate them.

#### Fight

- Between orks of the **same player** ---> the **instruction of moving is ignored**.


- Orks of different players AND **(A_health - movement) > B** ---> **B dies**
- Orks of different players AND **(A_health - movement) < B** ---> **A dies**
- Orks of different players AND **(A_health - movement) == B** ---> **random dies** (50%, 50%) and when it regenerates **the ork belongs to the winner player**.

#### Death

An ork regenerates in a random position adjacent to the sea (GRASS, FOREST, SAND)

### Points

At the end of the round:

- Count the number of orks for earch player for each city
- A_orks_on_city > B_orks_on_city ---> A conquers the city
- A_orks_on_city < B_orks_on_city ---> B conquers the city
- A_orks_on_city == B_orks_on_city ---> nothing changes

> **points = bonus_per_city_cell() * size_of_the_city**
>
> **points = bonus_per_path_cell() * size_of_the_path**
>
>
> Graph of conquered cities (for each connected component):
>
> **points = factor_connected_component * (2 ^ cities)**

### Workflow

1. Register instructions of all players
2. Execute them randomly
3. Regenerate dead orks
4. Computer points

## Game Parameters

```cpp
int nb_players () const;    // number of players (4)
int rows () const;          // number of rows of the board (70)
int cols () const;       // number of columns of the board (70)
int nb_rounds () const;     // number of rounds of the match (200)

int initial_health () const;    // initial health of each ork (100)
int nb_orks () const;           // number of orks each player controls initially

int cost (CellType t) const;
int cost_grass () const;        // cost in health of moving to a cell of type GRASS (1)
int cost_forest () const;       // cost in health of moving to a cell of type FOREST (2)
int cost_sand () const;         // cost in health of moving to a cell of type SAND (3)
int cost_city () const;         // cost in health of moving to a cell of type CITY (0)
int cost_path () const;         // cost in health of moving to a cell of type PATH (0)

int bonus_per_city_cell () const;   // bonus in points for each cell in a conquered city (1)
int bonus_per_path_cell () const;   // bonus in points for each cell in a conquered path (1)
int factor_connected_component () const;    // factor multiplying the size of the connected components (2)

bool player_ok (int pl) const;      // is playerid okay?
bool pos_ok (int i, int j) const;   // is position inside the board
bool pos_ok (Pos p) const;          // is position inside the board
int me ();                          // returns my id
```

## State.hh

```cpp
// GENERAL
int round () const;                 // current round
int total_score (int pl) const;     // score of a player
double status (int pl) const;       // percentage of cpu time used in the last round (0..1) if < 0 ---> user is dead
vector<int> orks(int pl);           // returns the ids of all the orks of a player


// CELL
Cell cell (int i, int j) const;     // copy of the cell (i, j)
Cell cell (Pos p) const;            // copy of the cell p


// UNIT
Unit unit (int id) const;           // returns information about the unit (0..nb_units())
int nb_units () const;              // total number of orks in the game


// CITY
typedef vector<Pos>  City;          // CITY
City city(int id) const;            // returns information about the city (0..nb_cities())
int nb_cities () const;             // total number of cities in the game
int city_owner(int id) const;       // city owner


// PATH
typedef pair< pair<int,int>, vector<Pos> >  Path;   // PATH
Path path(int id) const;            // returns information about the path (0..nb_paths())
int nb_paths () const;              // total number of paths in the game
int path_owner(int id) const;       // path owner
```

## Action.hh

```cpp
// COMMAND
Command (int id, Dir dir);          // create command (ork --> direction)
void execute(Command m);            // add command to the action (fails if a command is already present for this unit)
```

## Structs.hh

```cpp
// DIR
enum Dir {
  BOTTOM,    // South
  RIGHT,     // East
  TOP,       // North
  LEFT,      // West
  NONE,      // No direction
  DIR_SIZE   // Artificial value, for iterating.
};
inline bool dir_ok (Dir dir);       // is_valid(direction)


// POS
Pos (int i, int j);                                         // create position
friend ostream& operator<< (ostream& os, const Pos& p);     // print
friend bool operator== (const Pos& a, const Pos& b);        // equal
friend bool operator!= (const Pos& a, const Pos& b);        // not equal
friend bool operator< (const Pos& a, const Pos& b);         // less than (sorting)
Pos& operator+= (Dir d);                                    // Increment to direction
Pos operator+ (Dir d) const;                                // Add direction
Pos& operator+= (Pos p);                                    // Increment position
Pos operator+ (Pos p) const;                                // Add position


// CELL
enum CellType {
  WATER,
  GRASS,
  FOREST,
  SAND,
  CITY,
  PATH,
  CELL_TYPE_SIZE  // Artificial value, for iterating.
};
Cell (CellType t, int unit, int city, int path);            // create cell
/*
    CellType type; // The type of cell.
    int   unit_id; // The id of the unit on the cell if any, -1 otherwise.
    int   city_id; // If type == CITY, the id of the city, -1 otherwise.
    int   path_id; // If type == PATH, the id of the path, -1 otherwise.
*/


// UNIT
Unit (int id, int pl, Pos p = Pos(0, 0), int health = 0);   // create unit
/*
    int id;        // The unique id for this unit during the game.
    int player;    // The player that owns this unit.
    Pos pos;       // The position on the board.
    int health;    // The health of the unit.
*/
```

## Settings.hh

```cpp
static string version ();   // game name and version
int nb_players () const;    // number of players
```
