#include <bits/stdc++.h>
using namespace std;

int total_rows = 9, total_cols = 6;

int depth_R = 3;
int depth_B = 3;

int heur_ai1=1;
int heur_ai2=1;
int curr_ai=0;
int aivsai=0; // 0: human vs ai, 1: ai vs ai

int ai_game_choice;
int randvsai=0;

struct Cell
{
    int count;
    char color;
};
typedef vector<vector<Cell>> Board;

Board create_board()
{
    return Board(total_rows, vector<Cell>(total_cols, {0, ' '}));
}

int get_critical_mass(int r, int c)
{
    bool top = (r == 0);
    bool bot = (r == total_rows - 1);
    bool left = (c == 0);
    bool right = (c == c == total_cols - 1);
    if ((top || bot) && (left || right))
        return 2;
    if (top || bot || left || right)
        return 3;
    return 4;
}

// apply a move along with chain reactions using BFS
void apply_move(Board &b, int r, int c, char p)
{
    if (b[r][c].color != ' ' && b[r][c].color != p)
        return;
    if (b[r][c].color == ' ')
    {
        b[r][c].color = p;
        b[r][c].count = 1;
    }
    else
    {
        b[r][c].count++;
    }

    queue<pair<int, int>> Q;
    Q.push({r, c});
    int D[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!Q.empty())
    {
        auto pr = Q.front();
        Q.pop();
        int rr = pr.first, cc = pr.second;
        int crit = get_critical_mass(rr, cc);
        if (b[rr][cc].count < crit)
            continue;

        b[rr][cc].count -= crit;
        if (b[rr][cc].count == 0)
            b[rr][cc].color = ' ';

        for (int i = 0; i < 4; i++)
        {
            int nr = rr + D[i][0], nc = cc + D[i][1];
            if (nr < 0 || nr >= total_rows || nc < 0 || nc >= total_cols)
                continue;
            if (b[nr][nc].color == ' ')
            {
                b[nr][nc].color = p;
                b[nr][nc].count = 1;
            }
            else if (b[nr][nc].color != p)
            {
                b[nr][nc].color = p;
                b[nr][nc].count++;
            }
            else
            {
                b[nr][nc].count++;
            }
            if (b[nr][nc].count >= get_critical_mass(nr, nc))
                Q.push({nr, nc});
        }
    }
}

vector<pair<int, int>> get_legal_moves(const Board &b, char p)
{
    vector<pair<int, int>> v;
    for (int i = 0; i < total_rows; i++)
        for (int j = 0; j < total_cols; j++)
        {
            if (b[i][j].color == ' ' || b[i][j].color == p)
                v.emplace_back(i, j);
        }
    return v;
}

int heuristic_online(const Board &board, char player)
{
    
}

int heuristic_orb_count(const Board &board, char player)
{
    char opp = (player == 'B' ? 'R' : 'B');
    int p_sum = 0, o_sum = 0;
    for (int r = 0; r < total_rows; r++)
        for (int c = 0; c < total_cols; c++)
        {
            if (board[r][c].color == player)
                p_sum += board[r][c].count;
            else if (board[r][c].color == opp)
                o_sum += board[r][c].count;
        }
    return p_sum - o_sum;
}

int heuristic_cell_control(const Board &board, char player)
{
    char opp = (player == 'B' ? 'R' : 'B');
    int p_cells = 0, o_cells = 0;
    for (int r = 0; r < total_rows; r++)
        for (int c = 0; c < total_cols; c++)
        {
            if (board[r][c].color == player)
                p_cells++;
            else if (board[r][c].color == opp)
                o_cells++;
        }
    return p_cells - o_cells;
}

int heuristic_mobility(const Board &board, char player)
{
    char opp = (player == 'B' ? 'R' : 'B');
    auto legal = [&](char p)
    {
        auto moves = get_legal_moves(board, p);
        return (int)moves.size();
    };
    return legal(player) - legal(opp);
}

