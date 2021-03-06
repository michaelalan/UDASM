//
//  DASM.cpp
//  UDASM
//
//  Created by Uranux on 14-4-29.
//  Copyright (c) 2014年 Uranux. All rights reserved.
//

#include "DASM.h"

DASM::DASM(BYTE Arch, BYTE CPUMode, BYTE AddressMode, BYTE ByteOrder, string InputFileName, string OutputAddrFileName, string OutputBinFileName, string OutputAsmFileName, DWORD ImageLoadAddress, DWORD TextSectionRawDataAddress, DWORD TextSectionRawDataLen, DWORD TextSectionLoadAddress)
{
    this->BaseInit(Arch, CPUMode, AddressMode, ByteOrder);
    this->FileInit(InputFileName, OutputAddrFileName, OutputBinFileName, OutputAsmFileName, ImageLoadAddress, TextSectionRawDataAddress, TextSectionRawDataLen, TextSectionLoadAddress);
}

DASM::~DASM()
{
    
}

void DASM::Disassemble()
{
    switch (this->GetArch()) {
        case INTEL_X86:
            this->INTEL_X86_Init();
            this->INTEL_X86_Exec();
            break;
        case AT_AND_T:
            this->AT_AND_T_Init();
            this->AT_AND_T_Exec();
            break;
        default:
            break;
    }
}

void DASM::FileInit(string InputFileName, string OutputAddrFileName, string OutputBinFileName, string OutputAsmFileName, DWORD ImageLoadAddress, DWORD TextSectionRawDataAddress, DWORD TextSectionRawDataLen, DWORD TextSectionLoadAddress)
{
    this->GetInputFile() = ifstream(InputFileName);
    this->GetOutputAddrFile() = ofstream(OutputAddrFileName);
    this->GetOutputBinFile() = ofstream(OutputBinFileName);
    this->GetOutputAsmFile() = ofstream(OutputAsmFileName);
    this->SetImageLoadAddress(ImageLoadAddress);
    this->SetTextSectionRawDataAddress(TextSectionRawDataAddress);
    this->SetTextSectionRawDataLen(TextSectionRawDataLen);
    this->SetTextSectionLoadAddress(TextSectionLoadAddress);
    this->SetCntAddress(ImageLoadAddress + TextSectionLoadAddress);
    this->GetAsmStream() = stringstream();
    this->GetBinStream() = stringstream();
}

void DASM::BaseInit(BYTE Arch, BYTE DefaultCPUMode, BYTE DefaultAddressMode, BYTE DefaultByteOrder)
{
    this->SetArch(Arch);
    this->SetDefaultCPUMode(DefaultCPUMode);
    this->SetDefaultAddressMode(DefaultAddressMode);
    this->SetDefaultByteOrder(DefaultByteOrder);
    this->ResetMode();
}

void DASM::ResetMode()
{
    this->SetCPUMode(this->GetDefaultCPUMode());
    this->SetAddressMode(this->GetDefaultAddressMode());
    this->SetDefaultSegmentRegister(REG_NULL);
}

void DASM::INTEL_X86_Init()
{
    for (int i = 0; i < 4; i++)
    {
        Intelx86Def::GetPrefix(i) = Intelx86Def::InitPrefix(i);
    }
    Intelx86Def::GetSinglePrefixName() = Intelx86Def::InitSinglePrefixName();
    Intelx86Def::GetOpcode() = Intelx86Def::InitOpcode();
    Intelx86Def::GetSecondOpcode() = Intelx86Def::InitSecondOpcode();
    Intelx86Def::GetSubOpcode_TestGroup() = Intelx86Def::InitSubOpcode_TestGroup();
    Intelx86Def::GetSubOpcode_AddGroup() = Intelx86Def::InitSubOpcode_AddGroup();
    Intelx86Def::GetSubOpcode_RolGroup() = Intelx86Def::InitSubOpcode_RolGroup();
    Intelx86Def::GetSubOpcode_FaddGroup() = Intelx86Def::InitSubOpcode_FaddGroup();
    Intelx86Def::GetSubOpcode_FldGroup() = Intelx86Def::InitSubOpcode_FldGroup();
    Intelx86Def::GetSubOpcode_Fld64Group() = Intelx86Def::InitSubOpcode_Fld64Group();
    Intelx86Def::GetSubOpcode_FiaddGroup() = Intelx86Def::InitSubOpcode_FiaddGroup();
    Intelx86Def::GetSubOpcode_FildGroup() = Intelx86Def::InitSubOpcode_FildGroup();
    Intelx86Def::GetSubOpcode_Fild16Group() = Intelx86Def::InitSubOpcode_Fild16Group();
    Intelx86Def::GetSubOpcode_Inc8Group() = Intelx86Def::InitSubOpcode_Inc8Group();
    Intelx86Def::GetSubOpcode_Inc16_32Group() = Intelx86Def::InitSubOpcode_Inc16_32Group();
    Intelx86Def::GetReg(REGSIZE_8) = Intelx86Def::InitReg(REGSIZE_8);
    Intelx86Def::GetReg(REGSIZE_16) = Intelx86Def::InitReg(REGSIZE_16);
    Intelx86Def::GetReg(REGSIZE_32) = Intelx86Def::InitReg(REGSIZE_32);
    Intelx86Def::GetReg(REGSIZE_64) = Intelx86Def::InitReg(REGSIZE_64);
    Intelx86Def::GetReg(ADDRESSSIZE_16) = Intelx86Def::InitReg(ADDRESSSIZE_16);
    Intelx86Def::GetReg(SEGMENT_REG) = Intelx86Def::InitReg(SEGMENT_REG);
    Intelx86Def::GetReg(FLOAT_REG) = Intelx86Def::InitReg(FLOAT_REG);
    Intelx86Def::GetAddressHead() = Intelx86Def::InitAddressHead();
}

void DASM::AT_AND_T_Init()
{
    
}

string DASM::FormatAddress(DWORD Address)
{
    stringstream Formater;
    string RetVal = "";
    Formater << setiosflags(ios::uppercase) << setw(8) << setfill('0') << hex << Address;
    Formater >> RetVal;
    return RetVal;
}

void DASM::OutputLineAndReset()
{
    this->GetOutputAddrFile().write((FormatAddress(this->GetCntAddress()) + "\n").c_str(), 9);
    this->SetCntAddress(this->GetImageLoadAddress() + (DWORD)(this->GetInputFile().tellg()) - this->GetTextSectionRawDataAddress() + this->GetTextSectionLoadAddress());
    string BinByte, AsmVal;
    while (this->GetBinStream() >> BinByte)
    {
        this->GetOutputBinFile().write((BinByte + " ").c_str(), BinByte.length() + 1);
    }
    this->GetOutputBinFile().write("\n", 1);
    while (this->GetAsmStream() >> AsmVal)
    {
        this->GetOutputAsmFile().write((AsmVal + " ").c_str(), AsmVal.length() + 1);
    }
    this->GetOutputAsmFile().write("\n", 1);
    this->StringStreamClear(this->GetAsmStream());
    this->StringStreamClear(this->GetBinStream());
    this->ResetMode();
}

