#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
using namespace std;

#define myDEBUG

int line_number = 0;

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

#define NO_ENTRY_POINT "Expected \"PROGRAM\" in line " + to_string(line_number)
#define NO_BEGIN_POINT "Expected \"BEGIN\" in line " + to_string(line_number)

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

vector<Lex>* lexList;

void lexicalAnalysis(fstream& inFile);
bool checkLexError(string& lex);
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

    lexicalAnalysis(inFile);

    inFile.close();

    cout << endl << "SUCCESS" << endl;

#ifndef myDEBUG
    delete lexList;
#else
    lexList->clear();
    line_number = 0;
    goto enter_file_name;
#endif
    return 1;
}

void lexicalAnalysis(fstream& inFile)
{
    regex rgx_IDENT("[a-z]{2,}");
    regex rgx_CONST("[0-9]{1,}");
    string inLine;
    string curLex = "";
    while (getline(inFile, inLine))
    {
        ++line_number;
        for (unsigned i = 0; i < inLine.length(); ++i)
        {
            curLex += inLine[i];
            if (curLex == gPROGRAM)
            {
                if (i < (inLine.length() - 1) && (inLine[i + 1] != ' ' && inLine[i + 1] != '\t'))
                {
                    cout << "Error: in line " << line_number << " space or tab expected after \"PROGRAM\"" << endl;
                    exit(0);
                }

                lexList->push_back(Lex{ gPROGRAM, line_number, lexemType(PROGRAM_), -1 });
                curLex = "";
                ++i;
            }
            else if (curLex == gBEGIN)
            {
                for (unsigned gI = i+1; gI < inLine.length(); ++gI)
                {
                    if (inLine[gI] != ' ' || inLine[gI] != '\t' )
                    {
                        cout << "Error: in line " << line_number << " new line expected after \"BEGIN\"" << endl;
                        exit(0);
                    }
                }
                lexList->push_back(Lex{ gBEGIN, line_number, lexemType(BEGIN_), -1 });
                curLex = "";
                break;
            }
            else if (curLex == gVAR)
            {
                 if (i < (inLine.length() - 1) && (inLine[i + 1] != ' ' && inLine[i + 1] != '\t'))
                {
                    cout << "Error: in line " << line_number << " space or tab expected after \"VAR\"" << endl;
                    exit(0);
                }
                lexList->push_back(Lex{ gVAR, line_number, lexemType(VAR_), -1 });
                curLex = "";
                ++i;
            }
            else if (curLex == gEND)
            {
                for (unsigned gI = i+1; gI < inLine.length(); ++gI)
                {
                    if (inLine[gI] != ' ' || inLine[gI] != '\t' )
                    {
                        cout << "Error: in line " << line_number << " new line expected after \"END\"" << endl;
                        exit(0);
                    }
                }
                lexList->push_back(Lex{ gEND, line_number, lexemType(END_), -1 });
                curLex = "";
                break;
            }
            else if (curLex == gSCAN)
            {
                if (i < (inLine.length() - 1) && (inLine[i + 1] != ' ' && inLine[i + 1] != '\t'))
                {
                    cout << "Error: in line " << line_number << " space or tab expected after \"SCAN\"" << endl;
                    exit(0);
                }
                lexList->push_back(Lex{ gSCAN, line_number, lexemType(SCAN_), -1 });
                curLex = "";
                ++i;
            }
            else if (curLex == gPRINT)
            {
                 if (i < (inLine.length() - 1) && (inLine[i + 1] != ' ' && inLine[i + 1] != '\t'))
                {
                    cout << "Error: in line " << line_number << " space or tab expected after \"PRINT\"" << endl;
                    exit(0);
                }
                lexList->push_back(Lex{ gPRINT, line_number, lexemType(PRINT_), -1 });
                curLex = "";
                ++i;
            }
            else if (curLex == gWHILE)
            {
                 if (i < (inLine.length() - 1) && (inLine[i + 1] != ' ' && inLine[i + 1] != '\t'))
                {
                    cout << "Error: in line " << line_number << " space or tab expected after \"WHILE\"" << endl;
                    exit(0);
                }
                lexList->push_back(Lex{ gWHILE, line_number, lexemType(WHILE_), -1 });
                curLex = "";
                ++i;
            }
            else if (curLex == gDO)
            {
                lexList->push_back(Lex{ gDO, line_number, lexemType(DO_), -1 });
                curLex = "";
            }
            else if (curLex == gPLUS)
            {
                lexList->push_back(Lex{ gPLUS, line_number, lexemType(PLUS_), -1 });
                curLex = "";
            }
            else if (curLex == gMINUS)
            {
                if (i < inLine.length() - 1 && inLine[i + 1] == '>')
                {
                    lexList->push_back(Lex{ gASSIGNMENT, line_number, lexemType(ASSIGNMENT_), -1 });
                    curLex = "";
                    ++i;
                }
                else
                {
                    lexList->push_back(Lex{ gMINUS, line_number, lexemType(MINUS_), -1 });
                    curLex = "";
                }
            }
            else if (curLex == gMULT)
            {
                lexList->push_back(Lex{ gMULT, line_number, lexemType(MULT_), -1 });
                curLex = "";
            }
            else if (curLex == gDIV)
            {
                lexList->push_back(Lex{ gDIV, line_number, lexemType(DIV_), -1 });
                curLex = "";
            }
            else if (curLex == gMOD)
            {
                lexList->push_back(Lex{ gMOD, line_number, lexemType(MOD_), -1 });
                curLex = "";
            }
            else if (curLex == gEQUAL)
            {
                lexList->push_back(Lex{ gEQUAL, line_number, lexemType(EQUAL_), -1 });
                curLex = "";
            }
            else if (curLex == "<")
            {
                if (i < inLine.length() - 1 && inLine[i + 1] == '<')
                {
                    lexList->push_back(Lex{ gLESS, line_number, lexemType(LESS_), -1 });
                    curLex = "";
                    ++i;
                }
                else if (i < inLine.length() - 1 && inLine[i + 1] == '>')
                {
                    lexList->push_back(Lex{ gNOT_EQUAL, line_number, lexemType(NOT_EQUAL_), -1 });
                    curLex = "";
                    ++i;
                }
                else
                {
                    cout << "Error: unknown lexem '<' in line " << line_number << endl;
                    return;
                    exit(0);
                }
            }
            else if (curLex == gGRATER)
            {
                lexList->push_back(Lex{ gGRATER, line_number, lexemType(GRATER_), -1 });
                curLex = "";
            }

            else if (curLex == gNOT)
            {
                lexList->push_back(Lex{ gNOT, line_number, lexemType(NOT_), -1 });
                curLex = "";
            }
            else if (curLex == gAND)
            {
                lexList->push_back(Lex{ gAND, line_number, lexemType(AND_), -1 });
                curLex = "";
            }
            else if (curLex == gOR)
            {
                lexList->push_back(Lex{ gOR, line_number, lexemType(OR_), -1 });
                curLex = "";
            }
            else if (curLex == gCOM_ST)
            {
                lexList->push_back(Lex{ gCOM_ST, line_number, lexemType(COM_ST_), -1 });
                curLex = "";
            }
            else if (curLex == gCOM_END)
            {
                lexList->push_back(Lex{ gCOM_END, line_number, lexemType(COM_END_), -1 });
                curLex = "";
            }
            else if (curLex == gBLOCK_START)
            {
                lexList->push_back(Lex{ gBLOCK_START, line_number, lexemType(BLOCK_START_), -1 });
                curLex = "";
            }
            else if (curLex == gBLOCK_END)
            {
                lexList->push_back(Lex{ gBLOCK_END, line_number, lexemType(BLOCK_END_), -1 });
                curLex = "";
            }
            else if (curLex == gSEMICOLON)
            {
                lexList->push_back(Lex{ gSEMICOLON, line_number, lexemType(SEMICOLON_), -1 });
                curLex = "";
            }
            else if (inLine[i] == ' ' || inLine[i] == '\t')
            {
                if (curLex.length() == 1)
                {
                    curLex = "";
                }
                else
                {
                    cout << "Error: unknown lexem " << curLex << " in line " << line_number << endl;
                    return;
                    exit(0);
                }
            }
            else if (regex_match(curLex, rgx_CONST))
            {
                string nextPossibleConst = curLex;
                for (unsigned j = i + 1; j < inLine.length(); ++j)
                {
                    nextPossibleConst += inLine[j];
                    if (regex_match(nextPossibleConst, rgx_CONST))
                    {
                        if (nextPossibleConst.length() >= 10 && (unsigned long long)stoi(nextPossibleConst) > 2147483647)
                        {
                            cout << "Error: constant out of INT16 range in line " << line_number << endl;
                        }
                        curLex = nextPossibleConst;
                    }
                    else
                    {
                        lexList->push_back(Lex{ "CONST", line_number, lexemType(CONST_), stoi(curLex) });
                        curLex = "";
                        i = j - 1;
                        break;
                    }
                }
            }
            else if (regex_match(curLex, rgx_IDENT))
            {
                lexList->push_back(Lex{ curLex, line_number, lexemType(IDENT_), -1 });
                curLex += inLine[i + 1];
                if (regex_match(curLex, rgx_IDENT))
                {
                    cout << "Error: identifier too long in line " << line_number << endl;
                    return;
                    exit(0);
                }
                curLex = "";
            }
            else
            {
                if (!checkLexError(curLex))
                {
                    cout << "Error: unknown token \"" << curLex << "\" in line " << line_number << endl;
                    exit(0);
                }
            }
        }
    }

    for (auto& l : *lexList)
    {
        cout << l.token << endl;
    }
}

