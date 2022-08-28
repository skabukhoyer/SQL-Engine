#include"matrix_bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * matrix, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class Matrix_Cursor{
    public:
    Matrix_Page matrix_page;
    int pageIndex;
    string matrixName;
    int pagePointer;

    public:
    Matrix_Cursor(string matrixName, int pageIndex);
    vector<int> getNext();
    vector<int> getRandomPageRow();
    void nextPage(int pageIndex);
};