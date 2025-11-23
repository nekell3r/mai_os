#include "median_filter.h"
#include "../common/comm.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <chrono>

// ============================================================================
// Matrix Implementation
// ============================================================================

Matrix::Matrix(size_t rows, size_t cols) 
    : rows_(rows), cols_(cols), data_(rows * cols, 0) {}

Matrix::Matrix(const Matrix& other) 
    : rows_(other.rows_), cols_(other.cols_), data_(other.data_) {}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        rows_ = other.rows_;
        cols_ = other.cols_;
        data_ = other.data_;
    }
    return *this;
}

int& Matrix::at(size_t row, size_t col) {
    return data_[row * cols_ + col];
}

int Matrix::at(size_t row, size_t col) const {
    return data_[row * cols_ + col];
}

Matrix Matrix::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    size_t rows, cols;
    file >> rows >> cols;
    
    Matrix matrix(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            file >> matrix.at(i, j);
        }
    }
    
    return matrix;
}

Matrix Matrix::generateRandom(size_t rows, size_t cols, int min_val, int max_val) {
    Matrix matrix(rows, cols);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min_val, max_val);
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            matrix.at(i, j) = dis(gen);
        }
    }
    
    return matrix;
}

void Matrix::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create file: " + filename);
    }
    
    file << rows_ << " " << cols_ << "\n";
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            file << at(i, j);
            if (j < cols_ - 1) file << " ";
        }
        file << "\n";
    }
}

void Matrix::print() const {
    std::cout << "Matrix " << rows_ << "x" << cols_ << ":\n";
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            std::cout << at(i, j) << " ";
        }
        std::cout << "\n";
    }
}

// ============================================================================
// MedianFilter Implementation
// ============================================================================

MedianFilter::MedianFilter(const MedianFilterParams& params) 
    : params_(params) {
    stats_.total_time_ms = 0.0;
    stats_.avg_iteration_time_ms = 0.0;
    stats_.threads_used = params.max_threads;
}

int MedianFilter::computeMedian(const Matrix& input, size_t center_row, size_t center_col) {
    std::vector<int> window_values;
    int half_window = params_.window_size / 2;
    
    for (int di = -half_window; di <= half_window; ++di) {
        for (int dj = -half_window; dj <= half_window; ++dj) {
            int row = static_cast<int>(center_row) + di;
            int col = static_cast<int>(center_col) + dj;
            
            // Проверка границ (используем отражение или ближайшее значение)
            if (row < 0) row = 0;
            if (row >= static_cast<int>(input.rows())) row = input.rows() - 1;
            if (col < 0) col = 0;
            if (col >= static_cast<int>(input.cols())) col = input.cols() - 1;
            
            window_values.push_back(input.at(row, col));
        }
    }
    
    // Вычисляем медиану
    size_t n = window_values.size();
    std::nth_element(window_values.begin(), window_values.begin() + n/2, window_values.end());
    
    if (n % 2 == 1) {
        return window_values[n / 2];
    } else {
        // Для четного количества элементов берем среднее двух центральных
        int median1 = window_values[n / 2];
        std::nth_element(window_values.begin(), window_values.begin() + n/2 - 1, window_values.end());
        int median2 = window_values[n / 2 - 1];
        return (median1 + median2) / 2;
    }
}

// Windows threading implementation
DWORD WINAPI MedianFilter::processRows(LPVOID param) {
    ThreadTask* task = static_cast<ThreadTask*>(param);
    
    for (size_t row = task->start_row; row < task->end_row; ++row) {
        for (size_t col = 0; col < task->input->cols(); ++col) {
            int median = task->filter->computeMedian(*task->input, row, col);
            task->output->at(row, col) = median;
        }
    }
    
    return 0;
}

Matrix MedianFilter::applySingleIteration(const Matrix& input) {
    Matrix output(input.rows(), input.cols());
    
    if (params_.max_threads == 1) {
        // Однопоточная версия
        for (size_t row = 0; row < input.rows(); ++row) {
            for (size_t col = 0; col < input.cols(); ++col) {
                output.at(row, col) = computeMedian(input, row, col);
            }
        }
    } else {
        // Многопоточная версия (Windows API)
        size_t rows_per_thread = input.rows() / params_.max_threads;
        size_t remaining_rows = input.rows() % params_.max_threads;
        
        std::vector<HANDLE> threads(params_.max_threads);
        std::vector<ThreadTask> tasks(params_.max_threads);
        
        size_t current_row = 0;
        for (int i = 0; i < params_.max_threads; ++i) {
            size_t rows_for_this_thread = rows_per_thread;
            if (i < static_cast<int>(remaining_rows)) {
                rows_for_this_thread++;
            }
            
            tasks[i].input = &input;
            tasks[i].output = &output;
            tasks[i].start_row = current_row;
            tasks[i].end_row = current_row + rows_for_this_thread;
            tasks[i].window_size = params_.window_size;
            tasks[i].filter = this;
            
            threads[i] = CreateThread(
                nullptr,
                0,
                processRows,
                &tasks[i],
                0,
                nullptr
            );
            
            ASSERT_MSG(threads[i] != nullptr, "Не удалось создать поток");
            
            current_row += rows_for_this_thread;
        }
        
        // Ожидаем завершения всех потоков
        WaitForMultipleObjects(params_.max_threads, threads.data(), TRUE, INFINITE);
        
        // Закрываем дескрипторы потоков
        for (auto& handle : threads) {
            CloseHandle(handle);
        }
    }
    
    return output;
}

Matrix MedianFilter::apply(const Matrix& input) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    Matrix current = input;
    double total_iteration_time = 0.0;
    
    std::stringstream msg;
    msg << "Применение медианного фильтра с " << params_.max_threads << " потоками";
    LogMsg("MedianFilter", msg.str());
    
    for (int iter = 0; iter < params_.iterations; ++iter) {
        auto iter_start = std::chrono::high_resolution_clock::now();
        
        current = applySingleIteration(current);
        
        auto iter_end = std::chrono::high_resolution_clock::now();
        double iter_time = std::chrono::duration<double, std::milli>(iter_end - iter_start).count();
        total_iteration_time += iter_time;
        
        std::stringstream iter_msg;
        iter_msg << "Итерация " << (iter + 1) << "/" << params_.iterations 
                  << " завершена за " << iter_time << " мс";
        LogMsg("MedianFilter", iter_msg.str());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    stats_.total_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    stats_.avg_iteration_time_ms = total_iteration_time / params_.iterations;
    
    return current;
}
