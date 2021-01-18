#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <windows.h>
#include "codegenerator.h"
using namespace std;

// PRIORITY0 is the highest prioruty
#define PRIORYTY0 7
#define PRIORYTY1 6
#define PRIORYTY2 5
#define PRIORYTY3 4
#define PRIORYTY4 3
#define PRIORYTY5 2
#define PRIORYTY6 1
#define PRIORYTY7 0

#define gPROGRAM "PROGRAM"
#define gVAR "VAR"
#define gBEGIN "BEGIN"
#define gEND "END"
#define gSCAN "SCAN"
#define gPRINT "PRINT"
#define gASSIGNMENT "->"
#define gWHILE "WHILE"
#define gDO "DO"	//
#define gCOM_ST "#*"
#define gCOM_END "*#"
#define gSEMICOLON ';'
#define gINT16 "INT16"

struct gOperator
{
    string name;
    int priority;
};

gOperator gPLUS = { "+", PRIORYTY3 };
gOperator gMINUS = { "-", PRIORYTY3 };
gOperator gMULT = { "*", PRIORYTY2 };
gOperator gDIV = { "DIV", PRIORYTY2 };
gOperator gMOD = { "MOD", PRIORYTY2 };
gOperator gEQUAL = { "==", PRIORYTY5 };
gOperator gNOT_EQUAL = { "<>", PRIORYTY5 };
gOperator gGRATER = { ">>", PRIORYTY4 };
gOperator gLESS = { "<<", PRIORYTY4 };
gOperator gNOT = { "!!", PRIORYTY1 };
gOperator gAND = { "&&", PRIORYTY6 };
gOperator gOR = { "||", PRIORYTY7 };
gOperator gOPEN_BRACKET = { "(", PRIORYTY0 };
gOperator gCLOSE_BRACKET = { ")", PRIORYTY0 };
gOperator gUNKNOWN_OPERATOR = { "", PRIORYTY0 };

#define NO_ENTRY_POINT(ln) "Error in line " + to_string(ln) + ":No entry point, expected token \"PROGRAM\""
#define BEGIN_EXPECTED(ln) "Error in line " + to_string(ln) + ":Expected token \"BEGIN\""
#define WORD_BREAK_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected space or tab after token\"" + token + "\""
#define IDENT_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected identifier after token \"" + token + "\""
#define SEMICOLON_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected semicolon after token \"" + token + "\""
#define NEW_LINE_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected new line after token \"" + token + "\""
#define END_EXPECTED(ln) "Error in line " + to_string(ln) + ":Expected token \"END\""
#define NO_TOKENS_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":No tokens expected after \"" + token + "\""
#define IDENT_ALREADY_EXIST(ln, ident) "Error in line " + to_string(ln) + ":Identificator \"" + ident + "\" already exist"
#define CONSTANT_OUT_OF_RANGE(ln) "Error in line " + to_string(ln) + ":Constant out of range of type INT16"
#define CONSTANT_TOO_LONG(ln) "Error in line " + to_string(ln) + ":Constant too long"
#define CONSTANT_EXPECTED(ln) "Error in line " + to_string(ln) + ":Constant expected after token \"->\""
#define UNEXPECTED_TOKEN(ln) "Error in line " + to_string(ln) + ":Unexpected token"
#define END_OF_COMMENT_EXPECTED "Error:End of comment expected"
#define UNDECLARED_IDENT(ln, ident) "Error in line " + to_string(ln) + ":Undeclared identificator \"" + ident + "\""
#define OPEN_BRACKET_EXPECTED(ln, pos) "Error in line " + to_string(ln) + ":Expected open bracket before possition \"" + to_string(pos) + "\""
#define CLOSE_BRACKET_EXPECTED(ln) "Error in line " + to_string(ln) + ":Not enought closed brackets"
#define TOKEN_EXPECTED(ln, after_token, expected_token) \
    "Error in line " + to_string(ln) + ":Expected \"" + expected_token + "\" after token \"" + after_token + "\""
#define OPEN_BLOCK_EXPECTED(ln) "Error in line " + to_string(ln) + ":Expected \"BEGIN\" before \"END\""
#define CLOSE_BLOCK_EXPECTED(ln) "Error in line " + to_string(ln) + ":Unclosed block, \"END\" expected"

struct identifier
{
    short name;
    short value;
};

