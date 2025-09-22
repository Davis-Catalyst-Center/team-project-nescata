class AddrRegister:

  def __init__(self):
    self.value = [0, 0]
    self.hi_ptr = 1

  def set(self, data):
    self.value = [data >> 8, data & 0xff]
  
  def update(self, data):
    self.hi_ptr = not self.hi_ptr
    self.value[self.hi_ptr] = data

    self.value[0] &= 0b00111111
    

  def inc(self, inc):
    self.value[1] += inc
    
    if self.value[1] > 0xff:
      self.value[1] &= 0xff
      self.value[0] = (self.value[0] + 1) & 0b00111111

  def reset_latch(self):
    self.hi_ptr = 1

  def get(self):
    return self.value[0] << 8 | self.value[1]

