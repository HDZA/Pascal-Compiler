
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <stack>
#include <sstream>
#include <time.h>
#include <algorithm>
#include <stdlib.h>
using namespace std;
       
       
/*********************ENUM DATA TYPES GO HERE****************/
enum storeType { INTEGER, BOOLEAN, PROG_NAME, UNKNOWN };
enum allocation { YES, NO };
enum modes { VARIABLE, CONSTANT };
/*********************ENUM DATA TYPES SECTION ENDS***********/
       
       
/********************STRUCT DEFINING GOES HERE***************/
struct entry{ //define symbol table entry format{
    string internalName;
    string externalName;
    storeType dataType;
    modes mode;
    string value;
    allocation alloc;
    int units;
};
/********************STRUCT DEFINING SECTION ENDS***************/
       
       
/********************GLOBAL VARIABLES GO HERE****************/
const int MAX_SYMBOL_TABLE_SIZE = 256;
char charac;
const char END_OF_FILE = '$';// arbitrary choice
char prevCharac = ' ';
int lstLineNumber = 0;
string token;
int maxTempNo = -1;
int currentTempNo = -1;
int labelCounter = -1;
int beginCounter = 0;
string registerA = "";
vector<entry> symbolTable;
string keywords[19] = { "program", "begin", "end", "var", "const",
                        "integer", "boolean", "true", "false", "not",
                        "read", "write", "if", "then", "else", "repeat",
                        "while", "do", "until" };
bool error = false;
int iCounter, bCounter, pCounter = 0;
stack <string> Operator;
stack <string> Operand;
/********************GLOBAL VARIABLES SECTION ENDS***********/
       
       
/********************IFSTREAM AND OFSTREAM VARIABLES GO HERE****/
ifstream sourceFile;
ofstream listingFile, objectFile;
/********************IFSTREAM AND OFSTREAM SECTION ENDS*********/
       
       
/********************FUNCTION DEFINING GO HERE**************************/
//stage 0
char NextChar();
string NextToken();
void CreateListingHeader();
bool isNonKeyID();
//void PrintSymbolTable();
void CreateListingTrailer();
void processError(string);
string genInternalName(storeType);
storeType WhichType(string);
string WhichValue(string);
void insert(string, storeType, modes, string, allocation, int);
void parser();
void Prog();
void ProgStmt();
void Consts();
void Vars();
void BeginEndStmt();
void ConstStmts();
void VarStmts();
string Ids();
bool isBOOLEAN();
bool isINTEGER();
      
//stage 1
void EXEC_STMTS();
void EXEC_STMT();
void ASSIGN_STMT();
void READ_STMT();
void READ_LIST();
void WRITE_STMT();
void WRITE_LIST();
void EXPRESS();
void EXPRESSES();
void TERM();
void TERMS();
void FACTOR();
void FACTORS();
void PART();
void Code(string operatr, string operand1 = "", string operand2 = "");
void pushOperator(string);
void pushOperand(string);
string popOperator();
string popOperand();
string GetTemp();
void FreeTemp();
string GetLabel();
void EmitProgramCode();
void EmitEndCode();
void EmitReadCode(string);
void EmitWriteCode(string);
void EmitAssignCode(string, string);
void EmitAdditionCode(string, string);
void EmitSubtractionCode(string, string);
void EmitNegCode(string);
void EmitMultiplicationCode(string, string);
void EmitDivisionCode(string, string);
void EmitModuloCode(string, string);
void EmitAndCode(string, string);
void EmitOrCode(string, string);
void EmitNotCode(string);
void EmitEqualityCode(string, string);
void EmitNotEqualCode(string, string);
void EmitGreaterThanCode(string, string);
void EmitGreaterThanEqualCode(string, string);
void EmitLessThanCode(string, string);
void EmitLessThanEqualCode(string, string);
  
//stage 2
void IF_STMT();
void ELSE_PT();
void WHILE_STMT();
void REPEAT_STMT();
void NULL_STMT();
void EmitThenCode(string);
void EmitElseCode(string);
void EmitPostIfCode(string);
void EmitWhileCode();
void EmitDoCode(string);
void EmitPostWhileCode(string, string);
void EmitRepeatCode();
void EmitUntilCode(string, string);
   
   
  
/********************FUNCTION DEFINING ENDS HERE*************************/
       
int main(int argc, char **argv)
{
    sourceFile.open(argv[1]);
       
    // Check for success.
    if (sourceFile.fail())
    {
        cerr << "Error opening sourceFile for input.\n";
        return 1;
    }
               
    listingFile.open(argv[2]);
       
    // Check for success.
    if (listingFile.fail())
    {
        cerr << "Error opening listingFile for input.\n";
        return 1;
    }
       
    objectFile.open(argv[3]);
       
    // Check for success.
    if (objectFile.fail())
    {
        cerr << "Error opening objectFile for input.\n";
        return 1;
    }
           
    CreateListingHeader();
    parser();
    CreateListingTrailer();
    //PrintSymbolTable();
           
    sourceFile.close();
    listingFile.close();
    objectFile.close();
       
  return 0;
}
       
void CreateListingHeader()
{
    //print "STAGE2:", names, DATE, TIME OF DAY;
    //line numbers and source statements should be aligned under the headings
    time_t now = time (NULL);                                //taken from /usr/local/4301/src/getTime.C
    listingFile<<"STAGE2: Aldo Hernandez, David Cardenas, Jerry Bowden  "<<ctime(&now)<<endl;
    listingFile<<"LINE NO.              SOURCE STATEMENT\n\n";   //print "LINE NO:", "SOURCE STATEMENT";
}
       
void parser()
{
    NextChar();
           
    if (NextToken().compare("program"))
        processError("keyword \"program\" expected");
           
    Prog();
}
       
void CreateListingTrailer()
{
    listingFile << "\nCOMPILATION TERMINATED";
    cout << "COMPILATION TERMINATED";
           
    if (error)
    {
        listingFile << setw(6)  << " " << "1 ";
        cout << setw(6) << " " << "1 ";
    }
    else
    {
        listingFile << setw(6) << " " << "0 ";
        cout << setw(6) << " " << "0 ";
    }
               
    listingFile <<"ERRORS ENCOUNTERED";
    cout <<"ERRORS ENCOUNTERED\n";
}
       
void PrintSymbolTable()
{
    vector<entry>::iterator itr = symbolTable.begin();
    vector<entry>::iterator end = symbolTable.end();
           
    // Print the symbol table header --- just took Jerry's //Code from listing header and reused
    time_t now = time (NULL);                                //taken from /usr/local/4301/src/getTime.C
    objectFile<<"STAGE2: Aldo Hernandez, David Cardenas, Jerry Bowden  "<< ctime(&now) << endl;
    objectFile<<"Symbol Table\n\n";
           
    // now loop through SymbolTable vector and print out each entry
    for ( ; itr < end; itr++)
    {
        // turn on left justification for objectFile stream....stays set till changed (<< left)
        // if external name is longer than 15 chars....only print substring of first 15 chars
        if ( (itr->externalName).length() > 15)
            objectFile << left << setw(17) <<(itr->externalName).substr(0,15);
        else
            objectFile << left << setw(17) << itr->externalName;
               
        // print internal name
            objectFile << setw(3) << itr->internalName;
       
        // turn on right justification for objectFile stream....stays set till changed (<< right)
        // since itr->datatype returns 0,1,2....print appropriate dataType for each case
        switch(itr->dataType)
        {
            case 0:
                if ( (itr->internalName).length() > 3)
                    objectFile << right << setw(11) << "INTEGER";
                else
                    objectFile << right << setw(12) << "INTEGER";
                break;
            case 1:
                if ( (itr->internalName).length() > 3)
                    objectFile << right << setw(11) << "BOOLEAN";
                else
                    objectFile << right << setw(12) << "BOOLEAN";
                break;
            case 2:
                if ( (itr->internalName).length() > 3)
                    objectFile << right << setw(11) << "PROG_NAME";
                else
                    objectFile << right << setw(12) << "PROG_NAME";
                break;
            default:
                processError("invalid type for symbol");
                break;
        }
               
        // since itr->mode return 0,1....print appropriate mode for each case
        switch(itr->mode)
        {
            case 0:
                objectFile << setw(10) << "VARIABLE";
                break;
            case 1:
                objectFile << setw(10) << "CONSTANT";
                break;
        }
               
        // print value
        if (itr->value == "true")
            objectFile << setw(17) << "1";
        else if (itr->value == "false")
            objectFile << setw(17) << "0";
        else if ( (itr->value).length() > 15 )
        {
            objectFile << setw(17) <<(itr->value).substr(0,15);
        }
        else
            objectFile << setw(17) << itr->value;
               
        // since itr->alloc return 0,1 ... print appropriate word for each case
        switch(itr->alloc)
        {
            case 0:
                objectFile << setw(5) << "YES";
                break;
            case 1:
                objectFile << setw(5) << "NO";
                break;
        }
               
        // print units value
        objectFile << setw(3) << itr->units << '\n';
    }
           
    // set objectFile stream to right justified then exit
    objectFile << right;
    return;
}
       
//Production 1
void Prog() //token should be "program"  - Production 1
{
       
    if (token != "program")
        processError(" keyword \"program\" expected");  //process error: keyword "program" expected
                     
    ProgStmt();
           
    if (token == "const")
        Consts();
           
    if (token == "var")
        Vars();
           
    if (token != "begin")
        processError(" keyword \"const\", \"var\", or \"begin\" expected");      //process error: keyword "begin" expected
                    
    BeginEndStmt();
       
    if (token != "$")
        processError(" no text may follow \"end\"");      //process error: no text may follow "end"
}
       
//Production 2
void ProgStmt() //token should be "program"
{
     string x;
     //if (token.compare("program"))
     if (token != "program")
         processError(" keyword \"program\" expected"); //process error: keyword "program" expected
               
     x = NextToken();
     if (!isNonKeyID())
        processError(" program name expected"); // process error: program name expected
               
     if (NextToken() != ";")
        processError(" semicolon expected");    // process error: semicolon expected
               
     NextToken();
     insert(x,PROG_NAME,CONSTANT,x,NO,0);
}
       