int line_number = 0;
short unclosed_brackets_count = 0;
short unclosed_clocks_count = 0;
vector<identifier>* ident_list;
codeGenerator generateCode;
fstream outFile;
fstream lexemFile;
fstream identFile;
string filePath;

short checkForDuplicats(string& ident_string);
unsigned getConst(string& inLine, unsigned pos);
unsigned checkIdent(string& inLine, unsigned i);
string skipEmptyLines(fstream& inFile);
unsigned skipWordBreaks(const string& inLine, unsigned startPos);
short identExist(string& ident_string);
bool checkSemicolon(string& inLine, unsigned startPos);
void errorExit();

void translateToAsm(fstream& inFile);
void checkPROGRAM(fstream& inFile);
string checkVarDec(fstream& inFile);
void checkProgramBody(string& inLine, fstream& inFile);
unsigned solveExpression(string& inLine, unsigned startPos);
unsigned solveExpressionPart(string& inLine, unsigned startPos, gOperator prevOperator);
gOperator getNextOperator(string& inLine, unsigned pos);

int main()
{

enter_file_name:
    cout << "Please enter(or paste) path to file: " << endl;
    getline(cin, filePath);

    if (filePath.length() < 4 || filePath.substr(filePath.length() - 4, filePath.length() - 1) != ".g35")
    {
        cout << "Error: file must have .g35 extension. Please enter correct file name." << endl;
        goto enter_file_name;
    }

    fstream inFile;
    inFile.open(filePath, std::fstream::in);
    if (!inFile.is_open())
    {
        cout << "Can not open file.Please try again." << endl;
        goto enter_file_name;
    }

    outFile.open(filePath.substr(0, filePath.length() - 4) + "_error.txt", fstream::out);
    lexemFile.open(filePath.substr(0, filePath.length() - 4) + "_lexems_table.txt", fstream::out);
    identFile.open(filePath.substr(0, filePath.length() - 4) + "_ident_table.txt", fstream::out);

    lexemFile << "Line"
                        << "\t"
                        << "Lexem" << endl;
    lexemFile << "----------------------------------------------" << endl;

    identFile << "Line"
                        << "\t"
                        << "Identifier" << endl;
    identFile << "----------------------------------------------" << endl;

    ident_list = new vector<identifier>;
    generateCode.createFile(filePath);
    translateToAsm(inFile);

    outFile.close();
    lexemFile.close();
    identFile.close();
    inFile.close();

    remove((filePath.substr(0, filePath.length() - 4) + "_error.txt").c_str());

    delete ident_list;

    cout << "DONE successfully" << endl;
    system("pause");
    return 1;
}

short checkForDuplicats(string& ident_string)
{
    short ident = (short)ident_string[0] << 8 | (short)ident_string[1];
    for (auto& i : *ident_list)
    {
        if (i.name == ident)
        {
            outFile << IDENT_ALREADY_EXIST(line_number, ident_string) << endl;
            errorExit();
        }
    }
    return ident;
}

unsigned getConst(string& inLine, unsigned pos)
{
    unsigned endPos = pos;
    if (inLine[pos] == '-')
        ++endPos;

    regex rgx("[0-9]{1,}");
    string num = "";
    for (; endPos < inLine.length(); ++endPos)
    {
        num += inLine[endPos];
        if (regex_match(num, rgx))
        {
            if (stoi(num) > 32767 || stoi(num) < -32768)
            {
                outFile << CONSTANT_OUT_OF_RANGE(line_number) << endl;
                errorExit();
            }
            else if (num.length() > 5)
            {
                outFile << CONSTANT_TOO_LONG(line_number) << endl;
                errorExit();
            }
        }
        else
        {
            break;
        }
    }
    if (inLine[pos] == '-' && endPos - pos == 1)
        --endPos;

    return endPos;
}

unsigned skipWordBreaks(const string& inLine, unsigned startPos)
{
    for (; startPos < inLine.length(); ++startPos)
    {
        if (inLine[startPos] != ' ' && inLine[startPos] != '\t')
        {
            return startPos;
        }
    }
    return inLine.length();
}

short identExist(string& ident_string)
{
    short ident = (short)ident_string[0] << 8 | (short)ident_string[1];
    for (auto& i : *ident_list)
    {
        if (i.name == ident)
        {
            return ident;
        }
    }
    outFile << UNDECLARED_IDENT(line_number, ident_string) << endl;
    errorExit();
}

