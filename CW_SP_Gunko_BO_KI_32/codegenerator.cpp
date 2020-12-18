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
    this->filePath = filePath.substr(0, filePath.length() - 4) + ".asm";

    asmFile.open(this->filePath, std::fstream::out);

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
                         "	gPrintFormat db \"%d\",13,10,0\n"
                         "	gScanFormat db \"%d\",0\n"
                         "	pressAnyKeyMsg db \"Press any key to exit...\",0\n";
}

void codeGenerator::setProgramName(string& programName)
{
    this->programName = programName;
}

void codeGenerator::declareVar(string& varName)
{
    asmFile << "\t_" << varName << " dw ?\n";
}

void codeGenerator::declareVar(string& varName, short varValue)
{
    asmFile << "\t_" << varName << " dw " << to_string(varValue) << "\n";
}

void codeGenerator::starCode()
{
    asmFile << "\n.code\n_" + programName + ":\n";
}

void codeGenerator::endCode()
{
    asmFile << "invoke crt_printf, addr pressAnyKeyMsg\n"
                         "invoke  crt__getch\n"
                         "invoke ExitProcess, 0\n"
                         "end _"
                    << programName;
    asmFile.close();
}

void codeGenerator::scanCode(string& varName)
{
    asmFile << "invoke crt_scanf, addr gScanFormat, addr _" << varName << "\n";
}

void codeGenerator::printCode(string& varName)
{
    asmFile << "invoke crt_printf, addr gPrintFormat, _" << varName << "\n";
}

void codeGenerator::assignmentCode(string& varName)
{
    asmFile << "mov _" + varName << ", cx\n";
}

void codeGenerator::pushIdent(string& varName)
{
    asmFile << "push _" + varName + "\n";
}

void codeGenerator::pushConst(string& constName)
{
    asmFile << "push word ptr " + constName + "\n";
}

void codeGenerator::regMov(string rightOperand)
{
    asmFile << "mov bx, " << rightOperand << "\n";
}

void codeGenerator::movCx()
{
    asmFile << "mov cx, bx\n";
}

void codeGenerator::regOperator(string& operatorName, string leftOperand)
{
    if (operatorName == "+")
    {
        asmFile << "add bx, " << leftOperand << "\n\n";
    }
    else if (operatorName == "-")
    {
        asmFile << "mov ax, " << leftOperand
                        << "\n"
                             "sub ax, bx\n"
                             "mov bx, ax\n\n";
    }
    else if (operatorName == "*")
    {
        asmFile << "mov ax, " << leftOperand
                        << "\n"
                             "imul bx\n"
                             "mov bx, ax\n\n";
    }
    else if (operatorName == "==")
    {
        asmFile << "cmp bx, " << leftOperand
                        << "\n"
                             "sete bl\n"
                             "movzx bx, bl\n"
                             "\n";
    }
    else if (operatorName == "<>")
    {
        asmFile << "cmp bx, " << leftOperand
                        << "\n"
                             "setne bl\n"
                             "movzx bx, bl\n"
                             "\n";
    }
    else if (operatorName == ">>")
    {
        asmFile << "cmp " << leftOperand << ", bx\n"
                        << "\n"
                             "setg bl\n"
                             "movzx bx, bl\n"
                             "\n";
    }
    else if (operatorName == "<<")
    {
        asmFile << "cmp " << leftOperand << ", bx\n"
                        << "\n"
                             "setl bl\n"
                             "movzx bx, bl\n"
                             "\n";
    }
    else if (operatorName == "&&")
    {
        asmFile << "cmp " << leftOperand
                        << ", 0\n"

                             "je _LAND_" +
                                     to_string(andOperatorsCounter) +
                                     "\n"
                                     "cmp bx, 0\n"
                                     "je _LAND_" +
                                     to_string(andOperatorsCounter) +
                                     "\n"
                                     "mov bx, 1\n"
                                     "jmp _LAND_RES_" +
                                     to_string(andOperatorsCounter) +
                                     "\n"
                                     "_LAND_" +
                                     to_string(andOperatorsCounter) +
                                     ":\n"
                                     "mov bx, 0\n"
                                     "_LAND_RES_" +
                                     to_string(andOperatorsCounter) + ":\n\n";

        ++andOperatorsCounter;
    }
    else if (operatorName == "||")
    {
        asmFile << "or bx, " << leftOperand
                        << "\n"
                             "cmp bx, 0\n"
                             "setne bl\n"
                             "movzx bx, bl\n\n";
    }
    else if (operatorName == "DIV")
    {
        asmFile << "mov ax, " << leftOperand << "\n"
                        << "cwd\n"
                             "idiv bx\n"
                             "mov bx, ax\n\n";
    }
    else if (operatorName == "MOD")
    {
        asmFile << "mov ax, " << leftOperand << "\n"
                        << "cwd\n"
                             "div bx\n"
                             "mov bx, dx\n\n";
    }
    else if (operatorName == "!!")
    {
        asmFile << "cmp bx, 0\n"
                             "sete bl\n"
                             "movzx bx, bl\n\n";
    }
    else if (operatorName == "")
    {
    }
    else
    {
        cout << "BAD OPERATOR" << endl;
        exit(0);
    }
}