void DASM::INTEL_X86_Exec()
{
    this->GetInputFile().seekg(this->GetTextSectionRawDataAddress());
    Instruction *CntInstruction = new Instruction();
    long long InstructionStartPos = this->GetInputFile().tellg();
    long long PrefixPos[4];
    memset(PrefixPos, -1, sizeof(PrefixPos));
    BYTE PrefixConflicting = NO_PREFIX_CONFLICTION;
    while (this->GetInputFile())
    {
        if (this->GetCntAddress() >= this->GetImageLoadAddress() + this->GetTextSectionLoadAddress() + this->GetTextSectionRawDataLen())
            break;
        BYTE CntByte = this->ReadByte();
        string BinByte;
        string AsmVal;
        if (PrefixConflicting != NO_PREFIX_CONFLICTION)
        {
            string Val = Intelx86Def::GetSinglePrefixName()[CntByte];
            this->GetAsmStream() << Val + " ";
            this->OutputLineAndReset();
            if (this->GetInputFile().tellg() == PrefixPos[PrefixConflicting])
            {
                InstructionStartPos = this->GetInputFile().tellg();
                PrefixConflicting = NO_PREFIX_CONFLICTION;
            }
            continue;
        }

        // 判断指令前缀
        // 第一组
        if (Intelx86Def::GetPrefix(0).find(CntByte) != Intelx86Def::GetPrefix(0).end())
        {
            if (CntInstruction->GetRepeatPrefix() != NULL_VAL)
            {
                this->GetInputFile().seekg(InstructionStartPos);
                PrefixConflicting = 0;
                CntInstruction->Clear();
                this->StringStreamClear(this->GetAsmStream());
                this->StringStreamClear(this->GetBinStream());
                continue;
            }
            string Val = Intelx86Def::GetPrefix(0)[CntByte];
            this->GetAsmStream() << Val + " ";
            switch (CntByte) {
                case REPEAT_PREFIX_LOCK:
                    break;
                case REPEAT_PREFIX_REP: // REPEAT_PREFIX_REPE, REPEAT_PREFIX_REPZ
                    break;
                case REPEAT_PREFIX_REPNE: // REPEAT_PREFIX_REPNZ
                    break;
                default:
                    break;
            }
            CntInstruction->SetRepeatPrefix(CntByte);
            PrefixPos[0] = this->GetInputFile().tellg();
            continue;
        }
        // 第二组
        if (Intelx86Def::GetPrefix(1).find(CntByte) != Intelx86Def::GetPrefix(1).end())
        {
            if (CntInstruction->GetSegmentPrefix() != NULL_VAL)
            {
                this->GetInputFile().seekg(InstructionStartPos);
                PrefixConflicting = 1;
                CntInstruction->Clear();
                this->StringStreamClear(this->GetAsmStream());
                this->StringStreamClear(this->GetBinStream());
                continue;
            }
            switch (CntByte) {
                case SEGMENT_PREFIX_ES:
                    this->SetDefaultSegmentRegister(SEGMENT_PREFIX_ES);
                    break;
                case SEGMENT_PREFIX_CS:
                    this->SetDefaultSegmentRegister(SEGMENT_PREFIX_CS);
                    break;
                case SEGMENT_PREFIX_SS:
                    this->SetDefaultSegmentRegister(SEGMENT_PREFIX_SS);
                    break;
                case SEGMENT_PREFIX_DS:
                    this->SetDefaultSegmentRegister(SEGMENT_PREFIX_DS);
                    break;
                case SEGMENT_PREFIX_FS:
                    this->SetDefaultSegmentRegister(SEGMENT_PREFIX_FS);
                    break;
                case SEGMENT_PREFIX_GS:
                    this->SetDefaultSegmentRegister(SEGMENT_PREFIX_GS);
                    break;
                default:
                    break;
            }
            CntInstruction->SetSegmentPrefix(CntByte);
            PrefixPos[1] = this->GetInputFile().tellg();
            continue;
        }
        // 第三组
        if (Intelx86Def::GetPrefix(2).find(CntByte) != Intelx86Def::GetPrefix(2).end())
        {
            if (CntInstruction->GetOperandPrefix() != NULL_VAL)
            {
                this->GetInputFile().seekg(InstructionStartPos);
                PrefixConflicting = 2;
                CntInstruction->Clear();
                this->StringStreamClear(this->GetAsmStream());
                this->StringStreamClear(this->GetBinStream());
                continue;
            }
            switch (CntByte) {
                case OPERAND_PREFIX:
                    switch (this->GetCPUMode()) {
                        case CPU_MODE_32:
                            this->SetCPUMode(CPU_MODE_16);
                            break;
                        case CPU_MODE_16:
                            this->SetCPUMode(CPU_MODE_32);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            CntInstruction->SetOperandPrefix(CntByte);
            PrefixPos[2] = this->GetInputFile().tellg();
            continue;
        }
        // 第四组
        if (Intelx86Def::GetPrefix(3).find(CntByte) != Intelx86Def::GetPrefix(3).end())
        {
            if (CntInstruction->GetAddressPrefix() != NULL_VAL)
            {
                this->GetInputFile().seekg(InstructionStartPos);
                PrefixConflicting = 3;
                CntInstruction->Clear();
                this->StringStreamClear(this->GetAsmStream());
                this->StringStreamClear(this->GetBinStream());
                continue;
            }
            switch (CntByte) {
                case ADDRESS_PREFIX:
                    switch (this->GetAddressMode()) {
                        case CPU_MODE_32:
                            this->SetAddressMode(ADDRESS_MODE_16);
                            break;
                        case CPU_MODE_16:
                            this->SetAddressMode(ADDRESS_MODE_32);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            CntInstruction->SetAddressPrefix(CntByte);
            PrefixPos[3] = this->GetInputFile().tellg();
            continue;
        }
        BYTE MODRM;
        BYTE I8;
        WORD I16;
        DWORD I32;
        DWORD TargetAddress;
        string Operand[2];
        switch (CntByte) {
            case OPCODE_ADD + RM8_R8:
            case OPCODE_ADD + RM16_R16:
            case OPCODE_ADD + R8_RM8:
            case OPCODE_ADD + R16_RM16:
            case OPCODE_ADD + AL_I8:
            case OPCODE_ADD + RAX_I16:
            case OPCODE_OR + RM8_R8:
            case OPCODE_OR + RM16_R16:
            case OPCODE_OR + R8_RM8:
            case OPCODE_OR + R16_RM16:
            case OPCODE_OR + AL_I8:
            case OPCODE_OR + RAX_I16:
            case OPCODE_ADC + RM8_R8:
            case OPCODE_ADC + RM16_R16:
            case OPCODE_ADC + R8_RM8:
            case OPCODE_ADC + R16_RM16:
            case OPCODE_ADC + AL_I8:
            case OPCODE_ADC + RAX_I16:
            case OPCODE_SBB + RM8_R8:
            case OPCODE_SBB + RM16_R16:
            case OPCODE_SBB + R8_RM8:
            case OPCODE_SBB + R16_RM16:
            case OPCODE_SBB + AL_I8:
            case OPCODE_SBB + RAX_I16:
            case OPCODE_AND + RM8_R8:
            case OPCODE_AND + RM16_R16:
            case OPCODE_AND + R8_RM8:
            case OPCODE_AND + R16_RM16:
            case OPCODE_AND + AL_I8:
            case OPCODE_AND + RAX_I16:
            case OPCODE_SUB + RM8_R8:
            case OPCODE_SUB + RM16_R16:
            case OPCODE_SUB + R8_RM8:
            case OPCODE_SUB + R16_RM16:
            case OPCODE_SUB + AL_I8:
            case OPCODE_SUB + RAX_I16:
            case OPCODE_XOR + RM8_R8:
            case OPCODE_XOR + RM16_R16:
            case OPCODE_XOR + R8_RM8:
            case OPCODE_XOR + R16_RM16:
            case OPCODE_XOR + AL_I8:
            case OPCODE_XOR + RAX_I16:
            case OPCODE_CMP + RM8_R8:
            case OPCODE_CMP + RM16_R16:
            case OPCODE_CMP + R8_RM8:
            case OPCODE_CMP + R16_RM16:
            case OPCODE_CMP + AL_I8:
            case OPCODE_CMP + RAX_I16:
            case OPCODE_MOV + RM8_R8:
            case OPCODE_MOV + RM16_R16:
            case OPCODE_MOV + R8_RM8:
            case OPCODE_MOV + R16_RM16:
            case OPCODE_MOV + AL_I8:
            case OPCODE_MOV + SEGR_RM:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte - GET_RM(CntByte)] << " ";
                switch (GET_RM(CntByte)) {
                    case RM8_R8:
                    case RM16_R16:
                    case R8_RM8:
                    case R16_RM16:
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << this->ParseMODRM_RM_R(CntByte, MODRM);
                        break;
                    case AL_I8:
                    case RAX_I16:
                        this->GetAsmStream() << this->ParseRegisterOperand(CntByte, REG_EAX_32) << ", ";
                        this->GetAsmStream() << this->ParseImmediateOperand(CntByte);
                        break;
                    case SEGR_RM:
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << Intelx86Def::GetReg(SEGMENT_REG)[GET_REG(MODRM)] << ", ";
                        if (GET_MOD(MODRM) == MOD_R_NO_DISPLACEMENT)
                        {
                            this->GetAsmStream() << Intelx86Def::GetReg(REGSIZE_16)[GET_RM(MODRM)];
                        }
                        else
                        {
                            this->GetAsmStream() << this->ParseMemoryOperand(CntByte, MODRM, OPSIZE_16);
                        }
                        break;
                    default:
                        this->GetAsmStream() << "??? ";
                        break;
                }
                break;
            case OPCODE_INC + REG_EAX_32:
            case OPCODE_INC + REG_ECX_32:
            case OPCODE_INC + REG_EDX_32:
            case OPCODE_INC + REG_EBX_32:
            case OPCODE_INC + REG_ESP_32:
            case OPCODE_INC + REG_EBP_32:
            case OPCODE_INC + REG_ESI_32:
            case OPCODE_INC + REG_EDI_32:
            case OPCODE_DEC + REG_EAX_32:
            case OPCODE_DEC + REG_ECX_32:
            case OPCODE_DEC + REG_EDX_32:
            case OPCODE_DEC + REG_EBX_32:
            case OPCODE_DEC + REG_ESP_32:
            case OPCODE_DEC + REG_EBP_32:
            case OPCODE_DEC + REG_ESI_32:
            case OPCODE_DEC + REG_EDI_32:
            case OPCODE_PUSH + REG_EAX_32:
            case OPCODE_PUSH + REG_ECX_32:
            case OPCODE_PUSH + REG_EDX_32:
            case OPCODE_PUSH + REG_EBX_32:
            case OPCODE_PUSH + REG_ESP_32:
            case OPCODE_PUSH + REG_EBP_32:
            case OPCODE_PUSH + REG_ESI_32:
            case OPCODE_PUSH + REG_EDI_32:
            case OPCODE_POP + REG_EAX_32:
            case OPCODE_POP + REG_ECX_32:
            case OPCODE_POP + REG_EDX_32:
            case OPCODE_POP + REG_EBX_32:
            case OPCODE_POP + REG_ESP_32:
            case OPCODE_POP + REG_EBP_32:
            case OPCODE_POP + REG_ESI_32:
            case OPCODE_POP + REG_EDI_32:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte - GET_RM(CntByte)] << " ";
                this->GetAsmStream() << Intelx86Def::GetReg(this->GetCPUMode())[GET_RM(CntByte)];
                break;
            case OPCODE_PUSH_ES:
            case OPCODE_POP_ES:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << Intelx86Def::GetReg(SEGMENT_REG)[REG_ES];
                break;
            case OPCODE_PUSH_CS:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << Intelx86Def::GetReg(SEGMENT_REG)[REG_CS];
                break;
            case OPCODE_PUSH_SS:
            case OPCODE_POP_SS:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << Intelx86Def::GetReg(SEGMENT_REG)[REG_SS];
                break;
            case OPCODE_PUSH_DS:
            case OPCODE_POP_DS:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << Intelx86Def::GetReg(SEGMENT_REG)[REG_DS];
                break;
            case OPCODE_PUSH_I16_I32:
            case OPCODE_PUSH_I8:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << (GET_7TH_BIT(CntByte) == 0x00 ? this->ParseImmediateOperand_I16_I32() : this->ParseImmediateOperand_I8(SIGNED));
                break;
            case OPCODE_TEST_GROUP + RM8_R8:
            case OPCODE_TEST_GROUP + RM16_R16:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_TestGroup()[GET_OPCODE2(MODRM)] << " ";
                switch (GET_OPCODE2(MODRM)) {
                    case SUB_OPCODE_TEST_0:
                    case SUB_OPCODE_TEST_1:
                        this->GetAsmStream() << this->ParseMODRM_RM_IMM(CntByte, MODRM);
                        break;
                    default:
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM);
                        break;
                }
                break;
            case OPCODE_IMUL_I16_I32_3OP:
            case OPCODE_IMUL_I8_3OP:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << this->ParseMODRM_R_RM_IMM(CntByte, MODRM);
                break;
            case OPCODE_INS + RM8_R8:
            case OPCODE_INS + RM16_R16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte - GET_W_BIT(CntByte)] << " ";
                this->GetAsmStream() << this->ParseMemoryOperand(CntByte, REG_EDI_32) << ", " << this->ParseRegisterOperand(CntByte, REG_DX_16, OPSIZE_16);
                break;
            case OPCODE_OUTS + RM8_R8:
            case OPCODE_OUTS + RM16_R16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte - GET_W_BIT(CntByte)] << " ";
                this->GetAsmStream() << this->ParseRegisterOperand(CntByte, REG_DX_16, OPSIZE_16) << ", " << this->ParseMemoryOperand(CntByte, REG_ESI_32);
                break;
            case OPCODE_JO:
            case OPCODE_JNO:
            case OPCODE_JB:
            case OPCODE_JNB:
            case OPCODE_JE:
            case OPCODE_JNE:
            case OPCODE_JBE:
            case OPCODE_JA:
            case OPCODE_JS:
            case OPCODE_JNS:
            case OPCODE_JPE:
            case OPCODE_JPO:
            case OPCODE_JL:
            case OPCODE_JGE:
            case OPCODE_JLE:
            case OPCODE_JG:
            case OPCODE_LOOPNZ:
            case OPCODE_LOOPZ:
            case OPCODE_LOOP:
            case OPCODE_JECXZ:
            case OPCODE_JMP:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << "SHORT ";
                I8 = this->ReadByte();
                TargetAddress = CntAddress + (char)I8 + (DWORD)(this->GetInputFile().tellg() - InstructionStartPos);
                this->GetCPUMode() == CPU_MODE_16 ? TargetAddress &= 0x0000FFFF : 0;
                this->GetAsmStream() << setiosflags(ios::uppercase) << setw(this->GetCPUMode() == CPU_MODE_16 ? 4 : 8) << setfill('0') << hex << TargetAddress << "H";
                break;
            case OPCODE_ADD_GROUP:
            case OPCODE_ADD_GROUP + 0x01:
            case OPCODE_ADD_GROUP + 0x02:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_AddGroup()[GET_OPCODE2(MODRM)] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM_IMM(CntByte, MODRM);
                break;
            case OPCODE_ADD_GROUP + 0x03:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_AddGroup()[GET_OPCODE2(MODRM)] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM) << ", ";
                switch (GET_OPCODE2(MODRM)) {
                    case SUB_OPCODE_OR:
                    case SUB_OPCODE_AND:
                    case SUB_OPCODE_XOR:
                        this->GetAsmStream() << this->ParseImmediateOperand_I8(EXTEND_I16_I32);
                        break;
                    default:
                        this->GetAsmStream() << this->ParseImmediateOperand_I8(SIGNED);
                        break;
                }
                break;
            case OPCODE_TEST + RM8_R8:
            case OPCODE_TEST + RM16_R16:
            case OPCODE_XCHG + RM8_R8:
            case OPCODE_XCHG + RM16_R16:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte & 0xFE] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM) << ", " << this->ParseRegisterOperand(CntByte, GET_REG(MODRM));
                break;
            case OPCODE_XCHG_RAX_R:
            case OPCODE_XCHG_RAX_R + 0x01:
            case OPCODE_XCHG_RAX_R + 0x02:
            case OPCODE_XCHG_RAX_R + 0x03:
            case OPCODE_XCHG_RAX_R + 0x04:
            case OPCODE_XCHG_RAX_R + 0x05:
            case OPCODE_XCHG_RAX_R + 0x06:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[OPCODE_XCHG_RAX_R] << " ";
                this->GetAsmStream() << Intelx86Def::GetReg(this->GetCPUMode())[REG_EAX_32] << ", " << Intelx86Def::GetReg(this->GetCPUMode())[GET_RM(CntByte)];
                break;
            case OPCODE_LEA:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                MODRM = this->ReadByte();
                this->GetAsmStream() << this->ParseRegisterOperand(CntByte, GET_REG(MODRM)) << ", " << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_NULL);
                break;
            case OPCODE_POP_RM16_RM32:
                MODRM = this->ReadByte();
                switch (GET_OPCODE2(MODRM)) {
                    case 0x00:
                        this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM);
                        break;
                    default:
                        this->GetAsmStream() << "???";
                        this->GetInputFile().seekg(this->GetInputFile().tellg() - 1LL);
                        break;
                }
                break;
            case OPCODE_CALL_FAR:
            case OPCODE_JMP_FAR:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << "FAR ";
                I32 = this->ReadDWord();
                I16 = this->ReadWord();
                this->GetAsmStream() << this->FormatAsmWord(I16) << "H:" << this->FormatAsmDWord(I32) << "H";
                break;
            case OPCODE_MOV_RAX_M_ONLY_DISP:
            case OPCODE_MOV_RAX_M_ONLY_DISP + 0x01:
            case OPCODE_MOV_RAX_M_ONLY_DISP + 0x02:
            case OPCODE_MOV_RAX_M_ONLY_DISP + 0x03:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[OPCODE_MOV_RAX_M_ONLY_DISP] << " ";
                Operand[0] = this->ParseMemoryOperand(CntByte, this->GetAddressMode() == ADDRESSHEAD_32 ? 0x05 : 0x06);
                Operand[1] = this->ParseRegisterOperand(CntByte, REG_EAX_32);
                this->GetAsmStream() << (GET_D_BIT(CntByte) ? (Operand[0] + ", " + Operand[1]) : (Operand[1] + ", " + Operand[0]));
                break;
            case OPCODE_MOVS:
            case OPCODE_MOVS + 0x01:
            case OPCODE_CMPS:
            case OPCODE_CMPS + 0x01:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte & 0xFE] << " ";
                Operand[0] = this->ParseMemoryOperand(CntByte, REG_ESI_32);
                Operand[1] = this->ParseMemoryOperand(CntByte, REG_EDI_32);
                this->GetAsmStream() << (GET_D_BIT(CntByte) ? (Operand[0] + ", " + Operand[1]) : (Operand[1] + ", " + Operand[0]));
                break;
            case OPCODE_TEST_RAX_IMM + RM8_R8:
            case OPCODE_TEST_RAX_IMM + RM16_R16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[OPCODE_TEST_RAX_IMM] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM_IMM(CntByte, 0xC0);
                break;
            case OPCODE_STOS + RM8_R8:
            case OPCODE_STOS + RM16_R16:
            case OPCODE_SCAS + RM8_R8:
            case OPCODE_SCAS + RM16_R16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte & 0xFE] << " ";
                this->GetAsmStream() << this->ParseMemoryOperand(CntByte, REG_EDI_32);
                break;
            case OPCODE_LODS + RM8_R8:
            case OPCODE_LODS + RM16_R16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte & 0xFE] << " ";
                this->GetAsmStream() << this->ParseMemoryOperand(CntByte, REG_ESI_32);
                break;
            case OPCODE_MOV_R_IMM + REG_EAX_32:
            case OPCODE_MOV_R_IMM + REG_ECX_32:
            case OPCODE_MOV_R_IMM + REG_EDX_32:
            case OPCODE_MOV_R_IMM + REG_EBX_32:
            case OPCODE_MOV_R_IMM + REG_ESP_32:
            case OPCODE_MOV_R_IMM + REG_EBP_32:
            case OPCODE_MOV_R_IMM + REG_ESI_32:
            case OPCODE_MOV_R_IMM + REG_EDI_32:
            case OPCODE_MOV_R_IMM + REG_EAX_32 + 0x08:
            case OPCODE_MOV_R_IMM + REG_ECX_32 + 0x08:
            case OPCODE_MOV_R_IMM + REG_EDX_32 + 0x08:
            case OPCODE_MOV_R_IMM + REG_EBX_32 + 0x08:
            case OPCODE_MOV_R_IMM + REG_ESP_32 + 0x08:
            case OPCODE_MOV_R_IMM + REG_EBP_32 + 0x08:
            case OPCODE_MOV_R_IMM + REG_ESI_32 + 0x08:
            case OPCODE_MOV_R_IMM + REG_EDI_32 + 0x08:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[OPCODE_MOV_R_IMM] << " ";
                this->GetAsmStream() << (GET_5TH_BIT(CntByte) ? Intelx86Def::GetReg(this->GetCPUMode())[GET_RM(CntByte)] + ", " + this->ParseImmediateOperand_I16_I32() : Intelx86Def::GetReg(REGSIZE_8)[GET_RM(CntByte)] + ", " + this->ParseImmediateOperand_I8());
                break;
            case OPCODE_ROL_GROUP + RM8_R8:
            case OPCODE_ROL_GROUP + RM16_R16:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_RolGroup()[GET_OPCODE2(MODRM)] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM) << ", " << this->ParseImmediateOperand_I8();
                break;
            case OPCODE_ROL_GROUP_2 + RM8_R8:
            case OPCODE_ROL_GROUP_2 + RM16_R16:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_RolGroup()[GET_OPCODE2(MODRM)] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM) << ", 01H";
                break;
            case OPCODE_ROL_GROUP_3 + RM8_R8:
            case OPCODE_ROL_GROUP_3 + RM16_R16:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_RolGroup()[GET_OPCODE2(MODRM)] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM) << ", " << Intelx86Def::GetReg(REGSIZE_8)[REG_CL_8];
                break;
            case OPCODE_RETN_I16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << this->ParseImmediateOperand_I16();
                break;
            case OPCODE_LES:
            case OPCODE_LDS:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                MODRM = this->ReadByte();
                if (this->GetCPUMode() == CPU_MODE_16)
                {
                    this->GetAsmStream() << Intelx86Def::GetReg(REGSIZE_16)[GET_REG(MODRM)] << ", " << this->ParseMemoryOperand(CntByte, MODRM, OPSIZE_32);
                }
                else if (this->GetCPUMode() == CPU_MODE_32)
                {
                    this->GetAsmStream() << Intelx86Def::GetReg(REGSIZE_32)[GET_REG(MODRM)] << ", " << this->ParseMemoryOperand(CntByte, MODRM, OPSIZE_F);
                }
                break;
            case OPCODE_MOV_RM_IMM + RM8_R8:
            case OPCODE_MOV_RM_IMM + RM16_R16:
                MODRM = this->ReadByte();
                switch (GET_OPCODE2(MODRM)) {
                    case 0x00:
                        this->GetAsmStream() << Intelx86Def::GetOpcode()[OPCODE_MOV_RM_IMM] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM_IMM(CntByte, MODRM);
                        break;
                    default:
                        this->GetAsmStream() << "???";
                        this->GetInputFile().seekg(this->GetInputFile().tellg() - 1LL);
                        this->StringStreamClear(this->GetBinStream());
                        this->GetBinStream() << this->FormatByte(CntByte) << " ";
                        break;
                }
                break;
            case OPCODE_ENTER:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << this->ParseImmediateOperand_I16() << ", " << this->ParseImmediateOperand_I8();
                break;
            case OPCODE_RETF_I16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << this->ParseImmediateOperand_I16();
                break;
            case OPCODE_INT:
            case OPCODE_AAM:
            case OPCODE_AAD:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << this->ParseImmediateOperand_I8();
                break;
            case OPCODE_XLAT:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                this->GetAsmStream() << Intelx86Def::GetAddressHead()[ADDRESSHEAD_8] << " [" << Intelx86Def::GetReg(this->GetAddressMode())[REG_EBX_32] << " + " << this->ParseRegisterOperand(CntByte, REG_AL_8, OPSIZE_8) << "]";
                break;
            case OPCODE_FADD_GROUP:
            case OPCODE_FADD_GROUP | 0x04:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_FaddGroup()[GET_OPCODE2(MODRM)] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, GET_6TH_BIT(CntByte) ? OPSIZE_32 : OPSIZE_64);
                break;
            case OPCODE_FLD_GROUP_RM32:
                MODRM = this->ReadByte();
                switch (GET_OPCODE2(MODRM)) {
                    case SUB_OPCODE_FLDENV:
                    case SUB_OPCODE_FSTENV:
                        this->GetAsmStream() << Intelx86Def::GetSubOpcode_FldGroup()[GET_OPCODE2(MODRM)] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_NULL);
                        break;
                    case SUB_OPCODE_FLD:
                    case SUB_OPCODE_FST:
                    case SUB_OPCODE_FSTP:
                        this->GetAsmStream() << Intelx86Def::GetSubOpcode_FldGroup()[GET_OPCODE2(MODRM)] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_32);
                        break;
                    case SUB_OPCODE_FLDCW:
                    case SUB_OPCODE_FSTCW:
                        this->GetAsmStream() << Intelx86Def::GetSubOpcode_FldGroup()[GET_OPCODE2(MODRM)] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16);
                        break;
                    default:
                        this->GetAsmStream() << "???";
                        this->GetInputFile().seekg(this->GetInputFile().tellg() + 1LL);
                        this->StringStreamClear(this->GetBinStream());
                        this->GetBinStream() << this->FormatByte(CntByte) << " ";
                        break;
                }
                break;
            case OPCODE_FLD_GROUP_RM64:
                MODRM = this->ReadByte();
                switch (GET_OPCODE2(MODRM)) {
                    case SUB_OPCODE_FRSTOR:
                    case SUB_OPCODE_FSAVE:
                        this->GetAsmStream() << Intelx86Def::GetSubOpcode_Fld64Group()[GET_OPCODE2(MODRM)] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_NULL);
                        break;
                    case SUB_OPCODE_FLD:
                    case SUB_OPCODE_FISTTP:
                    case SUB_OPCODE_FST:
                    case SUB_OPCODE_FSTP:
                        this->GetAsmStream() << Intelx86Def::GetSubOpcode_Fld64Group()[GET_OPCODE2(MODRM)] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_64);
                        break;
                    case SUB_OPCODE_FSTSW:
                        this->GetAsmStream() << Intelx86Def::GetSubOpcode_Fld64Group()[GET_OPCODE2(MODRM)] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16);
                        break;
                    default:
                        this->GetAsmStream() << "???";
                        this->GetInputFile().seekg(this->GetInputFile().tellg() + 1LL);
                        this->StringStreamClear(this->GetBinStream());
                        this->GetBinStream() << this->FormatByte(CntByte) << " ";
                        break;
                }
                break;
            case OPCODE_FIADD_GROUP:
            case OPCODE_FIADD_GROUP | 0x04:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_FiaddGroup()[GET_OPCODE2(MODRM)] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, GET_6TH_BIT(CntByte) ? OPSIZE_32 : OPSIZE_16);
                break;
            case OPCODE_FILD_GROUP_RM32:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_FildGroup()[GET_OPCODE2(MODRM)] << " ";
                switch (GET_OPCODE2(MODRM)) {
                    case SUB_OPCODE_FLD_TBYTE:
                    case SUB_OPCODE_FSTP_TBYTE:
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_TBYTE);
                        break;
                    default:
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_32);
                        break;
                }
                break;
            case OPCODE_FILD_GROUP_RM16:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_Fild16Group()[GET_OPCODE2(MODRM)] << " ";
                switch (GET_OPCODE2(MODRM)) {
                    case SUB_OPCODE_FBLD_TBYTE:
                    case SUB_OPCODE_FBSTP_TBYTE:
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_TBYTE);
                        break;
                    case SUB_OPCODE_FILD64:
                    case SUB_OPCODE_FISTP64:
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_64);
                        break;
                    default:
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16);
                        break;
                }
                break;
            case OPCODE_IN + RM8_R8:
            case OPCODE_IN + RM16_R16:
            case OPCODE_OUT + RM8_R8:
            case OPCODE_OUT + RM16_R16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte & 0xFE] << " ";
                Operand[0] = this->ParseRegisterOperand(CntByte, REG_EAX_32);
                Operand[1] = this->ParseImmediateOperand_I8();
                this->GetAsmStream() << (GET_D_BIT(CntByte) ? Operand[1] + ", " + Operand[0] : Operand[0] + ", " + Operand[1]);
                break;
            case OPCODE_IN_DX + RM8_R8:
            case OPCODE_IN_DX + RM16_R16:
            case OPCODE_OUT_DX + RM8_R8:
            case OPCODE_OUT_DX + RM16_R16:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte & 0xFE] << " ";
                Operand[0] = this->ParseRegisterOperand(CntByte, REG_EAX_32);
                Operand[1] = Intelx86Def::GetReg(REGSIZE_16)[REG_DX_16];
                this->GetAsmStream() << (GET_D_BIT(CntByte) ? Operand[1] + ", " + Operand[0] : Operand[0] + ", " + Operand[1]);
                break;
            case OPCODE_CALL_NEAR:
            case OPCODE_JMP_NEAR:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte] << " ";
                if (this->GetCPUMode() == CPU_MODE_16)
                {
                    I16 = this->ReadWord();
                    TargetAddress = CntAddress + I16 + (DWORD)(this->GetInputFile().tellg() - InstructionStartPos);
                    this->GetAsmStream() << this->FormatAsmWord(TargetAddress) << "H";
                }
                else if (this->GetCPUMode() == CPU_MODE_32)
                {
                    I32 = this->ReadDWord();
                    TargetAddress = CntAddress + I32 + (DWORD)(this->GetInputFile().tellg() - InstructionStartPos);
                    this->GetAsmStream() << this->FormatAsmDWord(TargetAddress) << "H";
                }
                break;
            case OPCODE_INC_GROUP_RM8:
                MODRM = this->ReadByte();
                this->GetAsmStream() << Intelx86Def::GetSubOpcode_Inc8Group()[GET_OPCODE2(MODRM)] << " ";
                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_8);
                break;
            case OPCODE_INC_GROUP_RM16_RM32:
                MODRM = this->ReadByte();
                switch (GET_OPCODE2(MODRM)) {
                    case SUB_OPCODE_INC:
                    case SUB_OPCODE_DEC:
                    case SUB_OPCODE_CALL_NEAR:
                    case SUB_OPCODE_JMP_NEAR:
                    case SUB_OPCODE_PUSH:
                        this->GetAsmStream() << Intelx86Def::GetSubOpcode_Inc16_32Group()[GET_OPCODE2(MODRM)] << " ";
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16_32);
                        break;
                    case SUB_OPCODE_CALL_FAR:
                    case SUB_OPCODE_JMP_FAR:
                        this->GetAsmStream() << Intelx86Def::GetSubOpcode_Inc16_32Group()[GET_OPCODE2(MODRM)] << " ";
                        this->GetAsmStream() << "FAR " << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_32);
                        break;
                    default:
                        this->GetAsmStream() << "???";
                        this->GetInputFile().seekg(this->GetInputFile().tellg() - 1LL);
                        this->StringStreamClear(this->GetBinStream());
                        this->GetBinStream() << this->FormatByte(CntByte) << " ";
                        break;
                }
                break;
            case OPCODE_DAA:
            case OPCODE_DAS:
            case OPCODE_AAA:
            case OPCODE_AAS:
            case OPCODE_NOP:
            case OPCODE_CWDE:
            case OPCODE_CDQ:
            case OPCODE_WAIT:
            case OPCODE_PUSHFD:
            case OPCODE_POPFD:
            case OPCODE_SAHF:
            case OPCODE_LAHF:
            case OPCODE_RETN:
            case OPCODE_LEAVE:
            case OPCODE_RETF:
            case OPCODE_INT3:
            case OPCODE_INTO:
            case OPCODE_IRETD:
            case OPCODE_SALC:
            case OPCODE_INT1:
            case OPCODE_HLT:
            case OPCODE_CMC:
            case OPCODE_CLC:
            case OPCODE_STC:
            case OPCODE_CLI:
            case OPCODE_STI:
            case OPCODE_CLD:
            case OPCODE_STD:
                this->GetAsmStream() << Intelx86Def::GetOpcode()[CntByte];
                break;
            case OPCODE_MULTI_BYTE_OPCODE_0F:
                CntByte = this->ReadByte();
                switch (CntByte) {
                    case SECOND_OPCODE_JO:
                    case SECOND_OPCODE_JNO:
                    case SECOND_OPCODE_JB:
                    case SECOND_OPCODE_JNB:
                    case SECOND_OPCODE_JE:
                    case SECOND_OPCODE_JNE:
                    case SECOND_OPCODE_JBE:
                    case SECOND_OPCODE_JA:
                    case SECOND_OPCODE_JS:
                    case SECOND_OPCODE_JNS:
                    case SECOND_OPCODE_JPE:
                    case SECOND_OPCODE_JPO:
                    case SECOND_OPCODE_JL:
                    case SECOND_OPCODE_JGE:
                    case SECOND_OPCODE_JLE:
                    case SECOND_OPCODE_JG:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        if (this->GetCPUMode() == CPU_MODE_16)
                        {
                            I16 = this->ReadWord();
                            TargetAddress = CntAddress + I16 + (DWORD)(this->GetInputFile().tellg() - InstructionStartPos);
                            this->GetAsmStream() << this->FormatAsmWord(TargetAddress) << "H";
                        }
                        else if (this->GetCPUMode() == CPU_MODE_32)
                        {
                            I32 = this->ReadDWord();
                            TargetAddress = CntAddress + I32 + (DWORD)(this->GetInputFile().tellg() - InstructionStartPos);
                            this->GetAsmStream() << this->FormatAsmDWord(TargetAddress) << "H";
                        }
                        break;
                    case SECOND_OPCODE_CMPXCHG + RM8_R8:
                    case SECOND_OPCODE_CMPXCHG + RM16_R16:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[SECOND_OPCODE_CMPXCHG] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << this->ParseMODRM_RM_R(CntByte, MODRM);
                        break;
                    case SECOND_OPCODE_LSS:
                    case SECOND_OPCODE_LFS:
                    case SECOND_OPCODE_LGS:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << Intelx86Def::GetReg(this->GetCPUMode())[GET_REG(MODRM)] << ", ";
                        switch (this->GetCPUMode()) {
                            case CPU_MODE_16:
                                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_32);
                                break;
                            case CPU_MODE_32:
                                this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_F);
                                break;
                            default:
                                break;
                        }
                        break;
                    case SECOND_OPCODE_BTR:
                    case SECOND_OPCODE_BTC:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16_32) << ", " << this->ParseRegisterOperand(CntByte, GET_REG(MODRM));
                        break;
                    case SECOND_OPCODE_BSF:
                    case SECOND_OPCODE_BSR:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << Intelx86Def::GetReg(this->GetCPUMode())[GET_REG(MODRM)] << ", " << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16_32);
                        break;
                    case SECOND_OPCODE_MOVZX + RM8_R8:
                    case SECOND_OPCODE_MOVZX + RM16_R16:
                    case SECOND_OPCODE_MOVSX + RM8_R8:
                    case SECOND_OPCODE_MOVSX + RM16_R16:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte & 0xFE] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << Intelx86Def::GetReg(OPSIZE_32)[GET_REG(MODRM)] << ", ";
                        this->GetAsmStream() << (GET_W_BIT(CntByte) ? this->ParseMemoryOperand(CntByte, MODRM, OPSIZE_16) : this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_8));
                        break;
                    case SECOND_OPCODE_PUSH_FS:
                    case SECOND_OPCODE_POP_FS:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        this->GetAsmStream() << Intelx86Def::GetReg(SEGMENT_REG)[REG_FS];
                        break;
                    case SECOND_OPCODE_BT:
                    case SECOND_OPCODE_BTS:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16_32) << ", " << this->ParseRegisterOperand(CntByte, GET_REG(MODRM), OPSIZE_16_32);
                        break;
                    case SECOND_OPCODE_SHLD_I8:
                    case SECOND_OPCODE_SHRD_I8:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16_32) << ", " << this->ParseRegisterOperand(CntByte, GET_REG(MODRM), OPSIZE_16_32) << ", " << this->ParseImmediateOperand_I8();
                        break;
                    case SECOND_OPCODE_SHLD_CL:
                    case SECOND_OPCODE_SHRD_CL:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16_32) << ", " << this->ParseRegisterOperand(CntByte, GET_REG(MODRM), OPSIZE_16_32) << ", " << this->ParseRegisterOperand(CntByte, REG_CL_8, OPSIZE_8);
                        break;
                    case SECOND_OPCODE_PUSH_GS:
                    case SECOND_OPCODE_POP_GS:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        this->GetAsmStream() << Intelx86Def::GetReg(SEGMENT_REG)[REG_GS];
                        break;
                    case SECOND_OPCODE_FXSAVE:
                        // TODO THIRD_OPCODE
                        //this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        break;
                    case SECOND_OPCODE_IMUL:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << this->ParseRegisterOperand(CntByte, GET_REG(MODRM), OPSIZE_16_32) << ", " << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_16_32);
                        break;
                    case SECOND_OPCODE_UD1:
                    case SECOND_OPCODE_CPUID:
                    case SECOND_OPCODE_RSM:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte];
                        break;
                    case SECOND_OPCODE_SETO:
                    case SECOND_OPCODE_SETNO:
                    case SECOND_OPCODE_SETB:
                    case SECOND_OPCODE_SETNB:
                    case SECOND_OPCODE_SETE:
                    case SECOND_OPCODE_SETNE:
                    case SECOND_OPCODE_SETBE:
                    case SECOND_OPCODE_SETA:
                    case SECOND_OPCODE_SETS:
                    case SECOND_OPCODE_SETNS:
                    case SECOND_OPCODE_SETPE:
                    case SECOND_OPCODE_SETPO:
                    case SECOND_OPCODE_SETL:
                    case SECOND_OPCODE_SETGE:
                    case SECOND_OPCODE_SETLE:
                    case SECOND_OPCODE_SETG:
                        this->GetAsmStream() << Intelx86Def::GetSecondOpcode()[CntByte] << " ";
                        MODRM = this->ReadByte();
                        this->GetAsmStream() << this->ParseMODRM_RM(CntByte, MODRM, OPSIZE_8);
                        break;
                    default:
                        this->GetAsmStream() << "???";
                        this->GetInputFile().seekg(this->GetInputFile().tellg() - 1LL);
                        this->StringStreamClear(this->GetBinStream());
                        this->GetBinStream() << this->FormatByte(OPCODE_MULTI_BYTE_OPCODE_0F) << " ";
                        break;
                }
                break;
            default:
                this->GetAsmStream() << "???";
                break;
        }
        CntInstruction->Clear();
        this->OutputLineAndReset();
        InstructionStartPos = this->GetInputFile().tellg();
    }
    this->GetInputFile().close();
}

