#include "global.h"
/**
 * @brief 
 * SYNTAX: T1 <- GROUP BY attribute_name  FROM relation_name RETURN MAX|MIN|SUM|AVG(column_name)
 */
bool syntacticParseGROUP_BY()
{
    logger.log("syntacticParseGROUP_BY");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "RETURN")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    string aggregate = "";
    aggregate = tokenizedQuery[8].substr(0,3);
    if(aggregate=="MAX"){
        parsedQuery.aggregateType = MAX;
    }
    else if(aggregate=="MIN"){
        parsedQuery.aggregateType = MIN;
    }
    else if(aggregate=="SUM"){
        parsedQuery.aggregateType = SUM;
    }
    else if(aggregate=="AVG"){
        parsedQuery.aggregateType = AVG;
    }
    else{
        cout << "SYNTAC ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = GROUP_BY;
    parsedQuery.group_byResultRelationName= tokenizedQuery[0];
    parsedQuery.group_byRelationName= tokenizedQuery[6];
    parsedQuery.group_byAttributeName= tokenizedQuery[4];
    string last_token = tokenizedQuery[8];
    if(last_token.size()<6){
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    string column_name = last_token.substr(4,last_token.size()-5);
    parsedQuery.group_byAggregateColumnName= column_name;

    
    return true;
}

bool semanticParseGROUP_BY()
{
    logger.log("semanticParseGROUP_BY");

    if (tableCatalogue.isTable(parsedQuery.group_byResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.group_byRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.group_byAttributeName, parsedQuery.group_byRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.group_byAggregateColumnName, parsedQuery.group_byRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}


void executeGROUP_BY()
{
    logger.log("executeGROUP_BY");

    Table table = *tableCatalogue.getTable(parsedQuery.group_byRelationName);

    vector<string>resultant_columns(2);
    resultant_columns[0] = parsedQuery.group_byAttributeName;
    string column_name = "";
    switch(parsedQuery.aggregateType){
        case MIN:
            column_name = "MIN";
            break;
        case MAX:
            column_name = "MAX";
            break;
        case SUM:
            column_name = "SUM";
            break;
        case AVG:
            column_name = "AVG";
            break;
    }
    column_name += parsedQuery.group_byAggregateColumnName;
    resultant_columns[1] = column_name;
    Table* resultantTable = new Table(parsedQuery.group_byResultRelationName, resultant_columns);

    Cursor cursor = table.getCursor();
    vector<int> row = cursor.getNext();
    int attributeColumnIndex = table.getColumnIndex(parsedQuery.group_byAttributeName);
    int aggregateColumnIndex = table.getColumnIndex(parsedQuery.group_byAggregateColumnName);
    
    map<int,vector<int>>group;
    
    while (!row.empty())
    {
        int value1 = row[attributeColumnIndex];
        int value2 = row[aggregateColumnIndex];

        if(group.find(value1) == group.end()){
            group[value1] = {value2,value2,value2,1};
        }
        else{
            group[value1][0] = min(group[value1][0],value2);
            group[value1][1] = max(group[value1][0],value2);
            group[value1][2] += value2;
            group[value1][3] += 1;
        }
        row = cursor.getNext();
    }
    
    vector<int>result_row(2);
    if(parsedQuery.aggregateType == MIN){
        for(auto p: group){
            result_row[0] = p.first;
            result_row[1] = p.second[0];
            resultantTable->writeRow<int>(result_row);
            row = cursor.getNext();
        }
    }
    else if(parsedQuery.aggregateType == MAX){
        for(auto p: group){
            result_row[0] = p.first;
            result_row[1] = p.second[1];
            resultantTable->writeRow<int>(result_row);
            row = cursor.getNext();
        }
    }
    else if(parsedQuery.aggregateType == SUM){
        for(auto p: group){
            result_row[0] = p.first;
            result_row[1] = p.second[2];
            resultantTable->writeRow<int>(result_row);
            row = cursor.getNext();
        }
    }
    else if(parsedQuery.aggregateType == AVG){
        for(auto p: group){
            result_row[0] = p.first;
            result_row[1] = p.second[2]/p.second[3];
            resultantTable->writeRow<int>(result_row);
            row = cursor.getNext();
        }
    }
    
    if(resultantTable->blockify())
        tableCatalogue.insertTable(resultantTable);
    else{
        cout<<"Empty Table"<<endl;
        resultantTable->unload();
        delete resultantTable;
    }
    
    return;
}