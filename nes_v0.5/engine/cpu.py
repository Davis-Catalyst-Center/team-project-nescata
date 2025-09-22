from .bus import Bus
import time




class CPU:
  
  OPCODE_NAMES_MAP = [
    "BRK", "ORA", "KIL", "SLO", "DOP", "ORA", "ASL", "SLO", "PHP", "ORA", "ASL", "AAC", "TOP", "ORA", "ASL", "SLO",
    "BPL", "ORA", "KIL", "SLO", "DOP", "ORA", "ASL", "SLO", "CLC", "ORA", "NOP", "SLO", "TOP", "ORA", "ASL", "SLO",
    "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA", "PLP", "AND", "ROL", "AAC", "BIT", "AND", "ROL", "RLA",
    "BMI", "AND", "KIL", "RLA", "DOP", "AND", "ROL", "RLA", "SEC", "AND", "NOP", "RLA", "TOP", "AND", "ROL", "RLA",
    "RTI", "EOR", "KIL", "SRE", "DOP", "EOR", "LSR", "SRE", "PHA", "EOR", "LSR", "ASR", "JMP", "EOR", "LSR", "SRE",
    "BVC", "EOR", "KIL", "SRE", "DOP", "EOR", "LSR", "SRE", "CLI", "EOR", "NOP", "SRE", "TOP", "EOR", "LSR", "SRE",
    "RTS", "ADC", "KIL", "RRA", "DOP", "ADC", "ROR", "RRA", "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
    "BVS", "ADC", "KIL", "RRA", "DOP", "ADC", "ROR", "RRA", "SEI", "ADC", "NOP", "RRA", "TOP", "ADC", "ROR", "RRA",
    "DOP", "STA", "DOP", "AAX", "STY", "STA", "STX", "AAX", "DEY", "DOP", "TXA", "XAA", "STY", "STA", "STX", "AAX",
    "BCC", "STA", "KIL", "AXA", "STY", "STA", "STX", "AAX", "TYA", "STA", "TXS", "XAS", "SYA", "STA", "SXA", "AXA",
    "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", "TAY", "LDA", "TAX", "ATX", "LDY", "LDA", "LDX", "LAX",
    "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX", "CLV", "LDA", "TSX", "LAR", "LDY", "LDA", "LDX", "LAX",
    "CPY", "CMP", "DOP", "DCP", "CPY", "CMP", "DEC", "DCP", "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
    "BNE", "CMP", "KIL", "DCP", "DOP", "CMP", "DEC", "DCP", "CLD", "CMP", "NOP", "DCP", "TOP", "CMP", "DEC", "DCP",
    "CPX", "SBC", "DOP", "ISC", "CPX", "SBC", "INC", "ISC", "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
    "BEQ", "SBC", "KIL", "ISC", "DOP", "SBC", "INC", "ISC", "SED", "SBC", "NOP", "ISC", "TOP", "SBC", "INC", "ISC"
  ]
  
  OPCODE_DOC_MAP = bytearray([
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0,
    0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0,
  ])

  OPCODE_ADDR_MAP = bytearray([
    0, 8, 0, 8, 0, 2, 2, 2, 0, 1, 10, 1, 3,  3, 3, 3,
    0, 9, 0, 9, 0, 4, 4, 4, 0, 7, 0,  7, 6,  6, 6, 6,
    3, 8, 0, 8, 2, 2, 2, 2, 0, 1, 10, 1, 3,  3, 3, 3,
    0, 9, 0, 9, 0, 4, 4, 4, 0, 7, 0,  7, 6,  6, 6, 6,
    0, 8, 0, 8, 0, 2, 2, 2, 0, 1, 10, 1, 3,  3, 3, 3,
    0, 9, 0, 9, 0, 4, 4, 4, 0, 7, 0,  7, 6,  6, 6, 6,
    0, 8, 0, 8, 0, 2, 2, 2, 0, 1, 10, 1, 11, 3, 3, 3,
    0, 9, 0, 9, 0, 4, 4, 4, 0, 7, 0,  7, 6,  6, 6, 6,
    0, 8, 0, 8, 2, 2, 2, 2, 0, 0, 0,  1, 3,  3, 3, 3,
    0, 9, 0, 9, 4, 4, 5, 5, 0, 7, 0,  2, 7,  6, 5, 7,
    1, 8, 1, 8, 2, 2, 2, 2, 0, 1, 0,  2, 3,  3, 3, 3,
    0, 9, 0, 9, 4, 4, 5, 5, 0, 7, 0,  7, 6,  6, 7, 7,
    1, 8, 0, 8, 2, 2, 2, 2, 0, 1, 0,  1, 3,  3, 3, 3,
    0, 9, 0, 9, 0, 4, 4, 4, 0, 7, 0,  7, 6,  6, 6, 6,
    1, 8, 0, 8, 2, 2, 2, 2, 0, 1, 0,  1, 3,  3, 3, 3,
    0, 9, 0, 9, 0, 4, 4, 4, 0, 7, 0,  7, 6,  6, 6, 6
  ])

  OPCODE_SIZE_MAP = bytearray([
    1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    3, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3
  ])

  OPCODE_CYCLES_MAP = bytearray([
    7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
    2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
    2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7
  ])

  OPCODE_WRAP_MAP = bytearray([
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0
  ])

  OPCODE_PC_MAP = bytearray([
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1
  ])

  OPCODE_SIZE_PC_MAP = bytearray([
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 2, 2, 2, 2,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2
  ])



  IMPLIED    = 0
  IMMEDIATE  = 1
  ZEROPAGE   = 2
  ABSOLUTE   = 3
  ZEROPAGE_X = 4
  ZEROPAGE_Y = 5
  ABSOLUTE_X = 6
  ABSOLUTE_Y = 7
  INDIRECT_X = 8
  INDIRECT_Y = 9
  ACCUMULAT  = 10
  INDIRECT   = 11

  OP_ADDR_STRINGS = [
    "Implied",
    "Immediate",
    "ZeroPage",
    "Absolute",
    "ZeroPageX",
    "ZeroPageY",
    "AbsoluteX",
    "AbsoluteY",
    "IndirectX",
    "IndirectY",
    "Accumulator",
    "Indirect"
  ]

  IRQ = 0
  BRK = 1
  NMI = 2
  RESET = 3

  MASK_N = 0b10000000
  MASK_V = 0b01000000
  MASK_U = 0b00100000
  MASK_B = 0b00010000
  MASK_D = 0b00001000
  MASK_I = 0b00000100
  MASK_Z = 0b00000010
  MASK_C = 0b00000001

  STACK_PAGE = 0x0100

  INT_ADDR = [
    0xfffe,
    0xfffe,
    0xfffa,
    0xfffc
  ]

  def __init__(self, bus: Bus):

    self.last_opcode = 0x00

    self.bus = bus

    # Registers
    self.a = 0
    self.x = 0
    self.y = 0
    self.sp = 0
    self.pc = 0

    # Counters

    self.ticks = 0
    self.interrupts = 0

    self.killed = False

    self.cycles = 0
    self.cycles_wait = 0
    
    self.log = []
    
    self.set_flags(0b00000000)

    self.def_opcodes()
    self.def_oper_address()
  
  
  def dump(self):
    with open("hexdump.txt", "w") as f:
      f.write("0xADDR |  0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F\n")
      f.write("-----------------------------------------------------------\n")
      for i in range(0x1000):
        addr = i << 4
        f.write(f"0x{addr:04x} |")
        for j in range(16):
          f.write(f" {self.mem_read(addr + j):02x}")
          if j % 4 == 3: f.write(" ")
        f.write("\n")
        
  
  
  def def_oper_address(self):
    self.oper_address = [eval("self.oper_addr_" + s) for s in CPU.OP_ADDR_STRINGS]
  
  def oper_addr_Implied(self):     return print("bad operand address")                                    # Implied
  def oper_addr_Immediate(self):   return self.pc                                                         # Immediate
  def oper_addr_ZeroPage(self):    return self.mem_read(self.pc)                                          # ZeroPage
  def oper_addr_Absolute(self):    return self.mem_read_16(self.pc)                                       # Absolute
  def oper_addr_ZeroPageX(self):   return self.mem_read(self.pc) + self.x & 0xff                          # ZeroPageX
  def oper_addr_ZeroPageY(self):   return self.mem_read(self.pc) + self.y & 0xff                          # ZeroPageY
  def oper_addr_AbsoluteX(self):   return self.mem_read_16(self.pc) + self.x & 0xffff                     # AbsoluteX
  def oper_addr_AbsoluteY(self):   return self.mem_read_16(self.pc) + self.y & 0xffff                     # AbsoluteY
  def oper_addr_IndirectX(self):   return self.mem_read_16_wrap(self.mem_read(self.pc) + self.x & 0xff)   # IndirectX
  def oper_addr_IndirectY(self):   return self.mem_read_16_wrap(self.mem_read(self.pc)) + self.y & 0xffff # IndirectY
  def oper_addr_Accumulator(self): return print("bad operand address")                                    # Accumulator
  def oper_addr_Indirect(self):    return self.mem_read_16_wrap(self.mem_read_16(self.pc))                # Indirect

  def set_flags(self, value):
    self.flag_N = value & CPU.MASK_N != 0
    self.flag_V = value & CPU.MASK_V != 0
    self.flag_U = value & CPU.MASK_U != 0
    self.flag_B = value & CPU.MASK_B != 0
    self.flag_D = value & CPU.MASK_D != 0
    self.flag_I = value & CPU.MASK_I != 0
    self.flag_Z = value & CPU.MASK_Z != 0
    self.flag_C = value & CPU.MASK_C != 0
  
  def _set_flags(self, value):
    self.set_flags(value & 0b11101111 | 0b00100000)
  
  def get_flags(self):
    return (
      self.flag_N << 7 |
      self.flag_V << 6 |
      self.flag_U << 5 |
      self.flag_B << 4 |
      self.flag_D << 3 |
      self.flag_I << 2 |
      self.flag_Z << 1 |
      self.flag_C
    )
  
  def _get_flags(self, b_flag=0):
    return (
      self.flag_N << 7 |
      self.flag_V << 6 |
      CPU.MASK_U       |
      b_flag      << 4 |
      self.flag_D << 3 |
      self.flag_I << 2 |
      self.flag_Z << 1 |
      self.flag_C
    )

      # Flags / Status
    
      # 7 6 5 4 3 2 1 0
      # N V - B D I Z C
      # | | | | | | | |
      # | | | | | | | +-- Carry
      # | | | | | | +-- Zero
      # | | | | | +-- Interrupt Disable
      # | | | | +-- Decimal Mode (Not used in NES)
      # | | | +-- Break Command
      # | | +-- Unused
      # | +-- Overflow
      # +-- Negative
  
  def mem_read(self, addr):
    return self.bus.read_funcs[self.bus.read_func_idx[addr]](addr)
  
  def mem_write(self, addr, value):
    self.bus.write_funcs[self.bus.write_func_idx[addr]](addr, value)

  def mem_read_16(self, addr):
    return self.mem_read(addr) | (self.mem_read(addr + 1) << 8)
  
  def mem_read_16_wrap(self, addr):
    if addr & 0x00ff == 0x00ff:
      return self.mem_read(addr) | (self.mem_read(addr & 0xff00) << 8)
    return self.mem_read(addr) | (self.mem_read(addr + 1) << 8)
  
  def mem_write_16(self, addr, value):
    self.mem_write(addr, value & 0xff)
    self.mem_write(addr + 1, value >> 8)

  def stack_push(self, value):
    self.mem_write(0x100 + self.sp, value)
    self.sp = (self.sp - 1) & 0xff

  def stack_pull(self):
    self.sp = (self.sp + 1) & 0xff
    return self.mem_read(0x100 + self.sp)
  
  def stack_push_16(self, value):
    self.stack_push(value >> 8)
    self.stack_push(value & 0xff)

  def stack_pull_16(self):
    return self.stack_pull() | (self.stack_pull() << 8)

  def _opcode_cycles(self):
    self.cycles_wait += CPU.OPCODE_CYCLES_MAP[self.opcode]
    if CPU.OPCODE_WRAP_MAP[self.opcode] == 1:
      if self.pc & 0xff00 != self.pc + CPU.OPCODE_SIZE_MAP[self.opcode] & 0xff00:
        self.cycles_wait += 1
    self.pc += CPU.OPCODE_SIZE_PC_MAP[self.opcode]

  def _get_oper_address(self, mode):
    return self.oper_address[mode]()

  def _set_zn_flags(self, value):
    self.flag_N = value & CPU.MASK_N != 0
    self.flag_Z = value == 0

  def _branch(self, condition):
    if not condition:
      self.pc += 1
      return
    
    offs = self.mem_read(self.pc)
    
    self.cycles_wait += 1
    if self.pc & 0xff00 != self.pc + offs + CPU.OPCODE_SIZE_MAP[self.opcode] & 0xff00:
      self.cycles_wait += 1
      
    if offs & 0x80:
      self.pc -= offs ^ 0xff
    else:
      self.pc += offs + 1
    self.pc &= 0xffff
  
  def _add_to_accumulator(self, value):
    sum = self.a + value + self.flag_C
    self.flag_C = sum > 0xff
    self.flag_V = ~(self.a ^ value) & (self.a ^ sum) & 0x80 != 0
    self.a = sum & 0xff
    self._set_zn_flags(self.a)

  def _compare(self, val0, val1):
    self.flag_C = val0 >= val1
    self._set_zn_flags(val0 - val1)

  def _interrupt(self, interrupt_type):
    self.stack_push_16(self.pc)
    self.stack_push((self._get_flags() | CPU.MASK_B) & ~CPU.MASK_I)
    self.flag_I = 1
    self.flag_B = 0
    self.pc = self.mem_read_16(CPU.INT_ADDR[interrupt_type])

  # * Opcode functions --------------------------

  def _adc(self, mode):
    self._add_to_accumulator(self.mem_read(self.oper_address[mode]()))

  def _and(self, mode):
    self.a &= self.mem_read(self.oper_address[mode]())
    self._set_zn_flags(self.a)

  def _asl_acc(self):
    self.a, self.flag_C = self.a << 1 & 0xff, self.a & 0x80 != 0
    self._set_zn_flags(self.a)

  def _asl(self, mode):
    addr = self.oper_address[mode]()
    value = self.mem_read(addr)
    value, self.flag_C = value << 1 & 0xff, value & 0x80 != 0
    self.mem_write(addr, value)
    self._set_zn_flags(value)

  def _bcc(self, mode):
    self._branch(not self.flag_C)

  def _bcs(self, mode):
    self._branch(self.flag_C)

  def _beq(self, mode):
    self._branch(self.flag_Z)

  def _bit(self, mode):
    value = self.mem_read(self.oper_address[mode]())
    self.flag_N = value & CPU.MASK_N != 0
    self.flag_V = value & CPU.MASK_V != 0
    self.flag_Z = value & self.a == 0

  def _bmi(self, mode):
    self._branch(self.flag_N)

  def _bne(self, mode):
    self._branch(not self.flag_Z)

  def _bpl(self, mode):
    self._branch(not self.flag_N)

  def _brk(self, mode):
    self._interrupt(CPU.BRK)

  def _bvc(self, mode):
    self._branch(not self.flag_V)

  def _bvs(self, mode):
    self._branch(self.flag_V)

  def _clc(self, mode):
    self.flag_C = 0

  def _cld(self, mode):
    self.flag_D = 0

  def _cli(self, mode):
    self.flag_I = 0

  def _clv(self, mode):
    self.flag_V = 0

  def _cmp(self, mode):
    value = self.mem_read(self.oper_address[mode]())
    self.flag_C = self.a >= value
    self._set_zn_flags(self.a - value)

  def _cpx(self, mode):
    value = self.mem_read(self.oper_address[mode]())
    self.flag_C = self.x >= value
    self._set_zn_flags(self.x - value)

  def _cpy(self, mode):
    value = self.mem_read(self.oper_address[mode]())
    self.flag_C = self.y >= value
    self._set_zn_flags(self.y - value)

  def _dec(self, mode):
    addr = self.oper_address[mode]()
    value = (self.mem_read(addr) - 1) & 0xff
    self.mem_write(addr, value)
    self._set_zn_flags(value)

  def _dex(self, mode):
    self.x = (self.x - 1) & 0xff
    self._set_zn_flags(self.x)

  def _dey(self, mode):
    self.y = (self.y - 1) & 0xff
    self._set_zn_flags(self.y)

  def _eor(self, mode):
    self.a ^= self.mem_read(self.oper_address[mode]())
    self._set_zn_flags(self.a)

  def _inc(self, mode):
    addr = self.oper_address[mode]()
    value = self.mem_read(addr) + 1 & 0xff
    self.mem_write(addr, value)
    self._set_zn_flags(value)

  def _inx(self, mode):
    self.x = self.x + 1 & 0xff
    self._set_zn_flags(self.x)

  def _iny(self, mode):
    self.y = self.y + 1 & 0xff
    self._set_zn_flags(self.y)

  def _jmp(self, mode):
    self.pc = self.oper_address[mode]()

  def _jsr(self, mode):
    self.stack_push_16(self.pc + 1 & 0xffff)
    self.pc = self.oper_address[mode]()
  
  def _lda(self, mode):
    self.a = self.mem_read(self.oper_address[mode]())
    self._set_zn_flags(self.a)

  def _ldx(self, mode):
    self.x = self.mem_read(self.oper_address[mode]())
    self._set_zn_flags(self.x)

  def _ldy(self, mode):
    self.y = self.mem_read(self.oper_address[mode]())
    self._set_zn_flags(self.y)

  def _lsr_acc(self):
    self.flag_C = self.a & CPU.MASK_C
    self.a >>= 1
    self.flag_Z = self.a == 0
    self.flag_N = 0
  
  def _lsr(self, mode):
    addr = self.oper_address[mode]()
    value = self.mem_read(addr)
    value, self.flag_C = value >> 1, value & CPU.MASK_C
    self.mem_write(addr, value)
    self.flag_Z = value == 0
    self.flag_N = 0

  def _nop(self, mode):
    pass

  def _ora(self, mode):
    self.a |= self.mem_read(self.oper_address[mode]())
    self._set_zn_flags(self.a)

  def _pha(self, mode):
    self.stack_push(self.a)

  def _php(self, mode):
    self.stack_push(self._get_flags(b_flag=1))

  def _pla(self, mode):
    self.a = self.stack_pull()
    self._set_zn_flags(self.a)

  def _plp(self, mode):
    self._set_flags(self.stack_pull())

  def _rol_acc(self):
    self.a, self.flag_C = self.flag_C | (self.a << 1) & 0xff, self.a & 0x80 != 0
    self._set_zn_flags(self.a)

  def _rol(self, mode):
    addr = self.oper_address[mode]()
    value = self.mem_read(addr)
    value, self.flag_C = (value << 1 | self.flag_C) & 0xff, value & 0x80 != 0
    self.mem_write(addr, value)
    self._set_zn_flags(value)

  def _ror_acc(self):
    self.a, self.flag_C = (self.a >> 1) | (self.flag_C << 7), self.a & 1
    self._set_zn_flags(self.a)

  def _ror(self, mode):
    addr = self.oper_address[mode]()
    value = self.mem_read(addr)
    value, self.flag_C = value >> 1 | self.flag_C << 7, value & 1
    self.mem_write(addr, value)
    self._set_zn_flags(value)

  def _rti(self, mode):
    self._set_flags(self.stack_pull())
    self.pc = self.stack_pull() | (self.stack_pull() << 8)

  def _rts(self, mode):
    self.pc = (self.stack_pull() | (self.stack_pull() << 8)) + 1

  def _sbc(self, mode):
    self._add_to_accumulator(self.mem_read(self.oper_address[mode]()) ^ 0xff)

  def _sec(self, mode):
    self.flag_C = 1

  def _sed(self, mode):
    self.flag_D = 1

  def _sei(self, mode):
    self.flag_I = 1

  def _sta(self, mode):
    self.mem_write(self.oper_address[mode](), self.a)

  def _stx(self, mode):
    self.mem_write(self.oper_address[mode](), self.x)

  def _sty(self, mode):
    self.mem_write(self.oper_address[mode](), self.y)

  def _tax(self, mode):
    self.x = self.a
    self._set_zn_flags(self.x)

  def _tay(self, mode):
    self.y = self.a
    self._set_zn_flags(self.y)

  def _tsx(self, mode):
    self.x = self.sp
    self._set_zn_flags(self.x)

  def _txa(self, mode):
    self.a = self.x
    self._set_zn_flags(self.a)

  def _txs(self, mode):
    self.sp = self.x

  def _tya(self, mode):
    self.a = self.y
    self._set_zn_flags(self.a)


  # * Unofficial opcode functions ---------------

  def _aac(self, mode):
    addr = self.oper_address[mode]()
    value = self.mem_read(addr)
    self.a &= value
    self._set_zn_flags(self.a)
    self.flag_C = self.a & CPU.MASK_N

  def _aax(self, mode):
    value = self.a & self.x
    self.mem_write(self.oper_address[mode](), value)
    ### self._set_zn_flags(value)

  def _arr(self, mode):
    self._and(mode)
    self._ror(mode)
    b5 = self.a & 0b00010000 != 0
    b6 = self.a & 0b00001000 != 0
    self.flag_C = b6
    self.flag_V = b5 ^ b6

  def _asr(self, mode):
    self._and(mode)
    self._lsr(mode)

  def _atx(self, mode):
    addr = self.oper_address[mode]()
    value = self.mem_read(addr)
    self.a &= value
    self.x = self.a
    self._set_zn_flags(self.x)

  def _axa(self, mode):
    ### self.a &= self.a
    value = self.a & 0b00001111
    self.mem_write(self.oper_address[mode](), value)
    self._set_zn_flags(value)

  def _axs(self, mode):
    self.x &= self.a
    self.flag_C = self.x - self.mem_read(self.oper_address[mode]()) > 0x1ff

  def _dcp(self, mode):
    addr = self.oper_address[mode]()
    res = (self.mem_read(addr) - 1) & 0xff
    self.mem_write(addr, res)
    self._compare(self.a, res)

  def _dop(self, mode):
    pass

  def _isc(self, mode):
    addr = self.oper_address[mode]()
    value = (self.mem_read(addr) + 1) & 0xff
    self.mem_write(addr, value)
    self._add_to_accumulator(value ^ 0xff)

  def _kil(self, mode):
    self.killed = True

  def _lar(self, mode):
    res = self.mem_read(self.oper_address[mode]()) & self.sp
    self.a = res
    self.x = res
    self.sp = res
    self._set_zn_flags(res)

  def _lax(self, mode):
    value = self.mem_read(self.oper_address[mode]())
    self.a = value
    self.x = value
    self._set_zn_flags(value)

  # * NOP is the same as official

  def _rla(self, mode):
    self._rol(mode)
    self._and(mode)

  def _rra(self, mode):
    self._ror(mode)
    self._adc(mode)
  
  # * SBC is the same as official

  def _slo(self, mode):
    self._asl(mode)
    self._ora(mode)

  def _sre(self, mode):
    self._lsr(mode)
    self._eor(mode)

  def _sxa(self, mode):
    addr = self.oper_address[mode]()
    self.mem_write(addr, self.x & ((addr >> 8) + 1))

  def _sya(self, mode):
    addr = self.oper_address[mode]()
    self.mem_write(addr, self.y & ((addr >> 8) + 1))

  def _top(self, mode):
    pass

  def _xaa(self, mode):
    # * this opcode forces the user to kill themselves; avoid use
    pass
    ....__doc__

  def _xas(self, mode):
    addr = self.oper_address[mode]()
    self.sp = self.x & self.a
    self.mem_write(addr, self.sp & (addr & 0xff00 >> 8) + 1)

  # ----------------- WE GOT TO THE ACTUAL EXECUTION !! -----------------

  def execute_next(self):
    self.opcode = self.mem_read(self.pc)
    self.pc += 1
    self.single_opcode(self.opcode)

  def reset(self):
    self.a = 0
    self.x = 0
    self.y = 0
    self.sp = 0xfd
    # self.pc = 0xC000
    self.pc = self.mem_read_16(CPU.INT_ADDR[CPU.RESET])
    self.set_flags(0b00100100)
    self.ticks = 0
    self.cycles = 0
    self.killed = False
    self.opcode = 0x00
    self.interrupts = 0
    self.cycles_wait = 7

  def reset_and_run(self, *args, **kwargs):
    self.reset()
    self.run(*args, **kwargs)
  
  def save_log(self):
    with open("log.txt", "w") as f:
      f.write("\n".join(self.log))
  
  def log_line(self):
    # C000  4C F5 C5  JMP  A:00 X:00 Y:00 P:24 SP:FD PPU:  0, 21 CYC:7
    return f"{self.pc:04X}  {self.opcode:02X} " +\
      (f"{self.mem_read(self.pc + 1 & 0xffff):02X} " if CPU.OPCODE_SIZE_MAP[self.opcode] > 1 else "   ") +\
      (f"{self.mem_read(self.pc + 2 & 0xffff):02X}  " if CPU.OPCODE_SIZE_MAP[self.opcode] > 2 else "    ") +\
      CPU.OPCODE_NAMES_MAP[self.opcode] + "  " +\
      f"A:{self.a:02X} X:{self.x:02X} Y:{self.y:02X} P:{self.get_flags():08b} " +\
      f"SP:{self.sp:02X} PPU:{" " * (3 - len(str(self.bus.ppu.scanline)))}{self.bus.ppu.scanline}," +\
      f"{" " * (3 - len(str(self.bus.ppu.cycles)))}{self.bus.ppu.cycles} CYC:{self.cycles}"
  
  def run(self, frame_interrupt=None):
    
    while True:
      
      if self.killed:
        break

      self.bus.tick(self.cycles_wait)
      self.cycles += self.cycles_wait
      self.cycles_wait = 0

      if self.bus.poll_nmi():
        if self.bus.ppu.ctrl_flag_V:
          self._interrupt(CPU.NMI)
          self.bus.frames += 1
        frame_interrupt()

      # print(self.log_line())

      if self.pc == 0x0600:
        raise Exception()

      self.opcode = self.mem_read(self.pc)
      self.pc = self.pc + 1 & 0xffff
      
      # self.op_timer = time.time_ns()
      
      self.opcode_funcs[self.opcode]()
      
      # self.op_timer = time.time_ns() - self.op_timer
      # self.op_timers[self.opcode] += self.op_timer
      # self.op_counts[self.opcode] += 1
      
      
      self._opcode_cycles()

      self.ticks += 1


  def single_opcode(self, opcode):
    self.opcode_funcs[opcode]()
    
  def calc_op_average(self):
    avg = [(CPU.OPCODE_NAMES_MAP[i], CPU.OP_ADDR_STRINGS[CPU.OPCODE_ADDR_MAP[i]], self.op_timers[i] / self.op_counts[i] if self.op_counts[i] > 0 else 0) for i in range(256)]
    avg.sort(key=lambda x: x[2], reverse=True)
    avg = [f"{a} - {b}: {c}" for a, b, c in avg]
    return avg
  
  def def_opcodes(self):
    self.op_timers = [0 for _ in range(256)]
    self.op_counts = [0 for _ in range(256)]
    
    self.opcode_funcs = {i: self.__getattribute__(f"_op{i:02x}") for i in range(256)}
  
    self.opcode_funcs = (
      self._op00, self._op01, self._op02, self._op03, self._op04, self._op05, self._op06, self._op07,
      self._op08, self._op09, self._op0a, self._op0b, self._op0c, self._op0d, self._op0e, self._op0f,
      self._op10, self._op11, self._op12, self._op13, self._op14, self._op15, self._op16, self._op17,
      self._op18, self._op19, self._op1a, self._op1b, self._op1c, self._op1d, self._op1e, self._op1f,
      self._op20, self._op21, self._op22, self._op23, self._op24, self._op25, self._op26, self._op27,
      self._op28, self._op29, self._op2a, self._op2b, self._op2c, self._op2d, self._op2e, self._op2f,
      self._op30, self._op31, self._op32, self._op33, self._op34, self._op35, self._op36, self._op37, 
      self._op38, self._op39, self._op3a, self._op3b, self._op3c, self._op3d, self._op3e, self._op3f,
      self._op40, self._op41, self._op42, self._op43, self._op44, self._op45, self._op46, self._op47,
      self._op48, self._op49, self._op4a, self._op4b, self._op4c, self._op4d, self._op4e, self._op4f,
      self._op50, self._op51, self._op52, self._op53, self._op54, self._op55, self._op56, self._op57,
      self._op58, self._op59, self._op5a, self._op5b, self._op5c, self._op5d, self._op5e, self._op5f,
      self._op60, self._op61, self._op62, self._op63, self._op64, self._op65, self._op66, self._op67,
      self._op68, self._op69, self._op6a, self._op6b, self._op6c, self._op6d, self._op6e, self._op6f,
      self._op70, self._op71, self._op72, self._op73, self._op74, self._op75, self._op76, self._op77,
      self._op78, self._op79, self._op7a, self._op7b, self._op7c, self._op7d, self._op7e, self._op7f,
      self._op80, self._op81, self._op82, self._op83, self._op84, self._op85, self._op86, self._op87,
      self._op88, self._op89, self._op8a, self._op8b, self._op8c, self._op8d, self._op8e, self._op8f,
      self._op90, self._op91, self._op92, self._op93, self._op94, self._op95, self._op96, self._op97,
      self._op98, self._op99, self._op9a, self._op9b, self._op9c, self._op9d, self._op9e, self._op9f,
      self._opa0, self._opa1, self._opa2, self._opa3, self._opa4, self._opa5, self._opa6, self._opa7,
      self._opa8, self._opa9, self._opaa, self._opab, self._opac, self._opad, self._opae, self._opaf,
      self._opb0, self._opb1, self._opb2, self._opb3, self._opb4, self._opb5, self._opb6, self._opb7,
      self._opb8, self._opb9, self._opba, self._opbb, self._opbc, self._opbd, self._opbe, self._opbf,
      self._opc0, self._opc1, self._opc2, self._opc3, self._opc4, self._opc5, self._opc6, self._opc7,
      self._opc8, self._opc9, self._opca, self._opcb, self._opcc, self._opcd, self._opce, self._opcf,
      self._opd0, self._opd1, self._opd2, self._opd3, self._opd4, self._opd5, self._opd6, self._opd7,
      self._opd8, self._opd9, self._opda, self._opdb, self._opdc, self._opdd, self._opde, self._opdf,
      self._ope0, self._ope1, self._ope2, self._ope3, self._ope4, self._ope5, self._ope6, self._ope7,
      self._ope8, self._ope9, self._opea, self._opeb, self._opec, self._oped, self._opee, self._opef,
      self._opf0, self._opf1, self._opf2, self._opf3, self._opf4, self._opf5, self._opf6, self._opf7,
      self._opf8, self._opf9, self._opfa, self._opfb, self._opfc, self._opfd, self._opfe, self._opff
    )
  
  def _op00(self): self._brk(CPU.IMPLIED)    # 00 | BRK | 1 | 7
  def _op01(self): self._ora(CPU.INDIRECT_X) # 01 | ORA | 2 | 6
  def _op02(self): self._kil(CPU.IMPLIED)    # 02 | KIL | 1 | 0
  def _op03(self): self._slo(CPU.INDIRECT_X) # 03 | SLO | 2 | 8
  def _op04(self): pass    # 04 | DOP | 2 | 3
  def _op05(self): self._ora(CPU.ZEROPAGE)   # 05 | ORA | 2 | 3
  def _op06(self): self._asl(CPU.ZEROPAGE)   # 06 | ASL | 2 | 5
  def _op07(self): self._slo(CPU.ZEROPAGE)   # 07 | SLO | 2 | 5
  def _op08(self): self._php(CPU.IMPLIED)    # 08 | PHP | 1 | 3
  def _op09(self): self._ora(CPU.IMMEDIATE)  # 09 | ORA | 2 | 2
  def _op0a(self): self._asl_acc()           # 0a | ASL | 1 | 2
  def _op0b(self): self._aac(CPU.IMMEDIATE)  # 0b | AAC | 2 | 2
  def _op0c(self): pass   # 0c | TOP | 3 | 4
  def _op0d(self): self._ora(CPU.ABSOLUTE)   # 0d | ORA | 3 | 4
  def _op0e(self): self._asl(CPU.ABSOLUTE)   # 0e | ASL | 3 | 6
  def _op0f(self): self._slo(CPU.ABSOLUTE)   # 0f | SLO | 3 | 6
  def _op10(self): self._bpl(CPU.IMPLIED)    # 10 | BPL | 2 | 2**
  def _op11(self): self._ora(CPU.INDIRECT_Y) # 11 | ORA | 2 | 5*
  def _op12(self): self._kil(CPU.IMPLIED)    # 12 | KIL | 1 | 0
  def _op13(self): self._slo(CPU.INDIRECT_Y) # 13 | SLO | 2 | 8
  def _op14(self): pass    # 14 | DOP | 2 | 4
  def _op15(self): self._ora(CPU.ZEROPAGE_X) # 15 | ORA | 2 | 4
  def _op16(self): self._asl(CPU.ZEROPAGE_X) # 16 | ASL | 2 | 6
  def _op17(self): self._slo(CPU.ZEROPAGE_X) # 17 | SLO | 2 | 6
  def _op18(self): self._clc(CPU.IMPLIED)    # 18 | CLC | 1 | 2
  def _op19(self): self._ora(CPU.ABSOLUTE_Y) # 19 | ORA | 3 | 4*
  def _op1a(self): pass    # 1a | NOP | 1 | 2
  def _op1b(self): self._slo(CPU.ABSOLUTE_Y) # 1b | SLO | 3 | 7
  def _op1c(self): pass # 1c | TOP | 3 | 4*
  def _op1d(self): self._ora(CPU.ABSOLUTE_X) # 1d | ORA | 3 | 4*
  def _op1e(self): self._asl(CPU.ABSOLUTE_X) # 1e | ASL | 3 | 7
  def _op1f(self): self._slo(CPU.ABSOLUTE_X) # 1f | SLO | 3 | 7
  def _op20(self): self._jsr(CPU.ABSOLUTE)   # 20 | JSR | 3 | 6
  def _op21(self): self._and(CPU.INDIRECT_X) # 21 | AND | 2 | 6
  def _op22(self): self._kil(CPU.IMPLIED)    # 22 | KIL | 1 | 0
  def _op23(self): self._rla(CPU.INDIRECT_X) # 23 | RLA | 2 | 8
  def _op24(self): self._bit(CPU.ZEROPAGE)   # 24 | BIT | 2 | 3
  def _op25(self): self._and(CPU.ZEROPAGE)   # 25 | AND | 2 | 3
  def _op26(self): self._rol(CPU.ZEROPAGE)   # 26 | ROL | 2 | 5
  def _op27(self): self._rla(CPU.ZEROPAGE)   # 27 | RLA | 2 | 5
  def _op28(self): self._plp(CPU.IMPLIED)    # 28 | PLP | 1 | 4
  def _op29(self): self._and(CPU.IMMEDIATE)  # 29 | AND | 2 | 2
  def _op2a(self): self._rol_acc()           # 2a | ROL | 1 | 2
  def _op2b(self): self._aac(CPU.IMMEDIATE)  # 2b | AAC | 2 | 2
  def _op2c(self): self._bit(CPU.ABSOLUTE)   # 2c | BIT | 3 | 4
  def _op2d(self): self._and(CPU.ABSOLUTE)   # 2d | AND | 3 | 4
  def _op2e(self): self._rol(CPU.ABSOLUTE)   # 2e | ROL | 3 | 6
  def _op2f(self): self._rla(CPU.ABSOLUTE)   # 2f | RLA | 3 | 6
  def _op30(self): self._bmi(CPU.IMPLIED)    # 30 | BMI | 2 | 2**
  def _op31(self): self._and(CPU.INDIRECT_Y) # 31 | AND | 2 | 5*
  def _op32(self): self._kil(CPU.IMPLIED)    # 32 | KIL | 1 | 0
  def _op33(self): self._rla(CPU.INDIRECT_Y) # 33 | RLA | 2 | 8
  def _op34(self): pass    # 34 | DOP | 2 | 4
  def _op35(self): self._and(CPU.ZEROPAGE_X) # 35 | AND | 2 | 4
  def _op36(self): self._rol(CPU.ZEROPAGE_X) # 36 | ROL | 2 | 6
  def _op37(self): self._rla(CPU.ZEROPAGE_X) # 37 | RLA | 2 | 6
  def _op38(self): self._sec(CPU.IMPLIED)    # 38 | SEC | 1 | 2
  def _op39(self): self._and(CPU.ABSOLUTE_Y) # 39 | AND | 3 | 4*
  def _op3a(self): pass    # 3a | NOP | 1 | 2
  def _op3b(self): self._rla(CPU.ABSOLUTE_Y) # 3b | RLA | 3 | 7
  def _op3c(self): pass # 3c | TOP | 3 | 4*
  def _op3d(self): self._and(CPU.ABSOLUTE_X) # 3d | AND | 3 | 4*
  def _op3e(self): self._rol(CPU.ABSOLUTE_X) # 3e | ROL | 3 | 7
  def _op3f(self): self._rla(CPU.ABSOLUTE_X) # 3f | RLA | 3 | 7
  def _op40(self): self._rti(CPU.IMPLIED)    # 40 | RTI | 1 | 6
  def _op41(self): self._eor(CPU.INDIRECT_X) # 41 | EOR | 2 | 6
  def _op42(self): self._kil(CPU.IMPLIED)    # 42 | KIL | 1 | 0
  def _op43(self): self._sre(CPU.INDIRECT_X) # 43 | SRE | 2 | 8
  def _op44(self): pass    # 44 | DOP | 2 | 3
  def _op45(self): self._eor(CPU.ZEROPAGE)   # 45 | EOR | 2 | 3
  def _op46(self): self._lsr(CPU.ZEROPAGE)   # 46 | LSR | 2 | 5
  def _op47(self): self._sre(CPU.ZEROPAGE)   # 47 | SRE | 2 | 5
  def _op48(self): self._pha(CPU.IMPLIED)    # 48 | PHA | 1 | 3
  def _op49(self): self._eor(CPU.IMMEDIATE)  # 49 | EOR | 2 | 2
  def _op4a(self): self._lsr_acc()           # 4a | LSR | 1 | 2
  def _op4b(self): self._asr(CPU.IMMEDIATE)  # 4b | ASR | 2 | 2
  def _op4c(self): self._jmp(CPU.ABSOLUTE)   # 4c | JMP | 3 | 3
  def _op4d(self): self._eor(CPU.ABSOLUTE)   # 4d | EOR | 3 | 4
  def _op4e(self): self._lsr(CPU.ABSOLUTE)   # 4e | LSR | 3 | 6
  def _op4f(self): self._sre(CPU.ABSOLUTE)   # 4f | SRE | 3 | 6
  def _op50(self): self._bvc(CPU.IMPLIED)    # 50 | BVC | 2 | 2**
  def _op51(self): self._eor(CPU.INDIRECT_Y) # 51 | EOR | 2 | 5*
  def _op52(self): self._kil(CPU.IMPLIED)    # 52 | KIL | 1 | 0
  def _op53(self): self._sre(CPU.INDIRECT_Y) # 53 | SRE | 2 | 8
  def _op54(self): pass    # 54 | DOP | 2 | 4
  def _op55(self): self._eor(CPU.ZEROPAGE_X) # 55 | EOR | 2 | 4
  def _op56(self): self._lsr(CPU.ZEROPAGE_X) # 56 | LSR | 2 | 6
  def _op57(self): self._sre(CPU.ZEROPAGE_X) # 57 | SRE | 2 | 6
  def _op58(self): self._cli(CPU.IMPLIED)    # 58 | CLI | 1 | 2
  def _op59(self): self._eor(CPU.ABSOLUTE_Y) # 59 | EOR | 3 | 4*
  def _op5a(self): pass    # 5a | NOP | 1 | 2
  def _op5b(self): self._sre(CPU.ABSOLUTE_Y) # 5b | SRE | 3 | 7
  def _op5c(self): pass # 5c | TOP | 3 | 4*
  def _op5d(self): self._eor(CPU.ABSOLUTE_X) # 5d | EOR | 3 | 4*
  def _op5e(self): self._lsr(CPU.ABSOLUTE_X) # 5e | LSR | 3 | 7
  def _op5f(self): self._sre(CPU.ABSOLUTE_X) # 5f | SRE | 3 | 7
  def _op60(self): self._rts(CPU.IMPLIED)    # 60 | RTS | 1 | 6
  def _op61(self): self._adc(CPU.INDIRECT_X) # 61 | ADC | 2 | 6
  def _op62(self): self._kil(CPU.IMPLIED)    # 62 | KIL | 1 | 0
  def _op63(self): self._rra(CPU.INDIRECT_X) # 63 | RRA | 2 | 8
  def _op64(self): pass    # 64 | DOP | 2 | 3
  def _op65(self): self._adc(CPU.ZEROPAGE)   # 65 | ADC | 2 | 3
  def _op66(self): self._ror(CPU.ZEROPAGE)   # 66 | ROR | 2 | 5
  def _op67(self): self._rra(CPU.ZEROPAGE)   # 67 | RRA | 2 | 5
  def _op68(self): self._pla(CPU.IMPLIED)    # 68 | PLA | 1 | 4
  def _op69(self): self._adc(CPU.IMMEDIATE)  # 69 | ADC | 2 | 2
  def _op6a(self): self._ror_acc()           # 6a | ROR | 1 | 2
  def _op6b(self): self._arr(CPU.IMMEDIATE)  # 6b | ARR | 2 | 2
  def _op6c(self): self._jmp(CPU.INDIRECT)   # 6c | JMP | 3 | 5
  def _op6d(self): self._adc(CPU.ABSOLUTE)   # 6d | ADC | 3 | 4
  def _op6e(self): self._ror(CPU.ABSOLUTE)   # 6e | ROR | 3 | 6
  def _op6f(self): self._rra(CPU.ABSOLUTE)   # 6f | RRA | 3 | 6
  def _op70(self): self._bvs(CPU.IMPLIED)    # 70 | BVS | 2 | 2**
  def _op71(self): self._adc(CPU.INDIRECT_Y) # 71 | ADC | 2 | 5*
  def _op72(self): self._kil(CPU.IMPLIED)    # 72 | KIL | 1 | 0
  def _op73(self): self._rra(CPU.INDIRECT_Y) # 73 | RRA | 2 | 8
  def _op74(self): pass    # 74 | DOP | 2 | 4
  def _op75(self): self._adc(CPU.ZEROPAGE_X) # 75 | ADC | 2 | 4
  def _op76(self): self._ror(CPU.ZEROPAGE_X) # 76 | ROR | 2 | 6
  def _op77(self): self._rra(CPU.ZEROPAGE_X) # 77 | RRA | 2 | 6
  def _op78(self): self._sei(CPU.IMPLIED)    # 78 | SEI | 1 | 2
  def _op79(self): self._adc(CPU.ABSOLUTE_Y) # 79 | ADC | 3 | 4*
  def _op7a(self): pass    # 7a | NOP | 1 | 2
  def _op7b(self): self._rra(CPU.ABSOLUTE_Y) # 7b | RRA | 3 | 7
  def _op7c(self): pass # 7c | TOP | 3 | 4*
  def _op7d(self): self._adc(CPU.ABSOLUTE_X) # 7d | ADC | 3 | 4*
  def _op7e(self): self._ror(CPU.ABSOLUTE_X) # 7e | ROR | 3 | 7
  def _op7f(self): self._rra(CPU.ABSOLUTE_X) # 7f | RRA | 3 | 7
  def _op80(self): pass    # 80 | DOP | 2 | 2
  def _op81(self): self._sta(CPU.INDIRECT_X) # 81 | STA | 2 | 6
  def _op82(self): pass    # 82 | DOP | 2 | 2
  def _op83(self): self._aax(CPU.INDIRECT_X) # 83 | AAX | 2 | 6
  def _op84(self): self._sty(CPU.ZEROPAGE)   # 84 | STY | 2 | 3
  def _op85(self): self._sta(CPU.ZEROPAGE)   # 85 | STA | 2 | 3
  def _op86(self): self._stx(CPU.ZEROPAGE)   # 86 | STX | 2 | 3
  def _op87(self): self._aax(CPU.ZEROPAGE)   # 87 | AAX | 2 | 3
  def _op88(self): self._dey(CPU.IMPLIED)    # 88 | DEY | 1 | 2
  def _op89(self): pass    # 89 | DOP | 2 | 2
  def _op8a(self): self._txa(CPU.IMPLIED)    # 8a | TXA | 1 | 2
  def _op8b(self): pass  # 8b | XAA | 2 | 2
  def _op8c(self): self._sty(CPU.ABSOLUTE)   # 8c | STY | 3 | 4
  def _op8d(self): self._sta(CPU.ABSOLUTE)   # 8d | STA | 3 | 4
  def _op8e(self): self._stx(CPU.ABSOLUTE)   # 8e | STX | 3 | 4
  def _op8f(self): self._aax(CPU.ABSOLUTE)   # 8f | AAX | 3 | 4
  def _op90(self): self._bcc(CPU.IMPLIED)    # 90 | BCC | 2 | 2**
  def _op91(self): self._sta(CPU.INDIRECT_Y) # 91 | STA | 2 | 6
  def _op92(self): self._kil(CPU.IMPLIED)    # 92 | KIL | 1 | 0
  def _op93(self): self._axa(CPU.INDIRECT_Y) # 93 | AXA | 2 | 6
  def _op94(self): self._sty(CPU.ZEROPAGE_X) # 94 | STY | 2 | 4
  def _op95(self): self._sta(CPU.ZEROPAGE_X) # 95 | STA | 2 | 4
  def _op96(self): self._stx(CPU.ZEROPAGE_Y) # 96 | STX | 2 | 4
  def _op97(self): self._aax(CPU.ZEROPAGE_Y) # 97 | AAX | 2 | 4
  def _op98(self): self._tya(CPU.IMPLIED)    # 98 | TYA | 1 | 2
  def _op99(self): self._sta(CPU.ABSOLUTE_Y) # 99 | STA | 3 | 5
  def _op9a(self): self._txs(CPU.IMPLIED)    # 9a | TXS | 1 | 2
  def _op9b(self): self._xas(CPU.ZEROPAGE)   # 9b | XAS | 3 | 5
  def _op9c(self): self._sya(CPU.ABSOLUTE_Y) # 9c | SYA | 3 | 5
  def _op9d(self): self._sta(CPU.ABSOLUTE_X) # 9d | STA | 3 | 5
  def _op9e(self): self._sxa(CPU.ZEROPAGE_Y) # 9e | SXA | 3 | 5
  def _op9f(self): self._axa(CPU.ABSOLUTE_Y) # 9f | AXA | 3 | 5
  def _opa0(self): self._ldy(CPU.IMMEDIATE)  # a0 | LDY | 2 | 2
  def _opa1(self): self._lda(CPU.INDIRECT_X) # a1 | LDA | 2 | 6
  def _opa2(self): self._ldx(CPU.IMMEDIATE)  # a2 | LDX | 2 | 2
  def _opa3(self): self._lax(CPU.INDIRECT_X) # a3 | LAX | 2 | 6
  def _opa4(self): self._ldy(CPU.ZEROPAGE)   # a4 | LDY | 2 | 3
  def _opa5(self): self._lda(CPU.ZEROPAGE)   # a5 | LDA | 2 | 3
  def _opa6(self): self._ldx(CPU.ZEROPAGE)   # a6 | LDX | 2 | 3
  def _opa7(self): self._lax(CPU.ZEROPAGE)   # a7 | LAX | 2 | 3
  def _opa8(self): self._tay(CPU.IMPLIED)    # a8 | TAY | 1 | 2
  def _opa9(self): self._lda(CPU.IMMEDIATE)  # a9 | LDA | 2 | 2
  def _opaa(self): self._tax(CPU.IMPLIED)    # aa | TAX | 1 | 2
  def _opab(self): self._atx(CPU.ZEROPAGE)   # ab | ATX | 2 | 2
  def _opac(self): self._ldy(CPU.ABSOLUTE)   # ac | LDY | 3 | 4
  def _opad(self): self._lda(CPU.ABSOLUTE)   # ad | LDA | 3 | 4
  def _opae(self): self._ldx(CPU.ABSOLUTE)   # ae | LDX | 3 | 4
  def _opaf(self): self._lax(CPU.ABSOLUTE)   # af | LAX | 3 | 4
  def _opb0(self): self._bcs(CPU.IMPLIED)    # b0 | BCS | 2 | 2**
  def _opb1(self): self._lda(CPU.INDIRECT_Y) # b1 | LDA | 2 | 5*
  def _opb2(self): self._kil(CPU.IMPLIED)    # b2 | KIL | 1 | 0
  def _opb3(self): self._lax(CPU.INDIRECT_Y) # b3 | LAX | 2 | 5*
  def _opb4(self): self._ldy(CPU.ZEROPAGE_X) # b4 | LDY | 2 | 4
  def _opb5(self): self._lda(CPU.ZEROPAGE_X) # b5 | LDA | 2 | 4
  def _opb6(self): self._ldx(CPU.ZEROPAGE_Y) # b6 | LDX | 2 | 4
  def _opb7(self): self._lax(CPU.ZEROPAGE_Y) # b7 | LAX | 2 | 4
  def _opb8(self): self._clv(CPU.IMPLIED)    # b8 | CLV | 1 | 2
  def _opb9(self): self._lda(CPU.ABSOLUTE_Y) # b9 | LDA | 3 | 4*
  def _opba(self): self._tsx(CPU.IMPLIED)    # ba | TSX | 1 | 2
  def _opbb(self): self._lar(CPU.ABSOLUTE_Y) # bb | LAR | 3 | 4*
  def _opbc(self): self._ldy(CPU.ABSOLUTE_X) # bc | LDY | 3 | 4*
  def _opbd(self): self._lda(CPU.ABSOLUTE_X) # bd | LDA | 3 | 4*
  def _opbe(self): self._ldx(CPU.ABSOLUTE_Y) # be | LDX | 3 | 4*
  def _opbf(self): self._lax(CPU.ABSOLUTE_Y) # bf | LAX | 3 | 4*
  def _opc0(self): self._cpy(CPU.IMMEDIATE)  # c0 | CPY | 2 | 2
  def _opc1(self): self._cmp(CPU.INDIRECT_X) # c1 | CMP | 2 | 6
  def _opc2(self): pass    # c2 | DOP | 2 | 2
  def _opc3(self): self._dcp(CPU.INDIRECT_X) # c3 | DCP | 2 | 8
  def _opc4(self): self._cpy(CPU.ZEROPAGE)   # c4 | CPY | 2 | 3
  def _opc5(self): self._cmp(CPU.ZEROPAGE)   # c5 | CMP | 2 | 3
  def _opc6(self): self._dec(CPU.ZEROPAGE)   # c6 | DEC | 2 | 5
  def _opc7(self): self._dcp(CPU.ZEROPAGE)   # c7 | DCP | 2 | 5
  def _opc8(self): self._iny(CPU.IMPLIED)    # c8 | INY | 1 | 2
  def _opc9(self): self._cmp(CPU.IMMEDIATE)  # c9 | CMP | 2 | 2
  def _opca(self): self._dex(CPU.IMPLIED)    # ca | DEX | 1 | 2
  def _opcb(self): self._axs(CPU.IMMEDIATE)  # cb | AXS | 2 | 2
  def _opcc(self): self._cpy(CPU.ABSOLUTE)   # cc | CPY | 3 | 4
  def _opcd(self): self._cmp(CPU.ABSOLUTE)   # cd | CMP | 3 | 4
  def _opce(self): self._dec(CPU.ABSOLUTE)   # ce | DEC | 3 | 6
  def _opcf(self): self._dcp(CPU.ABSOLUTE)   # cf | DCP | 3 | 6
  def _opd0(self): self._bne(CPU.IMPLIED)    # d0 | BNE | 2 | 2**
  def _opd1(self): self._cmp(CPU.INDIRECT_Y) # d1 | CMP | 2 | 5*
  def _opd2(self): self._kil(CPU.IMPLIED)    # d2 | KIL | 1 | 0
  def _opd3(self): self._dcp(CPU.INDIRECT_Y) # d3 | DCP | 2 | 8
  def _opd4(self): pass    # d4 | DOP | 2 | 4
  def _opd5(self): self._cmp(CPU.ZEROPAGE_X) # d5 | CMP | 2 | 4
  def _opd6(self): self._dec(CPU.ZEROPAGE_X) # d6 | DEC | 2 | 6
  def _opd7(self): self._dcp(CPU.ZEROPAGE_X) # d7 | DCP | 2 | 6
  def _opd8(self): self._cld(CPU.IMPLIED)    # d8 | CLD | 1 | 2
  def _opd9(self): self._cmp(CPU.ABSOLUTE_Y) # d9 | CMP | 3 | 4*
  def _opda(self): pass    # da | NOP | 1 | 2
  def _opdb(self): self._dcp(CPU.ABSOLUTE_Y) # db | DCP | 3 | 7
  def _opdc(self): pass # dc | TOP | 3 | 4*
  def _opdd(self): self._cmp(CPU.ABSOLUTE_X) # dd | CMP | 3 | 4*
  def _opde(self): self._dec(CPU.ABSOLUTE_X) # de | DEC | 3 | 7
  def _opdf(self): self._dcp(CPU.ABSOLUTE_X) # df | DCP | 3 | 7
  def _ope0(self): self._cpx(CPU.IMMEDIATE)  # e0 | CPX | 2 | 2
  def _ope1(self): self._sbc(CPU.INDIRECT_X) # e1 | SBC | 2 | 6
  def _ope2(self): pass    # e2 | DOP | 2 | 2
  def _ope3(self): self._isc(CPU.INDIRECT_X) # e3 | ISC | 2 | 8
  def _ope4(self): self._cpx(CPU.ZEROPAGE)   # e4 | CPX | 2 | 3
  def _ope5(self): self._sbc(CPU.ZEROPAGE)   # e5 | SBC | 2 | 3
  def _ope6(self): self._inc(CPU.ZEROPAGE)   # e6 | INC | 2 | 5
  def _ope7(self): self._isc(CPU.ZEROPAGE)   # e7 | ISC | 2 | 5
  def _ope8(self): self._inx(CPU.IMPLIED)    # e8 | INX | 1 | 2
  def _ope9(self): self._sbc(CPU.IMMEDIATE)  # e9 | SBC | 2 | 2
  def _opea(self): pass    # ea | NOP | 1 | 2
  def _opeb(self): self._sbc(CPU.IMMEDIATE)  # eb | SBC | 2 | 2
  def _opec(self): self._cpx(CPU.ABSOLUTE)   # ec | CPX | 3 | 4
  def _oped(self): self._sbc(CPU.ABSOLUTE)   # ed | SBC | 3 | 4
  def _opee(self): self._inc(CPU.ABSOLUTE)   # ee | INC | 3 | 6
  def _opef(self): self._isc(CPU.ABSOLUTE)   # ef | ISC | 3 | 6
  def _opf0(self): self._beq(CPU.IMPLIED)    # f0 | BEQ | 2 | 2**
  def _opf1(self): self._sbc(CPU.INDIRECT_Y) # f1 | SBC | 2 | 5*
  def _opf2(self): self._kil(CPU.IMPLIED)    # f2 | KIL | 1 | 0
  def _opf3(self): self._isc(CPU.INDIRECT_Y) # f3 | ISC | 2 | 8
  def _opf4(self): pass    # f4 | DOP | 2 | 4
  def _opf5(self): self._sbc(CPU.ZEROPAGE_X) # f5 | SBC | 2 | 4
  def _opf6(self): self._inc(CPU.ZEROPAGE_X) # f6 | INC | 2 | 6
  def _opf7(self): self._isc(CPU.ZEROPAGE_X) # f7 | ISC | 2 | 6
  def _opf8(self): self._sed(CPU.IMPLIED)    # f8 | SED | 1 | 2
  def _opf9(self): self._sbc(CPU.ABSOLUTE_Y) # f9 | SBC | 3 | 4*
  def _opfa(self): pass    # fa | NOP | 1 | 2
  def _opfb(self): self._isc(CPU.ABSOLUTE_Y) # fb | ISC | 3 | 7
  def _opfc(self): pass # fc | TOP | 3 | 4*
  def _opfd(self): self._sbc(CPU.ABSOLUTE_X) # fd | SBC | 3 | 4*
  def _opfe(self): self._inc(CPU.ABSOLUTE_X) # fe | INC | 3 | 7
  def _opff(self): self._isc(CPU.ABSOLUTE_X) # ff | ISC | 3 | 7