void DASM::AT_AND_T_Exec()
{
    
}

void DASM::SetArch(BYTE Arch)
{
    this->Arch = Arch;
}

BYTE DASM::GetArch()
{
    return this->Arch;
}

void DASM::SetDefaultCPUMode(BYTE DefaultCPUMode)
{
    this->DefaultCPUMode = DefaultCPUMode;
}

BYTE DASM::GetDefaultCPUMode()
{
    return this->DefaultCPUMode;
}

void DASM::SetDefaultAddressMode(BYTE DefaultAddressMode)
{
    this->DefaultAddressMode = DefaultAddressMode;
}

BYTE DASM::GetDefaultAddressMode()
{
    return this->DefaultAddressMode;
}

void DASM::SetCPUMode(BYTE CPUMode)
{
    this->CPUMode = CPUMode;
}

BYTE DASM::GetCPUMode()
{
    return this->CPUMode;
}

void DASM::SetAddressMode(BYTE AddressMode)
{
    this->AddressMode = AddressMode;
}

BYTE DASM::GetAddressMode()
{
    return this->AddressMode;
}

void DASM::SetDefaultSegmentRegister(BYTE Register)
{
    this->DefaultSegmentRegister = Register;
}

BYTE DASM::GetDefaultSegmentRegister()
{
    return this->DefaultSegmentRegister;
}

