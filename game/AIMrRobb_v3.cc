#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME MrRobb_v3


struct PLAYER_NAME : public Player {

    /**
    * Factory: returns a new instance of this class.
    * Do not modify this function.
    */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
    * Types and attributes for your player can be defined here.
    */
	bool starting = true;

	vector<int> my_orks;
	vector<int> my_actions;
	int my_units;

    /***********************************
                INIT
    ***********************************/
	void init()
    {
    	my_orks = orks(me());
		my_units = int(my_orks.size());

		if (starting) {
			/*
			 ACTIONS:
				 -1 --> not my ork
				 0 ---> looking for a city
				 1 ---> fight or flight
			 */
			my_actions = vector<int> (nb_units(), -1);
			// cerr << "MY ACTIONS SIZE INIT --> " << my_actions.size()<< endl;
			for (int i = 0; i < my_units; i++) my_actions[my_orks[i]] = 0;
			starting = false;
		}
	}

	/***********************************
            UTILITIES
    ***********************************/
	int manhattan_distance(const Pos &p1, const Pos &p2)
    {
		return abs(p1.i - p2.i) + abs(p1.j - p2.j);
	}

	int custom_cost(CellType c)
	{
		if (c != CELL_TYPE_SIZE)
			return cost(c);
		else
			return max(cost_sand()*2, max(cost_grass()*2, cost_forest()*2));
	}
	
	static bool compPos(pair<Pos, int> p1, pair<Pos, int> p2)
	{
		return p1.second < p2.second;
	}
	
	static bool compEne(pair<Unit, vector<int> > p1, pair<Unit, vector<int> > p2) {
		if ((p1.second[0] == 1 or p1.second[1] == 1) and p2.second[0] == 0 and p2.second[1] == 0) {
			// p1 same city/path, p2 not same city/path ---> p1
			return true;
		}
		else if ((p2.second[0] == 1 or p2.second[1] == 1) and p1.second[0] == 0 and p1.second[1] == 0) {
			// p2 same city/path, p1 not same city/path ---> p2
			return false;
		}
		else {
			return p1.second[2] < p2.second[2];
		}
	}
	
	/***********************************
			 POSSIBILITIES queue
	 ***********************************/
	
	queue<Pos> which_cities(int ork)
	{
		vector< pair<Pos, int> > cities(nb_cities());
		
		for (int i = 0; i < nb_cities(); i++)
		{
			Pos min = city(i)[0];
			for (auto pos : city(i))
			{
				if (manhattan_distance(unit(ork).pos, pos) < manhattan_distance(unit(ork).pos, min))
				{
					min = pos;
				}
			}
			cities[i] = make_pair(min, manhattan_distance(min, unit(ork).pos));
		}
		
		sort(cities.begin(), cities.end(), compPos);
		
		queue<Pos> q;
		for (auto p : cities) {
			if (city_owner(cell(p.first).city_id) != me())
				q.push(p.first);
		}
		
		return q;
	}
	
	queue<Pos> which_paths(int ork)
	{
		vector< pair<Pos, int> > paths(0);
		
		for (int i = 0; i < nb_paths(); i++)
		{
			for (auto pos : path(i).second)
				paths.push_back(make_pair(pos, manhattan_distance(pos, unit(ork).pos)));
		}
		
		sort(paths.begin(), paths.end(), compPos);
		
		queue<Pos> q;
		for (auto p : paths) {
			if (path_owner(cell(p.first).path_id) != me() and cell(p.first).path_id != cell(unit(ork).pos).path_id)
				q.push(p.first);
		}
		
		return q;
	}
	
	queue<Unit> which_enemies(int ork)
	{
		vector< pair<Unit, vector<int> > > enemies(0);
		
		for (int i = 0; i < nb_units(); i++) {
			if (unit(i).player != me()) {
				vector<int> temp(3);
				temp[0] = cell(unit(ork).pos).city_id == -1 ? -1 : cell(unit(i).pos).city_id == cell(unit(ork).pos).city_id;	// same city
				temp[1] = cell(unit(ork).pos).path_id == -1 ? -1 : cell(unit(i).pos).path_id == cell(unit(ork).pos).path_id;	// same path
				temp[2] = manhattan_distance(unit(i).pos, unit(ork).pos);														// distance
				enemies.push_back(make_pair(unit(i), temp));
			}
		}
		
		sort(enemies.begin(), enemies.end(), compEne);
		
		queue<Unit> q;
		for (auto p : enemies) {
			q.push(p.first);
		}
		
		return q;
	}
	
