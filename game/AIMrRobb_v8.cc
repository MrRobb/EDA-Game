#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME MrRobb_v8
#include <list>

struct PLAYER_NAME : public Player {

    /**
    * Factory: returns a new instance of this class.
    * Do not modify this function.
    */
    static Player* factory () {
        return new PLAYER_NAME;
    }
	
	typedef vector< vector< pair<int, int> > > Graph;
    /**
    * Types and attributes for your player can be defined here.
    */
	bool starting = true;
	float bonus_cities = 1;
	float bonus_path = 1;
	vector<int> my_orks;
	vector<int> my_actions;
	vector< list< pair<Pos, Dir> > > directions;
	vector< queue< pair<int, int> > > plan;
	int my_units;
	Graph graph;
	int left_cities = 0;

    /***********************************
                INIT
    ***********************************/
	void printGraph() {
		for(int i = 0; i < nb_cities(); i++) {
			cerr << i << ":";
			for (auto el : graph[i]) {
				cerr << " (" << el.first << ", " << el.second << ")";
			}
			cerr << endl;
		}
	}
	
	void storeGraph() {
		// ciudad_id --> {ciudad_id , path_id}, {ciudad_id, path_id}
		for (int i = 0; i < nb_paths(); i++) {
			pair<int, int> p = path(i).first;
			graph[p.first].push_back(make_pair(p.second, i));
			graph[p.second].push_back(make_pair(p.first, i));
		}
	}
	
