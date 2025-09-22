
MASK_B = 0b10000000 # color emphasis B
MASK_G = 0b01000000 # color emphasis G
MASK_R = 0b00100000 # color emphasis R
MASK_s = 0b00010000 # sprite enable
MASK_b = 0b00001000 # background enable
MASK_M = 0b00000100 # sprite left column enable
MASK_m = 0b00000010 # background left column enable
MASK_g = 0b00000001 # grayscale

class MaskRegister:

  def __init__(self):
    self.flag_B = 0
    self.flag_G = 0
    self.flag_R = 0
    self.flag_s = 0
    self.flag_b = 0
    self.flag_M = 0
    self.flag_m = 0
    self.flag_g = 0
  
  def is_grayscale(self):
    return self.flag_g
  
  def leftmost_8pxl_background(self):
    return self.flag_m
  
  def leftmost_8pxl_sprite(self):
    return self.flag_M
  
  def show_background(self):
    return self.flag_b
  
  def show_sprites(self):
    return self.flag_s
  
  def emphasize(self):
    return [
      self.flag_R,
      self.flag_G,
      self.flag_B
    ]
  
  def set(self, value):
    self.flag_B = (value & MASK_B) >> 7
    self.flag_G = (value & MASK_G) >> 6
    self.flag_R = (value & MASK_R) >> 5
    self.flag_s = (value & MASK_s) >> 4
    self.flag_b = (value & MASK_b) >> 3
    self.flag_M = (value & MASK_M) >> 2
    self.flag_m = (value & MASK_m) >> 1
    self.flag_g = (value & MASK_g) >> 0

  def get(self):
    return (
      (self.flag_B << 7) |
      (self.flag_G << 6) |
      (self.flag_R << 5) |
      (self.flag_s << 4) |
      (self.flag_b << 3) |
      (self.flag_M << 2) |
      (self.flag_m << 1) |
      (self.flag_g << 0)
    )