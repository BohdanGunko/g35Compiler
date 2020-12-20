#include "codegenerator.h"

codeGenerator::codeGenerator()
{
}

codeGenerator::~codeGenerator()
{
    asmFile.close();
}

void codeGenerator::createFile(string& filePath)

{
    this->filePath = filePath.substr(0, filePath.length() - 4);

    asmFile.open(this->filePath + ".asm", std::fstream::out);

    if (!asmFile.is_open())
    {
        cout << "Can not create obj file." << endl;
        exit(0);
    }

    asmFile << ".586\n"
                         ".model flat ,stdcall\n"
                         "option casemap:none\n\n"
                         "include \\masm32\\include\\kernel32.inc\n"
                         "include \\masm32\\include\\msvcrt.inc\n"
                         "includelib \\masm32\\lib\\msvcrt.lib\n"
                         "\n.data\n"
                         "    tmpLeftOperand dw 0\n"
                         "	tmpRightOperand dw 0\n"
                         "	gPrintFormat db \"%s = %hd\",13,10,0\n"
                         "	gPrintBeforeScan db \"Enter %s = \",0\n"
                         "	gScanFormat db \"%hd\",0\n"
                         "	pressAnyKeyMsg db \"Press any key to exit...\",0\n";
}

void codeGenerator::setProgramName(string& programName)
{
    this->programName = programName;
}

void codeGenerator::declareVar(string& varName)
{
    asmFile << "\t_" << varName << " dw ?\n";
    asmFile << "\t_" << varName << "_gName db \"" << varName << "\",0\n";
}

void codeGenerator::declareVar(string& varName, short varValue)
{
    asmFile << "\t_" << varName << " dw " << to_string(varValue) << "\n";
    asmFile << "\t_" << varName << "_gName db \"" << varName << "\",0\n";
}

void codeGenerator::starCode()
{
    asmFile << "\n.code\n_" + programName + ":\n";
}

void codeGenerator::endCode()
{
    asmFile << "invoke crt_printf, addr pressAnyKeyMsg\n";
    asmFile << "invoke  crt__getch\n";
    asmFile << "ret\n";
    asmFile << "end _" << programName;
    asmFile.close();
}

void codeGenerator::scanCode(string& varName)
{
    asmFile << tabulationStr << "invoke crt_printf, addr gPrintBeforeScan, addr _" << varName << "_gName, _" << varName << "\n";
    asmFile << tabulationStr << "invoke crt_scanf, addr gScanFormat, addr _" << varName << "\n";
}

void codeGenerator::printCode(string& varName)
{
    asmFile << tabulationStr << "invoke crt_printf, addr gPrintFormat, addr _" << varName << "_gName, _" << varName << "\n";
}

void codeGenerator::assignmentCode(string& varName)
{
    asmFile << tabulationStr << "mov _" + varName << ", cx\n";
}

void codeGenerator::regMov(string rightOperand)
{
    asmFile << tabulationStr << "mov bx, " << rightOperand << "\n";
}

void codeGenerator::movCxBx()
{
    asmFile << tabulationStr << "mov cx, bx\n";
}

void codeGenerator::movBxCx()
{
    asmFile << tabulationStr << "mov bx, cx\n";
}