string skipEmptyLines(fstream& inFile)
{
    string inLine;
    regex rgx("[ \t]{0,}");

    while (getline(inFile, inLine))
    {
        ++line_number;
        if (!regex_match(inLine, rgx))
        {
            for (unsigned i = 0; i < inLine.length() - 1; ++i)
            {
                if (inLine[i] == '#' && inLine[i + 1] == '*')
                {
                    string beforeComment = inLine.substr(0, i);

                    unsigned j = i + 2;
                    do
                    {
                        for (; j < inLine.length(); ++j)
                        {
                            if (j < inLine.length() - 1 && inLine[j] == '*' && inLine[j + 1] == '#')
                            {
                                inLine = beforeComment + inLine.substr(j + 2);
                                if (skipWordBreaks(inLine, 0) == inLine.length())
                                    return (skipEmptyLines(inFile));

                                return inLine;
                            }
                        }
                        ++line_number;
                        j = 0;

                    } while (getline(inFile, inLine));

                    outFile << END_OF_COMMENT_EXPECTED << endl;
                    errorExit();
                }
            }
            return inLine;
        }
    }
    return "";
}

bool checkSemicolon(string& inLine, unsigned startPos)
{
    if (startPos < inLine.length() && inLine[startPos] == gSEMICOLON)
    {
        for (unsigned i = startPos + 1; i < inLine.length(); ++i)
        {
            if (inLine[i] != ' ' && inLine[i] != '\t')
            {
                outFile << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                errorExit();
            }
        }
        return true;
    }
    return false;
}

void errorExit()
{
    cout << "Ended unsuccessfully, see error on file" << endl;
    generateCode.clearFile();

    lexemFile.close();
    identFile.close();

    remove((filePath.substr(0, filePath.length() - 4) + "_lexems_table.txt").c_str());
    remove((filePath.substr(0, filePath.length() - 4) + "_ident_table.txt").c_str());
    outFile.close();
    exit(0);
}

void translateToAsm(fstream& inFile)
{
    checkPROGRAM(inFile);
    string inLine = checkVarDec(inFile);
    identFile.close();

    do
    {
        checkProgramBody(inLine, inFile);
        inLine = skipEmptyLines(inFile);
    } while (inLine != "");

    if (unclosed_clocks_count > 0)
    {
        outFile << CLOSE_BLOCK_EXPECTED(line_number) << endl;
        errorExit();
    }

    generateCode.endCode();

    cout << "Assembly code generated." << endl << endl;
    generateCode.assembleFile();
}

unsigned checkIdent(string& inLine, unsigned i)
{
    regex rgx("[a-z][a-z0-9]");

    string ident = inLine.substr(i, 2);

    if (!regex_match(ident, rgx))
    {
        return ++i;
    }
    return i;
}

void checkPROGRAM(fstream& inFile)
{
    string inLine = skipEmptyLines(inFile);
    unsigned i = skipWordBreaks(inLine, 0);

    if (inLine.substr(i, 7) != gPROGRAM)
    {
        outFile << NO_ENTRY_POINT(line_number) << endl;
        errorExit();
    }

    if (i + 7 >= skipWordBreaks(inLine, i + 7))
    {
        outFile << WORD_BREAK_EXPECTED(line_number, gPROGRAM) << endl;
        errorExit();
    }

    i = skipWordBreaks(inLine, i + 7);

    if (i != checkIdent(inLine, i))
    {
        outFile << IDENT_EXPECTED(line_number, gPROGRAM) << endl;
        errorExit();
    }
    string programName = inLine.substr(i, 2);

    i = skipWordBreaks(inLine, i + 2);
    if (i >= inLine.length() || inLine[i] != ';')
    {
        outFile << SEMICOLON_EXPECTED(line_number, programName) << endl;
        errorExit();
    }

    if (i != inLine.length() - 1 && skipWordBreaks(inLine, i + 1) != inLine.length())
    {
        outFile << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
        errorExit();
    }

    lexemFile << line_number << "\t\t" << gPROGRAM << endl;
    lexemFile << line_number << "\t\t"
                        << "program name(" + programName + ")" << endl;
    lexemFile << line_number << "\t\t" << gSEMICOLON << endl;

    inLine = skipEmptyLines(inFile);
    i = skipWordBreaks(inLine, 0);

    if (inLine.substr(i, 5) != gBEGIN)
    {
        outFile << BEGIN_EXPECTED(line_number) << endl;
        errorExit();
    }

    if (i != inLine.length() - 1 && skipWordBreaks(inLine, i + 5) != inLine.length())
    {
        outFile << NEW_LINE_EXPECTED(line_number, gBEGIN) << endl;
        errorExit();
    }
    ++unclosed_clocks_count;

    lexemFile << line_number << "\t\t" << gBEGIN << endl;

    generateCode.setProgramName(programName);
}