//Production 3
void Consts() //token should be "const"
{
     if (token != "const")
        processError(" keyword \"const\" expected");    //process error: keyword "const" expected
     NextToken();
     if (!isNonKeyID())
        processError(" non-keyword identifier must follow \"const\"");  //process error: non-keyword identifier must follow "const"
               
     ConstStmts();
 }
       
 //Production 4
 void Vars() //token should be "var"
{
     if(token != "var")
        processError(" keyword \"var\" expected");  //process error: keyword "var" expected
               
     NextToken();
     if (!isNonKeyID())
        processError(" non-keyword identifier must follow \"var\" ");   //process error: non-keyword identifier must follow "var"d
               
     VarStmts();
}
        
      
//Production 5
void BeginEndStmt() //token should be "begin"
{
     //if (token.compare("begin"))
     if (token != "begin")
        processError("keyword \"begin\" expected");   //process error: keyword "begin" expected
     else
     {
        // first begin so output RAMM program code
        if (beginCounter == 0)
             Code("program");
               
        // token is begin so increment counter
        beginCounter++;
     }
          
     NextToken();
     if ( isNonKeyID() || token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin"){
        EXEC_STMTS();
     }
       
     
     if (token != "end"){
        processError("keyword \"end\" expected");  // process error: keyword "end" expected
     }
     else
     {
        beginCounter--;
        //Code("end");
     }
      
    NextToken();
      
    if (beginCounter == 0 && token != ".")
        processError("period expected");
    else if (beginCounter > 0 && token != ";")
        processError("semicolon expected");
    else if (beginCounter < 0 )
        processError("no begin for this end"); 
      
    if (token == ".")
        Code("end");
       
    NextToken();
}
        
//Production 6
void ConstStmts() //token should be isNonKeyID()
{
    string x,y;
    int index = -1;
           
    if (!isNonKeyID())
        processError("non-keyword identifier expected");   //process error: non-keyword identifier expected
               
     x = token;
     if (NextToken() != "=")
        processError("\"=\" expected");    //process error: "=" expected
               
     y = NextToken();
            
     if ( y != "+" && y != "-" && y !="not" && !isNonKeyID() && !isBOOLEAN() && !isINTEGER())
        processError(" token to right of \"=\" illegal");   //process error: token to right of "=" illegal
               
     if (y == "+" || y == "-")
     {
        NextToken();
        if(!isINTEGER())
            processError("integer expected after sign");   //process error: integer expected after sign
                   
        y = y + token;
     }
     if (y == "not")
     {
        NextToken();
          
        //token could be externalName or "true" or "false"
        for (uint i = 0; i < symbolTable.size(); i++)
        {
            if (symbolTable[i].externalName == token)
                index = i;
        }
          
        // token was found in table...so check type for BOOLEAN
        if (index != -1)
        {
            if (symbolTable[index].dataType != BOOLEAN)
                processError("boolean expected after not");
        }
        // or it could be "true" or "false"
        else if(!isBOOLEAN())
        {
            processError("boolean expected after not");    //process error: boolean expected after not
        }
           
        // token was an externalName in symbolTable
        if (index != -1)
        {
            if (symbolTable[index].value == "true")
                y = "false";
            else if (symbolTable[index].value == "false")
                y = "true";
            else
                processError("invalid value for BOOLEAN type");
        }
        else if (token == "true")
        { 
            y = "false";
        }
        else
        {
             y = "true";
        }
     }
     if (NextToken() != ";")
        processError("\":\" expected");    //process error: semicolon expected
               
     insert(x,WhichType(y),CONSTANT,WhichValue(y),YES,1);
            
     NextToken();
           
     //if (!token.compare("begin")||!token.compare("var")||!isNonKeyID())
     if (token != "begin" && token != "var" && !isNonKeyID())
        processError("non-keyword identifier, \"begin\", or \"var\" expected"); //non-keyword identifier,"begin", or "var" expected
               
     //NextToken();    was causing errors
     if (isNonKeyID())
     {
        ConstStmts();
     }
}
        
//Production 7
void VarStmts() //token should be isNonKeyID()
{
     string x,y;
     if (!isNonKeyID())
        processError(" non-keyword identifier expected");   //process error: non-keyword identifier expected
               
     x = Ids();
     //if (token.compare(":"))
     if (token != ":")
        processError(" colon \":\" expected");  //process error: colon ":" expected
            
     NextToken();
     if(token != "integer" && token != "boolean")
        processError(" illegal type follows \":\" ");   //process error: illegal type follows ":"
               
     y = token;
     if(NextToken() != ";")
        processError("\":\" expected ");   //process error: semicolon expected
             
     if (y == "integer")
        insert(x,INTEGER, VARIABLE, "", YES,1);
     else if (y == "boolean")
        insert(x,BOOLEAN, VARIABLE, "", YES, 1);
     else if (y == "PROG_NAME")
        insert(x,PROG_NAME, VARIABLE, "", YES, 1);
     else
        processError("not a valid storeType");
             
     NextToken();
      
     //if (!token.compare("begin")||!isNonKeyID())
     if (token != "begin" && !isNonKeyID() )
        processError(" non-keyword identifier or \"begin\" expected");  //process error: non-keyword identifier or "begin" expected
           
     if (isNonKeyID())
        VarStmts();
}
        
//Production 8
string Ids() //token should be isNonKeyID()
{
     string temp,tempString;
            
     if (!isNonKeyID())
        processError(" non-keyword identifier expected");   //process error: non-keyword identifier expected
           
     tempString = token;
     temp = token;
            
     if(NextToken() == ",")
     {
        NextToken();
        if (!isNonKeyID())
            processError(" non-keyword identifier expected");   //process error: non-keyword identifier expected
                   
        tempString = temp + "," + Ids();
     }
            
     return tempString;
}
        
void insert(string externalName, storeType inType, modes inMode, string inValue,
            allocation inAlloc, int inUnits)
{
    string name;
    string::iterator itr = externalName.begin();
    string::iterator end = externalName.end();
           
           
    for ( ; itr < end; itr++)
    {
        name = "";
               
        while( (*itr != ',') && (itr < end))
        {
            name += *itr;
            itr++;
        }
               
        if (name != "")
        {
            if (name.length() > 15)
                name = name.substr(0,15);
               
            for (unsigned int i = 0; i < symbolTable.size(); i++)
            {
                // is externalName is what to check against here
                if (symbolTable[i].externalName == name)
                        processError("multiple name definition");
            }
                   
            if (find(keywords, keywords+19, name) != keywords+19)
                processError("illegal use of keyword");
            else
            {
                entry load;
                       
                if ( name.length() > 15)
                    load.externalName = name.substr(0,15);
                else
                    load.externalName = name;
                       
                if(isupper(name[0]))
                    load.internalName = name;
                else
                    load.internalName = genInternalName(inType);
                       
                load.dataType = inType;
                load.mode = inMode;
                load.value = inValue;
                load.alloc = inAlloc;
                load.units = inUnits;
                       
                symbolTable.push_back(load);
            }
        }
    }
}
       
storeType WhichType(string name)
{
    bool isInt = true;
           
    if ( (name == "true") || (name == "false") )
    {
        return BOOLEAN;
    }
    else if ( isdigit(name[0]) || name[0] == '+' || name[0] == '-' )
    {
        for (unsigned int x = 1; x < name.length(); x++)
        {
            if (!isdigit(name[x]))
                isInt = false;
        }
               
        if (isInt)
            return INTEGER;
    }
    else
    {
        for (unsigned int i = 0; i < symbolTable.size(); i++)
        {
            if (symbolTable[i].externalName == name)
                return symbolTable[i].dataType;
        }
           
    }
           
    // our defined error/exit function
   processError("reference to undefined constant");
           
    //shouldn't get here
    return PROG_NAME;
}
       
string WhichValue(string name)
{
    string value ="";
    bool isInt = true;
       
    // if it is a literal
    // check for boolean
    if ( name == "true" || name == "false" )
        value = name;
    // check for integer
    else if ( isdigit(name[0]) || name[0] == '+' || name[0] == '-' )
    {
        // check that all chars in name are digits
        for (unsigned int x = 1; x < name.length(); x++)
        {
            if (!isdigit(name[x]))
                isInt = false;
        }
               
        if (isInt)
            value = name;
    }
    // look in symbolTable for matching externalName
    else
    {
        if (name.length() > 15)
            name = name.substr(0,15);
                  
        for (unsigned int i = 0; i < symbolTable.size(); i++)
        {
            // is externalName what we should be checking?? maybe internalName??
            if (symbolTable[i].externalName == name)
                value = symbolTable[i].value;
        }
    }
        // not found in symbolTable so  process error
    if (value == "")
    {
        // our defined error/exit function
        processError("reference to undefined constant");
    }
       
    return value;
}
       
