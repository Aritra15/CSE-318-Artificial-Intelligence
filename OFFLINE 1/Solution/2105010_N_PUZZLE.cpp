#include <bits/stdc++.h>
using namespace std;

int N;
vector<vector<int>> goal;
map<int, pair<int, int>> goal_positions;

class PuzzleState
{
    vector<vector<int>> board;
    int g, h;
    PuzzleState *parent;

public:
    PuzzleState(vector<vector<int>> b, int g, int h, PuzzleState *p) : board(b), g(g), h(h), parent(p) {}

    int f() const
    {
        return g + h;
    }

    bool operator>(const PuzzleState &other) const
    {
        return (f() > other.f()) || (f() == other.f() && h > other.h);
    }

    string flatten()
    {
        string flat;
        for (const auto &row : board)
            for (int num : row)
                flat += to_string(num) + ",";
        return flat;
    }

    vector<vector<int>> getBoard() const { return board; }
    PuzzleState *getParent() const { return parent; }
    int getG() const { return g; }
    int getH() const { return h; }
};

void setGoals(int n)
{
    N = n;
    goal.assign(N, vector<int>(N));
    int num = 1;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            goal[i][j] = (i == N - 1 && j == N - 1) ? 0 : num++;

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            goal_positions[goal[i][j]] = {i, j};

    // cout << "Goal state:\n";
    // for (const auto &row : goal)
    // {
    //     for (int val : row)
    //     {
    //         cout << val << " ";
    //     }
    //     cout << endl;
    // }

    // cout << endl;
}

double hamming_distance(const vector<vector<int>> &board)
{
    int dist = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (board[i][j] != 0 && board[i][j] != goal[i][j])
                dist++;
    return dist;
}

double manhattan_distance(const vector<vector<int>> &board)
{
    int dist = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (board[i][j] != 0)
            {
                int gx = goal_positions[board[i][j]].first;
                int gy = goal_positions[board[i][j]].second;
                dist += abs(i - gx) + abs(j - gy);
            }
    return dist;
}

double euclidian_distance(const vector<vector<int>> &board)
{
    double dist = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (board[i][j] != 0)
            {
                int gx = goal_positions[board[i][j]].first;
                int gy = goal_positions[board[i][j]].second;
                dist += sqrt(pow(i - gx, 2) + pow(j - gy, 2));
            }
    return dist;
}

int linear_conflict_count(const vector<vector<int>> &board)
{
    int conflicts = 0;

    for (int i = 0; i < N; ++i)
    {
        vector<int> rowTiles;
        for (int j = 0; j < N; ++j)
            if (board[i][j] != 0 && goal_positions[board[i][j]].first == i)
                rowTiles.push_back(board[i][j]);

        for (int a = 0; a < rowTiles.size(); ++a)
            for (int b = a + 1; b < rowTiles.size(); ++b)
                if (goal_positions[rowTiles[a]].second > goal_positions[rowTiles[b]].second)
                    conflicts++;
    }

    for (int j = 0; j < N; ++j)
    {
        vector<int> colTiles;
        for (int i = 0; i < N; ++i)
            if (board[i][j] != 0 && goal_positions[board[i][j]].second == j)
                colTiles.push_back(board[i][j]);

        for (int a = 0; a < colTiles.size(); ++a)
            for (int b = a + 1; b < colTiles.size(); ++b)
                if (goal_positions[colTiles[a]].first > goal_positions[colTiles[b]].first)
                    conflicts++;
    }

    return conflicts;
}



double linear_conflict_distance(const vector<vector<int>> &board)
{
    return manhattan_distance(board) + 2 * linear_conflict_count(board);
}



int count_inversions(const vector<vector<int>> &board)
{
    vector<int> flat;
    for (const auto &row : board)
        for (int num : row)
            if (num != 0)
                flat.push_back(num);

    int inv = 0;
    for (int i = 0; i < flat.size(); ++i)
        for (int j = i + 1; j < flat.size(); ++j)
            if (flat[i] > flat[j])
                inv++;
    return inv;
}

bool is_solvable(const vector<vector<int>> &board)
{
    int inv = count_inversions(board);

    if (N % 2 == 1)
    {
        return inv % 2 == 0;
    }
    else
    {
        int blank_row;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                if (board[i][j] == 0)
                    blank_row = i;

        int row_from_bottom = N - blank_row;
        return (inv + row_from_bottom) % 2 == 1;
    }
}