    /***********************************
			 POSSIBILITIES
    ***********************************/
	void fill_map(vector< vector<Cell> > &map)
	{
		for (int i = 0; i < rows(); i++) {
			for (int j = 0; j < cols(); j++) {
				map[i][j] = cell(i, j);
			}
		}
	}

	void place_enemies(vector< vector<Cell> > &map)
	{
		for (int i = 0; i < nb_units(); i++)
		{
			Unit possible_enemy = unit(i);

			// If its an enemy
			if (possible_enemy.player != me())
			{
				map[possible_enemy.pos.i][possible_enemy.pos.j].type = CELL_TYPE_SIZE;

				if (pos_ok(possible_enemy.pos.i + 1, possible_enemy.pos.j))
					map[possible_enemy.pos.i + 1][possible_enemy.pos.j].type = CELL_TYPE_SIZE;

				if (pos_ok(possible_enemy.pos.i - 1, possible_enemy.pos.j))
					map[possible_enemy.pos.i - 1][possible_enemy.pos.j].type = CELL_TYPE_SIZE;

				if (pos_ok(possible_enemy.pos.i, possible_enemy.pos.j + 1))
					map[possible_enemy.pos.i][possible_enemy.pos.j + 1].type = CELL_TYPE_SIZE;

				if (pos_ok(possible_enemy.pos.i, possible_enemy.pos.j - 1))
					map[possible_enemy.pos.i][possible_enemy.pos.j - 1].type = CELL_TYPE_SIZE;
			}
		}
	}

	Pos which_city(int ork)
	{
		int cell_i = -1;
		int cell_j = -1;

		int ork_i = unit(ork).pos.i;
		int ork_j = unit(ork).pos.j;

		for (int i = 0; i < rows(); i++) {
			for (int j = 0; j < cols(); j++)
			{
				// Get cell
				Cell temp = cell(i, j);

				// Check if city
				if (temp.city_id != -1 and city_owner(temp.city_id) != me())
				{
					// Distance
					if (cell_i == -1 or abs(i - ork_i) + abs(j - ork_j) < abs(cell_i - ork_i) + abs(cell_j - ork_j)) {
						cell_i = i;
						cell_j = j;
					}
				}
			}
		}
		return Pos(cell_i, cell_j);
	}

	Pos which_path(int ork)
    {
		int path_i = -1;
		int path_j = -1;
		
		int ork_i = unit(ork).pos.i;
		int ork_j = unit(ork).pos.j;
		
		for (int i = 0; i < rows(); i++) {
			for (int j = 0; j < cols(); j++)
			{
				// Get cell
				Cell temp = cell(i, j);

				// Check if city
				if (temp.path_id != -1 and path_owner(temp.path_id) != me())
				{
					// Distance
					if (path_i == -1 or abs(i - ork_i) + abs(j - ork_j) < abs(path_i - ork_i) + abs(path_j - ork_j)) {
						path_i = i;
						path_j = j;
					}
				}
			}
		}

		return Pos(path_i, path_j);
	}

	Unit which_enemy(int ork)
    {
		Unit enemy;
		enemy.pos = {-1, -1};

		for (int i = 0; i < nb_units(); i++) {
			// nearer and (same city or path)
			if (unit(i).player != me() and ((manhattan_distance(unit(i).pos, unit(ork).pos) < manhattan_distance(enemy.pos, unit(ork).pos) and ((cell(unit(i).pos).city_id == cell(unit(ork).pos).city_id) or (cell(unit(i).pos).path_id == cell(unit(ork).pos).path_id))) or enemy.pos.i == -1)) {
				enemy = unit(i);
			}
		}

		return enemy;
	}

