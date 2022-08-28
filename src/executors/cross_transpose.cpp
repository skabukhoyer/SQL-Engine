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
	
	// if (!matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
	// {
	// 	cout << "SEMANTIC ERROR: Relation does not exist!" << endl;
	// 	return false;
	// }
	// return true;

	//Both tables must exist and resultant table shouldn't
    if (!matrixCatalogue.isMatrix(parsedQuery.cross_transFirstRelationName) || !matrixCatalogue.isMatrix(parsedQuery.cross_transSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Cross transpose matrix don't exist" << endl;
        return false;
    }
	// cout<<parsedQuery.crossFirstRelationName<<endl;
	Matrix matrix1 = *(matrixCatalogue.getMatrix(parsedQuery.cross_transFirstRelationName));
    Matrix matrix2 = *(matrixCatalogue.getMatrix(parsedQuery.cross_transSecondRelationName));

	//If both matrix are the same 
    if(matrix1.matrixName == matrix2.matrixName){
        cout << "SEMANTIC ERROR: both matrix are same" << endl;
        return false;
    }

	//check whether the dimension of the cross transpose matrix is same or not 
	// cout<<matrix1.columnCount<<" "<<matrix1.rowCount<<endl;
	if(matrix1.columnCount != matrix1.rowCount ){
		cout << "SEMANTIC ERROR: Not square matrix" << endl;
        return false;
	}

	if(matrix1.rowCount != matrix2.rowCount || matrix1.columnCount != matrix2.columnCount){
		cout << "SEMANTIC ERROR: matrix dimensions are not same" << endl;
        return false;
	}
    return true;

}


void executeCROSS_TRANSPOSE()
{

	logger.log("executeCROSS_TRANSPOSE"); 
	//Table table = *tableCatalogue.getTable(parsedQuery.selectionRelationName);
	
	Matrix matrix1 = *(matrixCatalogue.getMatrix(parsedQuery.cross_transFirstRelationName));
    Matrix matrix2 = *(matrixCatalogue.getMatrix(parsedQuery.cross_transSecondRelationName));

	for (int blockMatrixRowCounter = 0; blockMatrixRowCounter < matrix1.pageNames.size(); blockMatrixRowCounter++)
    {
        //rowCount = 0;
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


	cout <<" Maxtrix are cross transposed successfully!"<< endl;


	return;

}
