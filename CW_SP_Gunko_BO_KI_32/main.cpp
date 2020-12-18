#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
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
#define gDO "DO"
#define gCOM_ST "#*"
#define gCOM_END "*#"
#define gSEMICOLON ';'

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
#define TOKEN_EXPECTED(ln, after_token, expected_token) "Error in line " + to_string(ln) + ":Expected \"" + expected_token + "\" after token \"" + after_token + "\""
#define OPEN_BLOCK_EXPECTED(ln) "Error in line " + to_string(ln) + ":Expected \"BEGIN\" before \"END\""
#define CLOSE_BLOCK_EXPECTED(ln) "Error in line " + to_string(ln) + ":Unclosed block, \"END\" expected"

struct identifier
{
    short name;
    short value;
};

int line_number = 0;
short total_unclosed_brackets = 0;
short total_unclosed_blocks = 0;
vector<identifier>* ident_table;
codeGenerator generateCode;

short checkForDuplicats(string& ident_string);
unsigned getConst(string& inLine, unsigned pos);
unsigned checkIdent(string& inLine, unsigned i);
string skipEmptyLines(fstream& inFile);
short identExist(string& ident_string);
unsigned solveExpression(string& inLine, unsigned startPos);
unsigned solveExpressionPart(string& inLine, unsigned startPos, gOperator prevOperator);
bool checkSemicolon(string& inLine, unsigned startPos);

void translateToAsm(fstream& inFile);
void checkPROGRAM(fstream& inFile);
string checkVarDec(fstream& inFile);
void checkProgramBody(string& inLine, fstream& inFile);
gOperator getNextOperator(string& inLine, unsigned pos);

int main()
{
    string filePath;
enter_file_name:
    cout << "Please enter(or paste) path to file: " << endl;
    cin >> filePath;

    // to remove
    filePath = "S:\\LPNU\\Semestr 5\\System programming\\Course work\\cw_project\\testApp.g35";

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

    ident_table = new vector<identifier>;
    generateCode.createFile(filePath);
    translateToAsm(inFile);

    inFile.close();

    delete ident_table;
    return 1;
}

void translateToAsm(fstream& inFile)
{
    checkPROGRAM(inFile);
    string inLine = checkVarDec(inFile);
    do
    {
        checkProgramBody(inLine, inFile);
        inLine = skipEmptyLines(inFile);
    } while (inLine != "");

    if (total_unclosed_blocks > 0)
    {
        cout << CLOSE_BLOCK_EXPECTED(line_number) << endl;
        exit(0);
    }

    generateCode.endCode();

    generateCode.assembleFile();
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

                    cout << END_OF_COMMENT_EXPECTED << endl;
                    exit(0);
                }
            }
            return inLine;
        }
    }
    return "";
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

short checkForDuplicats(string& ident_string)
{
    short ident = (short)ident_string[0] << 8 | (short)ident_string[1];
    for (auto& i : *ident_table)
    {
        if (i.name == ident)
        {
            cout << IDENT_ALREADY_EXIST(line_number, ident_string) << endl;
            exit(0);
        }
    }
    return ident;
}

