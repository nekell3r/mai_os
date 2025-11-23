#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <string>
#include <Windows.h>

class Matrix {
public:
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix& other);
    Matrix& operator=(const Matrix& other);
    
    int& at(size_t row, size_t col);
    int at(size_t row, size_t col) const;
    
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    
    static Matrix loadFromFile(const std::string& filename);
    static Matrix generateRandom(size_t rows, size_t cols, int min_val = 0, int max_val = 255);
    void saveToFile(const std::string& filename) const;
    void print() const;
    
private:
    size_t rows_;
    size_t cols_;
    std::vector<int> data_;
};

struct MedianFilterParams {
    int window_size;
    int iterations;
    int max_threads;
    
    MedianFilterParams() 
        : window_size(3), iterations(1), max_threads(1) {}
};

class MedianFilter {
public:
    MedianFilter(const MedianFilterParams& params);
    Matrix apply(const Matrix& input);
    
    struct Stats {
        double total_time_ms;
        double avg_iteration_time_ms;
        int threads_used;
    };
    
    Stats getStats() const { return stats_; }
    
private:
    MedianFilterParams params_;
    Stats stats_;
    
    Matrix applySingleIteration(const Matrix& input);
    int computeMedian(const Matrix& input, size_t center_row, size_t center_col);
    
    struct ThreadTask {
        const Matrix* input;
        Matrix* output;
        size_t start_row;
        size_t end_row;
        int window_size;
        MedianFilter* filter;
    };
    
    static DWORD WINAPI processRows(LPVOID param);
};
