#ifndef RISCVDB_PROCESSOR_H
#define RISCVDB_PROCESSOR_H

#include <cctype>
#include <array>
#include <string>
#include <unordered_map>
#include "memorymap.h"

namespace riscvdb
{

class RiscvProcessor
{
public:
    typedef uint32_t Register;
    RiscvProcessor(MemoryMap& mem);

    void SetVerbose(const bool verbose);

    void Reset();

    Register GetPC() const;
    void SetPC(const Register newValue);

    Register GetReg(const unsigned regNum) const;
    void SetReg(const unsigned regNum, const Register newValue);

    unsigned long long GetInstructionCount() const;

    // Privilege levels
    static const uint8_t PRV_USER;
    static const uint8_t PRV_MACHINE;
    void SetPrivilegeLevel(const uint8_t prv);

    // CSR registers
    static const uint32_t csr_mvendorid;
    static const uint32_t csr_marchid;
    static const uint32_t csr_mimpid;
    static const uint32_t csr_mhartid;
    static const uint32_t csr_mstatus;
    static const uint32_t csr_misa;
    static const uint32_t csr_mie;
    static const uint32_t csr_mtvec;
    static const uint32_t csr_mscratch;
    static const uint32_t csr_mepc;
    static const uint32_t csr_mcause;
    static const uint32_t csr_mtval;
    static const uint32_t csr_mip;

    // Exceptions
    struct Exception
    {
        uint32_t interrupt;
        uint32_t exceptionCode;

        bool operator==(const Exception& rhs) const
        {
            return interrupt == rhs.interrupt && exceptionCode == rhs.exceptionCode;
        }
    };

    static const Exception ex_user_software_interrupt;
    static const Exception ex_machine_software_interrupt;
    static const Exception ex_user_timer_interrupt;
    static const Exception ex_machine_timer_interrupt;
    static const Exception ex_user_external_interrupt;
    static const Exception ex_machine_external_interrupt;
    static const Exception ex_instruction_address_misaligned;
    static const Exception ex_illegal_instruction;
    static const Exception ex_breakpoint;
    static const Exception ex_load_address_misaligned;
    static const Exception ex_store_address_misaligned;
    static const Exception ex_environment_call_from_Umode;
    static const Exception ex_environment_call_from_Mmode;


    uint32_t GetCSRValue(const uint32_t csr_num) const;

    struct set_csr_result{
        bool set_csr_undefined_num = false;
        bool set_csr_read_only = false;
        bool set_csr_user_mode = false;
    };
    set_csr_result SetCSRValue(const uint32_t csr_num, const uint32_t new_value);

    // Run next instruction
    void Step();

private:
    // Basic machine data
    MemoryMap& m_mem;   // main memory
    Register m_pc;  // program counter
    std::array<Register, 32> m_reg; // x0..x31 machine registers

    // Debug/run info
    unsigned long long m_instruction_count;
    bool m_verbose;

    // Machine mode control and status registers (CSRs)
    std::unordered_map<uint32_t, uint32_t> m_csr_table;

    // Exceptions
    void RaiseException(const Exception& exception_data);

    // Privilege level
    uint8_t m_prv;

    // Instruction implementation: 
    // Templates for instruction decoding
    using InstructionDecoder = void (RiscvProcessor::*)(uint32_t);
    using InstructionExector = void (RiscvProcessor::*)(void);
    struct Instruction {
        std::string displayName;
        InstructionDecoder decoder;
        InstructionExector executor;

        Instruction() = default;

        Instruction(std::string n, InstructionDecoder d, InstructionExector e)
        : displayName(n),
          decoder(d),
          executor(e)
        {}
    };

    void VerbosePrintInstruction(const Instruction& inst);

    std::unordered_map<uint32_t, Instruction> cmd_mapping_R;
    std::unordered_map<uint32_t, Instruction> cmd_mapping_ISB;
    std::unordered_map<uint32_t, Instruction> cmd_mapping_UJ;
    std::unordered_map<uint32_t, Instruction> cmd_mapping_SYSTEM;

    void ExecuteCmd(const uint32_t instruction);

    // Instruction type masks
    static const uint32_t mask_R = 0xFE00707F;
    static const uint32_t mask_ISB = 0x707F;
    static const uint32_t mask_UJ = 0x7F;
    static const uint32_t mask_SYSTEM = 0xFFF0707F;

