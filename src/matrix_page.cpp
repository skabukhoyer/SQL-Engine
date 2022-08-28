#include "global.h"
/**
 * @brief Construct a new Matrix_Page object. Never used as part of the code
 *
 */
Matrix_Page::Matrix_Page()
{
    this->pageName = "";
    this->matrixName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Matrix_Page:: Matrix_Page object given the matrix name and page
 * index. When matrixs are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<matrixname>_Page<pageindex>". For example, If the Matrix_Page being loaded is of
 * matrix "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param matrixName 
 * @param pageIndex 
 */
Matrix_Page::Matrix_Page(string matrixName, int pageIndex)
{
    logger.log("Matrix_Page::Matrix_Page");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_Matrix_Page" + to_string(pageIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    this->columnCount = matrix.columnsPerBlockCount[pageIndex];
    //cout << this->columnCount;

    uint maxRowCount = matrix.maxRowsPerBlock;
    //vector<int> row(columnCount, 0);
    //this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = matrix.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        vector<int> row;
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            fin >> number;
            row.emplace_back(number);
            //this->rows[rowCounter][columnCounter] = number;
        }
        this->rows.emplace_back(row);
    }
    fin.close();
}

Matrix_Page::Matrix_Page(string matrixName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Matrix_Page::Matrix_Page");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->matrixName + "_Matrix_Page" + to_string(pageIndex);
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Matrix_Page::getRow(int rowIndex)
{
    logger.log("Matrix_Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

void print_vec2D_2(vector<vector<int>> vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        for (int j = 0; j < vec[i].size(); j++)
        {
            cout << vec[i][j]<<" ";
        }
        cout << endl;
    }
}

void print_mat2D(int *mat, int m, int n) {
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << *((mat+i*n) + j)<<" ";
        }
        cout << endl;
    }
}

vector<vector<int>> transpose(vector<vector<int>> b)
{
    logger.log("Matrix_Page::transpose");
    if (b.size() == 0)
        return b;

    int trans_mat[b[0].size()][b.size()];
    vector<vector<int>> trans_vec;
   
    for (int i = 0; i < b.size(); i++)
    {
        vector<int> trans_row;
        for (int j = 0; j < b[i].size(); j++)
        {
            trans_mat[j][i] = b[i][j];
        }
    }
    for (int i = 0; i < b[0].size(); i++)
    {
        vector<int> trans_row;
        for (int j = 0; j < b.size(); j++)
        {
            trans_row.emplace_back(trans_mat[i][j]);
        }

        trans_vec.emplace_back(trans_row);
    }
   
    return trans_vec;    
}


vector<vector<int>> Matrix_Page::getrows()
{
    logger.log("Matrix_Page::getrows");
    return this->rows;

}

void Matrix_Page::performTranspose()
{
    logger.log("Matrix_Page::performTranspose");
    vector<vector<int>> temp_vec;

    temp_vec = transpose(this->rows);
    this->rows.clear();
    this->rows = temp_vec;
    this->rowCount = temp_vec.size();
    this->columnCount = temp_vec[0].size();

}

/**
 * @brief swaps pages between two different matrix
 * 
 */
void Matrix_Page:: swapPage(Matrix_Page page2){
    logger.log("Matrix_Page::swapPage");
    string tempPageName = this->pageName;
    this->pageName = page2.pageName;
    page2.pageName = tempPageName;
    this->writePage();
    page2.writePage(); 
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Matrix_Page::writePage()
{
    logger.log("Matrix_Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            if(this->rows[rowCounter][columnCounter] != -9999)
                fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}
