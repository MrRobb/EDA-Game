#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME MrRobb_v2


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

	// INIT
	void init(){
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
	
	/////////////////////////////// COMPUTE
	int manhattan_distance(const Pos &p1, const Pos &p2) {
		return abs(p1.i - p2.i) + abs(p1.j - p2.j);
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
	
	Pos which_path(int ork) {
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
	
	Unit which_enemy(int ork) {
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
	
	Dir decide_direction(int ork) {
		Dir d = NONE;
		// cerr << "START WHICH CITY" << endl;
		Pos c = which_city(ork);
		// cerr << "FINISH WHICH CITY" << endl;
		// cerr << "START WHICH PATH" << endl;
		Pos p = which_path(ork);
		// cerr << "FINISH WHICH PATH" << endl;
		// cerr << "START WHICH ENEMY" << endl;
		Unit e = which_enemy(ork);
		// cerr << "FINISH WHICH ENEMY" << endl;
		
		if (my_actions[ork] == 1 and manhattan_distance(e.pos, unit(ork).pos) < 4) {
			// cerr << "FIGHT from " << unit(ork).pos << " to " << e.pos << endl;
			d = find_my_way(unit(ork).pos, e.pos, ork);
		}
		else {
			// cerr << "CONQUER" << endl;
			if (c.i == -1 and c.j == -1 and p.i != -1 and p.j != -1) {
				// cerr << "NO CITY" << endl;
				d = find_my_way(unit(ork).pos, p, ork);
			}
			else if (p.i == -1 and p.j == -1 and c.i != -1 and c.j != -1) {
				// cerr << "NO PATH" << endl;
				d = find_my_way(unit(ork).pos, c, ork);
			}
			else {
				// cerr << "PATH and CITY" << endl;
				// EQUAL (default --> city)
				if (bonus_per_city_cell() == bonus_per_path_cell())
				{
					int dist_ork_city = abs(c.i - unit(ork).pos.i) + abs(c.j - unit(ork).pos.j);
					int dist_ork_path = abs(p.i - unit(ork).pos.i) + abs(p.j - unit(ork).pos.j);
					
					if (dist_ork_city < dist_ork_path) {
						// cerr << "GO CITY" << c << endl;
						d = find_my_way(unit(ork).pos, c, ork);
						// cerr << "WENT CITY" << endl;
					}
					else if (dist_ork_path < dist_ork_city) {
						// cerr << "GO PATH " << p << endl;
						d = find_my_way(unit(ork).pos, p, ork);
						// cerr << "WENT PATH" << endl;
					}
					else if (pos_ok(c)) {
						d = (random(-1, 1) >= 0) ? find_my_way(unit(ork).pos, c, ork) : find_my_way(unit(ork).pos, p, ork);
					}
				}
				
				// CITY
				else if (bonus_per_city_cell() > bonus_per_path_cell())
				{
					d = find_my_way(unit(ork).pos, c, ork);
				}
				
				// PATH
				else {
					d = find_my_way(unit(ork).pos, p, ork);
				}
			}
			
			// Si estoy en una ciudad o un path
			// cerr << (cell(unit(ork).pos).city_id != -1 or cell(unit(ork).pos).path_id != -1) << endl;
			// cerr << my_actions.size() << endl;
			if (cell(unit(ork).pos).city_id != -1 or cell(unit(ork).pos).path_id != -1) {
				my_actions[ork] = 1;
			}
		}
		
		// cerr << "d: " << d << " " << NONE << endl;
		return d;
	}
	
	/////////////////////////////// MAIN
    /**
    * Play method, invoked once per each round.
    */
    virtual void play () {
		init();
		for (int i = 0; i < my_units; i++) {
			// cerr << "START COMMAND -> ork: " << i << " of " << my_orks.size() << " vs " << orks(me()).size() << endl;
			execute(Command(my_orks[i], decide_direction(my_orks[i])));
			// cerr << "FINISH COMMAND" << endl;
		}
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