    /***********************************
                    A*
    ***********************************/
	int g(const int old_g, const Pos &now) {
		return old_g + cost(cell(now.i, now.j).type);
	}

	int h(const Pos &dest, const Pos &now) {
		return abs(now.i - dest.i) + abs(now.j - dest.j);
	}

	struct Node {
		int parent_i;
		int parent_j;

		int f;
		int g;
		int h;
	};

	bool isDestination(const int i, const int j, const Pos &destination) {
		return i == destination.i and j == destination.j;
	}

	Dir tracePath(const vector< vector<Node> > &cellDetails, Pos &dst, Pos &src) {
		int row = dst.i;
		int col = dst.j;

		stack< pair<int, int> > path;
		while (not (cellDetails[row][col].parent_i == row and cellDetails[row][col].parent_j == col)) {
			path.push(make_pair(row, col));
			int temp_row = cellDetails[row][col].parent_i;
			int temp_col = cellDetails[row][col].parent_j;
			row = temp_row;
			col = temp_col;
		}

		if (path.top().first - src.i == -1 and path.top().second == src.j) {
			return TOP;
		}

		if (path.top().first - src.i == 1  and path.top().second == src.j) {
			return BOTTOM;
		}

		if (path.top().first == src.i and path.top().second - src.j == 1 ) {
			return RIGHT;
		}

		if (path.top().first == src.i and path.top().second - src.j == -1) {
			return LEFT;
		}

		return DIR_SIZE;
	}

