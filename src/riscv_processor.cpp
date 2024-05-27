#include "riscv_processor.h"
#include <sstream>
#include <iostream>
#include <iomanip>

namespace riscvdb
{

RiscvProcessor::RiscvProcessor(MemoryMap& mem)
: m_mem(mem),
  m_pc(0),
  m_instruction_count(0)
{
    // Initialize all registers to zero
    std::for_each(m_reg.begin(),
                  m_reg.end(),
                  [&](unsigned int& x) mutable { x = 0; });

    // Initialize csr registers
    // TODO

    // Initialize privilege level
    // TODO

    // Setup lookup tables
    // R types
    cmd_mapping_R[mask_add] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_add);
    cmd_mapping_R[mask_add] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_add);
    cmd_mapping_R[mask_sub] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_sub);
    cmd_mapping_R[mask_sll] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_sll);
    cmd_mapping_R[mask_slt] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_slt);
    cmd_mapping_R[mask_sltu] =  Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_sltu);
    cmd_mapping_R[mask_xor] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_xor);
    cmd_mapping_R[mask_srl] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_srl);
    cmd_mapping_R[mask_sra] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_sra);
    cmd_mapping_R[mask_or] =    Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_or);
    cmd_mapping_R[mask_and] =   Instruction(&RiscvProcessor::decode_R,  &RiscvProcessor::execute_and);

    // I/S/B types
    cmd_mapping_ISB[mask_jalr] =  Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_jalr);
    cmd_mapping_ISB[mask_beq] =   Instruction(&RiscvProcessor::decode_B,  &RiscvProcessor::execute_beq);
    cmd_mapping_ISB[mask_bne] =   Instruction(&RiscvProcessor::decode_B,  &RiscvProcessor::execute_bne);
    cmd_mapping_ISB[mask_blt] =   Instruction(&RiscvProcessor::decode_B,  &RiscvProcessor::execute_blt);
    cmd_mapping_ISB[mask_bge] =   Instruction(&RiscvProcessor::decode_B,  &RiscvProcessor::execute_bge);
    cmd_mapping_ISB[mask_bltu] =  Instruction(&RiscvProcessor::decode_B,  &RiscvProcessor::execute_bltu);
    cmd_mapping_ISB[mask_bgeu] =  Instruction(&RiscvProcessor::decode_B,  &RiscvProcessor::execute_bgeu);
    cmd_mapping_ISB[mask_lb] =    Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_lb);
    cmd_mapping_ISB[mask_lh] =    Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_lh);
    cmd_mapping_ISB[mask_lw] =    Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_lw);
    cmd_mapping_ISB[mask_lbu] =   Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_lbu);
    cmd_mapping_ISB[mask_lhu] =   Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_lhu);
    cmd_mapping_ISB[mask_sb] =    Instruction(&RiscvProcessor::decode_S,  &RiscvProcessor::execute_sb);
    cmd_mapping_ISB[mask_sh] =    Instruction(&RiscvProcessor::decode_S,  &RiscvProcessor::execute_sh);
    cmd_mapping_ISB[mask_sw] =    Instruction(&RiscvProcessor::decode_S,  &RiscvProcessor::execute_sw);
    cmd_mapping_ISB[mask_addi] =  Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_addi);
    cmd_mapping_ISB[mask_slti] =  Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_slti);
    cmd_mapping_ISB[mask_sltiu] = Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_sltiu);
    cmd_mapping_ISB[mask_xori] =  Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_xori);
    cmd_mapping_ISB[mask_ori] =   Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_ori);
    cmd_mapping_ISB[mask_andi] =  Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_andi);
    cmd_mapping_ISB[mask_slli] =  Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_slli);
    cmd_mapping_ISB[mask_srli] =  Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_srli);
    cmd_mapping_ISB[mask_srai] =  Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_srai);
    // TODO add CSR instructions:
    // cmd_mapping_ISB[mask_csrrw] = Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrw);
    // cmd_mapping_ISB[mask_csrrs] = Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrs);
    // cmd_mapping_ISB[mask_csrrc] = Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrc);
    // cmd_mapping_ISB[mask_csrrwi] = Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrwi);
    // cmd_mapping_ISB[mask_csrrsi] = Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrsi);
    // cmd_mapping_ISB[mask_csrrci] = Instruction(&RiscvProcessor::decode_I,  &RiscvProcessor::execute_csrrci);

    // U/J types
    cmd_mapping_UJ[mask_lui] =   Instruction(&RiscvProcessor::decode_U,  &RiscvProcessor::execute_lui);
    cmd_mapping_UJ[mask_auipc] = Instruction(&RiscvProcessor::decode_U,  &RiscvProcessor::execute_auipc);
    cmd_mapping_UJ[mask_jal] =   Instruction(&RiscvProcessor::decode_J,  &RiscvProcessor::execute_jal);

    // SYSTEM types
    // TODO
    // cmd_mapping_SYSTEM[mask_fence] =  Instruction(NULL,  NULL);
    // cmd_mapping_SYSTEM[mask_mret] =   Instruction(NULL,  &RiscvProcessor::execute_mret);
    // cmd_mapping_SYSTEM[mask_ebreak] = Instruction(NULL,  &RiscvProcessor::execute_ebreak);
    // cmd_mapping_SYSTEM[mask_ecall] =  Instruction(NULL,  &RiscvProcessor::execute_ecall);
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
    std::cout << "instruction 0x";
    std::cout << std::setw(8) << std::setfill('0') << std::hex << cmd;
    std::cout << " ...  ";

    // Exception and interrupt checking
    if (m_pc % 4 != 0) {
        RaiseException(ex_instruction_address_misaligned);
        return;
    }

    // Load bits for interrupts
    // TODO CSR

    // Get and run the instruction decoder and executor
    bool instructionFound = false;
    InstructionDecoder decoder;
    InstructionExector executor;

    // Try R types
    if (cmd_mapping_R.find(cmd & mask_R) != cmd_mapping_R.end()) {
        decoder = cmd_mapping_R[cmd & mask_R].decoder;
        executor = cmd_mapping_R[cmd & mask_R].executor;
        instructionFound = true;
    }
    // Try I/S/B types
    else if (cmd_mapping_ISB.find(cmd & mask_ISB) != cmd_mapping_ISB.end()) {
        decoder = cmd_mapping_ISB[cmd & mask_ISB].decoder;
        executor = cmd_mapping_ISB[cmd & mask_ISB].executor;
        instructionFound = true;
    }
    // Try U/J types
    else if (cmd_mapping_UJ.find(cmd & mask_UJ) != cmd_mapping_UJ.end()) {
        decoder = cmd_mapping_UJ[cmd & mask_UJ].decoder;
        executor = cmd_mapping_UJ[cmd & mask_UJ].executor;
        instructionFound = true;
    }
    // Try "System" instructions
    else if (cmd_mapping_SYSTEM.find(cmd & mask_SYSTEM) != cmd_mapping_SYSTEM.end()) {
        decoder = cmd_mapping_SYSTEM[cmd & mask_SYSTEM].decoder;
        executor = cmd_mapping_SYSTEM[cmd & mask_SYSTEM].executor;
        instructionFound = true;
    }

    // Finally... decode the instruction and execute
    if (instructionFound)
    {
        if (decoder != NULL)
        {
            (this->*decoder)(cmd);
        }

        if (executor != NULL)
        {
            (this->*executor)();
        }

        std::cout << std::endl;
        return;
    }

    // No instruction matched
    std::cout << "unknown instruction" << std::endl;
    RaiseException(ex_illegal_instruction);
}


// Exception handling ----------------------------------------------------------
void RiscvProcessor::RaiseException(const RiscvProcessor::ExceptionReg& exception_data)
{
    // TODO
    throw std::runtime_error("RaiseException not implemented");
    (void)exception_data;
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
    // TODO move to verbose method
    std::cout << std::setw(6);
    std::cout << std::setfill(' ');
    std::cout << std::left;
    std::cout << "lui";

    std::cout << std::setw(0);
    std::cout << "x" << std::dec << m_decoded_rd;

    std::cout << std::setw(0);
    std::cout << std::setfill(' ');
    std::cout << "," << std::hex << m_decoded_imm;

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

} // namespace riscvdb