string NextToken(){
    token = "";
    while (token == ""){
        if (charac == '{'){//Here we run into the opening bracket for a comment, we skip down the input stream until we hit the END_OF_FILE or the closing bracket. We return an error if we hit EOF.
                    while(true){
                        NextChar();
                        if(charac ==END_OF_FILE){
                            break;
                        }else if(charac == '}'){
                            NextChar();
                            break;
                        }
                    }
                    if(charac == END_OF_FILE){
                        processError("unexpected end of file");
                    }else if(charac == '}'){
                        processError("token can't start \"}\"");
                    }
        }
        else if (charac == '}'){//Error checking section, check for an incorrect closing bracket. Return an error if found.
                processError("'}' cannot begin a token");
                     
        }
        else if (isspace(charac)){ //Check for spaces.
            NextChar();
        }
        else if (charac == ',' || charac == ';' || charac == '=' || charac == '+' || charac == '-' ||
                 charac == '.' ||  charac =='(' || charac ==')'  || charac == '*'){//Check for special characters ; , ; = + = .
            token = charac;
            NextChar();
        }
        else if(charac == ':' ){
            token = charac;
            NextChar();
            if(charac=='='){
                    token+=charac;
                    NextChar();
                    }
            }
        else if(charac=='<'){
            token = charac;
            NextChar();
                if(charac=='='){
                    token+=charac;
                    NextChar();
                    }
                else if(charac=='>'){
                    token+=charac;
                    NextChar();
                    }
                }
        else if(charac=='>'){
            token = charac;
            NextChar();
            if(charac=='='){
                    token+=charac;
                    NextChar();
                    }
                }
        else if(charac == '_'){ //Check the begining of the token for a leading _.
            processError("\"_\" cannot start a identifier");
        }
        else if (isalpha(charac)){ //Check for letters
                    
            //rule for us NO CAPS IN STAGE 0
            if (isupper(charac))
                processError("No caps allowed in stage 0");
                
            token = charac;
            charac = NextChar();
            if (charac == '_'){ //Check the end of the token to make sure that there are no trailing _.
            processError("\"_\" cannot start a identifier");}
            while (isalpha(charac) || isdigit(charac) || charac == '_'){
                token += charac; //Until you hit something other than letters or numbers, append the character to the current token.
                                NextChar();
            }
         
        }else if (isdigit(charac)){//Check  for digits
            token = charac;
            while (isdigit(NextChar())){ token += charac; }
        }else if (charac == END_OF_FILE){ //We've reached the end of the input stream.
            token = charac;
        }else{ //This else statement is here to cover unexpected symbols.
            processError("illegal symbol encountered"); //Process eror: illegal symbol
        }
    }
         
    if (token[0] == '_'){ //Check the end of the token to make sure that there are no trailing _.
            processError("\"_\" cannot start a identifier");
    }
    if (token[token.length() - 1] == '_'){ //Check the end of the token to make sure that there are no trailing _.
        processError("\"_\" cannot end an identifier");
    }
             
    return token;
}
           
char NextChar()
{
    char NextCharac;
           
    sourceFile.get(NextCharac);
           
    if (!sourceFile.good())
        charac = END_OF_FILE;
    else
    {
        prevCharac = charac;
        charac = NextCharac;
               
               
        if (lstLineNumber == 0)
        {
            lstLineNumber++;
            listingFile << setw(5) << lstLineNumber << '|';
        }
        else if (prevCharac == '\n')
        {
                lstLineNumber++;
                listingFile << setw(5) << lstLineNumber << '|';
        }
               
        listingFile << charac;
    }
           
    return charac;
}
       
bool isNonKeyID()
{
    // so we dont go out of bounds process token
    int end = (int)token.length();
    if(token[0]=='_')
        processError("cannot begin with \"_\" ");
       
    // process each char in token
    for (int x = 0; x < end; x++)
    {
        if(isupper(token[x]))
            processError("upper case characters not allowed");
        if (x == 0)
        {
            if (!isalpha(token[x]))
                return false;
        }
        else
        {
            if ( !isalpha(token[x]) && !isdigit(token[x]) && token[x] != '_' )
                return false;
        }
    }
          
           
    // if pointer is not at the end...token in keywords so is not non key id
    if (find(keywords, keywords+19, token) != keywords+19)
        return false;
    else
    // pointer not valid index = token is a non key id
        return true;
}
       
string genInternalName(storeType inType)
{
    string name;
    ostringstream ss;
       
    if (iCounter + bCounter + pCounter == MAX_SYMBOL_TABLE_SIZE)
        processError("Max number of symbols encountered");
       
    if (inType == INTEGER)
    {
        ss << iCounter;
        name = 'I' + ss.str();
        iCounter++;
    }
    else if (inType == BOOLEAN)
    {
        ss << bCounter;
        name = 'B' + ss.str();
        bCounter++;
    }
    else if (inType == PROG_NAME)
    {
        if (pCounter == 0)
        {
            name = "P0";
            pCounter++;
        }
        else
            processError("only one program name allowed");
    }
       
    return name;
}
       
void processError(string msg)
{
    error = true;
    listingFile << "\nError: Line " << lstLineNumber << ": " << msg <<"\n";
    cerr << "Error: Line " << lstLineNumber << ": " << msg <<"\n";
    CreateListingTrailer();
    //PrintSymbolTable();
           
    exit(EXIT_FAILURE);
}
       
bool isINTEGER(){
    int end = token.length();
    for (int x = 0; x < end; x++)
    {
        if (!isdigit(token[x]))
            return false;
    }
           
    return true;
}
       
// BOOLEAN means token must be "true" || "false" at least the way it is used in ConstStmts()
bool isBOOLEAN(){
    return (token == "true" || token == "false");
}
      
/******************************************* END OF STAGE 0 Code ********************************************************/
      
/******************************************* START STAGE 1 Code **********************************************************/
      
void EXEC_STMTS()
{
    if (isNonKeyID() || token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin")
    {
            EXEC_STMT();
            EXEC_STMTS();
    }
    else if (token == "end" || token == "until")
        ; //do nothing return to calling function
    else
    {
        processError("non-keyword identifier, \"read\", \"write\", \"if\", \"while\", \"repeat\", \";\", or \"begin\" expected");
    }
     
}
      
void EXEC_STMT()
{
    if(isNonKeyID()){
        ASSIGN_STMT();
    }else if(token == "read"){
        READ_STMT();
    }else if(token == "write"){
        WRITE_STMT();
    }else if (token == "if"){
        IF_STMT();
    }else if (token == "while"){
        WHILE_STMT();
    }else if (token == "repeat"){
        REPEAT_STMT();
    }else if (token == ";"){
        NULL_STMT();
    }else if (token == "begin"){
        BeginEndStmt();
    }else{
        processError("non-keyword id, read, or write statement expected");
    }
          
    // this to advance token from semicolon
    if (token == ";")
        NextToken();
}
      
void ASSIGN_STMT()
{
    string x;
        
        
    if (!isNonKeyID())
        processError("non_key_id expected");
          
    x = token;
    pushOperand(x);
          
    NextToken();
          
    if (token != ":=")
        processError("Expected assignment operator");
    else
        pushOperator(":=");
          
    NextToken();
    if ( token == "not" || token == "true" || token == "false" || token == "(" || token == "+" || token == "-" || isINTEGER() || isNonKeyID() )
        EXPRESS();
    else
        processError("Invalid expression:  'not', 'true', 'false', '(', '+', '-', integer, or non-keyword id expected");
          
    if(token != ";")
    {
        processError("\":\" expected ");   //process error: semicolon expected
    }
              
    Code(popOperator(), popOperand(), popOperand());
}
      
void READ_STMT()
{
    if (token!= "read")
        processError("expected \"read\" for read statement");
        
    NextToken();
          
    if (token != "(")
        processError("\"(\" expected");
          
    READ_LIST();
          
    if(NextToken() !=  ";"){
        processError("Expected ;");
    }
}
      
void READ_LIST(){
    string x = "";
    if(token!= "(")
    {
        processError("Expected (");
    }
    NextToken();
    x = Ids();
           
    if(token!= ")"){
        processError("Expected , or )");
    }
    else
        Code("read", x);
}
      
void WRITE_STMT()
{
    if(token != "write"){
        processError("Expected write");
    }
    NextToken();
          
    if (token != "(")
        processError("\"(\" expected");
          
    WRITE_LIST();
          
    if(NextToken() !=  ";"){
        processError("Expected ;");
    }
}
       
void WRITE_LIST(){
    string x = "";
           
    if(token != "("){
        processError("Expected (");
    }
    NextToken();
    x = Ids();
           
    if(token != ")"){
        processError("Expected , or )");
    }
    else   
        Code("write", x);
}
      
void EXPRESS()
{
 
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
            
    TERM();
          
    //NextToken();
          
    if ( token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
    {
         EXPRESSES();
    }
    else if (token == ")" || token == ";" || token == "then" || token =="do" || token =="until" || token == "begin")
    {
        ; // do nothing
    }
    else
        processError("Invalid expression");
}
       
void EXPRESSES(){
    string x = "";
      
    if ( token != "<>" && token != "=" && token != "<=" && token != ">=" && token != "<" && token != ">")
        processError("<>, =, <=, >=, <, or > expected");
        
    pushOperator(token);
          
    NextToken();
          
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("Invalid expression:  'not', 'true', 'false', '(', '+', '-', integer, or non-keyword id expected");
    else
        TERM();
              
    Code(popOperator(), popOperand(), popOperand());
          
    //NextToken();
          
    if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
    {
         EXPRESSES();
    }
    else if (token == ")" || token == ";" || token == "then" || token =="do" || token =="until")
    {
        ; // do nothing
    }
    else
        processError("Invalid expression");
}
      
void TERM()
{
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
         
    FACTOR();
          
    if ( token == "-" || token == "+" || token == "or")
    {
         TERMS();
    }
    else if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">" || token == ")" || token == ";" ||
             token == "then" || token =="do" || token =="until" || token == "begin")
    {
        ; // do nothing
    }
    else
    {
        processError("Invalid expression");
    }
}
       
void TERMS()
{
    string x ="";
              
    if ( token != "-" && token != "+" && token != "or")
        processError("-, +, or expected");
        
    pushOperator(token);
          
    NextToken();
          
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
    else
        FACTOR();
           
        
    Code(popOperator(), popOperand(), popOperand());
          
    if ( token == "-" || token == "+" || token == "or")
    {
         TERMS();
    }
    else if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">" || token == ")" || token == ";"
                           || token == "then" || token =="do" || token =="until")
    {
        ; // do nothing
    }
    else
        processError("Invalid expression");
}
      
void FACTOR(){
 
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
            
 
    PART();
          
    if ( token == "*" || token == "div" || token == "mod" || token == "and")
    {
         FACTORS();
    }
    else if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">" || token == ")" || token == ";" ||
             token == "-" || token == "+" || token == "or" || token == "then" || token =="do" || token =="until" || token == "begin")
    {
        ; // do nothing
    }
    else
    {
        processError("Invalid expression");
    }
}
       