int heuristic_edge_stability(const Board &board, char player)
{
    char opp = (player == 'B' ? 'R' : 'B');
    auto is_edge_not_corner = [&](int r, int c)
    {
        bool edge = (r == 0 || r == total_rows - 1 || c == 0 || c == total_cols - 1);
        bool corner = (r == 0 || r == total_rows - 1) && (c == 0 || c == total_cols - 1);
        return edge && !corner;
    };
    int p_sum = 0, o_sum = 0;
    for (int r = 0; r < total_rows; r++)
        for (int c = 0; c < total_cols; c++)
        {
            if (!is_edge_not_corner(r, c))
                continue;
            if (board[r][c].color == player)
                p_sum += board[r][c].count;
            else if (board[r][c].color == opp)
                o_sum += board[r][c].count;
        }
    return p_sum - o_sum;
}

int heuristic_critical_cells(const Board &board, char player)
{
    char opp = (player == 'B' ? 'R' : 'B');
    int p_sum = 0, o_sum = 0;
    for (int r = 0; r < total_rows; r++)
        for (int c = 0; c < total_cols; c++)
        {
            int crit = get_critical_mass(r, c);
            if (board[r][c].color == player && board[r][c].count == crit - 1)
                p_sum++;
            if (board[r][c].color == opp && board[r][c].count == crit - 1)
                o_sum++;
        }
    return p_sum - o_sum;
}

// a combination of different heuristics
int smart_heuristic_helper(const Board &b)
{
    int score = 0;
    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};
    for (int r = 0; r < total_rows; r++)
        for (int c = 0; c < total_cols; c++)
        {
            const Cell &cell = b[r][c];

            if (cell.color == ' ')
                continue;

            int crit = get_critical_mass(r, c),
                w = cell.count * 10;

            bool corner = ((r == 0 || r == total_rows - 1) && (c == 0 || c == total_cols - 1)),
                 edge = ((r == 0 || r == total_rows - 1) || (c == 0 || c == total_cols - 1));
            if (corner)
                w += 5;
            else if (edge)
                w += 3;

            if (cell.count == crit - 1)
                w += 6;
            else if (cell.count == crit - 2)
                w += 3;

            for (int d = 0; d < 4; d++)
            {
                int nr = r + dr[d], nc = c + dc[d];
                if (nr < 0 || nr >= total_rows || nc < 0 || nc >= total_cols)
                    continue;
                const Cell &nb = b[nr][nc];
                if (cell.color == 'B' && nb.color == 'R' && nb.count >= get_critical_mass(nr, nc) - 1)
                    w -= 8;
                if (cell.color == 'R' && nb.color == 'B' && nb.count >= get_critical_mass(nr, nc) - 1)
                    w += 8;
            }
            score += (cell.color == 'B' ? w : -w);
        }
    return score;
}

int smart_heuristic(const Board &b,char p)
{
    int h= smart_heuristic_helper(b);
    if (p == 'B')
        return h;
    else
        return -h;
}

double balanced_heuristic(const Board &b, char player) {
    char opp = (player=='B' ? 'R' : 'B');
    int dr[4]={-1,1,0,0}, dc[4]={0,0,-1,1};

    double Sp=0, So=0;
    for(int r=0;r<total_rows;r++){
      for(int c=0;c<total_cols;c++){
        const Cell &cell=b[r][c];
        if(cell.color==' ') 
            continue;
        
        double w = cell.count * 1.0;
        
        int crit = get_critical_mass(r,c);

        if(cell.count==crit-1) w += 4.0;
       
        bool corner = ((r==0||r==total_rows-1)&&(c==0||c==total_cols-1));
        bool edge   = ((r==0||r==total_rows-1)||(c==0||c==total_cols-1));
        w += corner?3: edge?2:0;
       
        int threats=0;
        for(int d=0;d<4;d++){
          int nr=r+dr[d], nc=c+dc[d];
          if(nr<0||nr>=total_rows||nc<0||nc>=total_cols) 
            continue;
          const Cell &nb=b[nr][nc];
          if(nb.color!=cell.color && nb.count>=get_critical_mass(nr,nc)-1) 
            threats++;
        }
        w += threats * 3.0;  

        if(cell.color==player) 
            Sp += w;
        else 
            So += w;
      }
    }

    //mobility
    Sp += 0.5 * get_legal_moves(b,player).size();
    So += 0.5 * get_legal_moves(b,opp).size();

    // cell control
    int cp=0, co=0;
    for(auto &row:b) for(auto &cell:row){
      if(cell.color==player) cp++;
      if(cell.color==opp)    co++;
    }
    Sp += 0.5 * cp;
    So += 0.5 * co;

    // 4) normalize difference
    double D = Sp - So;
    double N = Sp + So + 1e-6;      
    double score = 100.0 * (D / N);  // range [-100,100]

    return score;
}