void DASM::SetDefaultByteOrder(BYTE DefaultByteOrder)
{
    this->DefaultByteOrder = DefaultByteOrder;
}

BYTE DASM::GetDefaultByteOrder()
{
    return this->DefaultByteOrder;
}

void DASM::SetImageLoadAddress(DWORD ImageLoadAddress)
{
    this->ImageLoadAddress = ImageLoadAddress;
}

DWORD DASM::GetImageLoadAddress()
{
    return this->ImageLoadAddress;
}

void DASM::SetTextSectionRawDataAddress(DWORD TextSectionRawDataAddress)
{
    this->TextSectionRawDataAddress = TextSectionRawDataAddress;
}

DWORD DASM::GetTextSectionRawDataAddress()
{
    return this->TextSectionRawDataAddress;
}

void DASM::SetTextSectionRawDataLen(DWORD TextSectionRawDataLen)
{
    this->TextSectionRawDataLen = TextSectionRawDataLen;
}

DWORD DASM::GetTextSectionRawDataLen()
{
    return this->TextSectionRawDataLen;
}

void DASM::SetTextSectionLoadAddress(DWORD TextSectionLoadAddress)
{
    this->TextSectionLoadAddress = TextSectionLoadAddress;
}

DWORD DASM::GetTextSectionLoadAddress()
{
    return this->TextSectionLoadAddress;
}