void FACTORS(){
    string x = "";
          
    if ( token != "*" && token != "div" && token != "mod" && token != "and")
        processError("invalid FACTORS, *, div, mod, or and expected");
    
    pushOperator(token);
          
    NextToken();
      
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("not, true, false, (, +, -, non-key ID, or integer expected");
    else
        PART();
       
    Code(popOperator(), popOperand(), popOperand());
          
    if ( token == "*" || token == "div" || token == "mod" || token == "and")
    {
         FACTORS();
    }
    else if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">" || token == ")" || token == ";" ||
             token == "-" || token == "+" || token == "or" || token == "then" || token =="do" || token =="until")
    {
        ; // do nothing
    }
    else
        processError("multiplicative level operator expected");
}
      
void PART(){
      
    if(token == "not")
    {
        NextToken();
        if(token == "(")
        {
            NextToken();
            if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
                    processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
            EXPRESS();
            if(token !=")")
            {
                processError(") expected");
            }
            NextToken();
            Code("not", popOperand());
        }
        else if(isBOOLEAN())
        {
            if(token == "true")
            {
                pushOperand("false");
                NextToken();
            }
            else
            {
                pushOperand("true");
                NextToken();
            }
        }
        else if(isNonKeyID())
        {
            Code("not", token);
            NextToken();
        }
    }
    else if(token == "+")
    {
        NextToken();
        if(token == "(")
        {
            NextToken();
            if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
                processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
            EXPRESS();
            if(token != ")")
            {
                processError(") expected");
            }
            NextToken();
        }
        else if(isINTEGER() || isNonKeyID())
        {
            pushOperand(token);
            NextToken();
        }
        else
        {
            processError("expected '(', integer, or non-keyword id");
        }
    }
    else if(token == "-")
    {
        NextToken();
              
        if(token == "(")
        {
            NextToken();
            if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
                    processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
            EXPRESS();
            if(token != ")")
            {
                processError(") expected");
            }
            NextToken();
            Code("neg", popOperand());
        }
        else if(isINTEGER())
        {
            pushOperand("-"+token);
            NextToken();
        }
        else if(isNonKeyID())
        {
            Code("neg", token);
            NextToken();
        }
    }
    else if(isINTEGER() || isBOOLEAN() || isNonKeyID())
    {
        pushOperand(token);
        NextToken();
    }
    else if(token == "(")
    {
        NextToken();
        if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
            processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
                  
        EXPRESS();
        if(token !=")")
        {
            processError("Expected )");
        }
        else
        NextToken();
    }
    else
    {
        processError("Illegal keyword");
    }
}
    
void Code(string operatr, string operand1, string operand2)
{
    if (operatr == "program")
    {
        EmitProgramCode();
    }
    else if (operatr == "end")
    {
        EmitEndCode();
    }
    else if (operatr == "read")
    {
        EmitReadCode(operand1);
    }
    else if (operatr == "write")
    {
        EmitWriteCode(operand1);
    }
    else if (operatr == "+")
    {
        //addition must be binary
        if (operand1 == "" || operand2 == "")
            processError("addition requires two operands");
              
        EmitAdditionCode(operand1,operand2);
    }
    else if (operatr == "-")
    {
        //subtraction must be binary
        if (operand1 == "" || operand2 == "")
            processError("subtraction requires two operands");
               
        EmitSubtractionCode(operand1,operand2);
    }
    else if (operatr == "neg")
    {
        //negation must be unary
        if (operand2 != "")
            processError("negation takes one operand");
               
        EmitNegCode(operand1);
    }
    else if (operatr == "not")
    {
        if (operand2 != "")
            processError("not operation takes one operand");
                  
        EmitNotCode(operand1);
    }
    else if (operatr == "*")
    {
        //addition must be binary
        if (operand1 == "" || operand2 == "")
            processError("addition requires two operands");
                  
        EmitMultiplicationCode(operand1,operand2);
    }
    else if (operatr == "div")
    {
        //division must be binary
        if (operand1 == "" || operand2 == "")
            processError("division requires two operands");
                   
        EmitDivisionCode(operand1,operand2);
    }
    else if (operatr == "mod")
    {
        //modulo must be binary
        if (operand1 == "" || operand2 == "")
            processError("mod requires two operands");
                   
        EmitModuloCode(operand1,operand2);
    }
    else if (operatr == "and")
    {
        //addition must be binary
        if (operand1 == "" || operand2 == "")
            processError("and requires two operands");
                  
        EmitAndCode(operand1,operand2);
    }
    else if (operatr == "or")
    {
        //addition must be binary
        if (operand1 == "" || operand2 == "")
            processError("or requires two operands");
                  
        EmitOrCode(operand1,operand2);
    }
    else if (operatr == ".")
    {
        ; // do nothing
    }
    else if (operatr == "=")
    {
        //addition must be binary
        if (operand1 == "" || operand2 == "")
            processError("addition requires two operands");
                  
        EmitEqualityCode(operand1,operand2);
    }
    else if (operatr == ":=")
    {
        //addition must be binary
        if (operand1 == "" || operand2 == "")
            processError("addition requires two operands");
                  
        EmitAssignCode(operand1, operand2);
    }
    else if (operatr == "<>")
    {
        //not equal to than must be binary
        if (operand1 == "" || operand2 == "")
            processError("<> requires two operands");
                   
        EmitNotEqualCode(operand1,operand2);
    }
    else if (operatr == ">")
    {
        //greater than must be binary
        if (operand1 == "" || operand2 == "")
            processError("> requires two operands");
                   
        EmitGreaterThanCode(operand1,operand2);
    }
    else if (operatr == ">=")
    {
        //greater than equal to must be binary
        if (operand1 == "" || operand2 == "")
            processError(">= requires two operands");
                   
        EmitGreaterThanEqualCode(operand1,operand2);
    }
    else if (operatr == "<")
    {
        //less than must be binary
        if (operand1 == "" || operand2 == "")
            processError("< requires two operands");
                   
        EmitLessThanCode(operand1,operand2);
    }
    else if (operatr == "<=")
    {
        //less than equal to must be binary
        if (operand1 == "" || operand2 == "")
            processError("<= requires two operands");
                   
        EmitLessThanEqualCode(operand1,operand2);
    }
    //Changes to Code for new arguments
    else if (operatr =="then")
    {
        EmitThenCode(operand1);
    }
    else if (operatr =="else")
    {
        EmitElseCode(operand1);
    }
    else if (operatr =="post_if")
    {
        EmitPostIfCode(operand1);
    }
    else if (operatr =="while")
    {
        EmitWhileCode();
    }
    else if (operatr =="do")
    {
        EmitDoCode(operand1);
    }
    else if (operatr == "post_while")
    {
        EmitPostWhileCode(operand1, operand2);
    }
    else if (operatr =="repeat")
    {
        EmitRepeatCode();
    }
    else if (operatr =="until")
    {
        EmitUntilCode(operand1,operand2);
    }
    else
    {
        processError("undefined operation");
    }
}
      
void pushOperator(string name){
    Operator.push(name);
}
       
void pushOperand(string name){
    int nameIndex = -1;
    string internalName = "";
      
    if(name[0] == 'L')
    {
        Operand.push(name);
        return;
    }
      
    if (name.length() > 15)
            name = name.substr(0,15);
      
          
    for(unsigned int i = 0; i<symbolTable.size(); i++){
        if(symbolTable[i].externalName == name){
            nameIndex = i;
        }
    }
          
    if (nameIndex == -1)
    {
        for(unsigned int i = 0; i<symbolTable.size(); i++){
            if(symbolTable[i].value == name &&
               symbolTable[i].mode == CONSTANT){
                nameIndex = i;
            }
        }
    }
  
    if (nameIndex == -1)
    {
        //correct for boolean keywords
        if (name == "true")
            name = "TRUE";
        else if (name == "false")
            name = "FALSE";
              
        for(unsigned int i = 0; i<symbolTable.size(); i++){
            if(symbolTable[i].value == name &&
               symbolTable[i].mode == CONSTANT){
                nameIndex = i;
            }
        }
          
        //set back to original form
        if (name == "TRUE")
            name = "true";
        else if (name == "FALSE")
            name = "false";
    }
      
    // not in table so making a fresh insert
    if (nameIndex == -1)
    {
        if (name == "true")
        {
            internalName = genInternalName(WhichType(name));
            insert(internalName,WhichType(name),CONSTANT,WhichValue(name),YES,1);
            Operand.push(internalName);
        }
        else if (name == "false")
        {
            internalName = genInternalName(WhichType(name));
            insert(internalName,WhichType(name),CONSTANT,WhichValue(name),YES,1);
            Operand.push(internalName);
        }
        else
        {
            internalName = genInternalName(WhichType(name));
                  
            insert(internalName,WhichType(name),CONSTANT,WhichValue(name),YES,1);
            Operand.push(internalName);
        }
    }
    else
    {
        Operand.push(symbolTable[nameIndex].internalName);
    }
      
    return;
}
       
string popOperator(){
    string temp = "";
    if (!Operator.empty()){
        temp=Operator.top();
        Operator.pop();
    }else{
        processError("operator stack underflow");    
    }
      
    return temp;
}
       
string popOperand()
{
    string temp = "";
          
    if (!Operand.empty())
    {
        temp=Operand.top();
        Operand.pop();
    }else{
        processError("operator stack underflow");
    }
      
    return temp;
}
  
void EmitProgramCode()
{
    objectFile << left <<"STRT  NOP" << setw(10) << " " << setw(13) << symbolTable[0].value + " - Aldo Hernandez, Jerry Bowden, David Cardenas\n";
       
    //set back to default setting and return
    objectFile << right;
    return;
}
  