int evaluate(const Board &b, char p)
{
    if(!aivsai){
        if(!randvsai)
            return smart_heuristic(b, p);
        else{
            switch(heur_ai2){
                case 1: return smart_heuristic(b, p);
                case 2: return balanced_heuristic(b, p);
                case 3: return heuristic_orb_count(b, p);
                case 4: return heuristic_cell_control(b, p);
                case 5: return heuristic_mobility(b, p);
                case 6: return heuristic_edge_stability(b, p);
                case 7: return heuristic_critical_cells(b, p);
                default: return smart_heuristic(b, p); 
            }   
        }
    }else{
        int heuristic=1;
        if(curr_ai==1){
            heuristic = heur_ai1;
        }else{
            heuristic = heur_ai2;
        }
        switch(heuristic){
            case 1: return smart_heuristic(b, p);
            case 2: return balanced_heuristic(b, p);
            case 3: return heuristic_orb_count(b, p);
            case 4: return heuristic_cell_control(b, p);
            case 5: return heuristic_mobility(b, p);
            case 6: return heuristic_edge_stability(b, p);
            case 7: return heuristic_critical_cells(b, p);
            default: return smart_heuristic(b, p);
        }
    }
}

bool check_game_over(const Board &b, char &w)
{
    bool R = false, B = false;
    for (auto &row : b)
        for (auto &cell : row)
        {
            if (cell.color == 'R')
                R = true;
            if (cell.color == 'B')
                B = true;
        }
    if (R && B)
        return false;
    w = (R ? 'R' : (B ? 'B' : ' '));
    return true;
}

// minimax algo
int minimax(Board b, int depth, int alpha, int beta,bool maxing, char player, pair<int, int> &best)
{
    char w;
    if (check_game_over(b, w))
    {
        return (w == player ? +100000 : -100000);
    }
    if (depth == 0)
        return evaluate(b, player);

    char opp;
    if(player == 'B')
        opp = 'R';
    else
        opp = 'B';

    auto moves = maxing ? get_legal_moves(b, player): get_legal_moves(b, opp);

    if (moves.empty())
        return maxing ? -10000 : 10000;

    if (maxing)
    {
        int val = INT_MIN;
        for (auto &m : moves)
        {
            Board nb = b;
            apply_move(nb, m.first, m.second, player);
            pair<int, int> dum;
            int v = minimax(nb, depth - 1, alpha, beta, false, player, dum);
            if (v > val)
            {
                val = v;
                best = m;
            }
            alpha = max(alpha, v);
            if (beta <= alpha)
                break;
        }
        return val;
    }
    else
    {
        int val = INT_MAX;
        for (auto &m : moves)
        {
            Board nb = b;
            apply_move(nb, m.first, m.second, opp);
            pair<int, int> dum;
            int v = minimax(nb, depth - 1, alpha, beta, true, player, dum);
            val = min(val, v);
            beta = min(beta, v);
            if (beta <= alpha)
                break;
        }
        return val;
    }
}

// if there are multiple best moves,i randomly select one
pair<int, int> minimax_for(const Board &board, int depth_limit, char player)
{
    auto moves = get_legal_moves(board, player);
    if (moves.empty())
        return {-1, -1};

    int bestVal = INT_MIN;
    vector<pair<int, int>> bestMoves;
    for (auto &m : moves)
    {
        Board nb = board;
        apply_move(nb, m.first, m.second, player);
        pair<int, int> dum;
        int val = minimax(nb, depth_limit - 1, INT_MIN, INT_MAX, false, player, dum);
        if (val > bestVal)
        {
            bestVal = val;
            bestMoves = {m};
        }
        else if (val == bestVal)
        {
            bestMoves.push_back(m);
        }
    }
    static mt19937 rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, bestMoves.size() - 1);
    return bestMoves[dist(rng)];
}

struct GameInfo
{
    //human has played, ai has played, game over, winner
    bool h, a, o;
    char w;
};

void write_game_info(const GameInfo &g)
{
    ofstream f("gameinfo.txt");
    f << g.h << " " << g.a << " " << g.o << " " << g.w << "\n";
}

