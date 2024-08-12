#include "board.h"

#include <algorithm>
#include <random>

Board Board::create_goal(const unsigned size)
{
    std::vector<std::vector<unsigned>> board(size, std::vector<unsigned>(size));
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            board[i][j] = (i * size + j + 1);
        }
    }
    board[size - 1][size - 1] = 0;
    return Board(std::move(board));
}

Board Board::create_random(const unsigned size)
{
    std::vector<std::vector<unsigned>> board(size, std::vector<unsigned>(size));
    std::vector<unsigned> permutation(size * size);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::shuffle(permutation.begin(), permutation.end(), std::mt19937(std::random_device()()));
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            board[i][j] = permutation[i * size + j];
        }
    }
    return Board(std::move(board));
}

Board::Board(const std::vector<std::vector<unsigned>> & data)
    : m_data(data)
{
    initialize(m_data);
}

Board::Board(std::vector<std::vector<unsigned int>> && data)
    : m_data(std::move(data))
{
    initialize(m_data);
}

std::size_t Board::size() const
{
    return m_data.size();
}

bool Board::is_goal() const
{
    return m_hamming_distance == 0;
}

unsigned Board::hamming() const
{
    return m_hamming_distance;
}

unsigned Board::manhattan() const
{
    return m_manhattan_distance;
}

std::string Board::to_string() const
{
    std::string string;
    string.reserve(m_data.size() * m_data.size());
    for (std::size_t i = 0; i < m_data.size(); ++i) {
        if (i != 0) {
            string += '\n';
        }
        for (std::size_t j = 0; j < m_data.size(); ++j) {
            if (j != 0) {
                string += ' ';
            }
            string += std::to_string(m_data[i][j]);
        }
    }
    return string;
}

bool Board::is_solvable() const
{
    return m_data.empty() ||
            (m_inversions % 2 == 0 && (m_blank_row % 2 == 1 || m_data.size() % 2 == 1)) ||
            (m_inversions % 2 == 1 && m_data.size() % 2 == 0 && m_blank_row % 2 == 0);
}

void Board::initialize(const std::vector<std::vector<unsigned int>> & data)
{
    std::vector<unsigned> all;
    all.reserve(data.size() * data.size());
    std::string string;
    string.reserve(m_data.size() * m_data.size());

    for (std::size_t i = 0; i < data.size(); ++i) {
        if (i != 0) {
            string += '\n';
        }
        for (std::size_t j = 0; j < data[i].size(); ++j) {
            if (j != 0) {
                string += ' ';
            }
            string += std::to_string(m_data[i][j]);
            auto [expected_row, expected_col] = expected(data[i][j]);
            m_hamming_distance += static_cast<unsigned>(i != expected_row || j != expected_col);
            if (data[i][j] != 0) {
                all.push_back(data[i][j]);
                m_manhattan_distance += unsigned_dist(i, expected_row) + unsigned_dist(j, expected_col);
            }
            else {
                m_blank_row = i, m_blank_col = j;
            }
        }
    }
    hash = std::hash<std::string>{}(string);
    m_inversions = calc_inversions(all);
}

Board Board::my_move(const Move & m) const
{
    auto new_data = m_data;
    std::swap(new_data[m_blank_row][m_blank_col], new_data[m_blank_row + m.get_di()][m_blank_col + m.get_dj()]);
    return Board(std::move(new_data));
}

unsigned Board::unsigned_dist(unsigned int a, unsigned int b)
{
    return std::max(a, b) - std::min(a, b);
}

std::pair<std::size_t, std::size_t> Board::expected(unsigned value) const
{
    std::size_t expected_place = (value + m_data.size() * m_data.size() - 1) % (m_data.size() * m_data.size());
    std::size_t expected_row = expected_place / m_data.size(), expected_col = expected_place % m_data.size();
    return std::make_pair(expected_row, expected_col);
}

bool Board::is_valid_move(const Move & move) const
{
    if (move.is_horizontal()) {
        return !((m_blank_col == 0 && move.get_dj() == -1) || (m_blank_col + 1 == m_data.size() && move.get_dj() == 1));
    }
    return !((m_blank_row == 0 && move.get_di() == -1) || (m_blank_row + 1 == m_data.size() && move.get_di() == 1));
}

std::size_t Board::get_hash() const
{
    return hash;
}

unsigned Board::calc_inversions(std::vector<unsigned int> & arr)
{
    if (arr.empty()) {
        return 0;
    }
    std::vector<unsigned> temp(arr.size());
    return merge_sort(arr, temp, 0, arr.size() - 1);
}

unsigned Board::merge_sort(std::vector<unsigned int> & arr, std::vector<unsigned int> & temp, std::size_t left, std::size_t right)
{
    std::size_t mid;
    unsigned count = 0;
    if (right > left) {
        mid = (left + right) / 2;
        count = merge_sort(arr, temp, left, mid);
        count += merge_sort(arr, temp, mid + 1, right);
        count += merge(arr, temp, left, mid + 1, right);
    }
    return count;
}

unsigned Board::merge(std::vector<unsigned int> & arr, std::vector<unsigned int> & temp, std::size_t left, std::size_t mid, std::size_t right)
{
    std::size_t i = left, j = mid, k = left;
    unsigned count = 0;
    while (i + 1 <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        }
        else {
            temp[k++] = arr[j++];
            count += mid - i;
        }
    }
    while (i + 1 <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= right) {
        temp[k++] = arr[j++];
    }
    for (std::size_t it = left; it <= right; ++it) {
        arr[it] = temp[it];
    }
    //    std::copy(temp.begin() + left, temp.begin() + right + 1, arr.begin() + left);
    return count;
}

Move::Move(int di, int dj)
    : di(di)
    , dj(dj)
{
}

bool Move::is_vertical() const
{
    return dj == 0;
}

bool Move::is_horizontal() const
{
    return di == 0;
}

int Move::get_di() const
{
    return di;
}

int Move::get_dj() const
{
    return dj;
}

Move Move::reverse() const
{
    return Move(-di, -dj);
}
