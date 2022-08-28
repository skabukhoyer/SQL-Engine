#include "global.h"
#include <math.h>

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief Construct a new Matrix:: Matrix object used when an assignment command
 * is encountered. To create the matrix object both the matrix name and the
 * columns the matrix holds should be specified.
 *
 * @param matrixName 
 * @param columns 
 */
Matrix::Matrix(string matrixName, vector<string> columns)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/temp/" + matrixName + ".csv";
    this->matrixName = matrixName;
    this->columns = columns;
    this->columnCount = columns.size();
    //this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * columnCount));
    this->maxRowsPerBlock = floor(sqrt((uint)((BLOCK_SIZE) / (4))));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Matrix::extractColumnNames(string firstLine)
{
    logger.log("Matrix::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        // if (columnNames.count(word))
        //     return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    //this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * this->columnCount));
    //we are using the pages/blocks that store ( square blocks of the matrix )
    
    //this->maxRowsPerBlock = 3; //Need to change this
    this->maxRowsPerBlock = floor(sqrt((uint)((BLOCK_SIZE) / (4))));
    //this->maxColumnsPerBlock=3; //Need to change this
    this->maxColumnsPerBlock = floor(sqrt((uint)((BLOCK_SIZE) / (4))));

    return true;
}

struct string_info
{
    string result;
    int size;
};
typedef struct string_info string_info; 

string_info split_comma(string my_str, int index1, int index2)
{
    //cout<<my_str<<" "<<index1<<" "<<index2<<endl;
    vector<string> result;
    string res="";
    stringstream s_stream(my_str); //create string stream from the string
    string_info final_res;
    while(s_stream.good()) 
    {
        string substr;
        getline(s_stream, substr, ','); //get first string delimited by comma
        result.push_back(substr);
    }

    if (index2 >= result.size())
    {
        index2 = result.size();
    }

   for(int i = index1; i < index2; i++) 
   {    
    
        if (i==result.size()-1)
          {
            res = res+result.at(i); 
          }
        else
          {
            res = res+result.at(i)+","; 
          }
          
   }
   final_res.result = res;
   final_res.size = index2 - index1;


    return final_res;
}

void print_vec2D(vector<vector<int>> vec)
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
void print_vec2D_u(vector<vector<uint>> vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        for (int j = 0; j < vec[i].size(); j++)
        {
            cout << vec[j][i]<<" ";
        }
        cout << endl;
    }
}

void print_vec1D(vector<int> vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        cout << vec[i] <<" ";
    }
}

bool check_integer(string word)
{
    for (int i=0; i < word.length(); i++)
    {
        if (!isdigit(word[i]))
        {
            return false;
        }
    }

    return true;
}