GameInfo read_game_info()
{
    GameInfo g{false, false, false, ' '};
    ifstream f("gameinfo.txt");
    if (f)
        f >> g.h >> g.a >> g.o >> g.w;
    return g;
}

void write_board(const string &filename, const Board &b, const string &header)
{
    ofstream f(filename);
    f << header << "\n";
    for (int r = 0; r < total_rows; r++)
    {
        for (int c = 0; c < total_cols; c++)
        {
            if (b[r][c].color == ' ')
                f << "0 ";
            else
                f << b[r][c].count << b[r][c].color;
            if (c < total_cols - 1)
                f << " ";
        }
        f << "\n";
    }
}

Board read_board(const string &filename)
{
    Board b = create_board();
    ifstream f(filename);
    if (!f)
        return b;
    string header;
    getline(f, header);
    for (int r = 0; r < total_rows; r++)
    {
        for (int c = 0; c < total_cols; c++)
        {
            string s;
            f >> s;
            if (s != "0")
            {
                b[r][c].count = s[0] - '0';
                b[r][c].color = s[1];
            }
        }
    }
    return b;
}

string wait_for(const string &filename, const string &expected_header)
{
    while (true)
    {
        GameInfo g = read_game_info();
        if (g.o)
            return string("GAMEOVER:") + g.w;
        ifstream f(filename);
        if (f)
        {
            string h;
            getline(f, h);
            if (h == expected_header)
                return h;
        }
    }
}

void display_board(const Board &b)
{
    cout << "Current Board:\n";
    for (int r = 0; r < total_rows; r++)
    {
        for (int c = 0; c < total_cols; c++)
        {
            if (b[r][c].color == ' ')
            {
                cout << " . ";
            }
            else
            {
                cout << " " << b[r][c].count << b[r][c].color << " ";
            }
        }
        cout << "\n";
    }
    cout << "\n";
}


pair<int, int> get_random_move(Board &b, char player)
{
    vector<pair<int, int>> moves = get_legal_moves(b, player);
    if (moves.empty())
        return {-1, -1};
    static mt19937 rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}

void run_ai_vs_ai()
{
    //aivsai=1;
    const string filename = "gamestate.txt";
    GameInfo gameinfo{false, false, false, ' '};
    write_game_info(gameinfo);
    
    Board b = create_board();
    write_board(filename, b, "AI2 Move:");

    while (true)
    {
        
        //First AI's turn
        curr_ai=1-curr_ai;
        string t1 = wait_for(filename, "AI2 Move:");
        if (t1.rfind("GAMEOVER:", 0) == 0)
        {
            cout << "GAME OVER, winner " << t1.back() << "\n";
            break;
        }

        b = read_board(filename);
        //auto m1 = minimax_for(b, depth_R, 'R');
        pair<int, int> m1;
        if(randvsai){
            m1 = get_random_move(b, 'R');
        }else{
            m1 = minimax_for(b, depth_R, 'R');
        }

        apply_move(b, m1.first, m1.second, 'R');

        gameinfo.h = true;
       
        write_game_info(gameinfo);
        write_board(filename, b, "AI1 Move:");
        cout << "AI1(R,d=" << depth_R << ")->" << m1.first << "," << m1.second << "\n";

        char w;
        if (gameinfo.a && check_game_over(b, w))
        {
            cout << "GAME OVER, winner " << w << "\n";
            gameinfo.o = true;
            gameinfo.w = w;
            write_game_info(gameinfo);
            break;
        }

        // Second AI's turn
        curr_ai=1-curr_ai;
        string t2 = wait_for(filename, "AI1 Move:");
        if (t2.rfind("GAMEOVER:", 0) == 0)
        {
            cout << "GAME OVER, winner " << t2.back() << "\n";
            break;
        }

        b = read_board(filename);
        auto m2 = minimax_for(b, depth_B, 'B');
        apply_move(b, m2.first, m2.second, 'B');

        write_game_info(gameinfo);
        write_board(filename, b, "AI2 Move:");

        cout << "AI2(B,d=" << depth_B << ")->" << m2.first << "," << m2.second << "\n";

        gameinfo.a = true;
        if (gameinfo.h && check_game_over(b, w))
        {
            cout << "GAME OVER, winner " << w << "\n";
            gameinfo.o = true;
            gameinfo.w = w;
            write_game_info(gameinfo);
            break;
        }
    }
}

