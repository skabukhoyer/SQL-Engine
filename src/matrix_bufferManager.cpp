#include "global.h"

Matrix_BufferManager::Matrix_BufferManager()
{
    logger.log("Matrix_BufferManager::Matrix_BufferManager");
}

/**
 * @brief Function called to read a matrix_page from the buffer manager. If the matrix_page is
 * not present in the pool, the matrix_page is read and then inserted into the pool.
 *
 * @param matrixName 
 * @param pageIndex 
 * @return Matrix_Page 
 */
Matrix_Page Matrix_BufferManager::getPage(string matrixName, int pageIndex)
{
    logger.log("Matrix_BufferManager::getPage");
    string pageName = "../data/temp/"+matrixName + "_Page" + to_string(pageIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(matrixName, pageIndex);
}

/**
 * @brief Checks to see if a matrix_page exists in the pool
 *
 * @param pageName 
 * @return true 
 * @return false 
 */
bool Matrix_BufferManager::inPool(string pageName)
{
    logger.log("Matrix_BufferManager::inPool");
    for (auto matrix_page : this->matrix_pages)
    {
        if (pageName == matrix_page.pageName)
            return true;
    }
    return false;
}

/**
 * @brief If the matrix_page is present in the pool, then this function returns the
 * matrix_page. Note that this function will fail if the matrix_page is not present in the
 * pool.
 *
 * @param pageName 
 * @return Matrix_Page 
 */
Matrix_Page Matrix_BufferManager::getFromPool(string pageName)
{
    logger.log("Matrix_BufferManager::getFromPool");
    for (auto matrix_page : this->matrix_pages)
        if (pageName == matrix_page.pageName)
            return matrix_page;
}

/**
 * @brief Inserts matrix_page indicated by matrixName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted matrix_page from the pool and adds
 * the current matrix_page at the end. It naturally follows a queue data structure. 
 *
 * @param matrixName 
 * @param pageIndex 
 * @return Matrix_Page 
 */
Matrix_Page Matrix_BufferManager::insertIntoPool(string matrixName, int pageIndex)
{
    logger.log("Matrix_BufferManager::insertIntoPool");
    Matrix_Page matrix_page(matrixName, pageIndex);
    if (this->matrix_pages.size() >= BLOCK_COUNT)
        matrix_pages.pop_front();
    matrix_pages.push_back(matrix_page);
    return matrix_page;
}

/**
 * @brief The buffer manager is also responsible for writing matrix_pages. This is
 * called when new matrixs are created using assignment statements.
 *
 * @param matrixName 
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void Matrix_BufferManager::writePage(string matrixName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Matrix_BufferManager::writePage");
    Matrix_Page matrix_page(matrixName, pageIndex, rows, rowCount);
    matrix_page.writePage();
}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName 
 */
void Matrix_BufferManager::deleteFile(string fileName)
{

    if (remove(fileName.c_str()))
        logger.log("Matrix_BufferManager::deleteFile: Err");
    else 
        logger.log("Matrix_BufferManager::deleteFile: Success");
}

/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the matrixName and pageIndex.
 *
 * @param matrixName 
 * @param pageIndex 
 */
void Matrix_BufferManager::deleteFile(string matrixName, int pageIndex)
{
    logger.log("Matrix_BufferManager::deleteFile");
    string fileName = "../data/temp/"+matrixName + "_Matrix_Page" + to_string(pageIndex);
    this->deleteFile(fileName);
}