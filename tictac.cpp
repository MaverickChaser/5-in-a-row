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
// <-- end global vars -->

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
    
    forn(i, ub + 1, db - 1) {
        forn(j, lb + 1, rb - 1) {
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

int last_cost = 0;
// players position

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
    
    // prunning
    
    //    if (abs(cur_cost) < abs(last_cost)) {
    //        return last_cost = cur_cost;
    //    }
    
    pii move = mp(-1, -1);
    
    forn(i, L.first, R.first) {
        forn(j, L.second, R.second) {
            if (can(i, j) || !game_started) {
                if (a[L.first+3][L.second+2] == 0 && a[i + 1][j - 1] == 1 && a[i + 2][j - 2] == 1 && a[i+3][j-3]==1 && a[i+4][j-4]==1 && who == 1) {
                    //cerr << "c";
                }
                //printf("%d %d\n", i, j);
                a[i][j] = who;
                pii tmp1 = L, tmp2 = R;
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
                a[i][j] = who;
                
                // count new_score
                int new_score = minimax(depth - 1, !who, alpha, beta);
                last_cost = new_score;
                
                // update move
                if (who == cur_player) {
                    if (new_score > alpha) {
                        alpha = new_score;
                        move = mp(i - tmp1.first, j - tmp1.second);
                    }
                }
                else {
                    if (new_score < beta) {
                        beta = new_score;
                        move = mp(i - tmp1.first, j - tmp1.second);
                    }
                }
                
                // restore values
                L = tmp1, R = tmp2;
                a[i][j] = -1;
                
                if (alpha >= beta) {
                    break;
                }
            }
        }
    }
    
    // update global best_move
    best_move = move;
    return (who == cur_player ? alpha : beta);
    
}

void applyMove(int i, int j, int who) {
    a[L.first + i][L.second + j] = who;
    count_features(p1_pos, who);
    int h = R.first - L.first, w = R.second - L.second;
    if (i == 0) {
        L.first--;
    }
    if (i == h) {
        R.first++;
    }
    if (j == 0) {
        L.second--;
    }
    if (j == w) {
        R.second++;
    }
    if (p1_pos.back()) {
        printf("%s\n", "SDDASDS");
        winner = who;
    }
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
    
    //printf("%s %d %d\n", "Computer move: ", best_move.first, best_move.second);
    //printf("%d %d\n", R.first - L.first, R.second - L.second);
    printf("%s\n", "PRINTED");
    printf("%d %d\n", best_move.first, best_move.second);
    of << best_move.first << " " << best_move.second;
    of.close();
    game_started = 1;
    
    while (winner == -1) {    
        ifstream in("human");
        in >> cur.first >> cur.second;
        in.close();
        ofstream o("human");
        o << -1 << " " << -1;
        o.close();

        if (cur != mp(-1, -1)) {
            ofstream of("ai");
            if (!can(L.first + cur.first, L.second + cur.second)) {
                of << -2 << " " << -2;
                of.close();
                continue;
            }

            //printf("--> OK!\n");
            was = cur;
            applyMove(cur.first, cur.second, cur_player);
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
            applyMove(best_move.first, best_move.second, cur_player);
            
            cur_player ^= 1;
            
            //printf("%s %d %d\n", "Computer move: ", best_move.first, best_move.second);
            //printf("%d %d\n", R.first - L.first, R.second - L.second);
            
            printf("%s %d %d\n", "AI: ", best_move.first, best_move.second);
            of << best_move.first << " " << best_move.second;
            of.close();

        }

        usleep(100000);
        /*
        forn(y, L.first, R.first) {
            forn(x, L.second, R.second) {
                printf("%c", chr[a[y][x] + 1]);
            }
            puts("");
        }
        */

    }
    ofstream off("ai");
    off << -2 << " " << winner;
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
    
    
    //L = mp(MID, MID), R = mp(n + MID + 1, n + MID + 1);
    /*
     int a1=0, a2=0,a3=0,a4=0;
     int b1=0, b2=0, b3=0, b4=0;
     count_features(a1, a2, a3, a4, 0);
     count_features(b1, b2, b3, b4, 1);
     */
    //cur_player = 1;
    //  a[3][0] = 1;
    //count_features(b1, b2, b3, b4, 0);
    //cout << b1 << " " << b2 << " " << b3 << " " << b4 << endl;
    //cout << cost(a1,a2,a3,a4,b1,b2,b3,b4,1) << " ";
    //minimax(3, cur_player);
    //cout << best_move.first - MID << " " << best_move.second - MID << "\n";
    return 0;
}