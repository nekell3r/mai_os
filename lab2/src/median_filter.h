#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <string>
#include <Windows.h>

// Класс для работы с матрицей
class Matrix {
public:
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix& other);
    Matrix& operator=(const Matrix& other);
    
    int& at(size_t row, size_t col);
    int at(size_t row, size_t col) const;
    
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    
    // Загрузить матрицу из файла
    static Matrix loadFromFile(const std::string& filename);
    
    // Сгенерировать случайную матрицу
    static Matrix generateRandom(size_t rows, size_t cols, int min_val = 0, int max_val = 255);
    
    // Сохранить матрицу в файл
    void saveToFile(const std::string& filename) const;
    
    // Вывести матрицу на экран (для небольших матриц)
    void print() const;
    
private:
    size_t rows_;
    size_t cols_;
    std::vector<int> data_;
};

// Параметры для медианного фильтра
struct MedianFilterParams {
    int window_size;        // Размер окна (например, 3 для окна 3x3)
    int iterations;         // Количество применений фильтра (K)
    int max_threads;        // Максимальное количество потоков
    
    MedianFilterParams() 
        : window_size(3), iterations(1), max_threads(1) {}
};

// Класс для применения медианного фильтра (Windows)
class MedianFilter {
public:
    MedianFilter(const MedianFilterParams& params);
    
    // Применить фильтр к матрице
    Matrix apply(const Matrix& input);
    
    // Получить статистику выполнения
    struct Stats {
        double total_time_ms;
        double avg_iteration_time_ms;
        int threads_used;
    };
    
    Stats getStats() const { return stats_; }
    
private:
    MedianFilterParams params_;
    Stats stats_;
    
    // Применить одну итерацию фильтра
    Matrix applySingleIteration(const Matrix& input);
    
    // Вычислить медиану для окна
    int computeMedian(const Matrix& input, size_t center_row, size_t center_col);
    
    // Обработка части матрицы в отдельном потоке
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