string checkVarDec(fstream& inFile)
{
    string inLine = skipEmptyLines(inFile);

    if (inLine == "")
    {
        outFile << END_EXPECTED(line_number) << endl;
    }

    unsigned i = 0;
    while (1)
    {
        i = skipWordBreaks(inLine, 0);
        if (inLine.substr(i, 3) != gVAR)
        {
            generateCode.startCode();
            return inLine;
        }
        if (i + 3 >= skipWordBreaks(inLine, i + 3))
        {
            outFile << WORD_BREAK_EXPECTED(line_number, gVAR) << endl;
            errorExit();
        }
        i = skipWordBreaks(inLine, i + 3);

        if (inLine.substr(i, 5) != gINT16)
        {
            outFile << TOKEN_EXPECTED(line_number, gVAR, gINT16) << endl;
            errorExit();
        }

        if (i + 5 >= skipWordBreaks(inLine, i + 5))
        {
            outFile << WORD_BREAK_EXPECTED(line_number, gINT16) << endl;
            errorExit();
        }
        i = skipWordBreaks(inLine, i + 5);

        if (i != checkIdent(inLine, i))
        {
            outFile << IDENT_EXPECTED(line_number, gVAR) << endl;
            errorExit();
        }
        string ident_string = inLine.substr(i, 2);
        short ident = checkForDuplicats(ident_string);

        i = skipWordBreaks(inLine, i + 2);

        lexemFile << line_number << "\t\t" << gVAR << endl;
        lexemFile << line_number << "\t\t" << gINT16 << endl;
        lexemFile << line_number << "\t\t"
                            << "identifier(" + ident_string + ")" << endl;

        identFile << line_number << "\t\t" << ident_string << endl;

        if (inLine[i] == ';')
        {
            if (skipWordBreaks(inLine, i + 1) != inLine.length())
            {
                outFile << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                errorExit();
            }
            ident_list->push_back(identifier{ ident, 0 });
            generateCode.declareVar(ident_string);

            lexemFile << line_number << "\t\t" << gSEMICOLON << endl;

            inLine = skipEmptyLines(inFile);
            continue;
        }
        else if (inLine.substr(i, 2) == gASSIGNMENT)
        {
            i = skipWordBreaks(inLine, i + 2);
            unsigned constEndPos = getConst(inLine, i);
            if (constEndPos == i)
            {
                outFile << CONSTANT_EXPECTED(line_number) << endl;
                errorExit();
            }
            string numStr = inLine.substr(i, constEndPos - i);
            short constantVal = stoi(numStr);

            i = skipWordBreaks(inLine, constEndPos);
            if (inLine[i] != ';')
            {
                outFile << SEMICOLON_EXPECTED(line_number, numStr) << endl;
                errorExit();
            }
            i = skipWordBreaks(inLine, i + 1);
            if (i < inLine.length())
            {
                outFile << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                errorExit();
            }

            ident_list->push_back(identifier{ ident, constantVal });

            lexemFile << line_number << "\t\t" << gASSIGNMENT << endl;
            lexemFile << line_number << "\t\t"
                                << "const(" + numStr + ")" << endl;
            lexemFile << line_number << "\t\t" << gSEMICOLON << endl;

            generateCode.declareVar(ident_string, constantVal);
            inLine = skipEmptyLines(inFile);
        }
        else
        {
            outFile << UNEXPECTED_TOKEN(line_number) << endl;
            errorExit();
        }
    }
}