void DASM::AddressInc(DWORD IncVal)
{
    this->CntAddress += IncVal;
}

void DASM::SetCntAddress(DWORD CntAddress)
{
    this->CntAddress = CntAddress;
}

DWORD DASM::GetCntAddress()
{
    return this->CntAddress;
}

stringstream& DASM::GetAsmStream()
{
    return this->AsmStream;
}

stringstream& DASM::GetBinStream()
{
    return this->BinStream;
}

ifstream& DASM::GetInputFile()
{
    return this->InputFile;
}

ofstream& DASM::GetOutputAddrFile()
{
    return this->OutputAddrFile;
}

ofstream& DASM::GetOutputBinFile()
{
    return this->OutputBinFile;
}

ofstream& DASM::GetOutputAsmFile()
{
    return this->OutputAsmFile;
}

BYTE DASM::ReadByte()
{
    BYTE CntByte = 0;
    if (this->GetInputFile())
    {
        this->GetInputFile().read((char*)&CntByte, 1);
    }
    this->GetBinStream() << this->FormatByte(CntByte) << " ";
    return CntByte;
}

WORD DASM::ReadWord(BYTE ByteOrder)
{
    BYTE CntByte[2];
    for (int i = 0; i < 2; i++)
    {
        CntByte[i] = this->ReadByte();
    }
    if (ByteOrder == UDASM_UNKNOWN_ENDIAN)
    {
        ByteOrder = this->GetDefaultByteOrder();
    }
    return ByteOrder == UDASM_BIG_ENDIAN ? MAKE_WORD(CntByte[0], CntByte[1]) : MAKE_WORD(CntByte[1], CntByte[0]);
}

