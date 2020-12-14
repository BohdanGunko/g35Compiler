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

#define NO_ENTRY_POINT(ln) "Error in line " + to_string(ln) + ":No entry point, expected \"PROGRAM\""
#define BEGIN_EXPECTED(ln) "Error in line " + to_string(ln) + ":Expected \"BEGIN\""
#define WORD_BREAK_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected space or tab after \"" + token + "\""
#define IDENT_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected identifier after \"" + token + "\""
#define SEMICOLON_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected semicolon after \"" + token + "\""
#define NEW_LINE_EXPECTED(ln, token) "Error in line " + to_string(ln) + ":Expected new line after \"" + token + "\""

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

string skipEmptyLines(fstream& inFile);
void checkPROGRAM(fstream& inFile);
void translateToAsm(fstream& inFile);
void checkVarDecs(fstream& inFile);

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

    lexList = new vector<Lex>();

    translateToAsm(inFile);

    inFile.close();

    delete lexList;

    return 1;
}

void translateToAsm(fstream& inFile)
{
    checkPROGRAM(inFile);
    checkVarDec(inFile);
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
        exit(0);
    }

    if (i + 7 >= skipWordBreaks(inLine, i + 7))
    {
        cout << WORD_BREAK_EXPECTED(line_number, gPROGRAM) << endl;
        exit(0);
    }

    i = skipWordBreaks(inLine, i + 7);
    regex rgx("[a-z][a-z0-9]");
    string pName = inLine.substr(i, 2);

    if (!regex_match(pName, rgx))
    {
        cout << IDENT_EXPECTED(line_number, gPROGRAM) << endl;
        exit(0);
    }

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
    cout << "All good)" << endl;
}

void checkVarDecs(fstream& inFile)
{

}