/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    string_info line_temp;


    int pageCounter = 0;
    int numColumnBlocks = ceil(float(this->columnCount)/float(this->maxColumnsPerBlock));
    unordered_set<int> dummy;
    dummy.clear();

    //getline(fin, line);  To get the first column this is commented
    for (int columnBlockCounter = 0; columnBlockCounter < numColumnBlocks; columnBlockCounter++)
    {
        vector<int> row(this->maxColumnsPerBlock, -9999);
        vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
        this->distinctValuesInColumns.assign(this->maxColumnsPerBlock, dummy);
        this->distinctValuesPerColumnCount.assign(this->maxColumnsPerBlock, 0);
        vector<uint> pageNameRow; 

        while (getline(fin, line))
        {
            line_temp = split_comma(line, columnBlockCounter*this->maxColumnsPerBlock, columnBlockCounter*this->maxColumnsPerBlock+this->maxColumnsPerBlock);
            line = line_temp.result;
            //cout << line << " " << line_temp.size << endl;

            stringstream s(line);
            for (int columnCounter = 0; columnCounter < line_temp.size; columnCounter++)
            {
                if (!getline(s, word, ','))
                    return false;
                //cout << "Word: "<< word <<endl;
                if(!check_integer(word))
                {
                    cout << "The matrix contains a non integer!";
                    return false;
                }
                row[columnCounter] = stoi(word);
                rowsInPage[pageCounter][columnCounter] = row[columnCounter];
            }
            pageCounter++;
            this->tempColumnSize = line_temp.size;
            this->updateStatistics(row);
            if (pageCounter == this->maxRowsPerBlock)
            {
                matrix_bufferManager.writePage(this->matrixName, this->blockCount, rowsInPage, pageCounter);
                pageNameRow.emplace_back(this->blockCount);
                this->blockCount++;
                this->rowsPerBlockCount.emplace_back(pageCounter);
                this->columnsPerBlockCount.emplace_back(this->tempColumnSize);
                pageCounter = 0;
            }
        }
        if (pageCounter)
        {
            matrix_bufferManager.writePage(this->matrixName, this->blockCount, rowsInPage, pageCounter);
            pageNameRow.emplace_back(this->blockCount);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            this->columnsPerBlockCount.emplace_back(this->tempColumnSize);
            pageCounter = 0;
        }
        this->pageNames.emplace_back(pageNameRow);
        fin.clear();
        fin.seekg(0, ios::beg);
    }
    //print_vec(this->pageNames);
    this->rowCount = ceil(float(this->rowCount)/float(numColumnBlocks));

    if (this->rowCount == 0)
        return false;
    if (this->rowCount != this->columnCount)
    {
        cout << "Not a N x N matrix" <<endl;
        cout <<"Found a "<<rowCount<<"x"<<columnCount<<" matrix"<<endl;
        return false;
    }
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Matrix::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->tempColumnSize; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this matrix.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Matrix::isColumn(string columnName)
{
    logger.log("Matrix::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Matrix::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Matrix::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

vector<int> append_vec(vector<int> a, vector<int> b)
{
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

vector<vector<int>> append_matrices(vector<vector<int>> a, vector<vector<int>> b)
{
    vector<vector<int>> result_matrix;
    vector<int> result_row;
    if (a.size() != b.size())
    {
        cout << "The block matrices are not of the same sizes!"<<endl;
        exit(0);
    }

    for (int i = 0; i < a.size(); i++)
    {
        result_row = append_vec(a[i], b[i]);
        result_matrix.emplace_back(result_row);
    }

    return result_matrix;

}

/**
 * @brief Function prints the matrix. IN 20x20 DIMENSIONS
 *
 */

void Matrix::print()
{
    logger.log("Matrix::print");

    vector<vector<int>> result_matrix;
    int blocks=0;
    int row_flag = 1, col_flag = 1;

    for (int blockMatrixRowCounter = 0; blockMatrixRowCounter < this->pageNames.size() && col_flag; blockMatrixRowCounter++)
    {
        int idx = 0;
        row_flag=1;
        for (int blockMatrixColumnCounter = 0; blockMatrixColumnCounter < this->pageNames[blockMatrixRowCounter].size() && row_flag; blockMatrixColumnCounter++)
            {
                Matrix_Cursor matrix_cursor(this->matrixName, this->pageNames[blockMatrixRowCounter][blockMatrixColumnCounter]);
                vector<int> row;

                do
                {
                    row = matrix_cursor.getRandomPageRow();
                    if(row.size()==0)
                    {
                        continue;
                    }

                    if (blockMatrixRowCounter==0)
                    {
                        if(row.size()<=PRINT_COUNT)
                        {
                            result_matrix.emplace_back(row);
                        }
                        else{
                            vector<int> vect(row.begin(), row.begin()+PRINT_COUNT);
                            result_matrix.emplace_back(vect);
                        }
                        blocks++;
                        idx++;
                        if(result_matrix.size()==PRINT_COUNT){
                            row_flag = 0;
                            idx=0;
                        }

                    }
                    else
                    {
                        int col_size, need_size;
                        col_size = result_matrix[idx].size();
                        need_size = PRINT_COUNT - col_size;
                        if(need_size<=0){
                            col_flag=0;
                            break;
                        }
                        if(row.size()<=need_size){
                            result_matrix[idx] = append_vec(result_matrix[idx], row);
                        }
                        else{
                            vector<int> vect(row.begin(), row.begin()+need_size);
                            result_matrix[idx] = append_vec(result_matrix[idx], vect);
                        }
                        
                        idx++;
                        if(result_matrix[0].size()==PRINT_COUNT and blockMatrixColumnCounter==blocks){
                            col_flag=0;
                        }
                    }

                } while(!row.empty() && row_flag && col_flag);

            }
    }
    cout<<endl;
    print_vec2D(result_matrix);

}



/**
 * @brief This function returns one row of the matrix using the matrix_cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param matrix_cursor 
 * @return vector<int> 
 */
void Matrix::getNextPage(Matrix_Cursor *matrix_cursor)
{
    logger.log("Matrix::getNext");

        if (matrix_cursor->pageIndex < this->blockCount - 1)
        {
            matrix_cursor->nextPage(matrix_cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");

    vector<vector<int>> result_matrix;
    //print_vec2D_u(this->pageNames);
    //cout<<endl;
    
    for (int blockMatrixRowCounter = 0; blockMatrixRowCounter < this->pageNames.size(); blockMatrixRowCounter++)
    {
        rowCount = 0;
        for (int blockMatrixColumnCounter = 0; blockMatrixColumnCounter < this->pageNames[blockMatrixRowCounter].size(); blockMatrixColumnCounter++)
            {
                Matrix_Cursor matrix_cursor(this->matrixName, this->pageNames[blockMatrixRowCounter][blockMatrixColumnCounter]);
                //cout << this->pageNames[blockMatrixRowCounter][0]<<endl;
                vector<int> row;
                do
                {
                    row = matrix_cursor.getRandomPageRow();
                    //print_vec1D(row);
                    //cout<<endl;

                    if (blockMatrixRowCounter==0)
                    {
                        result_matrix.emplace_back(row);
                    }
                    else
                    {
                        result_matrix[rowCount] = append_vec(result_matrix[rowCount], row);
                        rowCount++;
                    }


                } while(!row.empty());
            }
    }
    //print_vec2D(result_matrix);
    if(!this->isPermanent())
        matrix_bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < result_matrix.size(); rowCounter++)
    {
        row = result_matrix[rowCounter];
        if (!row.empty())
            this->writeRow(row, fout);
    }
    fout.close();
    cout << "Exported the matrix successfully."<<endl;

    // //print headings
    // this->writeRow(this->columns, fout);

    // Matrix_Cursor matrix_cursor(this->matrixName, 0);
    // vector<int> row;
    // for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    // {
    //     row = matrix_cursor.getNext();
    //     this->writeRow(row, fout);
    // }
    // fout.close();
}

/**
 * @brief Function to check if matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the matrix from the database by deleting
 * all temporary files created as part of this matrix
 *
 */
void Matrix::unload(){
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        matrix_bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        matrix_bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a matrix_cursor that reads rows from this matrix
 * 
 * @return Matrix_Cursor 
 */
Matrix_Cursor Matrix::getCursor()
{
    logger.log("Matrix::getCursor");
    Matrix_Cursor matrix_cursor(this->matrixName, 0);
    return matrix_cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Matrix::getColumnIndex(string columnName)
{
    logger.log("Matrix::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}
