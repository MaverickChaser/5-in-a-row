#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <cassert>

#include <unistd.h>

#define pii pair<int, int>
#define mp make_pair

#define forn(var, start, end) for (int var=start; var<=end; var++)

typedef long long ll;
using namespace std;

const int N = 40;
const int MID = N / 2; //N / 2;
const int DEPTH = 6;
const int DELTA = 10;

const char chr[] = { '.', 'O', 'X' };
const int coef[] = { 2, 1, 4, 2, 200, 3 };

// <-- global vars -->
int a[N][N];
pii best_move = mp(-1, -1);
pii L, R;
int cur_player = 1, winner = -1, game_started = 0;
vector<int> p1_pos(7, 0), p2_pos(7, 0);
struct position {
    pii move, L, R;
    position(){}
    position(pii move, pii L, pii R): move(move), L(L), R(R) {}
};

vector<position> Moves;
// <-- end global vars -->
void count_features(vector<int> & pos, int who);

void applyMove(int i, int j, int who, int check = 0) {
    a[i][j] = who;
    Moves.push_back(position(mp(i, j), L, R));
    if (check) {
        count_features(p1_pos, who);
        if (p1_pos.back()) {
            winner = who;
        }
    }
    // /int h = R.first - L.first, w = R.second - L.second;
    if (i == L.first) {
        L.first--;
    }
    if (i == R.first) {
        R.first++;
    }
    if (j == L.second) {
        L.second--;
    }
    if (j == R.second) {
        R.second++;
    }
}

void deapplyMove() {
    position pos = Moves.back(); Moves.pop_back();
    a[pos.move.first][pos.move.second] = -1, L = pos.L, R = pos.R;
}

int cost(const vector<int> & pos, const vector<int> & op_pos, int who) {
    int one_move_win_fi = 0, one_move_win_se = 0;
    
    if (pos[6]) return 300;
    if (op_pos[6]) return -300;
    
    forn(i, 4, 6) {
        if (pos[i])
            one_move_win_fi = 1;
        if (op_pos[i])
            one_move_win_se = 1;
    }

    if (one_move_win_fi || (pos[2] && !one_move_win_se)) return 200;
    if (op_pos[4]) return -200;

    int ans = 0;
    forn(i, 0, 5) {
        ans += coef[i] * (pos[i] - op_pos[i]);
    }
    return ans;
}

bool can(int i, int j) {
    if (a[i][j] != -1) return 0;
    forn(dy, -1, 1)
    forn(dx, -1, 1)
    if (a[i + dy][j + dx] > -1)
        return 1;
    return 0;
}

void count_features(vector<int> & pos, int who) {
    int ub = L.first, db = R.first, lb = L.second, rb = R.second;   
    pii tp[] = { mp(1, 0), mp(0, 1), mp(1, 1), mp(1, -1) };
    int best = 0;
    
    forn(i, ub, db) {
        forn(j, lb, rb) {
            for (pii p : tp) {
                int u = p.first, v = p.second;
                int inc1 = 0, inc2 = 0;
                if (a[i - u][j - v] != who) {
                    int k = 0;
                    for (k = 0; k < 5 && a[i + u * k][j + v * k] == who; k++);
                    
                    if (a[i + u * k][j + v * k] == -1 && a[i - u][j - v] == -1) {
                        inc1 = 1;
                    }
                    else if (a[i + u * k][j + v * k] == -1 || a[i - u][j - v] == -1) {
                        inc2 = 1;
                    }
                    
                    if (k < 2) continue;
                    
                    if (k == 5 || inc1) {
                        pos[2 * k - 4]++;
                    }
                    else if (inc2) {
                        pos[2 * k - 3]++;
                    }
                    
                    /*
                    if (ret_val && k >= 3 && k > best) {
                        best = k;
                        if (a[i + u * k][j + v * k] == -1) {
                            best_move = mp(i + u * k - L.first, j + v * k - L.second);
                        }
                        else if (a[i - u][j - v] == -1) {
                            best_move = mp(i - u * k - L.first, j - v * k - L.second);
                        }
                    }
                    */

                }
            }
        }
    }
}


