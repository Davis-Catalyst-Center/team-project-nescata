
V_MASK = 0b10000000
S_MASK = 0b01000000
O_MASK = 0b00100000
UNUSD0 = 0b00010000
UNUSD1 = 0b00001000
UNUSD2 = 0b00000100
UNUSD3 = 0b00000010
UNUSD4 = 0b00000001

class StatRegister:

  def __init__(self):
    self.flag_V = 0
    self.flag_S = 0
    self.flag_O = 0

  def set_vblank_stat(self, status):
    self.flag_V = status

  def set_sprite_zero_hit(self, status):
    self.flag_S = status

  def set_sprite_overflow(self, status):
    self.flag_O = status

  def reset_vblank_stat(self):
    self.flag_V = 0

  def is_in_vblank(self):
    return self.flag_V

  def get(self):
    return (
      (self.flag_V << 7) |
      (self.flag_S << 6) |
      (self.flag_O << 5)
    )
  
  def set(self, value):
    self.flag_V = (value & V_MASK) >> 7
    self.flag_S = (value & S_MASK) >> 6
    self.flag_O = (value & O_MASK) >> 5