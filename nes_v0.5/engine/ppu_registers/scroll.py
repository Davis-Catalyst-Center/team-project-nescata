class ScrlRegister:

  def __init__(self):
    self.scroll_x = 0
    self.scroll_y = 0
    self.latch = False

  def write(self, data):
    if self.latch:
      self.scroll_y = data
    else:
      self.scroll_x = data
    self.latch = not self.latch

  def reset_latch(self):
    self.latch = False

  def get(self):
    return self.scroll_x | self.scroll_y << 5
  
  def set(self, value):
    self.scroll_x = value & 0b11111
    self.scroll_y = value >> 5