	Dir find_my_way(Pos src, Pos dst, int ork) {
		if (not pos_ok(src)) return DIR_SIZE;
		if (not pos_ok(dst)) return DIR_SIZE;
		if (src == dst) return NONE;

		// Closed list
		vector< vector<bool> > closedList(rows(), vector<bool> (cols(), false));

		// Path
		Node n;
		n.parent_i = -1;
		n.parent_j = -1;
		n.f = INT_MAX;
		n.g = INT_MAX;
		n.h = INT_MAX;
		vector< vector<Node> > cellDetails(rows(), vector<Node> (cols(), n));

		cellDetails[src.i][src.j].f = 0;
		cellDetails[src.i][src.j].g = 0;
		cellDetails[src.i][src.j].h = 0;
		cellDetails[src.i][src.j].parent_i = src.i;
		cellDetails[src.i][src.j].parent_j = src.j;

		set< pair<int, pair<int, int> > > openList;
		openList.insert(make_pair(0, make_pair(src.i, src.j)));
		bool foundDest = false;

		while (not openList.empty()) {
			// cerr << "SIZE: " << openList.size() << endl;
			pair< int, pair<int, int> > p = *openList.begin();

			openList.erase(openList.begin());
			int i = p.second.first;
			int j = p.second.second;
			closedList[i][j] = true;

			int new_f;
			int new_g;
			int new_h;

			// UP
			// cerr << "UP" << endl;
			if (pos_ok(i - 1, j) and cell(i - 1, j).type != WATER)
			{
				// Es mi destino
				if (isDestination(i - 1, j, dst)) {
					cellDetails[i - 1][j].parent_i = i;
					cellDetails[i - 1][j].parent_j = j;
					foundDest = true;
					return tracePath(cellDetails, dst, src);
				}

				// No es mi destino y hay que iterarlo
				else if (not closedList[i - 1][j]) {
					new_g = g(cellDetails[i][j].g, Pos(i - 1, j));
					new_h = h(dst, Pos(i - 1, j));
					new_f = new_g + new_h;

					if (cellDetails[i - 1][j].f == INT_MAX or cellDetails[i - 1][j].f > new_f) {
						openList.insert(make_pair(new_f, make_pair(i - 1, j)));

						cellDetails[i - 1][j].f = new_f;
						cellDetails[i - 1][j].g = new_g;
						cellDetails[i - 1][j].h = new_h;
						cellDetails[i - 1][j].parent_i = i;
						cellDetails[i - 1][j].parent_j = j;
					}
				}
			}
			// cerr << "FINISH UP" << endl;

			// RIGHT
			// cerr << "RIGHT" << endl;
			if (pos_ok(i, j + 1) and cell(i, j + 1).type != WATER)
			{
				// Es mi destino
				if (isDestination(i, j + 1, dst)) {
					cellDetails[i][j + 1].parent_i = i;
					cellDetails[i][j + 1].parent_j = j;
					foundDest = true;
					return tracePath(cellDetails, dst, src);
				}

				// No es mi destino y hay que iterarlo
				else if (not closedList[i][j + 1]) {
					new_g = g(cellDetails[i][j].g, Pos(i, j + 1));
					new_h = h(dst, Pos(i, j + 1));
					new_f = new_g + new_h;

					if (cellDetails[i][j + 1].f == INT_MAX or cellDetails[i][j + 1].f > new_f) {
						openList.insert(make_pair(new_f, make_pair(i, j + 1)));

						cellDetails[i][j + 1].f = new_f;
						cellDetails[i][j + 1].g = new_g;
						cellDetails[i][j + 1].h = new_h;
						cellDetails[i][j + 1].parent_i = i;
						cellDetails[i][j + 1].parent_j = j;
					}
				}
			}
			// cerr << "FINISH RIGHT" << endl;

			// BOTTOM
			// cerr << "BOTTOM" << endl;
			if (pos_ok(i + 1, j) and cell(i + 1, j).type != WATER)
			{
				// Es mi destino
				if (isDestination(i + 1, j, dst)) {
					cellDetails[i + 1][j].parent_i = i;
					cellDetails[i + 1][j].parent_j = j;
					foundDest = true;
					return tracePath(cellDetails, dst, src);
				}

				// No es mi destino y hay que iterarlo
				else if (not closedList[i + 1][j]) {
					new_g = g(cellDetails[i][j].g, Pos(i + 1, j));
					new_h = h(dst, Pos(i + 1, j));
					new_f = new_g + new_h;

					if (cellDetails[i + 1][j].f == INT_MAX or cellDetails[i + 1][j].f > new_f) {
						openList.insert(make_pair(new_f, make_pair(i + 1, j)));

						cellDetails[i + 1][j].f = new_f;
						cellDetails[i + 1][j].g = new_g;
						cellDetails[i + 1][j].h = new_h;
						cellDetails[i + 1][j].parent_i = i;
						cellDetails[i + 1][j].parent_j = j;
					}
				}
			}
			// cerr << "FINISH BOTTOM" << endl;

			// LEFT
			// cerr << "LEFT" << endl;
			if (pos_ok(i, j - 1) and cell(i, j - 1).type != WATER)
			{
				// Es mi destino
				if (isDestination(i, j - 1, dst)) {
					cellDetails[i][j - 1].parent_i = i;
					cellDetails[i][j - 1].parent_j = j;
					foundDest = true;
					return tracePath(cellDetails, dst, src);
				}

				// No es mi destino y hay que iterarlo
				else if (not closedList[i][j - 1]) {
					new_g = g(cellDetails[i][j].g, Pos(i, j - 1));
					new_h = h(dst, Pos(i, j - 1));
					new_f = new_g + new_h;

					if (cellDetails[i][j - 1].f == INT_MAX or cellDetails[i][j - 1].f > new_f) {
						openList.insert(make_pair(new_f, make_pair(i, j - 1)));

						cellDetails[i][j - 1].f = new_f;
						cellDetails[i][j - 1].g = new_g;
						cellDetails[i][j - 1].h = new_h;
						cellDetails[i][j - 1].parent_i = i;
						cellDetails[i][j - 1].parent_j = j;
					}
				}
			}
			// cerr << "FINISH LEFT" << endl;
		}

		// cerr << "RETURNING NONE" << endl;
		return NONE;
	}

