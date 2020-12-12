#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void lexicalAnalysis(fstream& inFile);

int main()
{


    string filePath;
enter_file_name:
    cout << "Please enter(or paste) path to file: "<< endl;
    cin>>filePath;
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

    lexicalAnalysis(inFile);


    inFile.close();


    return 0;
}



void lexicalAnalysis(fstream& inFile)
{


}
