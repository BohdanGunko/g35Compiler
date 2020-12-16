#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
using namespace std;

#define gPROGRAM "PROGRAM"
#define gVAR "VAR"
#define gBEGIN "BEGIN"
#define gEND "END"
#define gSCAN "SCAN"
#define gPRINT "PRINT"
#define gASSIGNMENT "->"
#define gWHILE "WHILE"
#define gDO "DO"
#define gPLUS '+'
#define gMINUS '-'
#define gMULT '*'
#define gDIV "DIV"
#define gMOD "MOD"
#define gEQUAL "=="
#define gNOT_EQUAL "<>"
#define gGRATER ">>"
#define gLESS "<<"
#define gNOT "!!"
#define gAND "&&"
#define gOR "||"
#define gCOM_ST "#*"
#define gCOM_END "*#"
#define gBLOCK_START '('
#define gBLOCK_END ')'
#define gSEMICOLON ';'

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
#define UNEXPECTED_TOKEN(ln, token) "Error in line " + to_string(ln) + ":Unexpected token after \"" + token + "\""
#define END_OF_COMMENT_EXPECTED "Error:End of comment expected"
#define UNDECLARED_IDENT(ln, ident) "Error in line " + to_string(ln) + ":Undeclared identificator \"" + ident + "\""
#define OPEN_BRACKET_EXPECTED(ln, pos) "Error in line " + to_string(ln) + ":Expected open bracket before possition \"" + to_string(pos) + "\""
#define CLOSE_BRACKET_EXPECTED(ln) "Error in line " + to_string(ln) + ":Not enought closed brackets"
#define TOKEN_EXPECTED(ln, after_token, expected_token) "Error in line " + to_string(ln) + ":Expected \"" + expected_token + "\" after token \"" + after_token + "\""

enum lexemType
{
    PROGRAM_,
    BEGIN_,
    VAR_,
    END_,
    SCAN_,
    PRINT_,
    ASSIGNMENT_,
    WHILE_,
    DO_,
    PLUS_,
    MINUS_,
    MULT_,
    DIV_,
    MOD_,
    EQUAL_,
    NOT_EQUAL_,
    GRATER_,
    LESS_,
    NOT_,
    AND_,
    OR_,
    COM_ST_,			 //#*
    COM_END_,			 //*#
    BLOCK_START_,	 //(
    BLOCK_END_,		 //)
    SEMICOLON_,		 //;
    IDENT_,
    CONST_,
    UNKNOWN_
};

struct identifier
{
    short name;
    short value;
    bool defined;
};

int line_number = 0;
short total_brackets = 0;
vector<identifier>* ident_table;

short checkForDuplicats(string& ident_string);
unsigned getConst(string& inLine, unsigned pos);
unsigned checkIdent(string& inLine, unsigned i);
string skipEmptyLines(fstream& inFile);
void solveExpression(string& inLine, unsigned startPos);
short identExist(string& ident_string);

void translateToAsm(fstream& inFile);
void checkPROGRAM(fstream& inFile);
string checkVarDec(fstream& inFile);
void checkProgramBody(string& inLine, fstream& inFile);
void getOperator(string& inLine, unsigned pos, string& prevToken);

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
    translateToAsm(inFile);

    inFile.close();

    delete ident_table;
    return 1;
}

void translateToAsm(fstream& inFile)
{
    checkPROGRAM(inFile);
    string inLine = checkVarDec(inFile);
    checkProgramBody(inLine, inFile);
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

                        ;
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
    string pName = inLine.substr(i, 2);

    i = skipWordBreaks(inLine, i + 2);
    if (i >= inLine.length() || inLine[i] != ';')
    {
        cout << SEMICOLON_EXPECTED(line_number, pName) << endl;
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
            cout << "SUCCESS" << endl;
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
            ident_table->push_back(identifier{ ident, 0, false });
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

            ident_table->push_back(identifier{ ident, constantVal, true });
            inLine = skipEmptyLines(inFile);
        }

        else
        {
            cout << UNEXPECTED_TOKEN(line_number, ident_string) << endl;
            exit(0);
        }
    }
}

void checkProgramBody(string& inLine, fstream& inFile)
{
    unsigned i;
    do
    {
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

            solveExpression(inLine, i);
        }
        else if (inLine.substr(i, 4) == gSCAN)
        {
            // we found SCAN
            cout << "SCAN found" << endl;
        }
        else if (inLine.substr(i, 5) == gPRINT)
        {
            // we found PRINT
            cout << "PRINT found" << endl;
        }
        else if (inLine.substr(i, 5) == gWHILE)
        {
            // we found WHILE
            cout << "WHILE found" << endl;
        }
        else if (inLine.substr(i, 3) == gEND)
        {
            // we found END
            cout << "END found" << endl;
        }
        else
        {
            // error bad token
            cout << "error bad token in line: " << line_number << endl;
        }

        inLine = skipEmptyLines(inFile);

    } while (inLine != "");
}