//emit HLT, BSS and DEC pseudo ops, and END
void EmitEndCode()
{
    istringstream ss;
    string opcode ="";
    int value = 0;
   
     objectFile << left << setw(6) << " " <<  setw(13) << "HLT" << '\n';  //end of execution stmts - declare symbols
        
        
        
     for (uint i = 1; i < symbolTable.size(); i++)
     {
        //if symbol has been allocated
        if (symbolTable[i].alloc == YES)
        {
            if (symbolTable[i].mode == CONSTANT)
            {
                opcode = "DEC";
                if (symbolTable[i].value == "true" || symbolTable[i].value == "TRUE")
                    value = 1;
                else if (symbolTable[i].value == "false" || symbolTable[i].value == "FALSE")
                    value = 0;
                else
                {
                    ss.clear();
                    ss.str(symbolTable[i].value);
                    ss >> value;
                }
            }
            // its a VARIABLE
            else
            {
                opcode = "BSS";       
                value = 1;
            }
               
               
            //print the RAMM code to objectFile for all allocated CONSTANTS and VARIABLES         
            objectFile << left << setw(6) << symbolTable[i].internalName <<  setw(4) << opcode;
   
            //make allowances for negative decimal values
            if (symbolTable[i].value[0] == '-')
                objectFile << '-' << right << setw(3) << setfill('0') << abs(value);
            else
                objectFile << right << setw(4) << setfill('0') << value;
               
            // finish the code line with comment of external name
            objectFile << left << setw(5) << setfill(' ') << " ";
  
            if ( isupper(symbolTable[i].externalName[0]) && symbolTable[i].value != "")
            {
                objectFile << symbolTable[i].value +'\n';
            }
            else
                objectFile << symbolTable[i].externalName +'\n';
        }
     }
        
     // last line of output to objectFile
     objectFile<<"      END STRT\n";
       
     //set back to default then return
     objectFile<< right;
     return;
}
  
// emit read code
// ids parameter could be a single external name or a list of names separated by comma
void EmitReadCode(string ids)
{
    string name;
    int index;
    string::iterator itr = ids.begin();
    string::iterator end = ids.end();
          
          
    for ( ; itr < end; itr++)
    {
        name = "";
              
        while( (*itr != ',') && (itr < end))
        {
            name += *itr;
            itr++;
        }
          
        if (name.length() > 15)
            name = name.substr(0,15);
              
        if (name != "")
        {
            //set to fail state
            index = -1;
               
            for (uint i = 0; i < symbolTable.size(); i++)
            {
                if (symbolTable[i].externalName == name)
                {
                    if(symbolTable[i].mode == CONSTANT)
                        processError("reading in of read-only location");
                    else if(symbolTable[i].mode == VARIABLE)
                        index = i;
                }
            }
               
            if(index == -1)
                processError("undefined symbol for read statement");
            else
                objectFile << left << setw(6) << " " <<  setw(4) << "RDI" << setw(4) << symbolTable[index].internalName
                                   << setw(5) << setfill(' ') << " " << "read in " + symbolTable[index].externalName + '\n'; 
        }
        //for loop iterates past comma before starting again
    }
   
    //set justification back to default setting then return
    objectFile << right;
    return;
}
  
// emit write code
// ids parameter could be a single external name or a list of names separated by comma
void EmitWriteCode(string ids)
{
    string name;
    int index;
    string::iterator itr = ids.begin();
    string::iterator end = ids.end();
          
          
    for ( ; itr < end; itr++)
    {
        name = "";
              
        while( (*itr != ',') && (itr < end))
        {
            name += *itr;
            itr++;
        }
          
        if (name.length() > 15)
            name = name.substr(0,15);
          
        if (name != "")
        {
            //set to fail state
            index = -1;
               
            for (uint i = 0; i < symbolTable.size(); i++)
            {
                if (symbolTable[i].externalName == name)
                    index = i;
            }
               
            if(index == -1)
                processError("undefined symbol for write statement");
            else
                objectFile << left << setw(6) << " " <<  setw(4) << "PRI" << setw(4) << symbolTable[index].internalName
                                   << setw(5) << setfill(' ') << " " << "write out " + symbolTable[index].externalName + '\n';
        }
        //for loop iterates past comma before starting again
    }
   
    //set justification back to default setting then return
    objectFile << right;
    return;
}
  
void EmitAssignCode(string operand1, string operand2){
    int op1 = 0;
    int op2 = 0;
       
    for (unsigned int k = 0; k < symbolTable.size(); k++){
        if (symbolTable[k].internalName == operand1){ //Find the internal name of operand1
            op1 = k;
        }
                  
        if (symbolTable[k].internalName == operand2){ //Find the internal name of operand2
            op2 = k;
        }
    }
       
    if(symbolTable[op2].dataType != symbolTable[op1].dataType){ //Check the data types against each other. They must match.
        processError("incompatible types");
    }
       
    if( symbolTable[op2].mode != VARIABLE){ //Check to see if operand 2 has a mode of VARIABLE.
        processError("left-hand side of assignment must be VARIABLE");
    }
       
    if(operand1 == operand2){
        return;
    }
       
    if(registerA!=operand1){
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand1 << setw(5) << " " << endl;
    }
       
    objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << operand2 << setw(5) << " " << operand2 << " := "<< operand1 << endl;
      
      
    //registerA now holds operand2 due to assignment
    registerA = operand2;
      
    if(operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
   
    //set back to default setting and return
    objectFile << right;
    return;
 }
  
void EmitAdditionCode(string operand1, string operand2){
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
            if (symbolTable[i].dataType != INTEGER){
                processError("integers required");
            }
        }
    }
    
    if (registerA[0] == 'T' && registerA != operand1 && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
        for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA){
                symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
            }
        }
    
    }
           
    if (registerA != operand2 && registerA != operand1){
            objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " "<< '\n'; // load operand2
            registerA = operand2;
    }
        
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(3) << "IAD " << setw(4) << operand1 << setw(5) << " "<<operand2<<" + "<<operand1<< endl;
    }else{
        objectFile << left << setw(6) << " " << setw(3) << "IAD " << setw(4) << operand2 << setw(5) << " "<<operand2<<" + "<<operand1<< endl;
    }
    
    if (operand1[0] == 'T'&& operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T'&& operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == registerA)
            symbolTable[i].dataType=INTEGER; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
      
     //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitSubtractionCode(string operand1, string operand2){
   
   for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
            if (symbolTable[i].dataType != INTEGER){
                processError("integers required");
            }
        }
    }
    
    if (registerA[0] == 'T' && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
            
            for (unsigned int i = 0; i < symbolTable.size(); i++){
                if (symbolTable[i].internalName == registerA){
                    symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
                }
            }
    }
   
    if (registerA != operand2){
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " "<< '\n'; // load operand2
        registerA = operand2;
    }
        
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(3) << "ISB " << setw(4) << operand1 << setw(5) << " "<<operand2<<" - "<<operand1<< endl;
    }
    
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
                symbolTable[i].dataType=INTEGER; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
      
    //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitNegCode(string operand1)
{
    int op = -1;
    string tempString = "";
   
    for (unsigned int k = 0; k < symbolTable.size(); k++){ //Find the internal name of operand2, we're going to need to gather some info first.
        if (symbolTable[k].internalName == operand1){
            if(symbolTable[k].dataType != INTEGER){
                processError("Operand must be of type INTEGER");
            }
        }
    }
     
    if (registerA[0] == 'T' && registerA != operand1 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
            
            for (unsigned int i = 0; i < symbolTable.size(); i++){
                if (symbolTable[i].internalName == registerA){
                    symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
                }
            }
    }
   
    if(registerA != operand1){ //If operand2 is not in the A register, emit code to load it into the register.
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand1 << setw(5) << " " << '\n'; // load operand1
        registerA = operand1;
    }
      
  
    //Find some CONSTANT that has a value of -1
    for (unsigned int k = 0; k < symbolTable.size(); k++){
        if(symbolTable[k].mode == CONSTANT && symbolTable[k].value=="-1" && symbolTable[k].dataType == INTEGER){
            op = k;
            tempString == symbolTable[k].internalName;
        }
    }
      
    // no INTEGER CONSTANT with value -1 was found so create and use
    if (op == -1)
    {
        // get next name available for an integer
        tempString = genInternalName(INTEGER);
          
        //insert that name into symbol table with value of -1
        insert(tempString, INTEGER, CONSTANT, "-1", YES, 1);
    }
      
    objectFile << left << setw(6) << " " << setw(3) << "IMU " << setw(4) << tempString << setw(5) << " " <<tempString + " * -1\n";
      
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
      
    registerA = GetTemp();
      
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == registerA)
            symbolTable[i].dataType=INTEGER; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA); 
  
    //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitMultiplicationCode(string operand1, string operand2){
      
    for (unsigned int i = 0; i < symbolTable.size(); i++)
    {
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2)
        {
            if (symbolTable[i].dataType != INTEGER)
            {
                processError("integers required");
            }
        }
    }
   
    if (registerA[0] == 'T' && registerA != operand1 && registerA != operand2 && registerA != "TRUE")
    {
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA"; //"store "<<operand2 << endl;
           
        for (unsigned int i = 0; i < symbolTable.size(); i++)
        {
            if (symbolTable[i].internalName == registerA)
            {
                symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
            }
        }
    }
 
    if (registerA != operand2 && registerA != operand1)
    {
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " " << endl;
        registerA = operand2;
    }
       
    if (registerA == operand2)
    {
        objectFile << left << setw(6) << " " << setw(4) << "IMU" << operand1 << setw(7) << " " << operand2 + " * " + operand1 << endl;
    }
    else
    {
        objectFile << left << setw(6) << " " << setw(4) << "IMU" << operand2 << setw(7) << " " << operand2 + " * " + operand1 +'\n';
    }
   
    if (operand1[0] == 'T' && operand1 != "TRUE")
        FreeTemp();
          
    if (operand2[0] == 'T' && operand2 != "TRUE")
        FreeTemp();
   
    registerA = GetTemp();
   
    for (unsigned int i = 0; i < symbolTable.size(); i++)
    {
        if (symbolTable[i].internalName == registerA)
            symbolTable[i].dataType=INTEGER; //change temp in registerA alloc to yes in table
    }
   
    pushOperand(registerA);
      
    //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitDivisionCode(string operand1, string operand2){
   for (unsigned int i = 0; i < symbolTable.size(); i++){
                if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
                    if (symbolTable[i].dataType != INTEGER){
                            processError("integers required");
                    }
        }
            }
    
    if (registerA[0] == 'T' && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n" ; // store registerA
            
            for (unsigned int i = 0; i < symbolTable.size(); i++){
                if (symbolTable[i].internalName == registerA){
                        symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
                }
            }
    }
   
    if (registerA != operand2){
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " " << '\n'; // load operand2
        registerA = operand2;
    }
        
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(4) << "IDV" << operand1 << setw(7) << " " << operand2 + " div " + operand1 + '\n';
    }
    
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
        }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
        }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
        symbolTable[i].dataType=INTEGER; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
  
    //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitModuloCode(string operand1, string operand2){
    for (unsigned int i = 0; i < symbolTable.size(); i++){
                if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
                    if (symbolTable[i].dataType != INTEGER){
                            processError("integers required");
                    }
                }      
    }
    
    if (registerA[0] == 'T' && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
            for (unsigned int i = 0; i < symbolTable.size(); i++){
                if (symbolTable[i].internalName == registerA){
                        symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
                }
            }
    }
   
    if (registerA != operand2){
            objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " " << '\n'; // load operand2
            registerA = operand2;
    }
        
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(3) << "IDV " << setw(4) << operand1 << setw(5) << " " <<operand2<<" mod "<<operand1<< endl;
    }
    
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
        }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
        }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
        symbolTable[i].dataType=INTEGER;        //change temp in registerA alloc to yes in table
        symbolTable[i].alloc = YES;
    }
    
    // RAMM code to store remainder into temp Variable
   objectFile << left << setw(6) << " " << setw(3) << "STQ " << setw(4) << registerA << setw(5) << " " << "store remainder\n"; // store registerA
     
   // RAMM code to load temp variable that you stored remainder into register A
   objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << registerA << setw(5) << " " << "load remainder\n"; // LDA registerA
     
    
    pushOperand(registerA);
      
    //set back to default setting and return
    objectFile << right;
    return;
}
   
