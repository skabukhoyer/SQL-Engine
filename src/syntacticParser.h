#include "tableCatalogue.h"
#include "matrixCatalogue.h"
// #include "matrix.h" 

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    LOAD_MATRIX,
    PRINT_MATRIX,
    CROSS_TRANSPOSE,
    EXPORT_MATRIX,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    GROUP_BY,
    SORT,
    SOURCE,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};
enum AggregateType
{
    MIN,
    MAX,
    SUM,
    AVG,
    NO_AGGREGATE
};
enum JoinType
{
    NESTED,
    PARTHASH,
    SIMPLE
};
class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;
    
    AggregateType aggregateType= NO_AGGREGATE;
    string group_byResultRelationName="";
    string group_byRelationName="";
    string group_byAttributeName="";
    string group_byAggregateColumnName="";

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    //Self Implemented Code
    string cross_transFirstRelationName = "";
    string cross_transSecondRelationName = "";
    //Self Implemented Code

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    JoinType joinType = SIMPLE ;
    int NO_BUFFER = 0;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    string sortRelationName = "";

    string sourceFileName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
//Self Implemented
bool syntacticParseLOAD_MATRIX();
bool syntacticParsePRINT_MATRIX();
bool syntacticParseCROSS_TRANSPOSE();
bool syntacticParseEXPORT_MATRIX();
//Self Implemented
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseGROUP_BY();
// bool syntacticParse
bool syntacticParseSORT();
bool syntacticParseSOURCE();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