DWORD DASM::ReadDWord(BYTE ByteOrder)
{
    WORD CntWord[2];
    for (int i = 0; i < 2; i++)
    {
        CntWord[i] = this->ReadWord(ByteOrder);
    }
    if (ByteOrder == UDASM_UNKNOWN_ENDIAN)
    {
        ByteOrder = this->GetDefaultByteOrder();
    }
    return ByteOrder == UDASM_BIG_ENDIAN ? MAKE_DWORD_2(CntWord[0], CntWord[1]) : MAKE_DWORD_2(CntWord[1], CntWord[0]);
}

string DASM::FormatByte(BYTE CntByte, BYTE SMod)
{
    stringstream FormatStream;
    if (SMod == SIGNED && GET_1ST_BIT(CntByte))
    {
        CntByte = ~CntByte + 1;
    }
    FormatStream << setiosflags(ios::uppercase) << setfill('0') << setw(2) << hex << (int)CntByte;
    string RetVal = "";
    FormatStream >> RetVal;
    return RetVal;
}

string DASM::FormatBinWord(WORD CntWord)
{
    string RetVal =
    this->FormatByte(GET_1ST_BYTE_FROM_WORD(CntWord)) + " " +
    this->FormatByte(GET_2ND_BYTE_FROM_WORD(CntWord));
    return RetVal;
}

