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
nb_players();    // number of players (4)
rows();          // number of rows of the board (70)
columns();       // number of columns of the board (70)
nb_rounds();     // number of rounds of the match (200)

initial_health();    // initial health of each ork (100)
cost_grass();        // cost in health of moving to a cell of type GRASS (1)
cost_forest();       // cost in health of moving to a cell of type FOREST (2)
cost_sand();         // cost in health of moving to a cell of type SAND (3)
cost_city();         // cost in health of moving to a cell of type CITY (0)
cost_path();         // cost in health of moving to a cell of type PATH (0)

bonus_per_city_cell();   // bonus in points for each cell in a conquered city (1)
bonus_per_path_cell();   // bonus in points for each cell in a conquered path (1)
factor_connected_component();    // factor multiplying the size of the connected components (2)
```