void solveExpression(string& inLine, unsigned startPos)
{
    for (unsigned i = startPos; i < inLine.length(); ++i)
    {
        i = skipWordBreaks(inLine, i);
        if (inLine[i] == gMINUS)
        {
            cout << OPEN_BRACKET_EXPECTED(line_number, i) << endl;
            exit(0);
        }

        else if (inLine[i] == gBLOCK_START)
        {
            ++total_brackets;
            solveExpression(inLine, i + 1);
            return;
        }
        else if (i != getConst(inLine, i))
        {
            unsigned constEnd = getConst(inLine, i);
            string constStr = inLine.substr(i, constEnd - i);
            short constNum = stoi(constStr);

            i = skipWordBreaks(inLine, constEnd);

            if (inLine[i] == gSEMICOLON)
            {
                if (total_brackets != 0)
                {
                    cout << CLOSE_BRACKET_EXPECTED(line_number) << endl;
                    exit(0);
                }
                if (skipWordBreaks(inLine, i + 1) != inLine.length())
                {
                    cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                    exit(0);
                }
                // to do: generete asm code //mov eax, ident; ?mov tmp_res, eax
                // check if rest on the line is empty
                cout << "Ident only" << endl;
                return;
            }

            if (inLine[i] == gBLOCK_END)
            {
                --total_brackets;
                if (total_brackets < 0)
                {
                    cout << OPEN_BRACKET_EXPECTED(line_number, i) << endl;
                    exit(0);
                }
                // to do: generete asm code //mov eax, ident; ?mov tmp_res, eax
                // check if rest on the line is empty

                i = skipWordBreaks(inLine, i + 1);

                if (inLine[i] == gSEMICOLON)
                {
                    if (total_brackets != 0)
                    {
                        cout << CLOSE_BRACKET_EXPECTED(line_number) << endl;
                        exit(0);
                    }
                    // to do: generete asm code //mov eax, ident; ?mov tmp_res, eax
                    // check if rest on the line is empty
                    if (skipWordBreaks(inLine, i + 1) != inLine.length())
                    {
                        cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                        exit(0);
                    }
                    cout << "Ident only" << endl;
                    return;
                }
                getOperator(inLine, i, constStr);
                return;
            }

            getOperator(inLine, i, constStr);
            return;
        }
        else if (i == checkIdent(inLine, i))
        {
            string ident_string = inLine.substr(i, 2);
            short ident_name = identExist(ident_string);

            i = skipWordBreaks(inLine, i + 2);

            if (i == inLine.length())
            {
                cout << SEMICOLON_EXPECTED(line_number, ident_string) << endl;
                exit(0);
            }

            if (inLine[i] == gSEMICOLON)
            {
                if (total_brackets != 0)
                {
                    cout << CLOSE_BRACKET_EXPECTED(line_number) << endl;
                    exit(0);
                }
                if (skipWordBreaks(inLine, i + 1) != inLine.length())
                {
                    cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                    exit(0);
                }
                // to do: generete asm code //mov eax, ident; ?mov tmp_res, eax
                // check if rest on the line is empty
                cout << "Ident only" << endl;
                return;
            }

            if (inLine[i] == gBLOCK_END)
            {
                --total_brackets;
                if (total_brackets < 0)
                {
                    cout << OPEN_BRACKET_EXPECTED(line_number, i) << endl;
                    exit(0);
                }
                // to do: generete asm code //mov eax, ident; ?mov tmp_res, eax
                // check if rest on the line is empty

                i = skipWordBreaks(inLine, i + 1);

                if (inLine[i] == gSEMICOLON)
                {
                    if (total_brackets != 0)
                    {
                        cout << CLOSE_BRACKET_EXPECTED(line_number) << endl;
                        exit(0);
                    }
                    // to do: generete asm code //mov eax, ident; ?mov tmp_res, eax
                    // check if rest on the line is empty
                    if (skipWordBreaks(inLine, i + 1) != inLine.length())
                    {
                        cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                        exit(0);
                    }
                    cout << "Ident only" << endl;
                    return;
                }
                getOperator(inLine, i, ident_string);
                return;
            }

            getOperator(inLine, i, ident_string);
            return;
        }
        else if (inLine[i] == gSEMICOLON)
        {
            // to do: shoe after what token error was
            cout << IDENT_EXPECTED(line_number, "") << endl;
            exit(0);
        }
        else
        {
            // to do: shoe after what token error was
            cout << UNEXPECTED_TOKEN(line_number, "") << endl;
            exit(0);
        }
    }
}

void getOperator(string& inLine, unsigned pos, string& prevToken)
{
    char l1 = inLine[pos];
    string l2 = inLine.substr(pos, 2);
    string l3 = inLine.substr(pos, 3);

    if (l1 == gPLUS)
    {
        solveExpression(inLine, pos + 1);
    }
    else if (l1 == gMINUS)
    {
        solveExpression(inLine, pos + 1);
    }
    else if (l1 == gMULT)
    {
        solveExpression(inLine, pos + 1);
    }
    else if (l2 == gEQUAL)
    {
        solveExpression(inLine, pos + 2);
    }
    else if (l2 == gNOT_EQUAL)
    {
        solveExpression(inLine, pos + 2);
    }
    else if (l2 == gGRATER)
    {
        solveExpression(inLine, pos + 2);
    }
    else if (l2 == gLESS)
    {
        solveExpression(inLine, pos + 2);
    }
    else if (l2 == gNOT)
    {
        solveExpression(inLine, pos + 2);
    }
    else if (l2 == gAND)
    {
        solveExpression(inLine, pos + 2);
    }
    else if (l2 == gOR)
    {
        solveExpression(inLine, pos + 2);
    }
    else if (l3 == gDIV)
    {
        solveExpression(inLine, pos + 3);
    }
    else if (l3 == gMOD)
    {
        solveExpression(inLine, pos + 3);
    }
    else
    {
        cout << UNEXPECTED_TOKEN(line_number, prevToken) << endl;
        exit(0);
    }
}