string DASM::FormatAsmWord(WORD CntWord)
{
    string RetVal =
    this->FormatByte(GET_1ST_BYTE_FROM_WORD(CntWord)) +
    this->FormatByte(GET_2ND_BYTE_FROM_WORD(CntWord));
    return RetVal;
}

string DASM::FormatBinDWord(DWORD CntDWord)
{
    string RetVal =
    this->FormatByte(GET_1ST_BYTE_FROM_DWORD(CntDWord)) + " " +
    this->FormatByte(GET_2ND_BYTE_FROM_DWORD(CntDWord)) + " " +
    this->FormatByte(GET_3RD_BYTE_FROM_DWORD(CntDWord)) + " " +
    this->FormatByte(GET_4TH_BYTE_FROM_DWORD(CntDWord));
    return RetVal;
}

string DASM::FormatAsmDWord(DWORD CntDWord)
{
    string RetVal =
    this->FormatByte(GET_1ST_BYTE_FROM_DWORD(CntDWord)) +
    this->FormatByte(GET_2ND_BYTE_FROM_DWORD(CntDWord)) +
    this->FormatByte(GET_3RD_BYTE_FROM_DWORD(CntDWord)) +
    this->FormatByte(GET_4TH_BYTE_FROM_DWORD(CntDWord));
    return RetVal;
}

