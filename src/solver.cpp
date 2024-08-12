#include "solver.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

namespace std {

template <>
struct hash<Board>
{
    std::size_t operator()(const Board & b) const
    {
        return b.get_hash();
    }
};

} // namespace std

namespace {

std::size_t heuristic(const Board & board, std::size_t depth)
{
    if (board.size() <= 3) {
        return std::atan(board.manhattan()) * board.manhattan() + depth;
    }
    else if (board.size() == 4) {
        return 2.25 * board.manhattan() + board.hamming() + depth;
    }
    else if (board.size() == 5) {
        return (board.manhattan() % 3 == 1 ? 3.34 : 2.5 + std::atan(board.manhattan())) * board.manhattan() + board.hamming();
    }
    else {
        return (board.size() / 2 + std::atan(board.manhattan() + depth)) * board.manhattan() + board.hamming() + depth;
    }
}

struct State
{
    State(const Board & board, std::size_t depth)
        : board(board)
        , heuristic_value(heuristic(board, depth))
        , depth(depth)
    {
    }

    Board board;
    std::size_t heuristic_value;
    std::size_t depth;
};

} // namespace

Solver::Solution Solver::solve(const Board & initial)
{
    static auto compare = [](const State & a, const State & b) {
        return a.heuristic_value > b.heuristic_value;
    };

    if (!initial.is_solvable()) {
        return Solver::Solution();
    }

    if (initial.size() <= 1 || initial.is_goal()) {
        return Solver::Solution({initial});
    }

    std::vector<Board> result;
    std::priority_queue<State, std::vector<State>, decltype(compare)> pq(compare);
    std::unordered_map<Board, Move> prev;
    pq.push(State(initial, 0));
    while (!pq.empty()) {
        auto state = pq.top();
        pq.pop();
        if (state.board.is_goal()) {
            result.emplace_back(std::move(state.board));
            while (result.back() != initial) {
                result.emplace_back(result.back().my_move(prev[result.back()].reverse()));
            }
            break;
        }
        for (int di = -1; di <= 1; ++di) {
            for (int dj = -1; dj <= 1; ++dj) {
                Move move(di, dj);
                if (di * di + dj * dj != 1 || !state.board.is_valid_move(move)) {
                    continue;
                }
                auto new_board = state.board.my_move(move);
                if (prev.find(new_board) == prev.end()) {
                    prev[new_board] = move;
                    pq.emplace(State(new_board, state.depth + 1));
                }
            }
        }
    }
    std::reverse(result.begin(), result.end());
    return Solver::Solution(std::move(result));
}

Solver::Solution::Solution(std::vector<Board> && moves)
    : m_moves(std::move(moves))
{
}