void EmitAndCode(string operand1, string operand2){
   
     
   for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
            if (symbolTable[i].dataType != BOOLEAN){
                processError("BOOLEANS required");
            }
        }
    }
     
    if (registerA[0] == 'T' && registerA != operand1 && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
             
            for (unsigned int i = 0; i < symbolTable.size(); i++){
                if (symbolTable[i].internalName == registerA){
                        symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
                }
            }
    }
       
    if (registerA != operand2 && registerA != operand1){
            objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << operand2 << setw(5) << " " << '\n'; // load operand2
            registerA = operand2;
    }
         
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(4) << "IMU" << setw(4) << operand1 << setw(5) << " " << operand2 << " and " << operand1 << endl;
    }else{
        objectFile << left << setw(6) << " " << setw(4) << "IMU" << setw(4) << operand2 << setw(5) << " " << operand2 << " and " << operand1 << endl;
    }
  
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
     
    registerA = GetTemp();
     
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
        symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
     
    pushOperand(registerA);
      
    //set back to default setting and return
    objectFile << right;
    return;
}
   
void EmitOrCode(string operand1, string operand2){
    string label;
    int iTrue = -1;
       
   for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
            if (symbolTable[i].dataType != BOOLEAN){
                processError("BOOLEANS required");
            }
        }
           
        //check to find BOOLEAN constant with value 1 or 0
        if (symbolTable[i].dataType == BOOLEAN && symbolTable[i].mode == CONSTANT)
        {
            if (symbolTable[i].internalName == "TRUE")
            {
                if (iTrue == -1)
                    iTrue = i;
            }
        }
    }
    
    if (registerA[0] == 'T' && registerA != operand1 && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
          
        for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA){
                symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
            }
        }
    }
           
    if (registerA != operand2 && registerA != operand1){
            objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " "<< '\n'; //load operand2
            registerA = operand2;
    }
        
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(3) << "IAD " << setw(4) << operand1 << setw(5) << " "<<operand1<<" or "<<operand1<< endl;
    }else{
        objectFile << left << setw(6) << " " << setw(3) << "IAD " << setw(4) << operand2 << setw(5) << " "<<operand1<<" or "<<operand2<< endl;
    }
      
    //get label for jump
    label = GetLabel();
      
    objectFile << left << setw(6) << " " << setw(4) << "AZJ" << setw(4) << label << "+1" << setw(3) << " " << '\n'; // jump to line after label
       
    if(iTrue == -1)
    {
        insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << "TRUE" << setw(5) << " " << '\n'; // LDA TRUE
    }
    else
    {
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT true
    }
    
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
        symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
      
     //set back to default setting and return
    objectFile << right;
    return;
}
  