vector<pair<int, int>> getNeighbours(int x, int y)
{
    vector<pair<int, int>> neighbours;
    if (x > 0)
        neighbours.emplace_back(x - 1, y);
    if (x + 1 < N)
        neighbours.emplace_back(x + 1, y);
    if (y > 0)
        neighbours.emplace_back(x, y - 1);
    if (y + 1 < N)
        neighbours.emplace_back(x, y + 1);
    return neighbours;
}

void printBoard(const vector<vector<int>> &board)
{
    for (const auto &row : board)
    {
        for (int num : row)
            cout << num << " ";
        cout << "\n";
    }
    cout << "\n";
}

void printPath(PuzzleState *state)
{
    vector<vector<vector<int>>> path;
    while (state)
    {
        path.push_back(state->getBoard());
        state = state->getParent();
    }
    reverse(path.begin(), path.end());
    for (const auto &b : path)
        printBoard(b);
}

struct PuzzleComparator
{
    bool operator()(const PuzzleState *a, const PuzzleState *b) const
    {
        return (a->f() > b->f()) || (a->f() == b->f() && a->getH() > b->getH());
    }
};

void aStar(vector<vector<int>> start, function<double(const vector<vector<int>> &)> heuristic)
{
    priority_queue<PuzzleState *, vector<PuzzleState *>, PuzzleComparator> pq;
    unordered_set<string> visited;

    PuzzleState *initial = new PuzzleState(start, 0, heuristic(start), nullptr);
    pq.push(initial);
    visited.insert(initial->flatten());

    int explored = 1;
    int expanded = 0;

    while (!pq.empty())
    {
        PuzzleState *current = pq.top();
        pq.pop();
        

        if (current->getBoard() == goal)
        {
            cout << "Minimum number of moves = " << current->getG() << "\n\n";
            printPath(current);
            cout << "Nodes explored: " << explored << endl;
            cout << "Nodes expanded: " << expanded << endl;

            // for(const auto& state : visited) {
            //     cout << state << "\n";
            // }
            // cout << "Visited nodes: " << visited.size() << endl;

            return;
        }
        expanded++;

        int x = -1, y = -1;
        bool found = false;
        for (int i = 0; i < N && !found; ++i)
            for (int j = 0; j < N; ++j)
                if (current->getBoard()[i][j] == 0)
                {
                    x = i;
                    y = j;
                    found = true;
                    break;
                }

        for (auto &p : getNeighbours(x, y))
        {
            int nx = p.first, ny = p.second;

            vector<vector<int>> new_board = current->getBoard();

            int temp = new_board[x][y];
            new_board[x][y] = new_board[nx][ny];
            new_board[nx][ny] = temp;

            PuzzleState *next_state = new PuzzleState(new_board, current->getG() + 1, heuristic(new_board), current);
            if (!visited.count(next_state->flatten()))
            {
                pq.push(next_state);
                visited.insert(next_state->flatten());
                explored++;
            }
            else
            {
                delete next_state;
            }
        }
    }
    cout << "Explored nodes: " << explored << endl;

    cout << "No solution found.\n";
}

int main()
{
    freopen("out.txt", "w", stdout);
    cin >> N;
    vector<vector<int>> board(N, vector<int>(N));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            cin >> board[i][j];

    setGoals(N);

    // string heuristic_choice;
    // cin >> heuristic_choice;

    // function<int(const vector<vector<int>>&)> heuristic;

    // if (heuristic_choice == "manhattan") heuristic = manhattan_distance;
    // else if (heuristic_choice == "hamming") heuristic = hamming_distance;
    // else if (heuristic_choice == "euclidean") heuristic = euclidian_distance;
    // else if (heuristic_choice == "linear") heuristic = linear_conflict_distance;
    // else {
    //     cout << "Invalid heuristic. Defaulting to manhattan.\n";
    //     heuristic = manhattan_distance;
    // }

    if (!is_solvable(board))
    {
        cout << "Unsolvable puzzle\n";
        return 0;
    }

    // aStar(board, heuristic);
    aStar(board, linear_conflict_distance);
    return 0;
}
