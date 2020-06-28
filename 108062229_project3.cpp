#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <map>
#include <set>

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

map<int, Point> sav_table;
int AIplayer;

class GameBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    GameBoard() {
        reset();
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    void put_disc(Point p) {
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
    }
    int get_sav() {
        set<int> final_v;
        for(Point p : next_valid_spots) {
            int tmp_v = 0;
            for (Point dir: directions) {
                Point test_p = p + dir;
                if (!is_disc_at(test_p, 3 - cur_player)) continue;
                tmp_v++;
                test_p = test_p + dir;
                while (is_spot_on_board(test_p) && get_disc(test_p) != 0) {
                    if (is_disc_at(test_p, cur_player)) {
                        break;
                    }
                    tmp_v++;
                    test_p = test_p + dir;
                }
            }
            final_v.insert(tmp_v);
        }
        return *final_v.rbegin();
    }/*
    void operator=(GameBoard rhs) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = rhs.board[i][j];
            }
        }
        for(int i = 0; i < 3; i++) disc_count[i] = rhs.disc_count[i];
        cur_player = rhs.cur_player;
        next_valid_spots = rhs.next_valid_spots;
        done = rhs.done;
        winner = rhs.winner;
    }*/
};


GameBoard state;

void read_board(std::ifstream& fin) {
    fin >> state.cur_player;
    AIplayer = state.cur_player;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            fin >> state.board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        state.next_valid_spots.push_back({x, y});
    }
}

int minmax(GameBoard state, int depth, int alpha, int beta) {
    if(state.done) {
        if(state.winner == AIplayer) return 100;
        else return -1;  
    }
    if(!depth) {
        return state.get_sav();
    }
    if(state.cur_player == AIplayer) {
        int max_sav = -1;
        for(auto p : state.next_valid_spots) {
            GameBoard next_state = state;
            next_state.put_disc(p);
            int max_val = minmax(next_state, depth - 1, alpha, beta);
            max_sav = max(max_sav, max_val);
            if(depth == 3) sav_table[max_sav] = p;
            alpha = max(alpha, max_val);
            if(beta <= alpha) break;
        }
        return max_sav;
    }
    else {
        int min_sav = 100;
        for(auto p : state.next_valid_spots) {
            GameBoard next_state = state;
            next_state.put_disc(p);
            int min_val = minmax(next_state, depth - 1, alpha, beta);
            min_sav = min(min_sav, min_val);
            if(depth == 3) sav_table[min_sav] = p;
            beta = min(beta, min_val);
            if(beta <= alpha) break;
        }
        return min_sav;
    }
}

void write_valid_spot(std::ofstream& fout) {
    //int n_valid_spots = next_valid_spots.size();
    int ord = minmax(state, 3, -INT_MAX, INT_MAX);
    // Remember to flush the output to ensure the last action is written to file.
    Point target_p = sav_table[ord];
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