    // Decoding functions and temporaries
    int32_t m_decoded_imm;
    uint32_t m_decoded_rs1;
    uint32_t m_decoded_rs2;
    uint32_t m_decoded_rd;
    void decode_R(uint32_t cmd);
    void decode_I(uint32_t cmd);
    void decode_S(uint32_t cmd);
    void decode_B(uint32_t cmd);
    void decode_U(uint32_t cmd);
    void decode_J(uint32_t cmd);

    // Instruction masks
    const uint32_t mask_lui = 0x37;
    const uint32_t mask_auipc = 0x17;
    const uint32_t mask_jal = 0x6F;
    const uint32_t mask_jalr = 0x67;
    const uint32_t mask_beq = 0x63;
    const uint32_t mask_bne = 0x1063;
    const uint32_t mask_blt = 0x4063;
    const uint32_t mask_bge = 0x5063;
    const uint32_t mask_bltu = 0x6063;
    const uint32_t mask_bgeu = 0x7063;
    const uint32_t mask_lb = 0x3;
    const uint32_t mask_lh = 0x1003;
    const uint32_t mask_lw = 0x2003;
    const uint32_t mask_lbu = 0x4003;
    const uint32_t mask_lhu = 0x5003;
    const uint32_t mask_sb = 0x23;
    const uint32_t mask_sh = 0x1023;
    const uint32_t mask_sw = 0x2023;
    const uint32_t mask_addi = 0x13;
    const uint32_t mask_slti = 0x2013;
    const uint32_t mask_sltiu = 0x3013;
    const uint32_t mask_xori = 0x4013;
    const uint32_t mask_ori = 0x6013;
    const uint32_t mask_andi = 0x7013;
    const uint32_t mask_slli = 0x1013;
    const uint32_t mask_srli = 0x5013;
    const uint32_t mask_srai = 0x40005013;
    const uint32_t mask_add = 0x33;
    const uint32_t mask_sub = 0x40000033;
    const uint32_t mask_sll = 0x1033;
    const uint32_t mask_slt = 0x2033;
    const uint32_t mask_sltu = 0x3033;
    const uint32_t mask_xor = 0x4033;
    const uint32_t mask_srl = 0x5033;
    const uint32_t mask_sra = 0x40005033;
    const uint32_t mask_or = 0x6033;
    const uint32_t mask_and = 0x7033;
    const uint32_t mask_fence = 0xF;
    const uint32_t mask_ecall = 0x73;
    const uint32_t mask_ebreak = 0x100073;
    const uint32_t mask_mret = 0x30200073;
    const uint32_t mask_csrrw = 0x1073;
    const uint32_t mask_csrrs = 0x2073;
    const uint32_t mask_csrrc = 0x3073;  
    const uint32_t mask_csrrwi = 0x5073;
    const uint32_t mask_csrrsi = 0x6073;
    const uint32_t mask_csrrci = 0x7073;  

    // Executor functions
    void execute_lui();
    void execute_auipc();
    void execute_jal();
    void execute_jalr();
    void execute_beq();
    void execute_bne();
    void execute_blt();
    void execute_bge();
    void execute_bltu();
    void execute_bgeu();
    void execute_lb();
    void execute_lh();
    void execute_lw();
    void execute_lbu();
    void execute_lhu();
    void execute_sb();
    void execute_sh();
    void execute_sw();
    void execute_addi();
    void execute_slti();
    void execute_sltiu();
    void execute_xori();
    void execute_ori();
    void execute_andi();
    void execute_slli();
    void execute_srli();
    void execute_srai();
    void execute_add();
    void execute_sub();
    void execute_sll();
    void execute_slt();
    void execute_sltu();
    void execute_xor();
    void execute_srl();
    void execute_sra();
    void execute_or();
    void execute_and();
    void execute_ebreak();
    void execute_mret();
    void execute_ecall();
    void execute_csrrw();
    void execute_csrrs();
    void execute_csrrc();
    void execute_csrrwi();
    void execute_csrrsi();
    void execute_csrrci();
};

} // namespace riscvdb


#endif  // RISCVDB_PROCESSOR_H