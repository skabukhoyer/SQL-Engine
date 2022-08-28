// #include"logger.h"
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files. 
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs. 
 *</p>
 */

class Matrix_Page{

    string matrixName;
    string pageIndex;
    int columnCount;
    int rowCount;
    vector<vector<int>> rows;

    public:

    string pageName = "";
    Matrix_Page();
    Matrix_Page(string matrixName, int pageIndex);
    Matrix_Page(string matrixName, int pageIndex, vector<vector<int>> rows, int rowCount);
    vector<int> getRow(int rowIndex);
    vector<vector<int>> getrows();
    void performTranspose();
    void swapPage(Matrix_Page);
    void writePage();
};