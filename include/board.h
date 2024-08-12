#pragma once

#include <string>
#include <vector>

class Move
{
public:
    Move() = default;

    Move(int di, int dj);

    bool is_vertical() const;

    bool is_horizontal() const;

    int get_di() const;

    int get_dj() const;

    Move reverse() const;

private:
    int di, dj;
};

class Board
{
public:
    static Board create_goal(unsigned size);

    static Board create_random(unsigned size);

    Board() = default;

    explicit Board(const std::vector<std::vector<unsigned>> & data);

    explicit Board(std::vector<std::vector<unsigned>> && data);

    std::size_t size() const;

    bool is_goal() const;

    unsigned hamming() const;

    unsigned manhattan() const;

    std::string to_string() const;

    bool is_solvable() const;

    Board my_move(const Move & m) const;

    std::size_t get_hash() const;

    bool is_valid_move(const Move & m) const;

    friend bool operator==(const Board & lhs, const Board & rhs)
    {
        return lhs.m_data == rhs.m_data;
    }

    friend bool operator!=(const Board & lhs, const Board & rhs)
    {
        return lhs.m_data != rhs.m_data;
    }

    friend std::ostream & operator<<(std::ostream & out, const Board & board)
    {
        return out << board.to_string();
    }

    const std::vector<unsigned> & operator[](std::size_t i) const
    {
        return m_data[i];
    }

private:
    std::vector<std::vector<unsigned>> m_data;
    unsigned m_inversions = 0;
    unsigned m_hamming_distance = 0;
    unsigned m_manhattan_distance = 0;
    std::size_t m_blank_row = 0, m_blank_col = 0;
    std::size_t hash = 0;

    void initialize(const std::vector<std::vector<unsigned>> & data);

    static unsigned unsigned_dist(unsigned a, unsigned b);

    std::pair<std::size_t, std::size_t> expected(unsigned value) const;

    static unsigned calc_inversions(std::vector<unsigned> & arr);

    static unsigned merge_sort(std::vector<unsigned> & arr, std::vector<unsigned> & temp, std::size_t left, std::size_t right);

    static unsigned merge(std::vector<unsigned> & arr, std::vector<unsigned> & temp, std::size_t left, std::size_t mid, std::size_t right);
};