string DASM::ParseMemoryOperand(BYTE Opcode, BYTE MODRM, BYTE OpSize)
{
    BYTE MOD = 0, RM = 0;
    BYTE Base = 0, Index = 0, Scale = 0, SIB = 0;
    string Operand;
    MOD = GET_MOD(MODRM);
    RM = GET_RM(MODRM);
    switch (OpSize) {
        case OPSIZE_8:
            Operand += Intelx86Def::GetAddressHead()[ADDRESSHEAD_8] + " ";
            break;
        case OPSIZE_16:
            Operand += Intelx86Def::GetAddressHead()[ADDRESSHEAD_16] + " ";
            break;
        case OPSIZE_32:
            Operand += Intelx86Def::GetAddressHead()[ADDRESSHEAD_32] + " ";
            break;
        case OPSIZE_64:
            Operand += Intelx86Def::GetAddressHead()[ADDRESSHEAD_64] + " ";
            break;
        case OPSIZE_F:
            Operand += Intelx86Def::GetAddressHead()[ADDRESSHEAD_F] + " ";
            break;
        case OPSIZE_16_32:
            Operand += Intelx86Def::GetAddressHead()[this->GetCPUMode()] + " ";
            break;
        case OPSIZE_32_64: // TODO
            //Operand += Intelx86Def::GetAddressHead()[this->GetCPUMode()] + " ";
            break;
        case OPSIZE_8_16_32:
            Operand += (GET_W_BIT(Opcode) == 0x00 ? Intelx86Def::GetAddressHead()[ADDRESSHEAD_8] : Intelx86Def::GetAddressHead()[this->GetCPUMode()]) + " ";
            break;
        case OPSIZE_8_32_64: // TODO
            break;
        case OPSIZE_NULL:
            Operand += Intelx86Def::GetAddressHead()[ADDRESSHEAD_NULL] + " ";
            break;
        default:
            Operand += (GET_W_BIT(Opcode) == 0x00 ? Intelx86Def::GetAddressHead()[ADDRESSHEAD_8] : Intelx86Def::GetAddressHead()[this->GetCPUMode()]) + " ";
            break;
    }
    if (this->GetDefaultSegmentRegister() != REG_NULL)
    {
        Operand += Intelx86Def::GetPrefix(1)[this->GetDefaultSegmentRegister()] + ":";
    }
    else if ((this->GetAddressMode() == ADDRESS_MODE_16) || (this->GetAddressMode() == ADDRESS_MODE_32 && RM != RM_SIB_FLAG))
    {
        if ((this->GetAddressMode() == ADDRESS_MODE_32 && MOD == MOD_M_NO_DISPLACEMENT && RM == RM_ONLY_DISPLACEMENT_32_FLAG) || (this->GetAddressMode() == ADDRESS_MODE_16 && MOD == MOD_M_NO_DISPLACEMENT && MOD == RM_ONLY_DISPLACEMENT_16_FLAG))
        {
            Operand += Intelx86Def::GetPrefix(1)[SEGMENT_PREFIX_DS] + ":";
        }
        else
        {
            switch (GET_RM(MODRM)) {
                case REG_EBP_32:
                case REG_ESP_32:
                    Operand += Intelx86Def::GetPrefix(1)[SEGMENT_PREFIX_SS] + ":";
                    break;
                case REG_EAX_32:
                case REG_EBX_32:
                case REG_ECX_32:
                case REG_EDX_32:
                case REG_EDI_32:
                case REG_ESI_32:
                    Operand += Intelx86Def::GetPrefix(1)[SEGMENT_PREFIX_DS] + ":";
                default:
                    break;
            }
        }
    }
    BYTE I8;
    DWORD I32;
    switch (this->GetAddressMode()) {
        case ADDRESS_MODE_16: // 16 Bits Addressing
            Operand += "[";
            Operand += (MOD == MOD_M_NO_DISPLACEMENT && RM == RM_ONLY_DISPLACEMENT_16_FLAG) ? "SMALL " + this->ParseDisplacement(MOD) : Intelx86Def::GetReg(ADDRESSSIZE_16)[RM];
            switch (MOD) {
                case MOD_M_DISPLACEMENT_8:
                case MOD_M_DISPLACEMENT_16_32:
                    Operand += " + " + this->ParseDisplacement(MOD);
                    break;
                default:
                    break;
            }
            Operand += "]";
            break;
        case ADDRESS_MODE_32: // 32 Bits Addressing
            if (MOD == MOD_M_NO_DISPLACEMENT && RM == RM_ONLY_DISPLACEMENT_32_FLAG)
            {
                Operand += "[" + this->ParseImmediateOperand_I32() + "]";
                break;
            }
            switch (RM)
            {
                case RM_SIB_FLAG: // Use SIB Memory Addressing
                    SIB = this->ReadByte();
                    Scale = GET_SCALE(SIB);
                    Index = GET_INDEX(SIB);
                    Base = GET_BASE(SIB);
                    if (this->GetDefaultSegmentRegister() == REG_NULL)
                    {
                        switch (Index) {
                            case REG_EBP_32:
                            case REG_ESP_32:
                                Operand += Intelx86Def::GetPrefix(1)[SEGMENT_PREFIX_SS] + ":";
                                break;
                            case REG_EAX_32:
                            case REG_EBX_32:
                            case REG_ECX_32:
                            case REG_EDX_32:
                            case REG_EDI_32:
                            case REG_ESI_32:
                                Operand += Intelx86Def::GetPrefix(1)[SEGMENT_PREFIX_DS] + ":";
                            default:
                                break;
                        }
                    }
                    Operand += "[";
                    if (Index != RM_SIB_FLAG)
                    {
                        Operand += Intelx86Def::GetReg(REGSIZE_32)[Index];
                        if (Scale != 0)
                        {
                            Operand += " * " + string(1, BYTE(1 << Scale) + '0');
                        }
                    }
                    if (Base == SIB_NO_BASE_FLAG && MOD == MOD_M_NO_DISPLACEMENT)
                    {
                        I32 = this->ReadDWord();
                        if (Index != RM_SIB_FLAG)
                        {
                            Operand += " + ";
                        }
                        Operand += this->FormatAsmDWord(I32) + "H";
                    }
                    else
                    {
                        if (Index != RM_SIB_FLAG)
                        {
                            Operand += " + ";
                        }
                        if (Base == RM_ONLY_DISPLACEMENT_32_FLAG)
                        {
                            Operand += this->ParseDisplacement(MOD);
                        }
                        else
                        {
                            Operand += this->ParseRegisterOperand(Opcode, Base, OPSIZE_32);
                            switch (MOD) {
                                case MOD_M_DISPLACEMENT_8:
                                    I8 = this->ReadByte();
                                    Operand += (GET_1ST_BIT(I8) ? " - " + this->FormatByte(~I8 + 1) : " + " + this->FormatByte(I8)) + "H";
                                    break;
                                case MOD_M_DISPLACEMENT_16_32:
                                    Operand += " + " + this->ParseDisplacement(MOD);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    Operand += "]";
                    break;
                default: // Use Non-SIB Memory Addressing
                    Operand += "[" + Intelx86Def::GetReg(REGSIZE_32)[RM];
                    switch (MOD) {
                        case MOD_M_DISPLACEMENT_8:
                            I8 = this->ReadByte();
                            Operand += (GET_1ST_BIT(I8) ? " - " + this->FormatByte(~I8 + 1) : " + " + this->FormatByte(I8)) + "H";
                            break;
                        case MOD_M_DISPLACEMENT_16_32:
                            Operand += " + " + this->ParseDisplacement(MOD);
                            break;
                        default:
                            break;
                    }
                    Operand += "]";
                    break;
                }
            break;
        default:
            break;
    }
    return Operand;
}

string DASM::ParseRegisterOperand(BYTE Opcode, BYTE REG, BYTE OpSize)
{
    string Operand;
    switch (OpSize) {
        case OPSIZE_8:
            Operand = Intelx86Def::GetReg(REGSIZE_8)[REG];
            break;
        case OPSIZE_16:
            Operand = Intelx86Def::GetReg(REGSIZE_16)[REG];
            break;
        case OPSIZE_32:
            Operand = Intelx86Def::GetReg(REGSIZE_32)[REG];
            break;
        case OPSIZE_64:
            Operand = Intelx86Def::GetReg(REGSIZE_64)[REG];
            break;
        case OPSIZE_F:
            Operand = Intelx86Def::GetReg(FLOAT_REG)[REG];
            break;
        case OPSIZE_SEGR:
            Operand = Intelx86Def::GetReg(SEGMENT_REG)[REG];
            break;
        case OPSIZE_16_32:
            Operand = Intelx86Def::GetReg(this->GetCPUMode())[REG];
            break;
        case OPSIZE_32_64: // TODO
            Operand = Intelx86Def::GetReg(this->GetCPUMode())[REG];
            break;
        case OPSIZE_8_16_32:
            Operand = (GET_W_BIT(Opcode) ? Intelx86Def::GetReg(this->GetCPUMode())[REG] : Intelx86Def::GetReg(REGSIZE_8)[REG]);
            break;
        case OPSIZE_8_32_64: // TODO
            break;
        default:
            break;
    }
    return Operand;
}

string DASM::ParseDisplacement(BYTE MOD)
{
    return MOD == MOD_M_DISPLACEMENT_8 ? this->ParseImmediateOperand_I8() : (this->GetAddressMode() == ADDRESS_MODE_16 ? this->ParseImmediateOperand_I16() : this->ParseImmediateOperand_I32());
}

string DASM::ParseImmediateOperand(BYTE Opcode)
{
    return GET_W_BIT(Opcode) == 0x00 ? this->ParseImmediateOperand_I8() : this->ParseImmediateOperand_I16_I32();
}

string DASM::ParseImmediateOperand_I8(BYTE SMod)
{
    BYTE I8 = this->ReadByte();
    string RetVal = "";
    switch (SMod) {
        case SIGNED:
            RetVal += (GET_1ST_BIT(I8) ? "-" : "");
            RetVal += this->FormatByte(I8, SIGNED) + "H";
            break;
        case UNSIGNED:
            RetVal += this->FormatByte(I8, UNSIGNED) + "H";
            break;
        case EXTEND_I16_I32:
            if (GET_1ST_BIT(I8))
            {
                RetVal += (this->GetCPUMode() == CPU_MODE_16 ? "FF" : "FFFFFF");
            }
            else
            {
                RetVal += (this->GetCPUMode() == CPU_MODE_16 ? "00" : "000000");
            }
            RetVal += this->FormatByte(I8, UNSIGNED) + "H";
            break;
        default:
            break;
    }
    return RetVal;
}

string DASM::ParseImmediateOperand_I16()
{
    WORD I16 = this->ReadWord();
    return this->FormatAsmWord(I16) + "H";
}

string DASM::ParseImmediateOperand_I32()
{
    DWORD I32 = this->ReadDWord();
    return this->FormatAsmDWord(I32) + "H";
}

string DASM::ParseImmediateOperand_I16_I32()
{
    return this->GetCPUMode() == CPU_MODE_16 ? this->ParseImmediateOperand_I16() : this->ParseImmediateOperand_I32();
}

string DASM::ParseMODRM_RM(BYTE Opcode, BYTE MODRM, BYTE OpSize)
{
    return GET_MOD(MODRM) == MOD_R_NO_DISPLACEMENT ? this->ParseRegisterOperand(Opcode, GET_RM(MODRM), OpSize) : this->ParseMemoryOperand(Opcode, MODRM, OpSize);
}

string DASM::ParseMODRM_RM_R(BYTE Opcode, BYTE MODRM, BYTE OpSize)
{
    string Operand[2];
    Operand[0] = this->ParseMODRM_RM(Opcode, MODRM, OpSize);
    Operand[1] = this->ParseRegisterOperand(Opcode, GET_REG(MODRM), OpSize);
    return GET_D_BIT(Opcode) == 0x00 ? Operand[0] + ", " + Operand[1] : Operand[1] + ", " + Operand[0];
}

string DASM::ParseMODRM_RM_IMM(BYTE Opcode, BYTE MODRM, BYTE OpSize)
{
    string Operand[2];
    Operand[0] = this->ParseMODRM_RM(Opcode, MODRM, OpSize);
    Operand[1] = this->ParseImmediateOperand(Opcode);
    // TODO  S-BIT
    return GET_S_BIT(Opcode) == 0x00 ? Operand[0] + ", " + Operand[1] : Operand[0] + ", " + Operand[1];
}

string DASM::ParseMODRM_R_RM_IMM(BYTE Opcode, BYTE MODRM, BYTE OpSize)
{
    string Operand[3];
    Operand[0] = this->ParseRegisterOperand(Opcode, GET_REG(MODRM), OpSize);
    Operand[1] = this->ParseMODRM_RM(Opcode, MODRM, OpSize);
    Operand[2] = GET_D_BIT(Opcode) == 0x00 ? this->ParseImmediateOperand_I16_I32() : this->ParseImmediateOperand_I8();
    // TODO  S-BIT
    return Operand[0] + ", " + Operand[1] + ", " + Operand[2];
}

void DASM::StringStreamClear(stringstream &SStream)
{
    SStream.clear();
    SStream.str("");
}
