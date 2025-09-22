class Controller:

  MASK_A      = 0b00000001
  MASK_B      = 0b00000010
  MASK_SELECT = 0b00000100
  MASK_START  = 0b00001000
  MASK_UP     = 0b00010000
  MASK_DOWN   = 0b00100000
  MASK_LEFT   = 0b01000000
  MASK_RIGHT  = 0b10000000

  def __init__(self):

    self.buttons = 0b00000000

    # 0b0_0_0_0_0_0_0_0
    #   | | | | | | | |
    #   | | | | | | | +- A
    #   | | | | | | +--- B
    #   | | | | | +----- Select
    #   | | | | +------- Start
    #   | | | +--------- Up
    #   | | +----------- Down
    #   | +------------- Left
    #   +--------------- Right
  
    self.strobe = 0

    self.button_index = 0

  def set_button(self, button, value):
    mask = 1 << button
    if value:
      self.buttons |= mask
    else:
      self.buttons &= ~mask

  def write(self, data):
    self.strobe = data & 1 != 0
    if self.strobe:
      self.button_index = 0

  def read(self):
    if self.button_index > 7:
      return 0xff
    response = self.buttons & (1 << self.button_index) != 0
    if not self.strobe and self.button_index < 8:
      self.button_index += 1
    return response