	void init()
    {
    	my_orks = orks(me());
		my_units = int(my_orks.size());
		left_cities = left();
		
		if (starting) {
			/*
			 ACTIONS:
				 -1 --> not my ork
				 0 ---> looking for a city
				 > 0 ---> fight or flight and bfs
			 */
			if (bonus_per_path_cell() != 0) {
				bonus_cities = bonus_per_city_cell() / bonus_per_path_cell();
			}
			if (bonus_per_city_cell() != 0) {
				bonus_path = bonus_per_path_cell() / bonus_per_city_cell();
			}
			graph = Graph (nb_cities());
			storeGraph();
			printGraph();
			directions = vector< list< pair<Pos, Dir> > >(nb_units());
			my_actions = vector<int> (nb_units(), -1);
			plan = vector< queue< pair<int, int> > > (nb_units());
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

	int points_city(const City &c)
	{
		return int(c.size()) * bonus_per_city_cell();
	}

	int points_path(const Path &p) {
		return int(p.second.size()) * bonus_per_path_cell();
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
	
	Pos nearestPosToCity(const Pos &p, const City &c) {
		Pos min = c[0];
		for (auto pos : c)
		{
			if (manhattan_distance(p, pos) < manhattan_distance(p, min))
			{
				min = pos;
			}
		}
		return min;
	}
	
	Pos centerPosToPath(const Path &pa) {
		return pa.second[pa.second.size()/2];
	}

	/***********************************
			 POSSIBILITIES queue
	 ***********************************/
	
	int isla(City c) {
		for (auto pos : c) {
			if (pos_ok(pos + BOTTOM) and cell(pos + BOTTOM).type == PATH) return 0;
			if (pos_ok(pos + TOP) and cell(pos + TOP).type == PATH) return 0;
			if (pos_ok(pos + LEFT) and cell(pos + LEFT).type == PATH) return 0;
			if (pos_ok(pos + RIGHT) and cell(pos + RIGHT).type == PATH) return 0;
		}
		return 100;
	}
	
	queue< pair<int, int> > bfs(int c)
	{
		queue< pair<int, int> > q;
		queue<int> t;
		vector<bool> visited(nb_cities(), false);
		
		t.push(c);
		while (not t.empty())
		{
			int node = t.front();
			for (auto adj : graph[node])
			{
				if (not visited[adj.first]) {
					q.push(adj);
					q.push(make_pair(node, adj.second));
					t.push(adj.first);
				}
			}
			
			visited[node] = true;
			t.pop();
		}
		
		return q;
	}
	
	void which_cities(int ork, queue<Pos> &q)
	{
		vector< pair<Pos, int> > cities(nb_cities());

		for (int i = 0; i < nb_cities(); i++)
		{
			City c = city(i);
			Pos min = nearestPosToCity(unit(ork).pos, c);
			cities[i] = make_pair(min, manhattan_distance(min, unit(ork).pos) + isla(city(i)));
		}

		sort(cities.begin(), cities.end(), compPos);
		
		for (auto p : cities) {
			q.push(p.first);
		}
	}

	void which_enemies(int ork, queue<Unit> &q)
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
		
		for (auto p : enemies) {
			q.push(p.first);
		}
	}

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
	
	void place_me(vector< vector<Cell> > &map)
	{
		for (int i = 0; i < nb_units(); i++)
		{
			Unit u = unit(i);
			if (u.player == me())
			{
				map[u.pos.i][u.pos.i].type = CELL_TYPE_SIZE;
			}
		}
	}

	bool enemy_in_pos(Pos p, int ork)
	{
		if (cell(p).type == WATER) return true;
		for (int i = 0; i < nb_units(); i++) {
			Unit u = unit(i);
			if (u.player != me() and u.health - cost(cell(p).type) >= unit(ork).health and
				(u.pos == p or
				 u.pos + Pos( 0, 1) == p or
				 u.pos + Pos( 0,-1) == p or
				 u.pos + Pos( 1, 0) == p or
				 u.pos + Pos( 1, 1) == p or
				 u.pos + Pos( 1,-1) == p or
				 u.pos + Pos(-1, 0) == p or
				 u.pos + Pos(-1, 1) == p or
				 u.pos + Pos(-1,-1) == p))
				return true;
			if (u.player == me() and ork != u.id and p == u.pos)
				return true;
		}
		return false;
	}

    /***********************************
                    A*
    ***********************************/
	int g(const int old_g, const Pos &now) {
		return old_g + 5 * cost(cell(now.i, now.j).type);
	}

	int h(const Pos &dest, const Pos &now) {
		return manhattan_distance(now, dest);
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
			pair< int, pair<int, int> > p = *openList.begin();

			openList.erase(openList.begin());
			int i = p.second.first;
			int j = p.second.second;
			closedList[i][j] = true;

			int new_f;
			int new_g;
			int new_h;
			
			vector< pair<int, int> > pos = {{i - 1, j}, {i, j + 1},{i + 1, j},{i, j - 1}};
			for (int k = 0; k < 4; k++)
			{
				if (pos_ok(pos[k].first, pos[k].second) and cell(pos[k].first, pos[k].second).type != WATER) {
					if (isDestination(pos[k].first, pos[k].second, dst)) {
						cellDetails[pos[k].first][pos[k].second].parent_i = i;
						cellDetails[pos[k].first][pos[k].second].parent_j = j;
						foundDest = true;
						return tracePath(cellDetails, dst, src);
					}
					
					else if (not closedList[pos[k].first][pos[k].second]) {
						new_g = g(cellDetails[i][j].g, Pos(pos[k].first, pos[k].second));
						new_h = h(dst, Pos(pos[k].first, pos[k].second));
						new_f = new_g + new_h;
						
						if (cellDetails[pos[k].first][pos[k].second].f == INT_MAX or cellDetails[pos[k].first][pos[k].second].f > new_f) {
							openList.insert(make_pair(new_f, make_pair(pos[k].first, pos[k].second)));
							
							cellDetails[pos[k].first][pos[k].second].f = new_f;
							cellDetails[pos[k].first][pos[k].second].g = new_g;
							cellDetails[pos[k].first][pos[k].second].h = new_h;
							cellDetails[pos[k].first][pos[k].second].parent_i = i;
							cellDetails[pos[k].first][pos[k].second].parent_j = j;
						}
					}
				}
			}
		}

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
				return find_my_way(myself, max, ork);
			}
		}
		else if (cell(myself).path_id != -1) {
			Path pa = path(cell(myself).path_id);
			Pos max (-1, -1);
			bool found = false;
			for (int i = 0; i < pa.second.size(); i++) {
				if (not found and pa.second[i] == myself) {
					if (i > 0)
						return find_my_way(myself, pa.second[i - 1], ork);
					else
						return NONE;
				}
				else if (found and pa.second[i] == myself) {
					if (i + 1 < pa.second.size())
						return find_my_way(myself, pa.second[i + 1], ork);
					else
						return NONE;
				}
				if (pa.second[i] == enemy) {
					found = true;
				}
			}
		}
		return NONE;
	}

    /***********************************
                MAIN BRAIN
    ***********************************/
	int left() {
		int num = 0;
		for (int i = 0; i < nb_cities(); i++) {
			if (city_owner(i) == -1) ++num;
		}
		return num;
	}
	
	void moving(int ork, Dir &d) {
		if (not directions[ork].empty()) {
			auto po = directions[ork].front();
			
			// Si lleva sin moverse un par de rondas
			for (auto p : directions[ork]) {
				if (p.first != po.first) return;
				if (p.second != po.second) return;
			}
			
			// Escoger dir random
			vector<Dir> v = {BOTTOM, TOP, LEFT, RIGHT};
			random_shuffle(v.begin(), v.end());
			int i = 0;
			while (i < 4 and v[i] != d and not pos_ok(unit(ork).pos + v[i])) {
				++i;
			}
			d = (i == 4 ? NONE : v[i]);
		}
	}
	
	bool adjacent_free(int city_id, int path_id) {
		
		if (city_id != -1) {
			return true;
		}
		if (path_id != -1) {
			return true;
		}
		return false;
	}
	
	Dir bfs_find_my_way(int ork)
	{
		Pos p = unit(ork).pos;
		Dir d = directions[ork].front().second;
		if (not directions[ork].empty()) {
			if (d != NONE and (cell(p + d).type == CITY or cell(p + d).type == PATH)) {
				return d;
			}
			else {
				if (d == LEFT) return BOTTOM;
				if (d == BOTTOM) return RIGHT;
				if (d == RIGHT) return TOP;
				return LEFT;
			}
		}
		return NONE;
	}
	
	void print_queue(queue< pair<int, int> > q) {
		while (not q.empty()) {
			cerr << "(" << q.front().first << "-->" << q.front().second << ")" << endl;
			q.pop();
		}
		cerr << endl;
	}
	
	Dir decide_direction(int ork)
	{
		Dir d;

		d = NONE;
		
		queue<Pos> cities;
		which_cities(ork, cities);
		
		queue<Unit> enemies;
		which_enemies(ork, enemies);

		// map = vector< vector<Cell> >(rows(), vector<Cell> (cols()));
		// fill_map(map);
		// place_enemies(map);
		// place_me(map);

		// Si estoy en una ciudad o un path
		
		// FIGHT MODE
//		while ((d == NONE) and not enemies.empty() and my_actions[ork] >= 0 and ((manhattan_distance(enemies.front().pos, unit(ork).pos) <= 10 and left_cities == 0) or (manhattan_distance(enemies.front().pos, unit(ork).pos) <= 3)) and enemies.front().health < unit(ork).health ) {
//			d = find_my_way(unit(ork).pos, enemies.front().pos, ork);
//			enemies.pop();
//		}
//
//		// FLIGHT MODE
//		while ((d == NONE or enemy_in_pos(unit(ork).pos + d, ork)) and not enemies.empty() and my_actions[ork] >= 0 and manhattan_distance(enemies.front().pos, unit(ork).pos) <= 10 and enemies.front().health > unit(ork).health) {
//			d = runaway(unit(ork).pos, enemies.front().pos, ork);
//			enemies.pop();
//		}
		
		// CONQUER MODE
		
		if (d == NONE and my_actions[ork] >= 0) {
			d = bfs_find_my_way(ork);
			
			if (d == NONE) {
				my_actions[ork] = -1;
			}
		}
		
		// Go to the nearest city
		if (d == NONE and my_actions[ork] == -1)
		{
			while ((d == NONE or enemy_in_pos(unit(ork).pos + d, ork)) and not cities.empty())
			{
				d = find_my_way(unit(ork).pos, cities.front(), ork);
				cities.pop();
			}
			
			if (cell(unit(ork).pos).city_id != -1)  {
				my_actions[ork] = cell(unit(ork).pos).city_id;
				plan[ork] = bfs(my_actions[ork]);
				print_queue(plan[ork]);
			}
		}
		
		// moving(ork, d);
			
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
			Dir d = decide_direction(my_orks[i]);
			directions[i].push_back(make_pair(unit(my_orks[i]).pos, d));
			if (round() >= 4) directions[i].pop_front();
			execute(Command(my_orks[i], d));
		}
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
