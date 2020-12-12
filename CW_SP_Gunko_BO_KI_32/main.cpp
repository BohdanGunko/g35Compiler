#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
using namespace std;

int line_number=0;

#define PROGRAM "PROGRAM"

#define NO_ENTRY_POINT "Expected \"PROGRAM\" in line " + to_string(line_number)
#define NO_BEGIN_POINT "Expected \"BEGIN\" in line " + to_string(line_number)




enum lexemType
{
    PROGRAM_,
    PROGRAM_NAME_,
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
    NULT_,
    DIV_,
    MOD_,
    EQUAL_,
    NOT_EQUAL_,
    GRATER_,
    LESS_,
    NOT_,
    AND_,
    OR_,
    COMMENT_START_, //#*
    COMMENT_END_,   //*#
    BLOCK_START_,   //(
    BLOCK_END_,     //)
    LINE_END_,      //;
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
void findEntryPoint(fstream& inFile);
void findBeginBlock(fstream& inFile);
void findVarDef(fstream& inFile);


int main()
{
    string filePath;
enter_file_name:
    cout << "Please enter(or paste) path to file: "<< endl;
    cin>>filePath;

    //to remove
    filePath ="S:\\LPNU\\Semestr 5\\System programming\\Course work\\cw_project\\testApp.g35";

    if( filePath.length()<4 || filePath.substr(filePath.length()-4, filePath.length()-1) != ".g35")
    {
        cout<<"Error: file must have .g35 extension. Please enter correct file name."<<endl;
        goto enter_file_name;
    }


    fstream inFile;
    inFile.open (filePath, std::fstream::in);
    if(!inFile.is_open())
    {
        cout<<"Can not open file.Please try again."<<endl;
        goto enter_file_name;

    }

    lexList = new vector<Lex>();

    lexicalAnalysis(inFile);


    inFile.close();

    cout<<endl<<"SUCCESS"<<endl;

    delete lexList;
    return 1;
}



void lexicalAnalysis(fstream& inFile)
{
    findEntryPoint(inFile);

    findBeginBlock(inFile);

    findVarDef(inFile);

    string inLine;
    getline(inFile, inLine);
    cout<<inLine<<endl;


}

void findEntryPoint(fstream& inFile)
{
    string inLine;
    regex rgx_EMPTY("[ \t]{0,}");
    regex rgx_PROGRAM("[ \t]{0,}PROGRAM[ \t]{1,}[a-z]{2}[ \t]{0,};[ \t]{0,}");

    while(getline(inFile, inLine))
    {
        ++line_number;
            if(regex_match(inLine, rgx_EMPTY))
            {
                continue;
            }
            else if(regex_match(inLine, rgx_PROGRAM))
            {

                regex rgx_pName("[a-z]{2}");
                smatch pName;
                regex_search(inLine,pName,rgx_pName);
                lexList->push_back(Lex{"",line_number,lexemType(PROGRAM_),-1});
                lexList->push_back(Lex{pName[0],line_number,lexemType(PROGRAM_NAME_),-1});
                lexList->push_back(Lex{"",line_number,lexemType(LINE_END_),-1});
                return;
            }
            else
            {
                cout<<NO_ENTRY_POINT<<endl;
                exit(0);
            }
    }
    exit(0);
}


void findBeginBlock(fstream& inFile)
{

    string inLine;
    regex rgx_EMPTY("[ \t]{0,}");
    regex rgx_BEGIN("[ \t]{0,}BEGIN[ \t]{0,}");

    while(getline(inFile, inLine))
    {
        ++line_number;
            if(regex_match(inLine, rgx_EMPTY))
            {
                continue;
            }
            else if(regex_match(inLine, rgx_BEGIN))
            {
                lexList->push_back(Lex{"",line_number,lexemType(BEGIN_),-1});
                return;
            }
            else
            {
                cout<<NO_BEGIN_POINT<<endl;
                exit(0);
            }
    }
    exit(0);
}

void findVarDef(fstream& inFile)
{
    streampos  prevPos;
    string inLine;
    while(1)
    {
        prevPos = inFile.tellg();
        getline(inFile, inLine);
        regex rgx_EMPTY("[ \t]{0,}");
        regex rgx_Var("[ \t]{0,}VAR[ \t]{1,}[a-z]{2}[ \t]{0,};");
        regex rgx_VarAssign("[ \t]{0,}VAR[ \t]{1,}[a-z]{2}[ \t]{0,}->[ \t]{0,}[-]{0,1}[0-9]{1,}[ \t]{0,};");

        if(regex_match(inLine, rgx_EMPTY))
        {
            continue;
        }
        else if(regex_match(inLine, rgx_Var))
        {
            cout<<"Var"<<endl;
        }
        else if(regex_match(inLine, rgx_VarAssign))
        {
            cout<<"Var assignment"<<line_number<<endl;
        }
        else
        {
            cout<<"No more vars declarations"<<endl;
            inFile.seekg(prevPos);
            return;
        }
    }
}







