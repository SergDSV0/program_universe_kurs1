#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>

using namespace std;

class Matrix {
public:
    virtual void print() const = 0;
    virtual void import(const string& filename) = 0;
    virtual void exportToFile(const string& filename) const = 0;
    virtual Matrix* add(const Matrix& other) const = 0;
    virtual Matrix* subtract(const Matrix& other) const = 0;
    virtual Matrix* multiplyScalar(double scalar) const = 0;
    virtual Matrix* multiplyMatrix(const Matrix& other) const = 0;
    virtual Matrix* transpose() const = 0;
    virtual ~Matrix() {}
};

class DenseMatrix : public Matrix {
private:
    vector<vector<double>> data;
    size_t rows, cols;

public:
    DenseMatrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
        data.resize(rows, vector<double>(cols, 0));
    }

    void set(size_t i, size_t j, double value) {
        if (i >= rows || j >= cols) throw out_of_range("Index out of bounds");
        data[i][j] = value;
    }

    double get(size_t i, size_t j) const {
        if (i >= rows || j >= cols) throw out_of_range("Index out of bounds");
        return data[i][j];
    }

    void print() const override {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                cout << data[i][j] << " ";
            }
            cout << endl;
        }
    }

    void import(const string& filename) override {
        ifstream file(filename);
        if (!file.is_open()) throw runtime_error("Failed to open file");
        file >> rows >> cols;
        data.resize(rows, vector<double>(cols, 0));
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                file >> data[i][j];
            }
        }
    }

    void exportToFile(const string& filename) const override {
        ofstream file(filename);
        if (!file.is_open()) throw runtime_error("Failed to open file");
        file << rows << " " << cols << endl;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                file << data[i][j] << " ";
            }
            file << endl;
        }
    }

    Matrix* add(const Matrix& other) const override {
        const DenseMatrix* denseOther = dynamic_cast<const DenseMatrix*>(&other);
        if (!denseOther || denseOther->rows != rows || denseOther->cols != cols)
            throw invalid_argument("Matrix dimensions must match");

        DenseMatrix* result = new DenseMatrix(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result->set(i, j, data[i][j] + denseOther->get(i, j));
            }
        }
        return result;
    }

    Matrix* subtract(const Matrix& other) const override {
        const DenseMatrix* denseOther = dynamic_cast<const DenseMatrix*>(&other);
        if (!denseOther || denseOther->rows != rows || denseOther->cols != cols)
            throw invalid_argument("Matrix dimensions must match");

        DenseMatrix* result = new DenseMatrix(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result->set(i, j, data[i][j] - denseOther->get(i, j));
            }
        }
        return result;
    }

    Matrix* multiplyScalar(double scalar) const override {
        DenseMatrix* result = new DenseMatrix(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result->set(i, j, data[i][j] * scalar);
            }
        }
        return result;
    }

    Matrix* multiplyMatrix(const Matrix& other) const override {
        const DenseMatrix* denseOther = dynamic_cast<const DenseMatrix*>(&other);
        if (!denseOther || cols != denseOther->rows)
            throw invalid_argument("Matrix dimensions must match for multiplication");

        DenseMatrix* result = new DenseMatrix(rows, denseOther->cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < denseOther->cols; ++j) {
                double sum = 0;
                for (size_t k = 0; k < cols; ++k) {
                    sum += data[i][k] * denseOther->get(k, j);
                }
                result->set(i, j, sum);
            }
        }
        return result;
    }

    Matrix* transpose() const override {
        DenseMatrix* result = new DenseMatrix(cols, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result->set(j, i, data[i][j]);
            }
        }
        return result;
    }
};

// Диагональная матрица
class DiagonalMatrix : public Matrix {
private:
    vector<double> data;
    size_t size;

public:
    DiagonalMatrix(size_t size) : size(size) {
        data.resize(size, 0);
    }

    void set(size_t i, double value) {
        if (i >= size) throw out_of_range("Index out of bounds");
        data[i] = value;
    }

    double get(size_t i) const {
        if (i >= size) throw out_of_range("Index out of bounds");
        return (i < size) ? data[i] : 0;
    }

    void print() const override {
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                if (i == j)
                    cout << data[i] << " ";
                else
                    cout << "0 ";
            }
            cout << endl;
        }
    }

    void import(const string& filename) override {
        ifstream file(filename);
        if (!file.is_open()) throw runtime_error("Failed to open file");
        file >> size;
        data.resize(size);
        for (size_t i = 0; i < size; ++i) {
            file >> data[i];
        }
    }

    void exportToFile(const string& filename) const override {
        ofstream file(filename);
        if (!file.is_open()) throw runtime_error("Failed to open file");
        file << size << endl;
        for (size_t i = 0; i < size; ++i) {
            file << data[i] << " ";
        }
    }

    Matrix* add(const Matrix& other) const override {
        const DiagonalMatrix* diagOther = dynamic_cast<const DiagonalMatrix*>(&other);
        if (!diagOther || diagOther->size != size)
            throw invalid_argument("Matrix dimensions must match");

        DiagonalMatrix* result = new DiagonalMatrix(size);
        for (size_t i = 0; i < size; ++i) {
            result->set(i, data[i] + diagOther->get(i));
        }
        return result;
    }

    Matrix* subtract(const Matrix& other) const override {
        const DiagonalMatrix* diagOther = dynamic_cast<const DiagonalMatrix*>(&other);
        if (!diagOther || diagOther->size != size)
            throw invalid_argument("Matrix dimensions must match");

        DiagonalMatrix* result = new DiagonalMatrix(size);
        for (size_t i = 0; i < size; ++i) {
            result->set(i, data[i] - diagOther->get(i));
        }
        return result;
    }

    Matrix* multiplyScalar(double scalar) const override {
        DiagonalMatrix* result = new DiagonalMatrix(size);
        for (size_t i = 0; i < size; ++i) {
            result->set(i, data[i] * scalar);
        }
        return result;
    }

    Matrix* multiplyMatrix(const Matrix& other) const override {
        const DiagonalMatrix* diagOther = dynamic_cast<const DiagonalMatrix*>(&other);
        if (!diagOther || diagOther->size != size)
            throw invalid_argument("Matrix dimensions must match");

        DiagonalMatrix* result = new DiagonalMatrix(size);
        for (size_t i = 0; i < size; ++i) {
            result->set(i, data[i] * diagOther->get(i));
        }
        return result;
    }

    Matrix* transpose() const override {
        return new DiagonalMatrix(size);
    }
};

// Пример использования
int main() {
    try {
        DenseMatrix m1(2, 2);
        m1.set(0, 0, 1);
        m1.set(0, 1, 2);
        m1.set(1, 0, 3);
        m1.set(1, 1, 4);

        m1.print();

        m1.exportToFile("matrix.txt");

        DenseMatrix m2(2, 2);
        m2.import("matrix.txt");
        m2.print();

        Matrix* m3 = m1.add(m2);
        m3->print();

        delete m3;
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}