// called from part() where operand1 is a token
// token means could be external Name as well as "true" or "false" keywords
void EmitNotCode(string operand1)
{
    string label;
    int op1 = -1;
    int iTrue = -1;
    int iFalse = -1;
      
    // check for matching externalName as well as "true" and "false" constants in table
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].externalName == operand1) //&& symbolTable[i].dataType != BOOLEAN )
        {
            if ( symbolTable[i].dataType != BOOLEAN )
                processError("BOOLEANS required");
            else
                op1 = i;
        }
          
        if (symbolTable[i].internalName == "TRUE")
            iTrue = i;
          
        if (symbolTable[i].internalName == "FALS")
            iFalse = i;
    }
      
    //if no matching externalNames were found....check against keywords "true" and "false" now
    if (op1 == -1)
    {
        if (operand1 != "true" && operand1 != "false")
            processError("not takes type BOOLEAN");
    } 
      
    //if nothing has been found and operand1 = "false" (BOOLEAN KEYWORD)
    //insert into symbolTable and LDA it
    if (op1 == -1 && operand1 == "false")
    {
        if (iFalse == -1)
        {
            insert("FALS",BOOLEAN,CONSTANT,"FALSE",YES,1);
            objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << "FALS" << setw(5) << " " << '\n'; // LDA FALS
        }
        else
            objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << symbolTable[iFalse].internalName << setw(5) << " " << '\n'; // LDA symbolTable[op1].internalName
    }
    //if nothing has been found and operand1 = "true" (BOOLEAN KEYWORD)
    //insert into symbolTable and LDA it
    else if(op1 == -1 && operand1 == "true")
    {
        if (iTrue == -1)
        {
            insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
            objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << "TRUE" << setw(5) << " "<< '\n'; // LDA TRUE
        }
        else
            objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << setw(5) << " " << '\n'; // LDA symbolTable[op1].internalName
    }
    //op != -1 and is a valid index to symbolTable
    else
    {
        if (registerA != symbolTable[op1].internalName)
            objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << symbolTable[op1].internalName << setw(5) << " " << '\n'; // LDA symbolTable[op1].internalName
    }
      
    //check condition and load registerA with with TRUE/FALS
    //get label for jump
    label = GetLabel();
       
    //set the jump condtion for false;
    objectFile << left << setw(6) << " " << setw(4) << "AZJ" << setw(4) << label << setw(5) << " " << "not " + operand1 + '\n'; // jump if operand1 is false   
      
    //now loop through symbolTable and find indexes for TRUE or FALS if they are in table
    for (uint i = 0; i < symbolTable.size(); i++)
    {
        if (symbolTable[i].internalName == "TRUE")
            iTrue = i;
        else if (symbolTable[i].internalName == "FALS")
            iFalse = i;
    }
      
    //condition is true so now we want to load in false
    //no boolean constant false found previously...insert one and then use it
    if(iFalse == -1)
    {
        insert("FALS",BOOLEAN,CONSTANT,"FALSE",YES,1);
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << "FALS" << setw(5) << " " << '\n'; // LDA FALS
        registerA = "FALS";
    }
    else
    {
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << symbolTable[iFalse].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT false
    }
      
    //RAMM Code to jump past code for condtion "true"
    objectFile << left << setw(6) << " " << setw(4) << "UNJ" << setw(4) << label << "+1" << setw(3) << " " << '\n';  // jump to line after label
      
    //condition is false so now we want to load in true
    //if no boolean constant true found previously...insert one and LDA it 
    if(iTrue == -1)
    {
        insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << "TRUE" << setw(5) << " " << '\n'; // LDA TRUE
        registerA = "TRUE";
    }
    //else use first boolean constant true in symabolTable
    else
    {
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT true
    }
      
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
      
    registerA = GetTemp();
      
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == registerA)
            symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA); 
  
    //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitEqualityCode(string operand1, string operand2){
    
    int op1 = -1;
    int op2 = -1;
    int iTrue = -1;
    int iFalse = -1;
    string label;
   
    // find index of operands in symbolTable and locate if TRUE or FALS is in table
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1)
            op1 = i;
          
        if (symbolTable[i].internalName == operand2)
            op2 = i;
              
        if (symbolTable[i].internalName == "TRUE")
            iTrue = i;
          
        if (symbolTable[i].internalName == "FALS")
            iFalse = i;
    }
      
    // if either operand is undefined
    if (op1 == -1 || op2 == -1)
        processError("undefined symbol in equality statement");
    // if the types of operands are mismatched
    if (symbolTable[op1].dataType != symbolTable[op2].dataType)
        processError("Incompatible types for equality");
      
    //if regA holds a TEMP variable not operand1 or operand2 store it
    if (registerA[0] == 'T' && registerA != operand1 && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
        for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA){
                symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
            }
        }
    }
      
    //if regA holds neither operand1 or operand2...then load operand 2 into regA    
    if (registerA != operand2 && registerA != operand1){
            objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " "<< '\n'; // load operand2
            registerA = operand2;
    }
      
    //if regA holds operand2 then subtract operand1 in RAMM code
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(4) << "ISB" << setw(4) << operand1 << setw(5) << " "<<operand2<<" = "<<operand1<< endl;
    // else operand1 must be in regA so subtract operand2 in RAMM code
    }else{
        objectFile << left << setw(6) << " " << setw(4) << "ISB" << setw(4) << operand2 << setw(5) << " "<<operand2<<" = "<<operand1<< endl;
    }
      
    //get label for jump
    label = GetLabel();
      
    objectFile << left << setw(6) << " " << setw(4) << "AZJ" << setw(4) << label << setw(5) << " " << '\n'; // jump to label
       
    //condition is false so now we want to load in false
    //no boolean constant false found previously...insert one and then use it
    if(iFalse == -1)
    {
        insert("FALS",BOOLEAN,CONSTANT,"FALSE",YES,1);
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << "FALS" << setw(5) << " " << '\n'; // LDA FALS
        registerA = "FALS";
    }
    else
    {
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << symbolTable[iFalse].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT false
    }
      
    //RAMM Code to jump past code for condtion "true"
    objectFile << left << setw(6) << " " << setw(4) << "UNJ" << setw(4) << label << "+1" << setw(3) << " " << '\n'; // jump to line after label
      
    //condition is false so now we want to load in true
    //if no boolean constant true found previously...insert one and LDA it 
    if(iTrue == -1)
    {
        insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << "TRUE" << setw(5) << " " << '\n'; // LDA TRUE
        registerA = "TRUE";
    }
    //else use first boolean constant true in symabolTable
    else
    {
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT true
    }
    
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == registerA)
            symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
 
     //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitNotEqualCode(string operand1, string operand2){
   
    int op1 = -1;
    int op2 = -1;
    int iTrue = -1;
    string label;
   
    // find index of operands in symbolTable and locate if TRUE or FALS is in table
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1)
            op1 = i;
          
        if (symbolTable[i].internalName == operand2)
            op2 = i;
              
        if (symbolTable[i].internalName == "TRUE")
            iTrue = i;
    }
      
    // if either operand is undefined
    if (op1 == -1 || op2 == -1)
        processError("undefined symbol in equality statement");
    // if the types of operands are mismatched
    if (symbolTable[op1].dataType != symbolTable[op2].dataType)
        processError("Incompatible types for equality");
      
    //if regA holds a TEMP variable not operand1 or operand2 store it
    if (registerA[0] == 'T' && registerA != operand1 && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
 
        for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA){
                symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
            }
        }
    }
      
    //if regA holds neither operand1 or operand2...then load operand 2 into regA    
    if (registerA != operand2 && registerA != operand1){
            objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " "<< '\n'; // load operand2
            registerA = operand2;
    }
      
    //if regA holds operand2 then subtract operand1 in RAMM code
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(4) << "ISB" << setw(4) << operand1 << setw(5) << " "<<operand2<<" <> "<<operand1<< endl;
    // else operand1 must be in regA so subtract operand2 in RAMM code
    }else{
        objectFile << left << setw(6) << " " << setw(4) << "ISB" << setw(4) << operand2 << setw(5) << " "<<operand2<<" <> "<<operand1<< endl;
    }
      
    //get label for jump
    label = GetLabel();
      
    objectFile << left << setw(6) << " " << setw(4) << "AZJ" << setw(4) << label << "+1" << setw(3) << " " << '\n'; // jump to line after label
      
    //condition is false so now we want to load in true
    //if no boolean constant true found previously...insert one and LDA it 
    if(iTrue == -1)
    {
        insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << "TRUE" << setw(5) << " " << '\n'; // LDA TRUE
        registerA = "TRUE";
    }
    //else use first boolean constant true in symabolTable
    else
    {
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT true
    }
    
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == registerA)
            symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
     
     //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitGreaterThanCode(string operand1, string operand2){
      
    string label;
    int iTrue = -1;
    int iFalse = -1;
   
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
            if (symbolTable[i].dataType != INTEGER){
                processError("> requires integers");
            }
        }
          
        if (symbolTable[i].internalName == "TRUE")
            iTrue = i;
          
        if (symbolTable[i].internalName == "FALS")
            iFalse = i;
    }
    
    if (registerA[0] == 'T' && registerA != operand1 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
            
            for (unsigned int i = 0; i < symbolTable.size(); i++){
                if (symbolTable[i].internalName == registerA){
                    symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
                }
            }
    }
   
    if (registerA != operand1){
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand1 << setw(5) << " "<< '\n'; // load operand1
        registerA = operand1;
    }
        
    if (registerA == operand1){
        objectFile << left << setw(6) << " " << setw(3) << "ISB " << setw(4) << operand2 << setw(5) << " "<<operand2<<" > "<<operand1<< endl;
    }
      
    // GetLabel for jumps
    label = GetLabel();
      
    objectFile << left << setw(6) << " " << setw(4) << "AMJ" << setw(4) << label << setw(5) << " " << '\n'; // jump to label
      
    if(iFalse == -1)
    {
        insert("FALS",BOOLEAN,CONSTANT,"FALSE",YES,1);
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << "FALS" << setw(5) << " " << '\n'; // LDA FALS
        registerA = "FALS";
    }
    else
    {
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << symbolTable[iFalse].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT false
    }  
      
    objectFile << left << setw(6) << " " << setw(4) << "UNJ" << setw(4) << label << "+1" << setw(3) << " " << '\n'; //jump to line after label
      
    if(iTrue == -1)
    {
        insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << "TRUE" << setw(5) << " " << '\n'; // LDA TRUE
        registerA = "TRUE";
    }
    //else use first boolean constant true in symabolTable
    else
    {
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT true
    }
      
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
                symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
      
    //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitGreaterThanEqualCode(string operand1, string operand2)
{
   
    string label;
    int iTrue = -1;
    int iFalse = -1;
   
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
            if (symbolTable[i].dataType != INTEGER){
                processError(">= requires integers");
            }
        }
          
        if (symbolTable[i].internalName == "TRUE")
            iTrue = i;
          
        if (symbolTable[i].internalName == "FALS")
            iFalse = i;
    }
    
    if (registerA[0] == 'T' && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; //store registerA
            
        for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA){
                symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
            }
        }
    }
   
    if (registerA != operand2){
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " "<< '\n'; // load operand2
        registerA = operand2;
    }
        
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(3) << "ISB " << setw(4) << operand1 << setw(5) << " "<<operand2<<" >= "<<operand1<< endl;
    }
      
    // GetLabel for jumps
    label = GetLabel();
      
    objectFile << left << setw(6) << " " << setw(4) << "AMJ" << setw(4) << label << setw(5) << " " << '\n'; // jump to label
      
    if(iTrue == -1)
    {
        insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
        objectFile << left << setw(6) << " "   << setw(4) << "LDA" << setw(4) << "TRUE" << setw(5) << " " << '\n'; // LDA TRUE
        registerA = "TRUE";
    }
    //else use first boolean constant true in symabolTable
    else
    {
        objectFile << left << setw(6) <<" " << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT
    }
      
    objectFile << left << setw(6) << " " << setw(4) << "UNJ" << setw(4) << label << "+1" << setw(3) << " " << '\n'; // jump to line after label
      
    if(iFalse == -1)
    {
        insert("FALS",BOOLEAN,CONSTANT,"FALSE",YES,1);
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << "FALS" << setw(5) << " " << '\n'; // LDA FALS
        registerA = "FALS";
    }
    else
    {
       objectFile << left << setw(6) << label<< setw(4) << "LDA" << setw(4) << symbolTable[iFalse].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT false
    }  
      
    
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
                symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
      
    //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitLessThanCode(string operand1, string operand2)
{
      
    string label;
    int iTrue = -1;
    int iFalse = -1;
   
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
            if (symbolTable[i].dataType != INTEGER){
                processError("< requires integers");
            }
        }
          
        if (symbolTable[i].internalName == "TRUE")
            iTrue = i;
          
        if (symbolTable[i].internalName == "FALS")
            iFalse = i;
    }
    
    if (registerA[0] == 'T' && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
            
        for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA){
                symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
            }
        }
    }
   
    if (registerA != operand2){
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << '\n';
        registerA = operand2;
    }
        
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(3) << "ISB " << setw(4) << operand1 << setw(5) << " "<<operand2<<" < "<<operand1<< endl;
    }
      
    // GetLabel for jumps
    label = GetLabel();
      
    objectFile << left << setw(6) << " " << setw(4) << "AMJ" << setw(4) << label << '\n';
      
    if(iFalse == -1)
    {
        insert("FALS",BOOLEAN,CONSTANT,"FALSE",YES,1);
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << "FALS" << '\n';
        registerA = "FALS";
    }
    else
    {
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << symbolTable[iFalse].internalName << '\n';
    }  
      
    objectFile << left << setw(6) << " " << setw(4) << "UNJ" << setw(4) << label << "+1" << '\n';
      
    if(iTrue == -1)
    {
        insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << "TRUE" << '\n';
        registerA = "TRUE";
    }
    //else use first boolean constant true in symabolTable
    else
    {
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << '\n';
    }
      
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
                symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
      
    //set back to default setting and return
    objectFile << right;
    return;
}
  
