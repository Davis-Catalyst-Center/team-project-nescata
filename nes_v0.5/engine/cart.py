class Cart:
  ROM_BANK_SIZE = 16384
  ROM_BANK_SHIFT = 14
  VROM_BANK_SIZE = 8192
  VROM_BANK_SHIFT = 13
  
  MIRROR_HORIZONTAL = 0
  MIRROR_VERTICAL = 1
  MIRROR_4_SCREEN = 2
  
  
  def __init__(self, path):
    
    self.filepath = path
    with open(self.filepath, "rb") as f:
      raw_data = f.read()

    self.write_enable = False

    self.open_data(raw_data)



  def open_data(self, raw_data):

    self.raw_header = raw_data[:16]

    if not self.raw_header[:4] == b"NES\x1a":
      raise Exception("Not a valid iNES file")
    
    self.rom_bank_count = self.raw_header[4]
    self.rom_size = self.rom_bank_count << Cart.ROM_BANK_SHIFT

    self.vrom_bank_count = self.raw_header[5]
    self.vrom_size = self.vrom_bank_count << Cart.VROM_BANK_SHIFT

    control_1 = self.raw_header[6]
    control_2 = self.raw_header[7]

    self.mapper = control_1 >> 4 | control_2 & 0b11110000

    self.four_screen = 0 != control_1 & 0b00001000
    self.trainer_p   = 0 != control_1 & 0b00000100 # ignore for now
    self.battery_ram = 0 != control_1 & 0b00000010 # ignore for now
    self.v_mirroring = 0 != control_1 & 0b00000001

    self.screen_mirroring = (
      Cart.MIRROR_4_SCREEN if self.four_screen else
      Cart.MIRROR_VERTICAL if self.v_mirroring else
      Cart.MIRROR_HORIZONTAL
    )

    self.version = control_2 >> 2 & 0b00000011
    if self.version != 0: raise Exception("Unsupported version " + str(self.version))
    
    self.trainer_size = self.trainer_p << 9

    self.rom_start = 16 | self.trainer_size
    self.rom_end = self.rom_start + self.rom_size

    self.vrom_start = self.rom_end
    self.vrom_end = self.vrom_start + self.vrom_size

    self.prg_rom = raw_data[self.rom_start:self.rom_end]
    if self.rom_size == Cart.ROM_BANK_SIZE:
      self.prg_rom = self.prg_rom + self.prg_rom

    self.chr_rom = raw_data[self.vrom_start:self.vrom_end]
    
    self.prg_rom = bytearray(self.prg_rom)
    self.chr_rom = bytearray(self.chr_rom)
    
    print("rom loaded!")
    print(f"mapper: {self.mapper}")
  
  def read_prg_rom(self, addr):
    return self.prg_rom[addr]
  
  def write_prg_rom(self, addr, value):
    if self.write_enable:
      self.prg_rom[addr] = value
    else:
      print("Attempted to write to read-only Cartridge PRG ROM address: " + hex(addr))
    