void codeGenerator::regOperator(string& operatorName, string leftOperand)
{
    if (operatorName == "+")
    {
        asmFile << tabulationStr << "add bx, " << leftOperand << "\n";
    }
    else if (operatorName == "-")
    {
        asmFile << tabulationStr << "mov ax, " << leftOperand << "\n";
        asmFile << tabulationStr << "sub ax, bx\n";
        asmFile << tabulationStr << "mov bx, ax\n";
    }
    else if (operatorName == "*")
    {
        asmFile << tabulationStr << "mov ax, " << leftOperand << "\n";
        asmFile << tabulationStr << "imul bx\n";
        asmFile << tabulationStr << "mov bx, ax\n";
    }
    else if (operatorName == "==")
    {
        asmFile << tabulationStr << "cmp bx, " << leftOperand << "\n";
        asmFile << tabulationStr << "sete bl\n";
        asmFile << tabulationStr << "movzx bx, bl\n";
    }
    else if (operatorName == "<>")
    {
        asmFile << tabulationStr << "cmp bx, " << leftOperand << "\n";
        asmFile << tabulationStr << "setne bl\n";
        asmFile << tabulationStr << "movzx bx, bl\n";
    }
    else if (operatorName == ">>")
    {
        asmFile << tabulationStr << "cmp " << leftOperand << ", bx\n";
        asmFile << tabulationStr << "setg bl\n";
        asmFile << tabulationStr << "movzx bx, bl\n";
    }
    else if (operatorName == "<<")
    {
        asmFile << tabulationStr << "cmp " << leftOperand << ", bx\n";
        asmFile << tabulationStr << "setl bl\n";
        asmFile << tabulationStr << "movzx bx, bl\n";
    }
    else if (operatorName == "&&")
    {
        asmFile << tabulationStr << "cmp " << leftOperand << ", 0\n";
        asmFile << tabulationStr << "je _LAND_" + to_string(andOperatorsCounter) + "\n";
        asmFile << tabulationStr << "cmp bx, 0\n";
        asmFile << tabulationStr << "je _LAND_" + to_string(andOperatorsCounter) + "\n";
        asmFile << tabulationStr << "mov bx, 1\n";
        asmFile << tabulationStr << "jmp _LAND_RES_" + to_string(andOperatorsCounter) + "\n";
        asmFile << tabulationStr << "_LAND_" + to_string(andOperatorsCounter) + ":\n";
        asmFile << tabulationStr << "mov bx, 0\n";
        asmFile << tabulationStr << "_LAND_RES_" + to_string(andOperatorsCounter) + ":\n";

        ++andOperatorsCounter;
    }
    else if (operatorName == "||")
    {
        asmFile << tabulationStr << "or bx, " << leftOperand << "\n";
        asmFile << tabulationStr << "cmp bx, 0\n";
        asmFile << tabulationStr << "setne bl\n";
        asmFile << tabulationStr << "movzx bx, bl\n";
    }
    else if (operatorName == "DIV")
    {
        asmFile << tabulationStr << "mov ax, " << leftOperand << "\n";
        asmFile << tabulationStr << "cwd\n";
        asmFile << tabulationStr << "idiv bx\n";
        asmFile << tabulationStr << "mov bx, ax\n";
    }
    else if (operatorName == "MOD")
    {
        asmFile << tabulationStr << "mov ax, " << leftOperand << "\n";
        asmFile << tabulationStr << "cwd\n";
        asmFile << tabulationStr << "div bx\n";
        asmFile << tabulationStr << "mov bx, dx\n";
    }
    else if (operatorName == "!!")
    {
        asmFile << tabulationStr << "cmp bx, 0\n";
        asmFile << tabulationStr << "sete bl\n";
        asmFile << tabulationStr << "movzx bx, bl\n";
    }
    else if (operatorName == "")
    {
    }
    else
    {
        cout << "Unknown operator passed to code generator" << endl;
        exit(0);
    }
}

void codeGenerator::pushCx()
{
    asmFile << tabulationStr << "push cx\n";
}

void codeGenerator::popCx()
{
    asmFile << tabulationStr << "mov cx, [esp]\n";
    asmFile << tabulationStr << "add esp, 2\n";
}
void codeGenerator::whileStart()
{
    ++blocks_count;
    asmFile << tabulationStr << "_while_begin_" + to_string(blocks_count) + ":\n";
    tabulationStr += "\t";
}
void codeGenerator::whileCmp()
{
    asmFile << tabulationStr << "cmp cx, 0\n";
    asmFile << tabulationStr << "je _while_end_" + to_string(blocks_count) + "\n";
}
void codeGenerator::whileEnd()
{
    tabulationStr.resize(tabulationStr.length() - 1);
    asmFile << tabulationStr << "jmp _while_begin_" + to_string(blocks_count) + "\n";
    asmFile << tabulationStr << "_while_end_" + to_string(blocks_count) + ":\n";
    --blocks_count;
}

void codeGenerator::assembleFile()
{
    string ml = "ml /Fo \"" + filePath + ".obj\"" + " /c /Zd /coff \"" + filePath + ".asm\"";
    string link = "link /SUBSYSTEM:CONSOLE  /OUT:\"" + filePath + ".exe\" \"" + filePath + ".obj\"";

    cout << "Assembling.." << endl;
    system(ml.c_str());

    cout << endl << "Linking..." << endl;
    system(link.c_str());
}

void codeGenerator::writeComment(string commentLine)
{
    asmFile << "\n" << tabulationStr << ";" << commentLine << "\n";
}

void codeGenerator::clearFile()
{
    asmFile.close();
    remove((filePath+".asm").c_str());
    remove((filePath+".obj").c_str());
    remove((filePath+".exe").c_str());
}
