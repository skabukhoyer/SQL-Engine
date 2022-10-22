#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 13 || tokenizedQuery[3] != "USING" || tokenizedQuery[7] != "ON" || tokenizedQuery[11] != "BUFFER")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    string joiningType = tokenizedQuery[4];
    if(joiningType=="NESTED"){
        parsedQuery.joinType = NESTED;
    }
    else if(joiningType == "PARTHASH"){
        parsedQuery.joinType = PARTHASH;
    }
    else{
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    // tokenizedQuery[5].pop_back();
    if(tokenizedQuery[5].size()<1){
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.joinFirstRelationName = tokenizedQuery[5];
    parsedQuery.joinSecondRelationName = tokenizedQuery[6];
    parsedQuery.joinFirstColumnName = tokenizedQuery[8];
    parsedQuery.joinSecondColumnName = tokenizedQuery[10];

    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    regex numeric("[-]?[0-9]+");
    string secondArgument = tokenizedQuery[12];
    if (regex_match(secondArgument, numeric))
    {
        
        parsedQuery.NO_BUFFER = stoi(secondArgument);
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout<<parsedQuery.joinFirstRelationName<<" "<<parsedQuery.joinSecondRelationName<<endl;
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void writeBucket(int index, vector<vector<int>> &bucket, pair<int,int> b, string tableName){
    logger.log("join.cpp: writeBucket");
    string pageName = "../data/temp/" + tableName + "_bucket" + to_string(index)+"_"+ to_string(b.first);
    ofstream fout(pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < bucket.size(); rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < bucket[0].size(); columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << bucket[rowCounter][columnCounter];
        }
        fout << endl;
    }
    BLOCK_ACCESS++;
    fout.close();
}

void readBucket(int index, vector<vector<int>> &bucket, pair<int,int> p, int cc, string tableName){
    logger.log("join.cpp: readBucket");
    string pageName = "../data/temp/" + tableName + "_bucket" + to_string(index)+"_"+ to_string(p.first);
    int rc = p.second;
    fstream fin(pageName, ios::in);
    int number;
    vector<vector<int>>rows(rc,vector<int>(cc));
    
    for (uint rowCounter = 0; rowCounter < rc; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < cc; columnCounter++)
        {
            fin >> number;
            rows[rowCounter][columnCounter] = number;
        }
    }
    bucket.insert(bucket.end(),rows.begin(),rows.end());
    BLOCK_ACCESS++;
    fin.close();
}
int getHash(int k, int m){
    return (k)%m;
}

void executeJOIN()
{
    logger.log("executeJOIN");

    Table table1 = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
    Table table2 = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));

    vector<string> columns;
    //Creating list of column names
    for (int columnCounter = 0; columnCounter < table1.columnCount; columnCounter++)
    {
        string columnName = table1.columns[columnCounter];
        columns.emplace_back(columnName);
    }

    for (int columnCounter = 0; columnCounter < table2.columnCount; columnCounter++)
    {
        string columnName = table2.columns[columnCounter];
        columns.emplace_back(columnName);
    }

    Table *resultantTable = new Table(parsedQuery.joinResultRelationName, columns);
    Cursor cursor1 = table1.getCursor();
    Cursor cursor2 = table2.getCursor();

    if(parsedQuery.joinType == NESTED){
        BLOCK_ACCESS = 0;
        int first_relation_page_index = 0;
        int second_relation_page_index = 0;
        vector<vector<vector<int>>>firstRelationPages;
        vector<vector<int>> secondRelationPage;
        vector<vector<int>> resultantPage;
        int buffer = parsedQuery.NO_BUFFER -2;
        int no_of_pages_in_first = table1.blockCount;
        int no_of_pages_in_second = table2.blockCount;
        
        int first_relation_column_index = table1.getColumnIndex(parsedQuery.joinFirstColumnName);
        int second_relation_column_index = table2.getColumnIndex(parsedQuery.joinSecondColumnName);
        vector<int> row1 = cursor1.getNext();
        
        for(int i=0;i<no_of_pages_in_first ;i+=buffer){

            firstRelationPages.clear();
            //fill the first table n-2 block 
            for(int j=0;j<min(buffer,no_of_pages_in_first-i);j++){
                vector<vector<int>>temp;
                
                while(!row1.empty() and temp.size()<table1.maxRowsPerBlock){
                    temp.push_back(row1);
                    row1 = cursor1.getNext();

                }
                firstRelationPages.push_back(temp);
                BLOCK_ACCESS++;
            }
            //fill second table a bolck and do computation
            cursor2 = table2.getCursor();
            vector<int> row2 = cursor2.getNext();
            for(int si = 0;si<no_of_pages_in_second;si++){
                secondRelationPage.clear();
                while(!row2.empty() and secondRelationPage.size()<table2.maxRowsPerBlock){
                    secondRelationPage.push_back(row2);
                    row2 = cursor2.getNext();
                }
                BLOCK_ACCESS++;
                for(int k=0;k<firstRelationPages.size();k++){
                    for(int l=0;l<firstRelationPages[k].size();l++){
                        for(int m=0;m<secondRelationPage.size();m++){
                            if(firstRelationPages[k][l][first_relation_column_index]== secondRelationPage[m][second_relation_column_index]){
                                vector<int>vec;
                                vec = firstRelationPages[k][l];
                                vec.insert(vec.end(),secondRelationPage[m].begin(),secondRelationPage[m].end());
                                resultantPage.push_back(vec);
                            }
                            if(resultantPage.size()==resultantTable->maxRowsPerBlock){
                                for(vector<int> v: resultantPage){
                                    resultantTable->writeRow<int>(v);
                                }
                                resultantPage.clear();
                            }
                        }
                    }
                }
            }
        }
        if(resultantPage.size() != 0){
            for(vector<int> v: resultantPage){
                resultantTable->writeRow<int>(v);
            }
            
            resultantPage.clear();
        }
        
        resultantTable->blockify();
        tableCatalogue.insertTable(resultantTable);
        cout<<"The number of block accesses needed: "<<BLOCK_ACCESS<<endl;
        return;
    }



    else if(parsedQuery.joinType == PARTHASH){
        // code here
        BLOCK_ACCESS=0;
        int first_relation_page_index = 0;
        int second_relation_page_index = 0;
        int buffer = parsedQuery.NO_BUFFER -1;

        vector<vector<vector<int>>>bucketPages(buffer);
        vector<vector<pair<int,int>>> bucketIndex1(buffer);
        vector<vector<pair<int,int>>> bucketIndex2(buffer);
        
        int no_of_pages_in_first = table1.blockCount;
        int no_of_pages_in_second = table2.blockCount;
        
        int first_relation_column_index = table1.getColumnIndex(parsedQuery.joinFirstColumnName);
        int second_relation_column_index = table2.getColumnIndex(parsedQuery.joinSecondColumnName);

        //Table1 hashing
        vector<int> row1 = cursor1.getNext();
        int index=-1;
        while(!row1.empty()){
            index = getHash(row1[first_relation_column_index],buffer);
            bucketPages[index].push_back(row1);
            if(bucketPages[index].size() == table1.maxRowsPerBlock){
                bucketIndex1[index].push_back({bucketIndex1[index].size(),bucketPages[index].size()});
                writeBucket(index,bucketPages[index],bucketIndex1[index].back(),parsedQuery.joinFirstRelationName);
                bucketPages[index].clear();
                BLOCK_ACCESS++;
            }

            row1 = cursor1.getNext();
        }
        for(int i=0;i<buffer;i++){
            if(bucketPages[i].size()>0){
                bucketIndex1[i].push_back({bucketIndex1[i].size(),bucketPages[i].size()});
                writeBucket(i,bucketPages[i],bucketIndex1[i].back(),parsedQuery.joinFirstRelationName);
                bucketPages[i].clear();
                BLOCK_ACCESS++;
            }
        }
        // Table2 hashing
        vector<int>row2 = cursor2.getNext();
        index=-1;
        while(!row2.empty()){
            index = getHash(row2[second_relation_column_index], buffer);
            bucketPages[index].push_back(row2);
            if(bucketPages[index].size() == table2.maxRowsPerBlock){
                bucketIndex2[index].push_back({bucketIndex2[index].size(),bucketPages[index].size()});
                writeBucket(index,bucketPages[index],bucketIndex2[index].back(),parsedQuery.joinSecondRelationName);
                bucketPages[index].clear();
                BLOCK_ACCESS++;
            }

            row2 = cursor2.getNext();
        }
        for(int i=0;i<buffer;i++){
            if(bucketPages[i].size()>0){
                bucketIndex2[i].push_back({bucketIndex2[i].size(),bucketPages[i].size()});
                writeBucket(i,bucketPages[i],bucketIndex2[i].back(),parsedQuery.joinSecondRelationName);
                bucketPages[i].clear();
                BLOCK_ACCESS++;
            }
        }
        cout<<"After partitioning: block access: "<<BLOCK_ACCESS<<endl;
        //Joining 
        vector<int> resultantRow;
        vector<vector<int>> table1Bucket;
        vector<vector<int>> table2Bucket;
        for(int i=0;i<buffer;i++){
            if(bucketIndex1[i].size()==0 or bucketIndex2[i].size()==0){
                continue;
            }
            else if(bucketIndex1[i].size()<=bucketIndex2[i].size()){ //take all the pages for a bucket of table1
                for(auto p: bucketIndex1[i]){
                    readBucket(i,table1Bucket,p,table1.columnCount,parsedQuery.joinFirstRelationName);
                }
                for(auto p: bucketIndex2[i]){
                    readBucket(i,table2Bucket,p,table2.columnCount,parsedQuery.joinSecondRelationName);
                    
                    for(int j=0;j<table2Bucket.size();j++){
                        for(int k=0;k<table1Bucket.size();k++){
                            
                            if(table1Bucket[k][first_relation_column_index]==table2Bucket[j][second_relation_column_index]){
                                resultantRow = table1Bucket[k];
                                resultantRow.insert(resultantRow.end(),table2Bucket[j].begin(),table2Bucket[j].end());
                                resultantTable->writeRow<int>(resultantRow);
                                
                            }
                        }
                        
                    }
                    table2Bucket.clear();
                }
                table1Bucket.clear();
            }
            else{
                for(auto p: bucketIndex2[i]){
                    readBucket(i,table2Bucket,p,table2.columnCount,parsedQuery.joinSecondRelationName);
                }
                for(auto p: bucketIndex1[i]){
                    readBucket(i,table1Bucket,p,table1.columnCount,parsedQuery.joinFirstRelationName);
                    for(int j=0;j<table1Bucket.size();j++){
                        for(int k=0;k<table2Bucket.size();k++){
                            if(table1Bucket[j][first_relation_column_index]==table2Bucket[k][second_relation_column_index]){
                                resultantRow = table1Bucket[j];
                                resultantRow.insert(resultantRow.end(),table2Bucket[k].begin(),table2Bucket[k].end());
                                resultantTable->writeRow<int>(resultantRow);
                                
                            }
                        }
                        
                    }
                    table1Bucket.clear();
                }
                table2Bucket.clear();
            }
        }
        
        resultantTable->blockify();
        tableCatalogue.insertTable(resultantTable);
        cout<<"The number of block accesses needed: "<<BLOCK_ACCESS<<endl;
    }
    
    return;
}