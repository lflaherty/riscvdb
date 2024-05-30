#include "riscv_processor.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <functional>

namespace riscvdb
{

// CSR numbers
const uint32_t RiscvProcessor::csr_mvendorid = 0xF11;
const uint32_t RiscvProcessor::csr_marchid = 0xF12;
const uint32_t RiscvProcessor::csr_mimpid = 0xF13;
const uint32_t RiscvProcessor::csr_mhartid = 0xF14;
const uint32_t RiscvProcessor::csr_mstatus = 0x300;
const uint32_t RiscvProcessor::csr_misa = 0x301;
const uint32_t RiscvProcessor::csr_mie = 0x304;
const uint32_t RiscvProcessor::csr_mtvec = 0x305;
const uint32_t RiscvProcessor::csr_mscratch = 0x340;
const uint32_t RiscvProcessor::csr_mepc = 0x341;
const uint32_t RiscvProcessor::csr_mcause = 0x342;
const uint32_t RiscvProcessor::csr_mtval = 0x343;
const uint32_t RiscvProcessor::csr_mip = 0x344;

// Privilege levels
const uint8_t RiscvProcessor::PRV_USER = 0;
const uint8_t RiscvProcessor::PRV_MACHINE = 3;

RiscvProcessor::RiscvProcessor(MemoryMap& mem)
: m_mem(mem),
  m_pc(0),
  m_instruction_count(0),
  m_verbose(false),
  m_prv(PRV_MACHINE)
{
    // initialize all values to default:
    Reset();

    // Setup lookup tables
    // R types
    cmd_mapping_R[mask_add] =   Instruction("add", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_add);
    cmd_mapping_R[mask_sub] =   Instruction("sub", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_sub);
    cmd_mapping_R[mask_sll] =   Instruction("sll", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_sll);
    cmd_mapping_R[mask_slt] =   Instruction("slt", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_slt);
    cmd_mapping_R[mask_sltu] =  Instruction("sltu", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_sltu);
    cmd_mapping_R[mask_xor] =   Instruction("xor", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_xor);
    cmd_mapping_R[mask_srl] =   Instruction("srl", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_srl);
    cmd_mapping_R[mask_sra] =   Instruction("sra", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_sra);
    cmd_mapping_R[mask_or] =    Instruction("or", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_or);
    cmd_mapping_R[mask_and] =   Instruction("and", &RiscvProcessor::decode_R,  &RiscvProcessor::execute_and);

    // I/S/B types
    cmd_mapping_ISB[mask_jalr] =  Instruction("jalr", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_jalr);
    cmd_mapping_ISB[mask_beq] =   Instruction("beq", &RiscvProcessor::decode_B,  &RiscvProcessor::execute_beq);
    cmd_mapping_ISB[mask_bne] =   Instruction("bne", &RiscvProcessor::decode_B,  &RiscvProcessor::execute_bne);
    cmd_mapping_ISB[mask_blt] =   Instruction("blt", &RiscvProcessor::decode_B,  &RiscvProcessor::execute_blt);
    cmd_mapping_ISB[mask_bge] =   Instruction("bge", &RiscvProcessor::decode_B,  &RiscvProcessor::execute_bge);
    cmd_mapping_ISB[mask_bltu] =  Instruction("bltu", &RiscvProcessor::decode_B,  &RiscvProcessor::execute_bltu);
    cmd_mapping_ISB[mask_bgeu] =  Instruction("bgeu", &RiscvProcessor::decode_B,  &RiscvProcessor::execute_bgeu);
    cmd_mapping_ISB[mask_lb] =    Instruction("lb", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_lb);
    cmd_mapping_ISB[mask_lh] =    Instruction("lh", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_lh);
    cmd_mapping_ISB[mask_lw] =    Instruction("lw", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_lw);
    cmd_mapping_ISB[mask_lbu] =   Instruction("lbu", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_lbu);
    cmd_mapping_ISB[mask_lhu] =   Instruction("lhu", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_lhu);
    cmd_mapping_ISB[mask_sb] =    Instruction("sb", &RiscvProcessor::decode_S,  &RiscvProcessor::execute_sb);
    cmd_mapping_ISB[mask_sh] =    Instruction("sh", &RiscvProcessor::decode_S,  &RiscvProcessor::execute_sh);
    cmd_mapping_ISB[mask_sw] =    Instruction("sw", &RiscvProcessor::decode_S,  &RiscvProcessor::execute_sw);
    cmd_mapping_ISB[mask_addi] =  Instruction("addi", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_addi);
    cmd_mapping_ISB[mask_slti] =  Instruction("slti", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_slti);
    cmd_mapping_ISB[mask_sltiu] = Instruction("sltiu", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_sltiu);
    cmd_mapping_ISB[mask_xori] =  Instruction("xori", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_xori);
    cmd_mapping_ISB[mask_ori] =   Instruction("ori", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_ori);
    cmd_mapping_ISB[mask_andi] =  Instruction("andi", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_andi);
    cmd_mapping_ISB[mask_slli] =  Instruction("slli", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_slli);
    cmd_mapping_ISB[mask_srli] =  Instruction("srli", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_srli);
    cmd_mapping_ISB[mask_srai] =  Instruction("srai", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_srai);
    cmd_mapping_ISB[mask_csrrw] = Instruction("csrrw", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrw);
    cmd_mapping_ISB[mask_csrrs] = Instruction("csrrs", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrs);
    cmd_mapping_ISB[mask_csrrc] = Instruction("csrrc", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrc);
    cmd_mapping_ISB[mask_csrrwi] = Instruction("csrrwi", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrwi);
    cmd_mapping_ISB[mask_csrrsi] = Instruction("csrrsi", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrsi);
    cmd_mapping_ISB[mask_csrrci] = Instruction("csrrci", &RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrci);

    // U/J types
    cmd_mapping_UJ[mask_lui] =   Instruction("lui", &RiscvProcessor::decode_U,  &RiscvProcessor::execute_lui);
    cmd_mapping_UJ[mask_auipc] = Instruction("auipc", &RiscvProcessor::decode_U,  &RiscvProcessor::execute_auipc);
    cmd_mapping_UJ[mask_jal] =   Instruction("jal", &RiscvProcessor::decode_J,  &RiscvProcessor::execute_jal);

    // SYSTEM types
    cmd_mapping_SYSTEM[mask_fence] =  Instruction("fence", nullptr,  nullptr);
    cmd_mapping_SYSTEM[mask_mret] =   Instruction("mret", nullptr,  &RiscvProcessor::execute_mret);
    cmd_mapping_SYSTEM[mask_ebreak] = Instruction("ebreak", nullptr,  &RiscvProcessor::execute_ebreak);
    cmd_mapping_SYSTEM[mask_ecall] =  Instruction("ecall", nullptr,  &RiscvProcessor::execute_ecall);
}

void RiscvProcessor::SetVerbose(const bool verbose)
{
  m_verbose = verbose;
}

void RiscvProcessor::Reset()
{
    m_pc = 0;
    m_instruction_count = 0;

    // Reset all registers to zero
    std::for_each(m_reg.begin(),
                  m_reg.end(),
                  [&](unsigned int& x) mutable { x = 0; });

    // Reset privilege level
    m_prv = PRV_MACHINE;

    // Reset csr registers
    m_csr_table[csr_mvendorid] = 0;
    m_csr_table[csr_marchid] = 0;
    m_csr_table[csr_mimpid] = 0x20190200;
    m_csr_table[csr_mhartid] = 0;
    m_csr_table[csr_mstatus] = 0;
    m_csr_table[csr_misa] = 0x40100100;
    m_csr_table[csr_mie] = 0;
    m_csr_table[csr_mtvec] = 0;
    m_csr_table[csr_mscratch] = 0;
    m_csr_table[csr_mepc] = 0;
    m_csr_table[csr_mcause] = 0;
    m_csr_table[csr_mtval] = 0;
    m_csr_table[csr_mip] = 0;
}

RiscvProcessor::Register RiscvProcessor::GetPC() const
{
    return m_pc;
}

void RiscvProcessor::SetPC(const Register newValue)
{
    m_pc = newValue;
}

RiscvProcessor::Register RiscvProcessor::GetReg(const unsigned regNum) const
{
    if (regNum > 31)
    {
        std::stringstream ss;
        ss << "register " << regNum << " exceeds registers x0..x31";
        throw std::out_of_range(ss.str());
    }

    return m_reg[regNum];
}

void RiscvProcessor::SetReg(const unsigned regNum, const Register newValue)
{
    if (regNum > 31)
    {
        std::stringstream ss;
        ss << "register " << regNum << " exceeds registers x0..x31";
        throw std::out_of_range(ss.str());
    }

    m_reg[regNum] = newValue;
}

unsigned long RiscvProcessor::GetInstructionCount() const
{
    return m_instruction_count;
}

void RiscvProcessor::SetPrivilegeLevel(const uint8_t prv)
{
    m_prv = prv;
}

uint32_t RiscvProcessor::GetCSRValue(const uint32_t csr_num) const
{
    uint32_t val = 0;

    switch (csr_num) {
        // Only allow valid CSR values
        case csr_mvendorid:
        case csr_marchid:
        case csr_mimpid:
        case csr_mhartid:
        case csr_mstatus:
        case csr_misa:
        case csr_mie:
        case csr_mtvec:
        case csr_mscratch:
        case csr_mepc:
        case csr_mcause:
        case csr_mtval:
        case csr_mip:
            val = m_csr_table.at(csr_num);
            break;

        default:
            {
                std::stringstream ss;
                ss << "csr number " << csr_num << " is invalid";
                throw std::invalid_argument(ss.str());
            }
    }

    return val;
}

RiscvProcessor::set_csr_result RiscvProcessor::SetCSRValue(const uint32_t csr_num, const uint32_t new_value)
{
    set_csr_result ret;
    ret.set_csr_user_mode = m_prv == PRV_USER;

    // Differnet csr numbers have different methods of update:
    switch (csr_num) {
        case csr_mvendorid:
        case csr_marchid:
        case csr_mimpid:
        case csr_mhartid:
            // Fixed values (illegal)
            ret.set_csr_read_only = true;
            break;

        case csr_misa:
            // Fixed values (not illegal)
            ret.set_csr_read_only = true;
            break;

        case csr_mstatus:
            // mask to only allow:
            //  mpp  12:11
            //  mpie  7
            //  mie   3
            m_csr_table[csr_num] = new_value & 0x1888;
            break;

        case csr_mie:
            // mask to only allow:
            //  meie  11 
            //  ueie  8
            //  mtie  7
            //  utie  4
            //  msie  3
            //  usie  0
            m_csr_table[csr_num] = new_value & 0x999;
            break;

        case csr_mtvec:
            if ((new_value & 0x1) == 1)
            {
                // fix bits 1 to 6 to 0
                m_csr_table[csr_num] = new_value & 0xFFFFFF81;
            }
            else
            {
                // fix bit 1 to 0
                m_csr_table[csr_num] = new_value & 0xFFFFFFFD;
            }
            break;

        case csr_mepc:
            // Bits 0 and 1 fixed to 0
            m_csr_table[csr_num] = new_value & 0xFFFFFFFC;
            break;

        case csr_mcause:
            // Only interrupt bit and 4-bit exception code field implemented
            //   interrupt bit    31
            //   4-bit exception  0:3
            m_csr_table[csr_num] = new_value & 0x8000000F;
            break;

        case csr_mip:
            // only the follwing bits set:
            //  meip  11  R
            //  ueip  8   RW
            //  mtip  7   R
            //  utip  4   RW
            //  msip  3   R
            //  usip  0   RW
            // Can only write to R/W fields
            m_csr_table[csr_num] = (m_csr_table[csr_num] & ~0x111) | (new_value & 0x111);
            break;

        // All other CSR values
        case csr_mscratch:
        case csr_mtval:
            // No specific rule applies
            m_csr_table[csr_num] = new_value;
            break;

        default:
            // Invalid number
            ret.set_csr_undefined_num = true;
    }

    return ret;
}

void RiscvProcessor::Step()
{
    // Fetch command at PC
    uint32_t instruction = m_mem.ReadWord(m_pc);

    // Execute command
    ExecuteCmd(instruction);

    // Increase PC
    m_pc += 4;  // increase by a word
    m_instruction_count++;

    // TODO check for breakpoint
}

void RiscvProcessor::ExecuteCmd(const uint32_t cmd)
{
  if (m_verbose)
  {
    std::cout << "instruction 0x";
    std::cout << std::setw(8) << std::setfill('0') << std::hex << cmd;
    std::cout << " ...    ";
  }

  // Exception and interrupt checking
  if (m_pc % 4 != 0) {
      RaiseException(ex_instruction_address_misaligned);
      return;
  }

  // Load bits for interrupts
  uint32_t mip = m_csr_table[csr_mip];
  uint32_t mip_usip = (mip >> 0) & 0x1;
  uint32_t mip_msip = (mip >> 3) & 0x1;
  uint32_t mip_utip = (mip >> 4) & 0x1;
  uint32_t mip_mtip = (mip >> 7) & 0x1;
  uint32_t mip_ueip = (mip >> 8) & 0x1;
  uint32_t mip_meip = (mip >> 11) & 0x1;

  uint32_t mie = m_csr_table[csr_mie];
  uint32_t mie_usie = (mie >> 0) & 0x1;
  uint32_t mie_msie = (mie >> 3) & 0x1;
  uint32_t mie_utie = (mie >> 4) & 0x1;
  uint32_t mie_mtie = (mie >> 7) & 0x1;
  uint32_t mie_ueie = (mie >> 8) & 0x1;
  uint32_t mie_meie = (mie >> 11) & 0x1;

  uint32_t mstatus_mie = (m_csr_table[csr_mstatus] >> 3) & 0x1;

  // Trigger interrupts
  // Machine external interrupt:
  if (mip_meip && mie_meie && ((mstatus_mie && m_prv == PRV_MACHINE) || (~mstatus_mie && m_prv == PRV_USER))) {
    RaiseException(ex_machine_external_interrupt);
    m_pc += 4;
    m_instruction_count++;
    return;
  }
  // Machine software interrupt:
  if (mip_msip && mie_msie && ((mstatus_mie && m_prv == PRV_MACHINE) || (~mstatus_mie && m_prv == PRV_USER))) {
    RaiseException(ex_machine_software_interrupt);
    m_pc += 4;
    m_instruction_count++;
    return;
  }
  // Machine timer interrupt
  if (mip_mtip && mie_mtie && ((mstatus_mie && m_prv == PRV_MACHINE) || (~mstatus_mie && m_prv == PRV_USER))) {
    RaiseException(ex_machine_timer_interrupt);
    m_pc += 4;
    m_instruction_count++;
    return;
  }
  // User external interrupt
  if (mip_ueip && mie_ueie && ((mstatus_mie && m_prv == PRV_MACHINE) || (~mstatus_mie && m_prv == PRV_USER))) {
    RaiseException(ex_user_external_interrupt);
    m_pc += 4;
    m_instruction_count++;
    return;
  }
  // User software interrupt:
  if (mip_usip && mie_usie && ((mstatus_mie && m_prv == PRV_MACHINE) || (~mstatus_mie && m_prv == PRV_USER))) {
    RaiseException(ex_user_software_interrupt);
    m_pc += 4;
    m_instruction_count++;
    return;
  }
  // User timer interrupt:
  if (mip_utip && mie_utie && ((mstatus_mie && m_prv == PRV_MACHINE) || (~mstatus_mie && m_prv == PRV_USER))) {
    RaiseException(ex_user_timer_interrupt);
    m_pc += 4;
    m_instruction_count++;
    return;
  }

    // Get and run the instruction decoder and executor
    bool instructionFound = false;
    Instruction instruction;
    // InstructionDecoder decoder;
    // InstructionExector executor;

    // Try R types
    if (cmd_mapping_R.find(cmd & mask_R) != cmd_mapping_R.end()) {
        instruction = cmd_mapping_R[cmd & mask_R];
        instructionFound = true;
    }
    // Try I/S/B types
    else if (cmd_mapping_ISB.find(cmd & mask_ISB) != cmd_mapping_ISB.end()) {
        instruction = cmd_mapping_ISB[cmd & mask_ISB];
        instructionFound = true;
    }
    // Try U/J types
    else if (cmd_mapping_UJ.find(cmd & mask_UJ) != cmd_mapping_UJ.end()) {
        instruction = cmd_mapping_UJ[cmd & mask_UJ];
        instructionFound = true;
    }
    // Try "System" instructions
    else if (cmd_mapping_SYSTEM.find(cmd & mask_SYSTEM) != cmd_mapping_SYSTEM.end()) {
        instruction = cmd_mapping_SYSTEM[cmd & mask_SYSTEM];
        instructionFound = true;
    }

    // Finally... decode the instruction and execute
    if (instructionFound)
    {
      if (m_verbose)
      {
        VerbosePrintInstruction(instruction);
      }

      if (instruction.decoder != nullptr)
      {
          (this->*instruction.decoder)(cmd);
      }

      if (instruction.executor != nullptr)
      {
          (this->*instruction.executor)();
      }

      if (m_verbose)
      {
        std::cout << std::endl;
      }
      return;
    }

    // No instruction matched
    if (m_verbose)
    {
      std::cout << "unknown instruction" << std::endl;
    }
    RaiseException(ex_illegal_instruction);
}

void RiscvProcessor::VerbosePrintInstruction(const RiscvProcessor::Instruction& inst)
{
  if (inst.decoder == &RiscvProcessor::decode_R)
  {
    // R type
    std::cout << std::setw(6);
    std::cout << std::setfill(' ');
    std::cout << std::left;
    std::cout << inst.displayName;

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rd;
    std::cout << ",";

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rs1;
    std::cout << ",";

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rs2;
  }
  else if (inst.decoder == &RiscvProcessor::decode_I)
  {
    // I type
    std::cout << std::setw(6);
    std::cout << std::setfill(' ');
    std::cout << std::left;
    std::cout << inst.displayName;

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rd;
    std::cout << ",";

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rs1;
    std::cout << ",";

    std::cout << std::setw(0);
    std::cout << std::setfill(' ');
    std::cout << std::hex << m_decoded_imm;
  }
  else if (inst.decoder == &RiscvProcessor::decode_S)
  {
    // S type
    std::cout << std::setw(6);
    std::cout << std::setfill(' ');
    std::cout << std::left;
    std::cout << inst.displayName;

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rs1;
    std::cout << ",";

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rs2;
    std::cout << ",";

    std::cout << std::setw(0);
    std::cout << std::setfill(' ');
    std::cout << std::hex << m_decoded_imm;
  }
  else if (inst.decoder == &RiscvProcessor::decode_B)
  {
    // B type
    std::cout << std::setw(6);
    std::cout << std::setfill(' ');
    std::cout << std::left;
    std::cout << inst.displayName;

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rs1;
    std::cout << ",";

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rs2;
    std::cout << ",";

    std::cout << std::setw(0);
    std::cout << std::setfill(' ');
    std::cout << std::hex << m_decoded_imm;
  }
  else if (inst.decoder == &RiscvProcessor::decode_U)
  {
    // U type
    std::cout << std::setw(6);
    std::cout << std::setfill(' ');
    std::cout << std::left;
    std::cout << inst.displayName;

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rd;

    std::cout << std::setw(0);
    std::cout << std::setfill(' ');
    std::cout << "," << std::hex << m_decoded_imm;
  }
  else if (inst.decoder == &RiscvProcessor::decode_J)
  {
    // J type
    std::cout << std::setw(6);
    std::cout << std::setfill(' ');
    std::cout << std::left;
    std::cout << inst.displayName;

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rd;

    std::cout << std::setw(0);
    std::cout << std::setfill(' ');
    std::cout << "," << std::hex << m_decoded_imm;
  }
  else if (inst.decoder == nullptr)
  {
    // SYSTEM type
    std::cout << std::setw(6);
    std::cout << std::setfill(' ');
    std::cout << std::left;
    std::cout << inst.displayName;
  }
  else
  {
    std::cout << "unknown for v";
  }
}


// Exception handling ----------------------------------------------------------
void RiscvProcessor::RaiseException(const RiscvProcessor::ExceptionReg& exception_data)
{
    // this didn't count as an instruction
    m_instruction_count--;

    // Set mcause
    uint32_t mcause = 0;
    mcause |= exception_data.interrupt << 31;
    mcause |= exception_data.exceptionCode & 0xF;
    SetCSRValue(csr_mcause, mcause);

    // Set mepc
    SetCSRValue(csr_mepc, m_pc);

    // Set mtval
    if (&exception_data == &ex_illegal_instruction)
    {
        // Store the instruction
        SetCSRValue(csr_mtval, m_mem.ReadWord(m_pc));
    }
    else if (&exception_data == &ex_instruction_address_misaligned)
    {
        // Store instr address
        SetCSRValue(csr_mtval, m_pc);
    }
    else if (&exception_data == &ex_load_address_misaligned ||
             &exception_data == &ex_store_address_misaligned)
    {
        // Store mem address
        SetCSRValue(csr_mtval, m_reg[m_decoded_rs1] + m_decoded_imm);
    }
    else
    {
        // Other instructions - set to 0
        SetCSRValue(csr_mtval, 0);
    }

    // Push interrupt enable stack (ie)
    uint32_t mstatus;

    mstatus = m_csr_table[csr_mstatus];
    uint32_t mie = (mstatus >> 3) & 0x1;
    mstatus = (mstatus & ~(0x1 << 3)) | 0 << 3; 
    mstatus = (mstatus & ~(0x1 << 7)) | mie << 7; 
    SetCSRValue(csr_mstatus, mstatus);

    // Push privilege level stack (mpp) (copy prv to mstatus and to go machine prv)
    mstatus = m_csr_table[csr_mstatus];
    mstatus = (mstatus & ~(0x3 << 11)) | m_prv << 11; // replace bits 12:11 with privilege level
    SetCSRValue(csr_mstatus, mstatus);
    SetPrivilegeLevel(PRV_MACHINE);

    // transfer to mtvec base
    uint32_t base = m_csr_table[csr_mtvec] & 0xFFFFFFFC;
    uint32_t mode = m_csr_table[csr_mtvec] & 0x1;
    // in vectored mode, need to add 4*cause
    if (mode == 1 && exception_data.interrupt == 1) {
        base += 4 * m_csr_table[csr_mcause];
    }
    m_pc = base - 4;
}


// Core instruction decoding ---------------------------------------------------
void RiscvProcessor::decode_R(uint32_t cmd) {
  // decode subfields
  m_decoded_rd = (cmd >> 7) & 0x1F;
  m_decoded_rs1 = (cmd >> 15) & 0x1F;
  m_decoded_rs2 = (cmd >> 20) & 0x1F;
}

void RiscvProcessor::decode_I(uint32_t cmd) {
  m_decoded_rd = (cmd >> 7) & 0x1F;
  m_decoded_rs1 = (cmd >> 15) & 0x1F;
  m_decoded_imm = (cmd >> 20) & 0xFFF;

  // Perform sign extension
  uint32_t imm_sign = (cmd >> 31) & 0x1;
  if (imm_sign == 0x1) {
    // sign extend
    m_decoded_imm |= 0xFFFFF000;
  }
}

void RiscvProcessor::decode_S(uint32_t cmd) {
  m_decoded_rs1 = (cmd >> 15) & 0x1F;
  m_decoded_rs2 = (cmd >> 20) & 0x1F;
  m_decoded_imm = 0x0;
  m_decoded_imm |= (cmd >> 7) & 0x1F;   // imm[4:0] in cmd[11:7]
  m_decoded_imm |= (cmd >> 20) & 0xFE0; // imm[11:5] in cmd[31:25]

  // Perform sign extension
  uint32_t imm_sign = (cmd >> 31) & 0x1;
  if (imm_sign == 0x1) {
    // sign extend
    m_decoded_imm |= 0xFFFFF000;
  }  
}

void RiscvProcessor::decode_B(uint32_t cmd) {
  // decode subfields
  m_decoded_rs1 = (cmd >> 15) & 0x1F;
  m_decoded_rs2 = (cmd >> 20) & 0x1F;
  m_decoded_imm = 0x0;
  m_decoded_imm |= (cmd << 4) & 0x800;   // imm[11] in cmd[7]
  m_decoded_imm |= (cmd >> 7) & 0x1E;    // imm[4:1] in cmd[11:8]
  m_decoded_imm |= (cmd >> 20) & 0x7E0;  // imm[10:5] in cmd[30:25]
  m_decoded_imm |= (cmd >> 19) & 0x1000; // imm[12] in cmd[31]

  // Perform sign extension
  uint32_t imm_sign = (cmd >> 31) & 0x1;
  if (imm_sign == 0x1) {
    // sign extend
    m_decoded_imm |= 0xFFFFE000;
  }
}

void RiscvProcessor::decode_U(uint32_t cmd) {
  m_decoded_rd = (cmd >> 7) & 0x1F;  // cut out rd value
  m_decoded_imm = cmd & 0xFFFFF000;  // imm = cmd[31:12] (the remainder=0)
}

void RiscvProcessor::decode_J(uint32_t cmd) {
  m_decoded_rd = (cmd >> 7) & 0x1F;
  m_decoded_imm = 0x0;
  m_decoded_imm |= (cmd >> 20) & 0x7FE;    // imm[10:1] in cmd[30:21]
  m_decoded_imm |= (cmd >> 9) & 0x800;     // imm[11] in cmd[20]
  m_decoded_imm |= cmd & 0xFF000;          // imm[19:12] in cmd[19:12]
  m_decoded_imm |= (cmd >> 11) & 0x100000; // imm[20] in cmd[31]

  // Perform sign extension
  uint32_t imm_sign = (cmd >> 31) & 0x1;
  if (imm_sign == 0x1) {
    // sign extend
    m_decoded_imm |= 0xFFE00000;
  }
}


void RiscvProcessor::execute_lui() {
  SetReg(m_decoded_rd, m_decoded_imm);
}

void RiscvProcessor::execute_auipc() {
  uint32_t sum = m_decoded_imm + m_pc;
  SetReg(m_decoded_rd, sum);
}

void RiscvProcessor::execute_jal() {
  // store PC+4 in rd
  SetReg(m_decoded_rd, m_pc+4);
  // jump
  m_pc += m_decoded_imm - 4;
}

void RiscvProcessor::execute_jalr() {
  // save the pc register value before overwriting
  uint32_t m_pc_saved = m_pc+4;

  // Jump  
  m_pc = m_reg[m_decoded_rs1]+m_decoded_imm-4;  // -4 (execute function will increment by 4)
  m_pc -= m_pc % 2; // ensure alignment
  
  // write PC+4 to rd (do this last since rd and rs1 can be the same)
  SetReg(m_decoded_rd, m_pc_saved);
}

void RiscvProcessor::execute_lb() {
  uint32_t address = m_reg[m_decoded_rs1] + m_decoded_imm;
  uint32_t data = m_mem.ReadWord(address);

  // get the byte out of the word
  uint32_t subword = address%4;
  data &= 0xFF << (subword*8);
  data >>= subword*8;

  // sign extend
  uint32_t data_sign = (data >> 7) & 0x1;
  if (data_sign == 0x1) {
    // sign extend
    data |= 0xFFFFFF00;
  }

  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_lh() {
  uint32_t address = m_reg[m_decoded_rs1] + m_decoded_imm;
  uint32_t data = m_mem.ReadWord(address);

  // Check address alignment
  if (address % 2 != 0) {
    // Alignment exception
    RaiseException(ex_load_address_misaligned);
    return;
  }

  // get the imporant halfword out of the data
  uint32_t subword = address%4;  
  data &= 0xFFFF << (subword*8);
  data >>= subword*8;

  if (subword == 3) {
    // need to read from the next word
    uint32_t data2 = m_mem.ReadWord(address+4);
    data |= (data2 << 8) & 0x0000FF00;
  }

  // sign extend
  uint32_t data_sign = (data >> 15) & 0x1;
  if (data_sign == 0x1) {
    // sign extend
    data |= 0xFFFF0000;
  }

  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_lw() {
  uint32_t address = m_reg[m_decoded_rs1] + m_decoded_imm;
  uint32_t data = m_mem.ReadWord(address);

  // Check address alignment
  if (address % 4 != 0) {
    // Alignment exception
    RaiseException(ex_load_address_misaligned);
    return;
  }

  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_lbu() {
  uint32_t address = m_reg[m_decoded_rs1] + m_decoded_imm;
  uint32_t data = m_mem.ReadWord(address);

  // get the byte out of the word
  uint32_t subword = address%4;
  data &= 0xFF << (subword*8);
  data >>= subword*8;

  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_lhu() {
  uint32_t address = m_reg[m_decoded_rs1] + m_decoded_imm;
  uint32_t data = m_mem.ReadWord(address);

  // Check address alignment
  if (address % 2 != 0) {
    // Alignment exception
    RaiseException(ex_load_address_misaligned);
    return;
  }

  // get the imporant halfword out of the data
  uint32_t subword = address%4;
  data &= 0xFFFF << (subword*8);
  data >>= subword*8;

  if (subword == 3) {
    // need to read from the next word
    uint32_t data2 = m_mem.ReadWord(address+4);
    data |= (data2 << 8) & 0x0000FF00;
  }

  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_sb() {
  uint32_t address = m_reg[m_decoded_rs1] + m_decoded_imm;
  uint32_t data = m_reg[m_decoded_rs2];

  // put the byte in the write memory byte
  uint32_t alignment = address%4;
  uint32_t mask = 0xFF << (8*alignment);
  data <<= 8*alignment;

  m_mem.WriteWord(address, data, mask);
}

void RiscvProcessor::execute_sh() {
  uint32_t address = m_reg[m_decoded_rs1] + m_decoded_imm;
  uint32_t data = m_reg[m_decoded_rs2];

  // Check address alignment
  if (address % 2 != 0) {
    // Alignment exception
    RaiseException(ex_store_address_misaligned);
    return;
  }

  // put the byte in the write memory byte
  uint32_t alignment = address%4;
  uint32_t mask = 0xFFFF << (8*alignment);
  data <<= 8*alignment;

  m_mem.WriteWord(address, data, mask);

  if (alignment == 3) {
    // need two words for this
    // write the upper word
    uint32_t data2 = (data >> 24) & 0xFF;
    m_mem.WriteWord(address+4, data2, 0xFF);
  }
}

void RiscvProcessor::execute_sw() {
  uint32_t address = m_reg[m_decoded_rs1] + m_decoded_imm;   // the memory class will align this
  uint32_t data = m_reg[m_decoded_rs2];

  // Check address alignment
  if (address % 4 != 0) {
    // Alignment exception
    RaiseException(ex_store_address_misaligned);
    return;
  }

  m_mem.WriteWord(address, data, 0xFFFFFFFF);
}

void RiscvProcessor::execute_beq() {
  if (m_reg[m_decoded_rs1] == m_reg[m_decoded_rs2]) {
    // Perform jump
    m_pc += m_decoded_imm-4;  // -4 (execute function will increment by 4)
  }
}

void RiscvProcessor::execute_bne() {
  if (m_reg[m_decoded_rs1] != m_reg[m_decoded_rs2]) {
    // Perform jump
    m_pc += m_decoded_imm-4;  // -4 (execute function will increment by 4)
  }
}

void RiscvProcessor::execute_blt() {
  if ((int32_t)m_reg[m_decoded_rs1] < (int32_t)m_reg[m_decoded_rs2]) {
    // Perform jump
    m_pc += m_decoded_imm-4;  // -4 (execute function will increment by 4)
  }
}

void RiscvProcessor::execute_bge() {
  if ((int32_t)m_reg[m_decoded_rs1] >= (int32_t)m_reg[m_decoded_rs2]) {
    // Perform jump
    m_pc += m_decoded_imm-4;  // -4 (execute function will increment by 4)
  }
}

void RiscvProcessor::execute_bltu() {
  if ((uint32_t)m_reg[m_decoded_rs1] < (uint32_t)m_reg[m_decoded_rs2]) {
    // Perform jump
    m_pc += m_decoded_imm-4;  // -4 (execute function will increment by 4)
  }
}

void RiscvProcessor::execute_bgeu() {
  if ((uint32_t)m_reg[m_decoded_rs1] >= (uint32_t)m_reg[m_decoded_rs2]) {
    // Perform jump
    m_pc += m_decoded_imm-4;  // -4 (execute function will increment by 4)
  }
}

void RiscvProcessor::execute_addi() {
  // rd = reg[rs1] + imm
  int32_t rs1_data = m_reg[m_decoded_rs1];
  SetReg(m_decoded_rd, rs1_data+m_decoded_imm);
}

void RiscvProcessor::execute_slti() {
  if ((int32_t)m_reg[m_decoded_rs1] < (int32_t)m_decoded_imm) {
    // Write 1
    SetReg(m_decoded_rd, 0x1);
  } else {
    SetReg(m_decoded_rd, 0x0);
  }
}

void RiscvProcessor::execute_sltiu() {
  if ((uint32_t)m_decoded_imm == 0x1) {
    // special case
    if (m_reg[m_decoded_rs1] == 0) {
      SetReg(m_decoded_rd, 0x1);
    } else {
      SetReg(m_decoded_rd, 0x0);
    }
  } else {
    if ((uint32_t)m_reg[m_decoded_rs1] < (uint32_t)m_decoded_imm) {
      // Write 1
      SetReg(m_decoded_rd, 0x1);
    } else {
      SetReg(m_decoded_rd, 0x0);
    }
  }
}

void RiscvProcessor::execute_xori() {
  SetReg(m_decoded_rd, m_reg[m_decoded_rs1] ^ m_decoded_imm);
}

void RiscvProcessor::execute_ori() {
 SetReg(m_decoded_rd, m_reg[m_decoded_rs1] | m_decoded_imm);
}

void RiscvProcessor::execute_andi() {
  SetReg(m_decoded_rd, m_reg[m_decoded_rs1] & m_decoded_imm);
}

void RiscvProcessor::execute_slli() {
  uint32_t shamt = m_decoded_imm & 0x1F;
  uint32_t data = m_reg[m_decoded_rs1] << shamt;
  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_srli() {
  uint32_t shamt = m_decoded_imm & 0x1F;
  uint32_t data = m_reg[m_decoded_rs1] >> shamt;
  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_srai() {
  uint32_t shamt = m_decoded_imm & 0x1F;
  uint32_t data = m_reg[m_decoded_rs1];
  
  uint32_t data_sign = (data >> 31) & 0x1;  // store sign before shift
  data >>= shamt;  // actually perform shift

  // sign extend
  if (data_sign == 0x1) {
    // sign extend - fill by "shamt" ones
    for (int i = 31-shamt; i < 32; ++i) {
      data |= (0x1 << i);
    }
  }

  SetReg(m_decoded_rd, data);

}

void RiscvProcessor::execute_add() {
  int32_t sum = m_reg[m_decoded_rs1] + m_reg[m_decoded_rs2];
  SetReg(m_decoded_rd, sum);
}

void RiscvProcessor::execute_sub() {
  int32_t sum = m_reg[m_decoded_rs1] - m_reg[m_decoded_rs2];
  SetReg(m_decoded_rd, sum);
}

void RiscvProcessor::execute_sll() {
  uint32_t shamt = m_reg[m_decoded_rs2] & 0x1F;
  uint32_t data = m_reg[m_decoded_rs1] << shamt;
  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_slt() {
  if ((int32_t)m_reg[m_decoded_rs1] < (int32_t)m_reg[m_decoded_rs2]) {
    // Write 1
    SetReg(m_decoded_rd, 0x1);
  } else {
    SetReg(m_decoded_rd, 0x0);
  }
}

void RiscvProcessor::execute_sltu() {
  if ((uint32_t)m_reg[m_decoded_rs2] == 0x1) {
    // special case
    if (m_reg[m_decoded_rs1] == 0) {
      SetReg(m_decoded_rd, 0x1);
    } else {
      SetReg(m_decoded_rd, 0x0);
    }
  } else {
    if ((uint32_t)m_reg[m_decoded_rs1] < (uint32_t)m_reg[m_decoded_rs2]) {
      // Write 1
      SetReg(m_decoded_rd, 0x1);
    } else {
      SetReg(m_decoded_rd, 0x0);
    }
  }
}

void RiscvProcessor::execute_xor() {
  SetReg(m_decoded_rd, m_reg[m_decoded_rs1] ^ m_reg[m_decoded_rs2]);
}

void RiscvProcessor::execute_srl() {
  uint32_t shamt = m_reg[m_decoded_rs2] & 0x1F;
  uint32_t data = m_reg[m_decoded_rs1] >> shamt;
  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_sra() {
  uint32_t shamt = m_reg[m_decoded_rs2] & 0x1F;
  uint32_t data = m_reg[m_decoded_rs1];
  
  uint32_t data_sign = (data >> 31) & 0x1;  // store sign before shift
  data >>= shamt;  // actually perform shift

  // sign extend
  if (data_sign == 0x1) {
    // sign extend - fill by "shamt" ones
    for (int i = 31-shamt; i < 32; ++i) {
      data |= (0x1 << i);
    }
  }

  SetReg(m_decoded_rd, data);
}

void RiscvProcessor::execute_or() {
  SetReg(m_decoded_rd, m_reg[m_decoded_rs1] | m_reg[m_decoded_rs2]);
}

void RiscvProcessor::execute_and() {
  SetReg(m_decoded_rd, m_reg[m_decoded_rs1] & m_reg[m_decoded_rs2]);
}

// Privilege handling ----------------------------------------------------------
void RiscvProcessor::execute_ebreak() {
  RaiseException(ex_breakpoint);
}

void RiscvProcessor::execute_mret() {
  // Exception: User mode
  if (m_prv == PRV_USER) {
    RaiseException(ex_illegal_instruction);
    return;
  }


  // Recover exception
  m_pc = m_csr_table[csr_mepc] - 4;

  // Read mstatus
  uint32_t mstatus = m_csr_table[csr_mstatus];
  uint32_t mpp = (mstatus >> 11) & 0x3;
  uint32_t mpie = (mstatus >> 7) & 0x1;
  uint32_t mie = (mstatus >> 3) & 0x1;

  // change prv to MPP
  SetPrivilegeLevel(mpp);

  // MIE set to MPIE, MPIE set to 1, MPP set to U
  mie = mpie;
  mpie = 1;
  mpp = PRV_USER;
  mstatus = 0;
  mstatus |= (mpp & 0x3) << 11;
  mstatus |= (mpie & 0x1) << 7;
  mstatus |= (mie & 0x1) << 3;
  SetCSRValue(csr_mstatus, mstatus);
}

void RiscvProcessor::execute_ecall() {
  // Exception checking
  if (m_prv == PRV_USER) {
    RaiseException(ex_environment_call_from_Umode);
  } else if (m_prv == PRV_MACHINE) {
    RaiseException(ex_environment_call_from_Mmode);
  }
}


// CSR instruction handling ----------------------------------------------------
void RiscvProcessor::execute_csrrw() {
  uint32_t csr_num = m_decoded_imm & 0xFFF;  // remove sign extension

  // Read values
  uint32_t csr_val = m_csr_table[csr_num];
  uint32_t reg_rs1 = m_reg[m_decoded_rs1];
  uint32_t reg_rd = m_reg[m_decoded_rd];

  // Write values
  set_csr_result ret = SetCSRValue(csr_num, reg_rs1);
  SetReg(m_decoded_rd, csr_val);

  // Illegal instruction Exception check
  if (ret.set_csr_read_only || ret.set_csr_undefined_num || ret.set_csr_user_mode) {
    // Undo work
    SetReg(m_decoded_rd, reg_rd);
    if (ret.set_csr_user_mode)   // also undo set_csr if in user mode
      SetCSRValue(csr_num, csr_val);

    // Raise expcetion
    RaiseException(ex_illegal_instruction);
    return;
  }
}

void RiscvProcessor::execute_csrrs() {
  uint32_t csr_num = m_decoded_imm & 0xFFF;  // remove sign extension

  // Read values
  uint32_t csr_val = m_csr_table[csr_num];
  uint32_t reg_rs1 = m_reg[m_decoded_rs1];
  uint32_t reg_rd = m_reg[m_decoded_rd];

  // Write reg
  SetReg(m_decoded_rd, csr_val);

  // Set bits in reg
  uint32_t csr_val_new = csr_val | reg_rs1;

  set_csr_result ret;
  if (m_decoded_rs1 != 0)
    ret = SetCSRValue(csr_num, csr_val_new);


  // Illegal instruction Exception check
  if (ret.set_csr_read_only || ret.set_csr_undefined_num || ret.set_csr_user_mode) {
    // Undo work
    SetReg(m_decoded_rd, reg_rd);
    if (ret.set_csr_user_mode)   // also undo set_csr if in user mode
      SetCSRValue(csr_num, csr_val);

    // Raise expcetion
    RaiseException(ex_illegal_instruction);
    return;
  }
}

void RiscvProcessor::execute_csrrc() {
  uint32_t csr_num = m_decoded_imm & 0xFFF;  // remove sign extension

  // Read values
  uint32_t csr_val = m_csr_table[csr_num];
  uint32_t reg_rs1 = m_reg[m_decoded_rs1];
  uint32_t reg_rd = m_reg[m_decoded_rd];

  // Write reg
  SetReg(m_decoded_rd, csr_val);

  // Set bits in reg
  uint32_t csr_val_new = csr_val & ~reg_rs1;
  
  set_csr_result ret;
  if (m_decoded_rs1 != 0)
    ret = SetCSRValue(csr_num, csr_val_new);


  // Illegal instruction Exception check
  if (ret.set_csr_read_only || ret.set_csr_undefined_num || ret.set_csr_user_mode) {
    // Undo work
    SetReg(m_decoded_rd, reg_rd);
    if (ret.set_csr_user_mode)   // also undo set_csr if in user mode
      SetCSRValue(csr_num, csr_val);

    // Raise expcetion
    RaiseException(ex_illegal_instruction);
    return;
  }
}

void RiscvProcessor::execute_csrrwi() {
  uint32_t csr_num = m_decoded_imm & 0xFFF;  // remove sign extension

  // Read values
  uint32_t csr_val = m_csr_table[csr_num];
  uint32_t reg_rd = m_reg[m_decoded_rd];

  // Write values
  set_csr_result ret = SetCSRValue(csr_num, m_decoded_rs1);
  SetReg(m_decoded_rd, csr_val);


  // Illegal instruction Exception check
  if (ret.set_csr_read_only || ret.set_csr_undefined_num || ret.set_csr_user_mode) {
    // Undo work
    SetReg(m_decoded_rd, reg_rd);
    if (ret.set_csr_user_mode)   // also undo set_csr if in user mode
      SetCSRValue(csr_num, csr_val);

    // Raise expcetion
    RaiseException(ex_illegal_instruction);
    return;
  }
}

void RiscvProcessor::execute_csrrsi() {
  uint32_t csr_num = m_decoded_imm & 0xFFF;  // remove sign extension

  // Read values
  uint32_t csr_val = m_csr_table[csr_num];
  uint32_t reg_rd = m_reg[m_decoded_rd];

  // Write reg
  SetReg(m_decoded_rd, csr_val);

  // Set bits in reg
  uint32_t csr_val_new = csr_val | m_decoded_rs1;

  set_csr_result ret;
  if (m_decoded_rs1 != 0)
    ret = SetCSRValue(csr_num, csr_val_new);


  // Illegal instruction Exception check
  if (ret.set_csr_read_only || ret.set_csr_undefined_num || ret.set_csr_user_mode) {
    // Undo work
    SetReg(m_decoded_rd, reg_rd);
    if (ret.set_csr_user_mode)
    {
      // also undo set_csr if in user mode
      SetCSRValue(csr_num, csr_val);
    }

    // Raise expcetion
    RaiseException(ex_illegal_instruction);
    return;
  }
}

void RiscvProcessor::execute_csrrci() {
  uint32_t csr_num = m_decoded_imm & 0xFFF;  // remove sign extension

  // Read values
  uint32_t csr_val = m_csr_table[csr_num];
  uint32_t reg_rd = m_reg[m_decoded_rd];

  // Write reg
  SetReg(m_decoded_rd, csr_val);

  // Set bits in reg
  uint32_t csr_val_new = csr_val & ~m_decoded_rs1;

  set_csr_result ret;
  if (m_decoded_rs1 != 0)
  {
    ret = SetCSRValue(csr_num, csr_val_new);
  }


  // Illegal instruction Exception check
  if (ret.set_csr_read_only || ret.set_csr_undefined_num || ret.set_csr_user_mode) {
    // Undo work
    SetReg(m_decoded_rd, reg_rd);
    if (ret.set_csr_user_mode)
    {
      // also undo set_csr if in user mode
      SetCSRValue(csr_num, csr_val);
    }

    // Raise expcetion
    RaiseException(ex_illegal_instruction);
    return;
  }
}

} // namespace riscvdb
