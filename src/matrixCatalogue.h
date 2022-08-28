#include "matrix.h"

/**
 * @brief The MatrixCatalogue acts like an index of matrixs existing in the
 * system. Everytime a matrix is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the matrixCatalogue. 
 *
 */
class MatrixCatalogue
{

    unordered_map<string, Matrix*> matrixs;

public:
    MatrixCatalogue() {}
    void insertMatrix(Matrix* matrix);
    void deleteMatrix(string matrixName);
    Matrix* getMatrix(string matrixName);
    bool isMatrix(string matrixName);
    bool isColumnFromMatrix(string columnName, string matrixName);
    void print();
    ~MatrixCatalogue();
};