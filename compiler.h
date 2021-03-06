#ifndef __X_H_INCLUDED__  
#define __X_H_INCLUDED__
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
#endif