void run_ui()
{
    const string filename = "gamestate.txt";
    Board board = create_board();
    GameInfo gameinfo{false, false, false, ' '};

    write_game_info(gameinfo);
    write_board(filename, board, "Human Move:"); 

    display_board(board);

    while (true)
    {
        //human move
        
        cout << "Your turn (Red). Enter row col: ";
        int r, c;
        cin >> r >> c;

        if (r < 0 || r >= total_rows || c < 0 || c >= total_cols ||
            (board[r][c].color != ' ' && board[r][c].color != 'R'))
        {
            cout << "Invalid move—try again.\n";
            continue;
        }

        apply_move(board, r, c, 'R');
        if (!gameinfo.h)
        {
            gameinfo.h = true;
            write_game_info(gameinfo);
        }

        write_board(filename, board, "Human Move:");
        cout << "You played: " << r << " " << c << "\n";
        display_board(board);

        char w;
        if (gameinfo.a && check_game_over(board, w))
        {
            gameinfo.o = true;
            gameinfo.w = w;
            write_game_info(gameinfo);
            display_board(board);
            cout << (w == 'B' ? "AI (Blue) wins!\n"
                              : "Human (Red) wins!\n");
            break;
        }

        //AI move
        auto move = minimax_for(board, 4, 'B');
        if (move.first < 0)
        {
            cout << "AI has no legal move—ending.\n";
            break;
        }
        apply_move(board, move.first, move.second, 'B');
        if (!gameinfo.a)
        {
            gameinfo.a = true;
            write_game_info(gameinfo);
        }

        write_board(filename, board, "AI Move:");
        cout << "AI played: " << move.first << " " << move.second << "\n";
        display_board(board);

        if (gameinfo.h && check_game_over(board, w))
        {
            gameinfo.o = true;
            gameinfo.w = w;
            write_game_info(gameinfo);
            display_board(board);
            cout << (w == 'B' ? "AI (Blue) wins!\n"
                              : "Human (Red) wins!\n");
            break;
        }

        
    }
}

void get_aivsai_choices()
{
    cout << "AI vs AI mode selected.\n";
    
    cout << "1. Smart Heuristic\n";
    cout << "2. Balanced Heuristic\n";
    cout << "3. Orb Count Heuristic\n";
    cout << "4. Cell Control Heuristic\n";
    cout << "5. Mobility Heuristic\n";
    cout << "6. Edge Stability Heuristic\n";
    cout << "7. Critical Cells Heuristic\n";

    cout << "Choose heuristics for AI1 (Red):\n";
    cin >> heur_ai1;

    cout << "Choose heuristics for AI2 (Blue):\n";
    cin >> heur_ai2;
}

void get_randomvsai_choices(){
    cout<<"Random AI vs Heuristic AI mode selected.\n";
    cout << "1. Smart Heuristic\n";
    cout << "2. Balanced Heuristic\n";
    cout << "3. Orb Count Heuristic\n";
    cout << "4. Cell Control Heuristic\n";
    cout << "5. Mobility Heuristic\n";
    cout << "6. Edge Stability Heuristic\n";
    cout << "7. Critical Cells Heuristic\n";

    cout << "Choose heuristics for AI2 (Blue):\n";
    cin >> heur_ai2;
}


void get_choice(){
    cout<< "AI vs AI mode selected.\n";
    cout<<"1. Random AI vs Heuristic AI\n";
    cout<<"2. Heuristic AI vs Heuristic AI\n";
    cout<<"Choose AI game mode: ";
    cin >> ai_game_choice;
}

int main(int argc, char *argv[])
{
    cout << "Welcome to the Chain Reaction Game!\n";
    cout << "Options:\n";
    cout << "1. Human vs AI\n";
    cout << "2. AI vs AI\n";
    int choice;
    cin >> choice;
    if (choice == 1)
    {
        run_ui();
    }
    else if (choice == 2)
    {
        
        get_choice();
        if(ai_game_choice==1){
            randvsai=1;
            get_randomvsai_choices();
        }else if(ai_game_choice==2){
            randvsai=0;
            aivsai=1;
            get_aivsai_choices();
        }else{
            cout << "Invalid choice. Exiting.\n";
            return 1;
        }
        
        run_ai_vs_ai();
    }
    else
    {
        cout << "Invalid choice. Exiting.\n";
        return 1;
    }

    return 0;
}
