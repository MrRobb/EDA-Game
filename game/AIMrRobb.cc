#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME MrRobb


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
	int instruction = 1;
	
	vector<int> my_orks;
	int my_units;

	// INIT
	void init(){
		my_orks = orks(me());
		my_units = int(my_orks.size());
	}
	
	/////////////////////////////// COMPUTE
	Pos which_city(int ork) {
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
				if (temp.city_id != -1)
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
				if (temp.path_id != -1)
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
		cerr << "Found" << endl;
		if (path.top().first - src.i == -1 and path.top().second == src.j) {
			cerr << "TOP" << endl;
			return TOP;
		}
		
		if (path.top().first - src.i == 1  and path.top().second == src.j) {
			cerr << "BOTTOM" << endl;
			return BOTTOM;
		}
		
		if (path.top().first == src.i and path.top().second - src.j == 1 ) {
			cerr << "RIGHT" << endl;
			return RIGHT;
		}
		
		if (path.top().first == src.i and path.top().second - src.j == -1) {
			cerr << "LEFT" << endl;
			return LEFT;
		}
		cerr << "ERROR" << endl;
		return DIR_SIZE;
	}
	
	Dir find_my_way(Pos src, Pos dst) {
		cerr << "Finding my way from " << src << " to " << dst << endl;
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
			
			// UP
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
			
			// RIGHT
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
			
			// BOTTOM
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
			
			// LEFT
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
		}
		
		return DIR_SIZE;
	}
	
	Dir decide_direction(int ork) {
		cerr << "ork_id: " << ork << endl;
		Dir d = DIR_SIZE;
		do {
			Pos c = which_city(ork);
			Pos p = which_path(ork);
			
			cerr << "finding from " << unit(ork).pos << " to " << c  << " or " << p << endl;
			// EQUAL (default --> city)
			if (bonus_per_city_cell() == bonus_per_path_cell())
			{
				// TO-DO: SHORTEST DISTANCE (city vs path)
				int dist_ork_city = abs(c.i - unit(ork).pos.i) + abs(c.j - unit(ork).pos.j);
				int dist_ork_path = abs(p.i - unit(ork).pos.i) + abs(p.j - unit(ork).pos.j);
				
				if (dist_ork_city < dist_ork_path) {
					d = find_my_way(unit(ork).pos, c);
				}
				else if (dist_ork_path < dist_ork_city) {
					d = find_my_way(unit(ork).pos, p);
				}
				else {
					d = (random(-1, 1) >= 0) ? find_my_way(unit(ork).pos, c) : find_my_way(unit(ork).pos, p);
				}
			}
			
			// CITY
			else if (bonus_per_city_cell() > bonus_per_path_cell())
			{
				d = find_my_way(unit(ork).pos, c);
			}
			
			// PATH
			else {
				d = find_my_way(unit(ork).pos, p);
			}
			
			if (d == DIR_SIZE) cerr << "is DIR_SIZE :(" << endl;
		} while (d == DIR_SIZE);
		
		return d;
	}
	
	/////////////////////////////// MAIN
    /**
    * Play method, invoked once per each round.
    */
    virtual void play () {
		cerr << "INIT STARTED" << endl;
		init();
		cerr << "INIT FINISHED" << endl;
		for (int i = 0; i < my_units and instruction < 1000; i++) {
			execute(Command(my_orks[i], decide_direction(my_orks[i])));
			instruction++;
		}
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
