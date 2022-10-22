#include "global.h"
/**
 * @brief 
 * SYNTAX: CROSS_TRANSPOSE matrix_name1 matrix_name2
 */
bool syntacticParseCROSS_TRANSPOSE()
{	
	logger.log("syntacticParseCROSS_TRANSPOSE");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
	parsedQuery.queryType = CROSS_TRANSPOSE;
	parsedQuery.cross_transFirstRelationName = tokenizedQuery[1];
    parsedQuery.cross_transSecondRelationName = tokenizedQuery[2];
	
    return true;
	
}

bool semanticParseCROSS_TRANSPOSE()
{

	logger.log("semanticParseCROSS_TRANSPOSE");
	
	//check matrix exist or not
	if (!matrixCatalogue.isMatrix(parsedQuery.cross_transFirstRelationName)) 
	{
		cout << "SEMANTIC ERROR: "<< parsedQuery.cross_transFirstRelationName<<" Matrix does not exist!" << endl;
		return false;
	}
	if(!matrixCatalogue.isMatrix(parsedQuery.cross_transSecondRelationName)){
		cout << "SEMANTIC ERROR: "<< parsedQuery.cross_transSecondRelationName<<" Matrix does not exist!" << endl;
		return false;
	}
	
	Matrix matrix1 = *(matrixCatalogue.getMatrix(parsedQuery.cross_transFirstRelationName));
    Matrix matrix2 = *(matrixCatalogue.getMatrix(parsedQuery.cross_transSecondRelationName));

	//check for square matrix
	if(matrix1.columnCount != matrix1.rowCount ){
		cout << "SEMANTIC ERROR: Not square matrix" << endl;
        return false;
	}

	//check for dimension of two matirx
	if(matrix1.rowCount != matrix2.rowCount || matrix1.columnCount != matrix2.columnCount){
		cout << "SEMANTIC ERROR: matrix dimensions are not same" << endl;
        return false;
	}
    return true;

}


void executeCROSS_TRANSPOSE()
{

	logger.log("executeCROSS_TRANSPOSE"); 
	
	Matrix matrix1 = *(matrixCatalogue.getMatrix(parsedQuery.cross_transFirstRelationName));
    Matrix matrix2 = *(matrixCatalogue.getMatrix(parsedQuery.cross_transSecondRelationName));

	if(matrix1.matrixName==matrix2.matrixName){
		for (int blockMatrixRowCounter = 0; blockMatrixRowCounter < matrix1.pageNames.size(); blockMatrixRowCounter++)
    	{
			for (int blockMatrixColumnCounter = 0; blockMatrixColumnCounter < matrix1.pageNames[blockMatrixRowCounter].size(); blockMatrixColumnCounter++)
			{

					if(blockMatrixRowCounter == blockMatrixColumnCounter)
					{
						Matrix_Cursor matrix_cursor(matrix1.matrixName, matrix1.pageNames[blockMatrixRowCounter][blockMatrixColumnCounter]);
						Matrix_Page current_matrix_page = matrix_cursor.matrix_page;

						current_matrix_page.performTranspose();      
						current_matrix_page.writePage();     		
					}
					else if(blockMatrixRowCounter > blockMatrixColumnCounter)
					{
						
						Matrix_Cursor matrix_cursor1(matrix1.matrixName, matrix1.pageNames[blockMatrixRowCounter][blockMatrixColumnCounter]);
						Matrix_Cursor matrix_cursor2(matrix1.matrixName, matrix1.pageNames[blockMatrixColumnCounter][blockMatrixRowCounter]);   

						Matrix_Page current_matrix_page_1 = matrix_cursor1.matrix_page;         		
						Matrix_Page current_matrix_page_2 = matrix_cursor2.matrix_page;

						current_matrix_page_1.performTranspose();
						current_matrix_page_2.performTranspose(); 

						current_matrix_page_1.swapPage(current_matrix_page_2);       		
						
					}
			}
		}

    }
	else
	{

		for (int blockMatrixRowCounter = 0; blockMatrixRowCounter < matrix1.pageNames.size(); blockMatrixRowCounter++)
		{
			for (int blockMatrixColumnCounter = 0; blockMatrixColumnCounter < matrix1.pageNames[blockMatrixRowCounter].size(); blockMatrixColumnCounter++)
			{

					
				Matrix_Cursor matrix_cursor1(matrix1.matrixName, matrix1.pageNames[blockMatrixRowCounter][blockMatrixColumnCounter]); //accessing block row wise
				Matrix_Cursor matrix_cursor2(matrix2.matrixName, matrix2.pageNames[blockMatrixColumnCounter][blockMatrixRowCounter]); //accessing block column wise     		
				
				Matrix_Page matrix1_current_page = matrix_cursor1.matrix_page;
				Matrix_Page matrix2_current_page = matrix_cursor2.matrix_page;

				matrix1_current_page.performTranspose();
				matrix2_current_page.performTranspose();

				matrix1_current_page.swapPage(matrix2_current_page);
						
						
			}

		}
	}

	cout <<"Matrix are cross transposed successfully!"<< endl;
	return;
}