void checkProgramBody(string& inLine, fstream& inFile)
{
    unsigned i;

    i = skipWordBreaks(inLine, 0);

    generateCode.writeComment(inLine.substr(i));

    if (i == checkIdent(inLine, i))
    {
        string ident_string = inLine.substr(i, 2);

        identExist(ident_string);

        i = skipWordBreaks(inLine, i + 2);

        if (inLine.substr(i, 2) != gASSIGNMENT)
        {
            outFile << TOKEN_EXPECTED(line_number, ident_string, gASSIGNMENT) << endl;
            errorExit();
        }

        i = skipWordBreaks(inLine, i + 2);

        lexemFile << line_number << "\t\t"
                            << "identifier(" + ident_string + ")" << endl;
        lexemFile << line_number << "\t\t" << gASSIGNMENT << endl;

        i = solveExpression(inLine, i);

        if (!checkSemicolon(inLine, i))
        {
            outFile << SEMICOLON_EXPECTED(line_number, "Position " + to_string(i)) << endl;
            errorExit();
        }

        lexemFile << line_number << "\t\t" << gSEMICOLON << endl;

        generateCode.assignmentCode(ident_string);
    }
    else if (inLine.substr(i, 4) == gSCAN)
    {
        if (i + 4 == skipWordBreaks(inLine, i + 4))
        {
            outFile << WORD_BREAK_EXPECTED(line_number, gSCAN) << endl;
            errorExit();
        }

        i = skipWordBreaks(inLine, i + 4);

        if (i != checkIdent(inLine, i))
        {
            outFile << IDENT_EXPECTED(line_number, gSCAN) << endl;
            errorExit();
        }

        string ident_string = inLine.substr(i, 2);

        identExist(ident_string);

        i = skipWordBreaks(inLine, i + 2);

        if (!checkSemicolon(inLine, i))
        {
            outFile << SEMICOLON_EXPECTED(line_number, ident_string) << endl;
            errorExit();
        }

        lexemFile << line_number << "\t\t" << gSCAN << endl;
        lexemFile << line_number << "\t\t"
                            << "identifier(" + ident_string + ")" << endl;
        lexemFile << line_number << "\t\t" << gSEMICOLON << endl;

        generateCode.scanCode(ident_string);
    }
    else if (inLine.substr(i, 5) == gPRINT)
    {
        if (i + 5 == skipWordBreaks(inLine, i + 5))
        {
            outFile << WORD_BREAK_EXPECTED(line_number, gPRINT) << endl;
            errorExit();
        }

        i = skipWordBreaks(inLine, i + 5);

        if (i != checkIdent(inLine, i))
        {
            outFile << IDENT_EXPECTED(line_number, gPRINT) << endl;
            errorExit();
        }

        string ident_string = inLine.substr(i, 2);

        identExist(ident_string);

        i = skipWordBreaks(inLine, i + 2);

        if (!checkSemicolon(inLine, i))
        {
            outFile << SEMICOLON_EXPECTED(line_number, ident_string) << endl;
            errorExit();
        }

        lexemFile << line_number << "\t\t" << gPRINT << endl;
        lexemFile << line_number << "\t\t"
                            << "identifier(" + ident_string + ")" << endl;
        lexemFile << line_number << "\t\t" << gSEMICOLON << endl;

        generateCode.printCode(ident_string);
    }
    else if (inLine.substr(i, 5) == gWHILE)
    {
        if (i + 5 == skipWordBreaks(inLine, i + 5))
        {
            outFile << WORD_BREAK_EXPECTED(line_number, gWHILE) << endl;
            errorExit();
        }

        generateCode.whileStart();

        i = skipWordBreaks(inLine, i + 5);
        lexemFile << line_number << "\t\t" << gWHILE << endl;

        i = solveExpression(inLine, i);

        if (inLine[i - 1] != ' ' && inLine[i - 1] != '\t')
        {
            outFile << WORD_BREAK_EXPECTED(line_number, "Position " + to_string(i)) << endl;
            errorExit();
        }

        if (inLine.substr(i, 2) != gDO)
        {
            outFile << TOKEN_EXPECTED(line_number, "Condition", gDO) << endl;
            errorExit();
        }

        i = skipWordBreaks(inLine, i + 2);

        if (i != inLine.length())
        {
            outFile << NEW_LINE_EXPECTED(line_number, gDO) << endl;
            errorExit();
        }

        lexemFile << line_number << "\t\t" << gDO << endl;

        inLine = skipEmptyLines(inFile);

        i = skipWordBreaks(inLine, 0);

        if (inLine.substr(i, 5) != gBEGIN)
        {
            outFile << TOKEN_EXPECTED(line_number, gDO, gBEGIN) << endl;
            errorExit();
        }

        i = skipWordBreaks(inLine, i + 5);

        if (i != inLine.length())
        {
            outFile << NEW_LINE_EXPECTED(line_number, gBEGIN) << endl;
            errorExit();
        }

        ++unclosed_clocks_count;

        lexemFile << line_number << "\t\t" << gBEGIN << endl;

        generateCode.whileCmp();
    }
    else if (inLine.substr(i, 3) == gEND)
    {
        lexemFile << line_number << "\t\t" << gEND << endl;

        i = skipWordBreaks(inLine, i + 3);

        --unclosed_clocks_count;

        if (i != inLine.length())
        {
            outFile << NEW_LINE_EXPECTED(line_number, gEND) << endl;
            errorExit();
        }

        if (unclosed_clocks_count < 0)
        {
            outFile << OPEN_BLOCK_EXPECTED(line_number) << endl;
            errorExit();
        }
        else if (unclosed_clocks_count == 0)
        {
            inLine = skipEmptyLines(inFile);
            if (inLine != "")
            {
                short program_end_line = line_number;
                outFile << NO_TOKENS_EXPECTED(line_number, "end of the program: \"END\" in line " + to_string(program_end_line)) << endl;
                errorExit();
            }
            return;
        }

        generateCode.whileEnd();
    }
    else
    {
        outFile << UNEXPECTED_TOKEN(line_number) << endl;
        errorExit();
    }
}