void EmitLessThanEqualCode(string operand1, string operand2)
{
   
    string label;
    int iTrue = -1;
    int iFalse = -1;
   
    for (unsigned int i = 0; i < symbolTable.size(); i++){
        if (symbolTable[i].internalName == operand1 || symbolTable[i].internalName == operand2){
            if (symbolTable[i].dataType != INTEGER){
                processError(">= requires integers");
            }
        }
          
        if (symbolTable[i].internalName == "TRUE")
            iTrue = i;
          
        if (symbolTable[i].internalName == "FALS")
            iFalse = i;
    }
    
    if (registerA[0] == 'T' && registerA != operand2 && registerA != "TRUE"){
        objectFile << left << setw(6) << " " << setw(3) << "STA " << setw(4) << registerA << setw(5) << " " << "deassign registerA\n"; // store registerA
            
        for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA){
                symbolTable[i].alloc=YES;  //change temp in registerA alloc to yes in table
            }
        }
    }
   
    if (registerA != operand2){
        objectFile << left << setw(6) << " " << setw(3) << "LDA " << setw(4) << operand2 << setw(5) << " " << '\n'; // load operand2
        registerA = operand2;
    }
        
    if (registerA == operand2){
        objectFile << left << setw(6) << " " << setw(3) << "ISB " << setw(4) << operand1 << setw(5) << " "<<operand2<<" <= "<<operand1<< endl;
    }
      
    // GetLabel for jumps
    label = GetLabel();
      
    objectFile << left << setw(6) << " " << setw(4) << "AMJ" << setw(4) << label << setw(5) << " " << '\n';  // jump to label
    objectFile << left << setw(6) << " " << setw(4) << "AZJ" << setw(4) << label << setw(5) << " " << '\n'; // jump to label
      
    if(iFalse == -1)
    {
        insert("FALS",BOOLEAN,CONSTANT,"FALSE",YES,1);
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << "FALS" << setw(5) << " " << '\n'; // LDA FALS
        registerA = "FALS";
    }
    else
    {
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << symbolTable[iFalse].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT false
    }  
      
    objectFile << left << setw(6) << " " << setw(4) << "UNJ" << setw(4) << label << "+1" << setw(3) << " " << '\n'; // jump to line after label
      
    if(iTrue == -1)
    {
        insert("TRUE",BOOLEAN,CONSTANT,"TRUE",YES,1);
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << "TRUE" << setw(5) << " " << '\n'; //LDA TRUE
        registerA = "TRUE";
    }
    else
    {
        objectFile << left << setw(6) << label << setw(4) << "LDA" << setw(4) << symbolTable[iTrue].internalName << setw(5) << " " << '\n'; // LDA BOOLEAN CONSTANT true
    }
    
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
    if (operand2[0] == 'T' && operand2 != "TRUE"){
        FreeTemp();
    }
    
    registerA = GetTemp();
    
    for (unsigned int i = 0; i < symbolTable.size(); i++){
            if (symbolTable[i].internalName == registerA)
                symbolTable[i].dataType=BOOLEAN; //change temp in registerA alloc to yes in table
    }
    
    pushOperand(registerA);
      
    //set back to default setting and return
    objectFile << right;
    return;
}
  
string GetTemp()
{
    string temp;
    currentTempNo++;
    
    ostringstream convert;
    convert << currentTempNo;
    temp = 'T' + convert.str();
    
    if(currentTempNo > maxTempNo){
        insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
        maxTempNo++;
    }
    return temp;
}
   
void FreeTemp()
{
    currentTempNo--;
    if(currentTempNo < -1)
        processError("compiler error, currentTempNo should be >= -1");
}
  
string GetLabel()
{
    string label;
    stringstream ss;
      
    labelCounter++;
    ss << labelCounter;
    label = 'L' + ss.str();
      
    return label;
}
  
/******************************************* STAGE 1 CODE ENDS HERE ******************************************************/
  
/******************************************* START STAGE 2 Code **********************************************************/
  
void IF_STMT()
{
    int index = -1;
  
    if(token !="if"){
        processError(" keyword \"if\" expected"); 
    }
  
    NextToken();
      
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
    else
        EXPRESS();
      
    for (uint i = 0; i < symbolTable.size(); i++)
    {
        if (symbolTable[i].internalName == Operand.top())
            index = (int) i;   
    }
      
    if (symbolTable[index].dataType != BOOLEAN)
        processError("if predicate must be BOOLEAN");  
      
    if (token != "then")
        processError("then expected");
      
    Code("then", popOperand());
  
    NextToken();
  
    if (token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin" || isNonKeyID())
        EXEC_STMT();   
    else
        processError("non-keyword identifier, \"read\", \"write\", \"if\", \"while\", \"repeat\", \";\", or \"begin\" expected");
          
    if (token == "else" || token == "end" || token == "until" || token == "begin" || token == "while" || token == "if" || token == "repeat" || token == "write" || token == ";" || isNonKeyID())
        ELSE_PT();
    else
        processError("invalid ELSE_PT");
}
  
  
  
void ELSE_PT()
{
    if(token == "else"){
      
        Code("else", popOperand());
  
        NextToken();
      
        if (token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin" || isNonKeyID())
        {
            EXEC_STMT();
            Code("post_if", popOperand());
        }
        else
            processError("invalid EXEC_STMT after else");
    }
    else if (token == "end" || token == "until" || token == "begin" || token == "while" || token == "if" || token == "repeat" || token == "write" || token == ";" || isNonKeyID())
        Code("post_if", popOperand());
    else
        processError("invalid ELSE_PT");
}
  
  
void WHILE_STMT()
{
    int index = -1;
      
    if(token !="while"){
        processError(" keyword \"while\" expected"); 
    }
    else
        Code("while");
      
    NextToken();
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
    else
        EXPRESS();
  
    for (uint i = 0; i < symbolTable.size(); i++)
    {
        if (symbolTable[i].internalName == Operand.top())
            index = (int) i;
    }
      
    if (symbolTable[index].dataType != BOOLEAN)
        processError("while predicate must be BOOLEAN");
      
    if (token != "do")
        processError("invalid do for while");
    else
        Code("do", popOperand());
  
    NextToken();
  
    if (token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin" || isNonKeyID())
    {
        EXEC_STMT();
        Code("post_while", popOperand(), popOperand());
    }  
    else
        processError("invalid EXEC_STMT after else");
      
}
  
  
void REPEAT_STMT()
{
    int index = -1;
    if(token !="repeat"){
        processError("keyword \"repeat\" expected"); 
    }
    else
        Code("repeat");
  
    NextToken();
     
    if (token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin" || isNonKeyID() || token == "end")
    {
        EXEC_STMTS();
    }
  
    if (token != "until"){
       processError("expected until");
    }
      
    NextToken();
      
    if ( token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isINTEGER() && !isNonKeyID() )
        processError("Invalid expression: not, true, false, (, +, -, non-key ID, or integer expected");
    else
    {
        EXPRESS();
          
        for (uint i = 0; i < symbolTable.size(); i++)
        {
            if (symbolTable[i].internalName == Operand.top())
                index = (int)i;
        }
      
        if (symbolTable[index].dataType != BOOLEAN)
            processError("until predicate must be BOOLEAN"); 
          
        Code("until", popOperand(), popOperand());
    }
  
    if (token != ";")
        processError("semicolon expected");
    else
        NextToken();
}
  
  
void NULL_STMT()
{
    if (token != ";")
        processError("invalid null statement");
    else
        NextToken();
}
  
void EmitThenCode(string operand1)
{
    string tempLabel = GetLabel();
      
    if (registerA != operand1)
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << operand1 << setw(5) << " " << '\n'; // "LDA " << operand1 + '\n';
      
    objectFile << left << setw(6) << " " << setw(4) << "AZJ" << setw(4) << tempLabel << setw(5) << " " << "jump if condition is false\n";
  
    pushOperand(tempLabel);
      
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
      
    registerA = "";
}
  
void EmitElseCode(string operand1)
{
    string tempLabel = GetLabel();
  
    objectFile << left << setw(6) << " " << setw(4) << "UNJ" << setw(4) << tempLabel << setw(5) << " " << "jump to end if\n";
      
    objectFile << left << setw(6) << operand1 << setw(4) << "NOP" << setw(4) << " " << setw(5) << " " << "else\n";
      
    pushOperand(tempLabel);
    registerA = "";
}
  
void EmitPostIfCode(string operand1)
{
    objectFile << left << setw(6) << operand1 << setw(4) << "NOP" << setw(4) << " " << setw(5) << " " << "end if\n";
  
    registerA = "";
}
  
void EmitWhileCode()
{
    string tempLabel = GetLabel();
      
    objectFile << left << setw(6) << tempLabel << setw(4) << "NOP" << setw(4) << " " << setw(5) << " " << "while\n";
      
    pushOperand(tempLabel);
      
    registerA = "";
}
  
void EmitDoCode(string operand1)
{
    string tempLabel = GetLabel();
      
    if (registerA != operand1)
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << operand1 << setw(5) << " " << '\n'; // LDA operand1
      
    objectFile << left << setw(6) << " " << setw(4) << "AZJ" << setw(4) << tempLabel << setw(5) << " " << "do\n"; //jump if condition is false
      
    pushOperand(tempLabel);
  
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
      
    registerA = "";
}
  
void EmitPostWhileCode(string operand1, string operand2)
{
    objectFile << left << setw(6) << " " << setw(4) << "UNJ" << setw(4) << operand2 << setw(5) << " " << "end while\n";
  
    objectFile << left << setw(6) << operand1 << setw(4) << "NOP" << setw(4) << " " << setw(5) << " " << '\n';
      
    registerA = "";
}
  
void EmitRepeatCode()
{
    string tempLabel = GetLabel();
  
    objectFile << left << setw(6) << tempLabel << setw(4) << "NOP" << setw(4) << " " << setw(5) << " " << "repeat\n";
      
    pushOperand(tempLabel);
    registerA = "";
}
  
void EmitUntilCode(string operand1, string operand2)
{
  
    if (registerA != operand1)
        objectFile << left << setw(6) << " " << setw(4) << "LDA" << setw(4) << operand1 << setw(5) << " " << '\n'; //"LDA " << operand1 + '\n';
      
    objectFile << left << setw(6) << " " << setw(4) << "AZJ" << setw(4) << operand2 << setw(5) << " " << "until\n"; // "jump if condition is false\n";
      
    if (operand1[0] == 'T' && operand1 != "TRUE"){
        FreeTemp();
    }
      
    registerA = "";
}