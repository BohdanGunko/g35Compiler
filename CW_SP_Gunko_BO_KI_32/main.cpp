#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
using namespace std;

#define myDEBUG

#define gPROGRAM "PROGRAM"
#define gVAR "VAR"
#define gBEGIN "BEGIN"
#define gEND "END"
#define gSCAN "SCAN"
#define gPRINT "PRINT"
#define gASSIGNMENT "->"
#define gWHILE "WHILE"
#define gDO "DO"
#define gPLUS "+"
#define gMINUS "-"
#define gMULT "*"
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
#define gBLOCK_START "("
#define gBLOCK_END ")"
#define gSEMICOLON ";"

#define NO_ENTRY_POINT(ln) "Error in line " + to_string(ln) + ":No entry point, expected 'PROGRAM'"
#define WORD_BREAK_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected space or tab after " + token
#define IDENT_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected identifier after " + token
#define SEMICOLON_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected semicolon after " + token
#define NEW_LINE_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected new line after " + token

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

struct Lex
{
    string token;
    int lineNumber;
    lexemType lexType;
    int posInTable;
};

int line_number = 0;
vector<Lex>* lexList;

void translateToAsm(fstream& inFile);
string skipEmptyLines(fstream& inFile);
void checkPROGRAM(fstream& inFile);

void err_exit()
{
    lexList->clear();
    line_number = 1;
    exit(0);
}

int main()
{
#ifdef myDEBUG
    lexList = new vector<Lex>();
#endif

    string filePath;
enter_file_name:
    cout << "Please enter(or paste) path to file: " << endl;
    cin >> filePath;

#ifdef myDEBUG
    // to remove
    filePath = "S:\\LPNU\\Semestr 5\\System programming\\Course work\\cw_project\\testApp.g35";
#endif

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

#ifndef myDEBUG
    lexList = new vector<Lex>();
#endif

    translateToAsm(inFile);

    inFile.close();

#ifndef myDEBUG
    delete lexList;
#else
    lexList->clear();
    line_number = 0;
    goto enter_file_name;
#endif
    return 1;
}

void translateToAsm(fstream& inFile)
{
    checkPROGRAM(inFile);
}

string skipEmptyLines(fstream& inFile)
{
    string inLine;
    regex rgx("[ \t]{0,}");

    while (getline(inFile, inLine))
    {
        if (regex_match(inLine, rgx))
        {
            ++line_number;
        }
        else
        {
            return inLine;
        }
    }
    return "";
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

void checkPROGRAM(fstream& inFile)
{
    string inLine = skipEmptyLines(inFile);
    unsigned i = skipWordBreaks(inLine, 0);

    if (inLine.substr(i, 7) != gPROGRAM)
    {
        cout << NO_ENTRY_POINT(line_number) << endl;
        err_exit();
    }
    if (i + 7 < skipWordBreaks(inLine, i + 7))
    {
        i = skipWordBreaks(inLine, i + 7);
        regex rgx("[a-z]{2}");
        string pName = inLine.substr(i, 2);
        if (regex_match(pName, rgx))
        {
            i = skipWordBreaks(inLine, i + 2);
            if (i < inLine.length() && inLine[i] == ';')
            {
                if (i == inLine.length()-1 || skipWordBreaks(inLine, i+1)==inLine.length())
                {
                    cout << "All good)" << endl;
                }
                else
                {
                    cout << NEW_LINE_EXPECTED(line_number, gSEMICOLON) << endl;
                    err_exit();
                }
            }
            else
            {
                cout << SEMICOLON_EXPECTED(line_number, pName) << endl;
                err_exit();
            }
        }
        else
        {
            cout << IDENT_EXPECTED(line_number, gPROGRAM) << endl;
            err_exit();
        }
    }
    else
    {
        cout << WORD_BREAK_EXPECTED(line_number, gPROGRAM) << endl;
        err_exit();
    }
}