unsigned solveExpression(string& inLine, unsigned startPos)
{
    gOperator curOperator = gUNKNOWN_OPERATOR;

    do
    {
        startPos = solveExpressionPart(inLine, startPos + curOperator.name.length(), curOperator);
        generateCode.regOperator(curOperator.name, "cx");
        generateCode.movCxBx();

        curOperator = getNextOperator(inLine, startPos);
    } while (curOperator.name != gUNKNOWN_OPERATOR.name);
    if (unclosed_brackets_count != 0)
    {
        outFile << CLOSE_BRACKET_EXPECTED(line_number) << endl;
        errorExit();
    }
    return startPos;
}

unsigned solveExpressionPart(string& inLine, unsigned startPos, gOperator prevOperator)
{
    startPos = skipWordBreaks(inLine, startPos);
    if (inLine.substr(startPos, 1) == gOPEN_BRACKET.name)
    {
        lexemFile << line_number << "\t\t" << gOPEN_BRACKET.name << endl;

        short t_u_b = unclosed_brackets_count;
        ++unclosed_brackets_count;
        gOperator nextOperator = gUNKNOWN_OPERATOR;

        generateCode.pushCx();
        ++startPos;

        do
        {
            startPos = solveExpressionPart(inLine, startPos + nextOperator.name.length(), nextOperator);
            generateCode.regOperator(nextOperator.name, "cx");
            generateCode.movCxBx();
            nextOperator = getNextOperator(inLine, startPos);

            if (nextOperator.name == gUNKNOWN_OPERATOR.name)
            {
                generateCode.movBxCx();
                generateCode.popCx();
                return startPos;
            }
        } while (t_u_b < unclosed_brackets_count && startPos < inLine.length() - 1);

        if (t_u_b > unclosed_brackets_count)
        {
            return startPos;
        }

        if (nextOperator.name == gUNKNOWN_OPERATOR.name)
        {
            generateCode.movBxCx();
            generateCode.popCx();
            return startPos;
        }

        if (nextOperator.priority <= prevOperator.priority)
        {
            generateCode.movBxCx();
            generateCode.popCx();
            return startPos;
        }

        startPos = solveExpressionPart(inLine, startPos + nextOperator.name.length(), prevOperator);

        generateCode.regOperator(nextOperator.name, "cx");
        generateCode.popCx();

        return startPos;
    }
    else if (inLine.substr(startPos, 2) == gNOT.name)
    {
        lexemFile << line_number << "\t\t" << gNOT.name << endl;

        startPos = solveExpressionPart(inLine, startPos + 2, gNOT);
        generateCode.regOperator(gNOT.name, "");
        return startPos;
    }
    else if (startPos == checkIdent(inLine, startPos))
    {
        string ident_string = inLine.substr(startPos, 2);
        identExist(ident_string);

        lexemFile << line_number << "\t\t"
                            << "identifier(" + ident_string + ")" << endl;

        startPos = skipWordBreaks(inLine, startPos + 2);

        while (inLine.substr(startPos, 1) == gCLOSE_BRACKET.name)
        {
            lexemFile << line_number << "\t\t" << gCLOSE_BRACKET.name << endl;

            --unclosed_brackets_count;
            if (unclosed_brackets_count < 0)
            {
                outFile << OPEN_BRACKET_EXPECTED(line_number, startPos) << endl;
                errorExit();
            }

            startPos = skipWordBreaks(inLine, startPos + 1);

            if (inLine.substr(startPos, 1) != gCLOSE_BRACKET.name)
            {
                generateCode.regMov("_" + ident_string);
                return startPos;
            }
        }

        gOperator nextOperator = getNextOperator(inLine, startPos);

        if (nextOperator.name == gUNKNOWN_OPERATOR.name)
        {
            generateCode.regMov("_" + ident_string);
            return startPos;
        }

        lexemFile << line_number << "\t\t" << nextOperator.name << endl;

        if (nextOperator.priority <= prevOperator.priority)
        {
            generateCode.regMov("_" + ident_string);
            return startPos;
        }

        startPos = solveExpressionPart(inLine, startPos + nextOperator.name.length(), prevOperator);

        generateCode.regOperator(nextOperator.name, "_" + ident_string);

        return startPos;
    }
    else if (startPos != getConst(inLine, startPos))
    {
        unsigned constEnd = getConst(inLine, startPos);
        string constStr = inLine.substr(startPos, constEnd - startPos);
        short constNum = stoi(constStr);

        lexemFile << line_number << "\t\t"
                            << "const(" + constStr + ")" << endl;

        if (constNum < 0 && prevOperator.name != gUNKNOWN_OPERATOR.name)
        {
            outFile << OPEN_BRACKET_EXPECTED(line_number, startPos) << endl;
            errorExit();
        }

        startPos = skipWordBreaks(inLine, constEnd);

        while (inLine.substr(startPos, 1) == gCLOSE_BRACKET.name)
        {
            lexemFile << line_number << "\t\t" << gCLOSE_BRACKET.name << endl;

            --unclosed_brackets_count;
            if (unclosed_brackets_count < 0)
            {
                outFile << OPEN_BRACKET_EXPECTED(line_number, startPos) << endl;
                errorExit();
            }

            startPos = skipWordBreaks(inLine, startPos + 1);

            if (inLine.substr(startPos, 1) != gCLOSE_BRACKET.name)
            {
                generateCode.regMov(constStr);
                return startPos;
            }
        }

        gOperator nextOperator = getNextOperator(inLine, startPos);

        if (nextOperator.name == gUNKNOWN_OPERATOR.name)
        {
            generateCode.regMov(constStr);
            return startPos;
        }

        lexemFile << line_number << "\t\t" << nextOperator.name << endl;

        if (nextOperator.priority <= prevOperator.priority)
        {
            generateCode.regMov(constStr);
            return startPos;
        }

        startPos = solveExpressionPart(inLine, startPos + nextOperator.name.length(), prevOperator);
        generateCode.regOperator(nextOperator.name, constStr);
        return startPos;
    }
    else
    {
        outFile << UNEXPECTED_TOKEN(line_number) << endl;
        errorExit();
    }
}

gOperator getNextOperator(string& inLine, unsigned pos)
{
    string l1 = inLine.substr(pos, 1);
    string l2 = inLine.substr(pos, 2);
    string l3 = inLine.substr(pos, 3);

    if (l1 == gPLUS.name)
    {
        return gPLUS;
    }
    else if (l1 == gMINUS.name)
    {
        return gMINUS;
    }
    else if (l1 == gMULT.name)
    {
        return gMULT;
    }
    else if (l2 == gEQUAL.name)
    {
        return gEQUAL;
    }
    else if (l2 == gNOT_EQUAL.name)
    {
        return gNOT_EQUAL;
    }
    else if (l2 == gGRATER.name)
    {
        return gGRATER;
    }
    else if (l2 == gLESS.name)
    {
        return gLESS;
    }
    else if (l2 == gAND.name)
    {
        return gAND;
    }
    else if (l2 == gOR.name)
    {
        return gOR;
    }
    else if (l3 == gDIV.name)
    {
        return gDIV;
    }
    else if (l3 == gMOD.name)
    {
        return gMOD;
    }
    else
    {
        return gUNKNOWN_OPERATOR;
    }
}