short identExist(string& ident_string)
{
    short ident = (short)ident_string[0] << 8 | (short)ident_string[1];
    for (auto& i : *ident_table)
    {
        if (i.name == ident)
        {
            return ident;
        }
    }
    cout << UNDECLARED_IDENT(line_number, ident_string) << endl;
    exit(0);
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
                cout << CONSTANT_OUT_OF_RANGE(line_number) << endl;
                exit(0);
            }
            else if (num.length() > 5)
            {
                cout << CONSTANT_TOO_LONG(line_number) << endl;
                exit(0);
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

void checkPROGRAM(fstream& inFile)
{
    string inLine = skipEmptyLines(inFile);
    unsigned i = skipWordBreaks(inLine, 0);

    if (inLine.substr(i, 7) != gPROGRAM)
    {
        cout << NO_ENTRY_POINT(line_number) << endl;
        exit(0);
    }

    if (i + 7 >= skipWordBreaks(inLine, i + 7))
    {
        cout << WORD_BREAK_EXPECTED(line_number, gPROGRAM) << endl;
        exit(0);
    }

    i = skipWordBreaks(inLine, i + 7);

    if (i != checkIdent(inLine, i))
    {
        cout << IDENT_EXPECTED(line_number, gPROGRAM) << endl;
        exit(0);
    }
    string programName = inLine.substr(i, 2);

    i = skipWordBreaks(inLine, i + 2);
    if (i >= inLine.length() || inLine[i] != ';')
    {
        cout << SEMICOLON_EXPECTED(line_number, programName) << endl;
        exit(0);
    }

    if (i != inLine.length() - 1 && skipWordBreaks(inLine, i + 1) != inLine.length())
    {
        cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
        exit(0);
    }

    // to do: generate asm code

    inLine = skipEmptyLines(inFile);
    i = skipWordBreaks(inLine, 0);

    if (inLine.substr(i, 5) != gBEGIN)
    {
        cout << BEGIN_EXPECTED(line_number) << endl;
        exit(0);
    }

    if (i != inLine.length() - 1 && skipWordBreaks(inLine, i + 5) != inLine.length())
    {
        cout << NEW_LINE_EXPECTED(line_number, gBEGIN) << endl;
        exit(0);
    }
    ++total_unclosed_blocks;

    generateCode.setProgramName(programName);
}

string checkVarDec(fstream& inFile)
{
    string inLine = skipEmptyLines(inFile);

    if (inLine == "")
    {
        cout << END_EXPECTED(line_number) << endl;
    }

    unsigned i = skipWordBreaks(inLine, 0);

    if (inLine.substr(i, 3) == gEND)
    {
        if (skipWordBreaks(inLine, i + 3) != inLine.length() || skipEmptyLines(inFile) != "")
        {
            cout << NO_TOKENS_EXPECTED(line_number, gEND) << endl;
            exit(0);
        }
        cout << "Compiled" << endl;
        exit(1);
    }

    i = 0;
    while (1)
    {
        i = skipWordBreaks(inLine, 0);
        if (inLine.substr(i, 3) != gVAR)
        {
            generateCode.starCode();
            return inLine;
        }
        if (i + 3 >= skipWordBreaks(inLine, i + 3))
        {
            cout << WORD_BREAK_EXPECTED(line_number, gVAR) << endl;
            exit(0);
        }
        i = skipWordBreaks(inLine, i + 3);

        if (i != checkIdent(inLine, i))
        {
            cout << IDENT_EXPECTED(line_number, gVAR) << endl;
            exit(0);
        }
        string ident_string = inLine.substr(i, 2);
        short ident = checkForDuplicats(ident_string);

        i = skipWordBreaks(inLine, i + 2);

        if (inLine[i] == ';')
        {
            if (skipWordBreaks(inLine, i + 1) != inLine.length())
            {
                cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                exit(0);
            }
            ident_table->push_back(identifier{ ident, 0 });
            generateCode.declareVar(ident_string);
            inLine = skipEmptyLines(inFile);
            continue;
        }
        else if (inLine.substr(i, 2) == gASSIGNMENT)
        {
            i = skipWordBreaks(inLine, i + 2);
            unsigned constEndPos = getConst(inLine, i);
            if (constEndPos == i)
            {
                cout << CONSTANT_EXPECTED(line_number) << endl;
                exit(0);
            }
            string numStr = inLine.substr(i, constEndPos - i);
            short constantVal = stoi(numStr);

            i = skipWordBreaks(inLine, constEndPos);
            if (inLine[i] != ';')
            {
                cout << SEMICOLON_EXPECTED(line_number, numStr) << endl;
                exit(0);
            }
            i = skipWordBreaks(inLine, i + 1);
            if (i < inLine.length())
            {
                cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                exit(0);
            }

            ident_table->push_back(identifier{ ident, constantVal });

            generateCode.declareVar(ident_string, constantVal);
            inLine = skipEmptyLines(inFile);
        }

        else
        {
            cout << UNEXPECTED_TOKEN(line_number) << endl;
            exit(0);
        }
    }
}

void checkProgramBody(string& inLine, fstream& inFile)
{
    unsigned i;

    i = skipWordBreaks(inLine, 0);

    if (i == checkIdent(inLine, i))
    {
        string ident_string = inLine.substr(i, 2);
        i = skipWordBreaks(inLine, i + 2);

        if (inLine.substr(i, 2) != gASSIGNMENT)
        {
            cout << TOKEN_EXPECTED(line_number, ident_string, gASSIGNMENT) << endl;
            exit(0);
        }

        i = skipWordBreaks(inLine, i + 2);

        i = solveExpression(inLine, i);

        if (!checkSemicolon(inLine, i))
        {
            cout << SEMICOLON_EXPECTED(line_number, "Position " + to_string(i)) << endl;
            exit(0);
        }

        generateCode.assignmentCode(ident_string);
        // to do:generate assigning asm code
    }
    else if (inLine.substr(i, 4) == gSCAN)
    {
        if (i + 4 == skipWordBreaks(inLine, i + 4))
        {
            cout << WORD_BREAK_EXPECTED(line_number, gSCAN) << endl;
            exit(0);
        }

        i = skipWordBreaks(inLine, i + 4);

        if (i != checkIdent(inLine, i))
        {
            cout << IDENT_EXPECTED(line_number, gSCAN) << endl;
            exit(0);
        }

        string ident_string = inLine.substr(i, 2);

        short ident = identExist(ident_string);

        i = skipWordBreaks(inLine, i + 2);

        if (!checkSemicolon(inLine, i))
        {
            cout << SEMICOLON_EXPECTED(line_number, ident_string) << endl;
            exit(0);
        }

        generateCode.scanCode(ident_string);
    }
    else if (inLine.substr(i, 5) == gPRINT)
    {
        if (i + 5 == skipWordBreaks(inLine, i + 5))
        {
            cout << WORD_BREAK_EXPECTED(line_number, gPRINT) << endl;
            exit(0);
        }

        i = skipWordBreaks(inLine, i + 5);

        if (i != checkIdent(inLine, i))
        {
            cout << IDENT_EXPECTED(line_number, gPRINT) << endl;
            exit(0);
        }

        string ident_string = inLine.substr(i, 2);

        short ident = identExist(ident_string);

        i = skipWordBreaks(inLine, i + 2);

        if (!checkSemicolon(inLine, i))
        {
            cout << SEMICOLON_EXPECTED(line_number, ident_string) << endl;
            exit(0);
        }

        generateCode.printCode(ident_string);
    }
    else if (inLine.substr(i, 5) == gWHILE)
    {
        if (i + 5 == skipWordBreaks(inLine, i + 5))
        {
            cout << WORD_BREAK_EXPECTED(line_number, gWHILE) << endl;
            exit(0);
        }

        generateCode.whileStart();

        i = skipWordBreaks(inLine, i + 5);


        i = solveExpression(inLine, i);

        if (inLine[i - 1] != ' ' && inLine[i - 1] != '\t')
        {
            cout << WORD_BREAK_EXPECTED(line_number, "Position " + to_string(i)) << endl;
            exit(0);
        }

        if (inLine.substr(i, 2) != gDO)
        {
            cout << TOKEN_EXPECTED(line_number, "Condition", gDO) << endl;
            exit(0);
        }

        i = skipWordBreaks(inLine, i + 2);

        if (i != inLine.length())
        {
            cout << NEW_LINE_EXPECTED(line_number, gDO) << endl;
            exit(0);
        }

        inLine = skipEmptyLines(inFile);

        i = skipWordBreaks(inLine, 0);

        if (inLine.substr(i, 5) != gBEGIN)
        {
            cout << TOKEN_EXPECTED(line_number, gDO, gBEGIN) << endl;
            exit(0);
        }


        i = skipWordBreaks(inLine, i + 5);

        if (i != inLine.length())
        {
            cout << NEW_LINE_EXPECTED(line_number, gBEGIN) << endl;
            exit(0);
        }

        ++total_unclosed_blocks;

        generateCode.whileCmp();
    }
    else if (inLine.substr(i, 3) == gEND)
    {
        i = skipWordBreaks(inLine, i + 3);

        --total_unclosed_blocks;

        if (i != inLine.length())
        {
            cout << NEW_LINE_EXPECTED(line_number, gEND) << endl;
            exit(0);
        }

        if (total_unclosed_blocks < 0)
        {
            cout << OPEN_BLOCK_EXPECTED(line_number) << endl;
            exit(0);
        }
        else if (total_unclosed_blocks == 0)
        {
            inLine = skipEmptyLines(inFile);
            if (inLine != "")
            {
                short program_end_line = line_number;
                cout << NO_TOKENS_EXPECTED(line_number, "end of the program: \"END\" in line " + to_string(program_end_line)) << endl;
                exit(0);
            }
            return;
        }

        generateCode.whileEnd();
    }
    else
    {
        // error bad token
        cout << UNEXPECTED_TOKEN(line_number) << endl;
        exit(0);
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
        cout << curOperator.name << endl;
    } while (curOperator.name != gUNKNOWN_OPERATOR.name);
    if (total_unclosed_brackets != 0)
    {
        cout << CLOSE_BRACKET_EXPECTED(line_number) << endl;
        exit(0);
    }
    return startPos;
}

unsigned solveExpressionPart(string& inLine, unsigned startPos, gOperator prevOperator)
{
    startPos = skipWordBreaks(inLine, startPos);
    if (inLine.substr(startPos, 1) == gOPEN_BRACKET.name)
    {
        cout << "(" << endl;
        short t_u_b = total_unclosed_brackets;
        ++total_unclosed_brackets;
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
        } while (t_u_b < total_unclosed_brackets && startPos < inLine.length() - 1);

        if (t_u_b > total_unclosed_brackets)
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

        cout << prevOperator.name << endl;
        cout << "ident_string"
                        ""
                 << endl;

        return startPos;
    }
    else if (inLine.substr(startPos, 2) == gNOT.name)
    {
        cout << "!!" << endl;

        startPos = solveExpressionPart(inLine, startPos + 2, gNOT);
        generateCode.regOperator(gNOT.name, "");
        return startPos;
    }
    else if (startPos == checkIdent(inLine, startPos))
    {
        string ident_string = inLine.substr(startPos, 2);
        short ident = identExist(ident_string);

        startPos = skipWordBreaks(inLine, startPos + 2);

        while (inLine.substr(startPos, 1) == gCLOSE_BRACKET.name)
        {
            --total_unclosed_brackets;
            if (total_unclosed_brackets < 0)
            {
                cout << OPEN_BRACKET_EXPECTED(line_number, startPos) << endl;
                exit(0);
            }

            startPos = skipWordBreaks(inLine, startPos + 1);

            cout << ident_string << endl;
            cout << ")" << endl;
            if (inLine.substr(startPos, 1) != gCLOSE_BRACKET.name)
            {
                generateCode.regMov("_" + ident_string);
                return startPos;
            }
        }

        gOperator nextOperator = getNextOperator(inLine, startPos);

        if (nextOperator.name == gUNKNOWN_OPERATOR.name)
        {
            cout << ident_string << endl;
            generateCode.regMov("_" + ident_string);
            return startPos;
        }

        if (nextOperator.priority <= prevOperator.priority)
        {
            generateCode.regMov("_" + ident_string);
            return startPos;
        }

        startPos = solveExpressionPart(inLine, startPos + nextOperator.name.length(), prevOperator);

        generateCode.regOperator(nextOperator.name, "_" + ident_string);

        cout << ident_string << endl;

        return startPos;
        // to do: generate asm code for nextOperator
    }
    else if (startPos != getConst(inLine, startPos))
    {
        unsigned constEnd = getConst(inLine, startPos);
        string constStr = inLine.substr(startPos, constEnd - startPos);
        short constNum = stoi(constStr);

        if (constNum < 0 && prevOperator.name != gUNKNOWN_OPERATOR.name)
        {
            cout << OPEN_BRACKET_EXPECTED(line_number, startPos) << endl;
            exit(0);
        }

        startPos = skipWordBreaks(inLine, constEnd);

        while (inLine.substr(startPos, 1) == gCLOSE_BRACKET.name)
        {
            --total_unclosed_brackets;
            if (total_unclosed_brackets < 0)
            {
                cout << OPEN_BRACKET_EXPECTED(line_number, startPos) << endl;
                exit(0);
            }

            startPos = skipWordBreaks(inLine, startPos + 1);

            cout << constStr << endl;
            cout << ")" << endl;
            if (inLine.substr(startPos, 1) != gCLOSE_BRACKET.name)
            {
                generateCode.regMov(constStr);
                return startPos;
            }
        }

        gOperator nextOperator = getNextOperator(inLine, startPos);

        if (nextOperator.name == gUNKNOWN_OPERATOR.name)
        {
            cout << constStr << endl;
            generateCode.regMov(constStr);
            return startPos;
        }
        if (nextOperator.priority <= prevOperator.priority)
        {
            generateCode.regMov(constStr);
            return startPos;
        }

        startPos = solveExpressionPart(inLine, startPos + nextOperator.name.length(), prevOperator);
        generateCode.regOperator(nextOperator.name, constStr);
        return startPos;
        // to do: generate asm code for nextOperator
    }
    else
    {
        cout << UNEXPECTED_TOKEN(line_number) << endl;
        exit(0);
    }
    // const
    // ident
    //!!
}

bool checkSemicolon(string& inLine, unsigned startPos)
{
    if (startPos < inLine.length() && inLine[startPos] == gSEMICOLON)
    {
        for (unsigned i = startPos + 1; i < inLine.length(); ++i)
        {
            if (inLine[i] != ' ' && inLine[i] != '\t')
            {
                cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                exit(0);
            }
        }
        return true;
    }
    return false;
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
