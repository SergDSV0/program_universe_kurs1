#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <chrono>
#include <fstream>
#include <stdexcept>

class Vector {
protected:
    std::vector<double> data;
    bool initialized = false;

public:
    // Конструктор
    Vector() = default;

    // Инициализация константным значением
    void initialize(double value, size_t size) {
        data.assign(size, value);
        initialized = true;
    }

    // Инициализация случайными значениями
    void initialize_random(size_t size, double min = 0.0, double max = 1.0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        data.resize(size);
        std::generate(data.begin(), data.end(), [&]() { return dis(gen); });
        initialized = true;
    }

    // Проверка инициализации
    void check_initialized() const {
        if (!initialized) {
            throw std::runtime_error("Vector is not initialized.");
        }
    }

    // Нахождение минимального значения
    double min() const {
        check_initialized();
        return *std::min_element(data.begin(), data.end());
    }

    // Индекс минимального значения
    size_t min_index() const {
        check_initialized();
        return std::distance(data.begin(), std::min_element(data.begin(), data.end()));
    }

    // Нахождение максимального значения
    double max() const {
        check_initialized();
        return *std::max_element(data.begin(), data.end());
    }

    // Индекс максимального значения
    size_t max_index() const {
        check_initialized();
        return std::distance(data.begin(), std::max_element(data.begin(), data.end()));
    }

    // Нахождение суммы значений
    double sum() const {
        check_initialized();
        return std::accumulate(data.begin(), data.end(), 0.0);
    }

    // Нахождение среднего значения
    double mean() const {
        check_initialized();
        return sum() / data.size();
    }

    // Нахождение эвклидовой нормы
    double euclidean_norm() const {
        check_initialized();
        return std::sqrt(std::accumulate(data.begin(), data.end(), 0.0, [](double sum, double val) { return sum + val * val; }));
    }

    // Нахождение скалярного произведения
    double dot(const Vector& other) const {
        check_initialized();
        if (data.size() != other.data.size()) {
            throw std::invalid_argument("Vectors must be of the same size.");
        }
        return std::inner_product(data.begin(), data.end(), other.data.begin(), 0.0);
    }

    // Экспорт данных
    void export_to_file(const std::string& filename) const {
        check_initialized();
        std::ofstream file(filename);
        for (const double& val : data) {
            file << val << "\n";
        }
    }

    // Импорт данных
    void import_from_file(const std::string& filename) {
        std::ifstream file(filename);
        data.clear();
        double value;
        while (file >> value) {
            data.push_back(value);
        }
        initialized = true;
    }

    // Метод для тестирования времени выполнения
    static void test_execution_time(const Vector& vec) {
        auto start = std::chrono::high_resolution_clock::now();
        vec.sum(); // Просто пример замера для суммы
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Execution time: " << duration.count() << " seconds\n";
    }
};

// Класс для однопоточной обработки
class VectorSingleThread : public Vector {
public:
    VectorSingleThread() = default;

    // Пример метода, который можно реализовать для однопоточной обработки
    double compute_sum() const {
        check_initialized();
        return sum();
    }
};

// Класс для многопоточной обработки с использованием std::thread
class VectorMultiThread : public Vector {
public:
    VectorMultiThread() = default;

    // Пример метода для вычислений в многопоточном режиме
    double compute_sum() const {
        check_initialized();
        size_t num_threads = std::thread::hardware_concurrency();
        size_t chunk_size = data.size() / num_threads;
        std::vector<std::thread> threads;
        std::vector<double> partial_sums(num_threads, 0.0);

        auto sum_chunk = [&](size_t thread_id) {
            size_t start = thread_id * chunk_size;
            size_t end = (thread_id == num_threads - 1) ? data.size() : start + chunk_size;
            partial_sums[thread_id] = std::accumulate(data.begin() + start, data.begin() + end, 0.0);
            };

        for (size_t i = 0; i < num_threads; ++i) {
            threads.push_back(std::thread(sum_chunk, i));
        }

        for (auto& th : threads) {
            th.join();
        }

        return std::accumulate(partial_sums.begin(), partial_sums.end(), 0.0);
    }
};

// Функция для замера производительности многопоточной обработки
void performance_test(size_t size) {
    VectorMultiThread vec;
    vec.initialize_random(size);

    for (size_t num_threads = 1; num_threads <= 16; num_threads *= 2) {
        auto start = std::chrono::high_resolution_clock::now();
        vec.compute_sum(); // Запуск метода с многопоточными вычислениями
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Threads: " << num_threads << ", Time: " << duration.count() << " seconds\n";
    }
}

int main() {
    // Пример замера времени выполнения для однопоточной обработки
    Vector vec;
    vec.initialize_random(1000000);
    Vector::test_execution_time(vec);

    // Пример теста производительности для многопоточной обработки
    performance_test(1e7);  // Пример для размера вектора 10^7

    return 0;
}
