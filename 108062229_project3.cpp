#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <map>

using namespace std;

struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
}};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
map<int, Point> sav_table;

bool is_spot_on_board(Point p) { 
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
int get_disc(Point p) {
    return board[p.x][p.y];
}
void set_disc(Point p, int disc) {
    board[p.x][p.y] = disc;
}
bool is_disc_at(Point p, int disc) {
    if (!is_spot_on_board(p))
        return false;
    if (get_disc(p) != disc)
       return false;
    return true;
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

int get_sav(Point p) {
    int final_v = -1;
    int tmp_v = 0;
    for (Point dir: directions) {
        Point test_p = p + dir;
        if (!is_disc_at(test_p, 3 - player)) continue;
        tmp_v++;
        test_p = test_p + dir;
        while (is_spot_on_board(test_p) && get_disc(test_p) != 0) {
            if (is_disc_at(test_p, player)) {
                final_v = tmp_v;
                break;
            }
            tmp_v++;
            test_p = test_p + dir;
        }
    }
   return final_v;
}

/*
Point minmax(Point p, int depth, int alpha, int beta, int cur_player) {
    if(!depth || board.done())
}
*/
void write_valid_spot(std::ofstream& fout) {
    //int n_valid_spots = next_valid_spots.size();
    for(Point p : next_valid_spots) 
        sav_table[get_sav(p)] = p;
    // Remember to flush the output to ensure the last action is written to file.
    map<int, Point>::iterator iter = sav_table.end();
    iter--;
    Point target_p = iter->second;
    fout << target_p.x << " " << target_p.y << std::endl;
    fout.flush();
}


int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