int minimax(int depth, int who, int alpha, int beta) {
    //int ret = 0;
    //if (depth == DEPTH) ret = 1;
    p1_pos.assign(7, 0); 
    p2_pos.assign(7, 0);
    count_features(p1_pos, who);
    count_features(p2_pos, !who);
    
    int sgn = (who == cur_player ? 1 : -1);
    int cur_cost = cost(p1_pos, p2_pos, who) * sgn;
    
    if (abs(cur_cost) > 90 && depth < DEPTH) {
        return cur_cost;
    } 
    
    if (depth == 0)
        return cur_cost;
    
    vector<pair<int, pii> > all_moves;

    forn(i, L.first, R.first) {
        forn(j, L.second, R.second) {
            if (can(i, j) || !game_started) {
                applyMove(i, j, who);
                // count new_score
                int new_score = minimax(0, !who, alpha, beta);
                all_moves.push_back(mp(new_score, mp(i, j)));
                // restore values
                deapplyMove();
            }
        }
    }
    
    sort(all_moves.begin(), all_moves.end(), greater<pair<int, pii> >());
    pii move = mp(-1, -1);
    for (int i=0; i<3 && i<all_moves.size(); i++) {
        pii current_move = all_moves[i].second;
        applyMove(current_move.first, current_move.second, who);
        int new_score = minimax(depth - 1, !who, alpha, beta);
        if (who == cur_player) {
            if (new_score > alpha) {
                alpha = new_score;
                move = current_move;
            }
        }
        else {
            if (new_score < beta) {
                beta = new_score;
                move = current_move;
            }
        }
        deapplyMove();
    }
    // update global best_move
    best_move = move;
    return (who == cur_player ? alpha : beta);
    
}

void RunGame() {
    // initiliaze vars
    forn(i, 0, N - 1) forn(j, 0, N - 1) a[i][j] = -1;
    cur_player = 1, L = mp(MID, MID), R = mp(MID, MID);
    pii was = mp(-1, -1), cur;
    // 

    ofstream of("ai");
    minimax(DEPTH, cur_player, -200, 200);
    
    if (best_move == mp(-1, -1)) {
        assert(0);
    }
    // SET
    applyMove(best_move.first, best_move.second, cur_player);
    
    cur_player ^= 1;
    
    printf("1 %d %d\n", best_move.first, best_move.second);
    of << 1 << " " << best_move.first << " " << best_move.second;
    of.close();
    game_started = 1;
    int X = 2, O = 1;

    while (winner == -1) {    
        ifstream in("human");
        int num;
        in >> num >> cur.first >> cur.second;
        in.close();


        if (num == O) {
            O += 1;
            printf("%s\n", "HUMAN move ");
            ofstream of("ai");
            if (!can(cur.first, cur.second)) {
                of << -2 << " " << -2 << " " << -2;
                of.close();
                continue;
            }

            //printf("--> OK!\n");
            was = cur;
            applyMove(cur.first, cur.second, cur_player, 1);
            cur_player ^= 1;

            minimax(DEPTH, cur_player, -200, 200);
            
            if (best_move == mp(-1, -1)) {
                forn(y, L.first, R.first) {
                    forn(x, L.second, R.second) {
                        printf("%c", chr[a[y][x] + 1]);
                    }
                    puts("");
                }
                assert(0);
            }
            // SET
            applyMove(best_move.first, best_move.second, cur_player, 1);

            // DEBUG OUTPUT
            printf("%s %d %d\n", "AI: ", best_move.first, best_move.second);
                
            printf("%d\n", X);
            forn(y, L.first, R.first) {
                forn(x, L.second, R.second) {
                    printf("%c", chr[a[y][x] + 1]);
                }
                puts("");
            }

            //
            
            cur_player ^= 1;
            
            
            of << X << " " << best_move.first << " " << best_move.second;
            of.close();
            X++;
        }

        usleep(1000);
        /*
        forn(y, L.first, R.first) {
            forn(x, L.second, R.second) {
                printf("%c", chr[a[y][x] + 1]);
            }
            puts("");
        }
        */

    }
    usleep(1000000);
    ofstream off("ai");
    off << -2 << " " << -2 << " " << winner;
    off.close();
}

int main() {
    //int n;
    //cin >> n;
    
    RunGame();
    
    /*
     forn(i, MID+1, n+MID)
     forn(j, MID+1, n+MID)
     cin>>a[i][j];
     */
    
    return 0;
}