bool checkLexError(string& lex)
{
    if (lex.length() >= 3 && lex[0] == 'V')
    {
        return 0;
    }
    else if (lex.length() >= 5 && lex[0] == 'B')
    {
        return 0;
    }
    else if (lex.length() >= 3 && lex[0] == 'E')
    {
        return 0;
    }
    else if (lex.length() >= 4 && lex[0] == 'S')
    {
        return 0;
    }
    else if (lex.length() >= 5 && lex[0] == 'W')
    {
        return 0;
    }
    else if (lex.length() >= 2 && lex[0] == 'D' && lex[1] == 'O')
    {
        return 0;
    }
    else if (lex.length() >= 3 && lex[0] == 'D' && lex[1] == 'I')
    {
        return 0;
    }
    else if (lex.length() >= 3 && lex[0] == 'M')
    {
        return 0;
    }
    else if (lex.length() >= 2 && lex[0] == '=')
    {
        return 0;
    }
    else if (lex.length() >= 2 && lex[0] == '>')
    {
        return 0;
    }
    else if (lex.length() >= 2 && lex[0] == '!')
    {
        return 0;
    }
    else if (lex.length() >= 2 && lex[0] == '&')
    {
        return 0;
    }
    else if (lex.length() >= 2 && lex[0] == '|')
    {
        return 0;
    }
    else if (lex.length() >= 2 && lex[0] == '#')
    {
        return 0;
    }
    else if (lex.length() >= 2 && lex[0] == '*')
    {
        return 0;
    }
    else if (lex.length() >= 7 && lex[0] == 'P' && lex[1] == 'R' && lex[2] == 'O')
    {
        return 0;
    }
    else if (lex.length() >= 5 && lex[0] == 'P' && lex[1] == 'R' && lex[2] == 'I')
    {
        return 0;
    }
    return 1;
}
