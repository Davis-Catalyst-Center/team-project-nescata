
V_MASK  = 0b10000000 # generate NMI
P_MASK  = 0b01000000 # master/slave select
H_MASK  = 0b00100000 # sprite size
B_MASK  = 0b00010000 # background pattern address
S_MASK  = 0b00001000 # sprite pattern address
I_MASK  = 0b00000100 # vram address increment
N_MASK  = 0b00000011 # nametable bits

class CtrlRegister:

  def __init__(self):
    self.flag_V = 0
    self.flag_P = 0
    self.flag_H = 0
    self.flag_B = 0
    self.flag_S = 0
    self.flag_I = 0
    self.flag_N = 0
  
  def nametable_addr(self):
    return 0x2000 | (self.flag_N << 10)
  
  def vram_addr_inc(self):
    return 32 if self.flag_I else 1
    
  def sprt_pattern_addr(self):
    return 4096 if self.flag_S else 0

  def bknd_pattern_addr(self):
    return 4096 if self.flag_B else 0
    
  def sprt_size(self):
    return 16 if self.flag_H else 8
    
  def master_slave(self):
    return self.flag_H
    
  def gen_vblank_nmi(self):
    return self.flag_V
  
  def set(self, value):
    self.flag_V = value & V_MASK != 0
    self.flag_P = value & P_MASK != 0
    self.flag_H = value & H_MASK != 0
    self.flag_B = value & B_MASK != 0
    self.flag_S = value & S_MASK != 0
    self.flag_I = value & I_MASK != 0
    self.flag_N = value & N_MASK

  def get(self):
    return (
      self.flag_V << 7 |
      self.flag_P << 6 |
      self.flag_H << 5 |
      self.flag_B << 4 |
      self.flag_S << 3 |
      self.flag_I << 2 |
      self.flag_N
    )