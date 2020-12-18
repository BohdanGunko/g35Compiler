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
    asmFile << "invoke crt_printf, addr pressAnyKeyMsg\n"
                         "invoke  crt__getch\n"
                         "ret\n"
                         "end _"
                    << programName;
    asmFile.close();
}

void codeGenerator::scanCode(string& varName)
{
    asmFile << "invoke crt_printf, addr gPrintBeforeScan, addr _" << varName << "_gName, _" << varName << "\n";
    asmFile << "invoke crt_scanf, addr gScanFormat, addr _" << varName << "\n";
}

void codeGenerator::printCode(string& varName)
{
    asmFile << "invoke crt_printf, addr gPrintFormat, addr _" << varName << "_gName, _" << varName << "\n";
}

void codeGenerator::assignmentCode(string& varName)
{
    asmFile << "mov _" + varName << ", cx\n";
}

void codeGenerator::regMov(string rightOperand)
{
    asmFile << "mov bx, " << rightOperand << "\n";
}

void codeGenerator::movCxBx()
{
    asmFile << "mov cx, bx\n";
}

void codeGenerator::movBxCx()
{
    asmFile << "mov bx, cx\n";
}

void codeGenerator::regOperator(string& operatorName, string leftOperand)
{
    if (operatorName == "+")
    {
        asmFile << "add bx, " << leftOperand << "\n";
    }
    else if (operatorName == "-")
    {
        asmFile << "mov ax, " << leftOperand
                        << "\n"
                             "sub ax, bx\n"
                             "mov bx, ax\n";
    }
    else if (operatorName == "*")
    {
        asmFile << "mov ax, " << leftOperand
                        << "\n"
                             "imul bx\n"
                             "mov bx, ax\n";
    }
    else if (operatorName == "==")
    {
        asmFile << "cmp bx, " << leftOperand
                        << "\n"
                             "sete bl\n"
                             "movzx bx, bl\n";
    }
    else if (operatorName == "<>")
    {
        asmFile << "cmp bx, " << leftOperand
                        << "\n"
                             "setne bl\n"
                             "movzx bx, bl\n";
    }
    else if (operatorName == ">>")
    {
        asmFile << "cmp " << leftOperand << ", bx\n"
                        << "setg bl\n"
                             "movzx bx, bl\n";
    }
    else if (operatorName == "<<")
    {
        asmFile << "cmp " << leftOperand << ", bx\n"
                        << "setl bl\n"
                             "movzx bx, bl\n";
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
                                     to_string(andOperatorsCounter) + ":\n";

        ++andOperatorsCounter;
    }
    else if (operatorName == "||")
    {
        asmFile << "or bx, " << leftOperand
                        << "\n"
                             "cmp bx, 0\n"
                             "setne bl\n"
                             "movzx bx, bl\n";
    }
    else if (operatorName == "DIV")
    {
        asmFile << "mov ax, " << leftOperand << "\n"
                        << "cwd\n"
                             "idiv bx\n"
                             "mov bx, ax\n";
    }
    else if (operatorName == "MOD")
    {
        asmFile << "mov ax, " << leftOperand << "\n"
                        << "cwd\n"
                             "div bx\n"
                             "mov bx, dx\n";
    }
    else if (operatorName == "!!")
    {
        asmFile << "cmp bx, 0\n"
                             "sete bl\n"
                             "movzx bx, bl\n";
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

void codeGenerator::pushCx()
{
    asmFile << "push cx\n";
}

void codeGenerator::popCx()
{
    asmFile << "mov cx, [esp]\n"
                         "add esp, 2\n";
}
void codeGenerator::whileStart()
{
    ++blocks_count;
    asmFile << "_while_begin_" + to_string(blocks_count) + ":\n";
}
void codeGenerator::whileCmp()
{
    asmFile << "cmp cx, 0\n"
                         "je _while_end_" +
                                 to_string(blocks_count) + "\n";
}
void codeGenerator::whileEnd()
{
    asmFile << "jmp _while_begin_" + to_string(blocks_count) +
                                 "\n"
                                 "_while_end_" +
                                 to_string(blocks_count) + ":\n";
    --blocks_count;
}

void codeGenerator::assembleFile()
{
    string ml = "      \"P:\\ml /Fo \"P:\\test.obj\" /c /Zd /coff \""+filePath+ " \"";
    string link = "P:\\link /SUBSYSTEM:CONSOLE  /OUT:\"P:\\test.exe\" \"P:\\test.obj\"";
    //string link = "      \"P:\\link /Fo \"P:\\test.exe\" /c /Zd /coff \"P:\\test.obj\"";

    system(ml.c_str());
    system(link.c_str());
}