	Dir runaway(Pos myself, Pos enemy, int ork) {
		if (cell(myself).city_id != -1) {
			City c = city(cell(myself).city_id);
			Pos max (-1, -1);
			for (auto p : c) {
				if (max.i == -1 or (manhattan_distance(p, enemy) > manhattan_distance(max, enemy))) {
					max = p;
				}
			}
			if (manhattan_distance(max, enemy) > 2) {
				cerr << "AVOID" << endl;
				return find_my_way(myself, max, ork);
			}
		}
		else if (cell(myself).path_id != -1) {
			cerr << "PATH...SHIT" << endl;
		}
		else {
			cerr << "NO_CITY" << endl;
		}
		return NONE;
	}

    /***********************************
                MAIN BRAIN
    ***********************************/
	Dir decide_direction(int ork)
	{
		Dir d = NONE;
		
		queue<Pos> cities = which_cities(ork);
		queue<Pos> paths = which_paths(ork);
		queue<Unit> enemies = which_enemies(ork);
		
		vector< vector<Cell> > map(rows(), vector<Cell> (cols()));
		fill_map(map);
		
		// FIGHT MODE
		while (d == NONE and my_actions[ork] == 1 and not enemies.empty() and manhattan_distance(enemies.front().pos, unit(ork).pos) <= 4 and enemies.front().health < unit(ork).health) {
			// cerr << "FIGHT" << endl;
			d = find_my_way(unit(ork).pos, enemies.front().pos, ork);
			enemies.pop();
		}
		
		// FLIGHT MODE
		while (d == NONE and my_actions[ork] == 1 and not enemies.empty() and manhattan_distance(enemies.front().pos, unit(ork).pos) <= 4 and enemies.front().health > unit(ork).health) {
			d = runaway(unit(ork).pos, enemies.front().pos, ork);
			enemies.pop();
		}
		
		// CONQUER MODE
		if (d == NONE) {
			// cerr << "CONQUER" << endl;
			if (cities.empty() and not paths.empty()) {
				// cerr << "NO CITY" << endl;
				do {
					d = find_my_way(unit(ork).pos, paths.front(), ork);
					paths.pop();
				} while (d == NONE and not paths.empty());
			}
			else if (paths.empty() and not cities.empty()) {
				// cerr << "NO PATH" << endl;
				do {
					d = find_my_way(unit(ork).pos, cities.front(), ork);
					cities.pop();
				} while (d == NONE and not cities.empty());
			}
			else
			{
				// cerr << "PATH and CITY" << endl;
				// EQUAL (default --> city)
				if (bonus_per_city_cell() == bonus_per_path_cell())
				{
					while (d == NONE and not cities.empty() and not paths.empty())
					{
						int dist_ork_city = manhattan_distance(cities.front(), unit(ork).pos);
						int dist_ork_path = manhattan_distance(paths.front(), unit(ork).pos);
						
						if (dist_ork_city < dist_ork_path) {
							d = find_my_way(unit(ork).pos, cities.front(), ork);
							cities.pop();
						}
						else if (dist_ork_path < dist_ork_city) {
							d = find_my_way(unit(ork).pos, paths.front(), ork);
							paths.pop();
						}
						else {
							d = find_my_way(unit(ork).pos, cities.front(), ork);
							cities.pop();
						}
					}
				}
				
				// TODO: CITY
				else if (bonus_per_city_cell() > bonus_per_path_cell())
				{
					d = find_my_way(unit(ork).pos, cities.front(), ork);
				}
				
				// TODO: PATH
				else {
					d = find_my_way(unit(ork).pos, paths.front(), ork);
				}
			}
			
			// Si estoy en una ciudad o un path
			// cerr << (cell(unit(ork).pos).city_id != -1 or cell(unit(ork).pos).path_id != -1) << endl;
			// cerr << my_actions.size() << endl;
			if (cell(unit(ork).pos).city_id != -1 or cell(unit(ork).pos).path_id != -1)
			{
				my_actions[ork] = 1;
			}
		}
		
		return d;
	}

    /***********************************
                    MAIN
    ***********************************/
    /**
    * Play method, invoked once per each round.
    */
    virtual void play () {
		init();
		for (int i = 0; i < my_units; i++) {
			execute(Command(my_orks[i], decide_direction(my_orks[i])));
			// cerr << "FINISH COMMAND" << endl;
		}
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
