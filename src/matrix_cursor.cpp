#include "global.h"

Matrix_Cursor::Matrix_Cursor(string matrixName, int pageIndex)
{
    logger.log("Matrix_Cursor::Matrix_Cursor");
    this->matrix_page = matrix_bufferManager.getPage(matrixName, pageIndex);
    this->pagePointer = 0;
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
}

/**
 * @brief This function reads the next row from the matrix_page. The index of the
 * current row read from the matrix_page is indicated by the pagePointer(points to row
 * in matrix_page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Matrix_Cursor::getNext()
{
    logger.log("Matrix_Cursor::geNext");
    vector<int> result = this->matrix_page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        matrixCatalogue.getMatrix(this->matrixName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->matrix_page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

vector<int> Matrix_Cursor::getRandomPageRow()
{
    logger.log("Matrix_Cursor::getRandomPageRow");
    vector<int> result = this->matrix_page.getRow(this->pagePointer);
    this->pagePointer++;
    return result;
}


/**
 * @brief Function that loads Matrix_Page indicated by pageIndex. Now the cursor starts
 * reading from the new matrix_page.
 *
 * @param pageIndex 
 */
void Matrix_Cursor::nextPage(int pageIndex)
{
    logger.log("Matrix_Cursor::nextPage");
    this->matrix_page = matrix_bufferManager.getPage(this->matrixName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}