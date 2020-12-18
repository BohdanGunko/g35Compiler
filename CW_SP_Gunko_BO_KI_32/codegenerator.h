#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class codeGenerator
{
public:
    fstream asmFile;
    string filePath;
    string programName;
    string preinitStr;
    unsigned andOperatorsCounter = 1;

    codeGenerator();
    ~codeGenerator();
    void createFile(string& filePath);
    void setProgramName(string& programName);
    void declareVar(string& varName);
    void declareVar(string& varName, short varValue);
    void starCode();
    void endCode();
    void scanCode(string& varName);
    void printCode(string& varName);
    void assignmentCode(string& varName);
    void regMov(string rightOperand);
    void movCxBx();
    void movBxCx();
    void regOperator(string& operatorName, string leftOperand);
    void pushCx();
    void popCx();
};

#endif	// CODEGENERATOR_H
