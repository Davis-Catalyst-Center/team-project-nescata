from .cart import Cart
from .ppu import PPU
from .controller import Controller


RAM_MIRRORS_END = 0x1fff

PPU_REGISTERS = 0x2000
PPU_REGISTERS_MIRRORS_END = 0x3fff

class Bus:

  def __init__(self, ppu: PPU):

    #  _______________ $10000  _______________
    # | PRG-ROM       |       |               |
    # | Upper Bank    |       |               |
    # |_ _ _ _ _ _ _ _| $C000 | PRG-ROM       |
    # | PRG-ROM       |       |               |
    # | Lower Bank    |       |               |
    # |_______________| $8000 |_______________|
    # | SRAM          |       | SRAM          |
    # |_______________| $6000 |_______________|
    # | Expansion ROM |       | Expansion ROM |
    # |_______________| $4020 |_______________|
    # | I/O Registers |       |               |
    # |_ _ _ _ _ _ _ _| $4000 |               |
    # | Mirrors       |       | I/O Registers |
    # | $2000-$2007   |       |               |
    # |_ _ _ _ _ _ _ _| $2008 |               |
    # | I/O Registers |       |               |
    # |_______________| $2000 |_______________|
    # | Mirrors       |       |               |
    # | $0000-$07FF   |       |               |
    # |_ _ _ _ _ _ _ _| $0800 |               |
    # | RAM           |       | RAM           |
    # |_ _ _ _ _ _ _ _| $0200 |               |
    # | Stack         |       |               |
    # |_ _ _ _ _ _ _ _| $0100 |               |
    # | Zero Page     |       |               |
    # |_______________| $0000 |_______________|


    self.memory = bytearray(0x10000)

    self.cart = None

    self.controllers = [None, None]

    self.ppu = ppu

    self.cycles = 0
    self.frames = 0
    
    
    self.def_read_func_idx()
    self.def_read_funcs()
    
    self.def_write_func_idx()
    self.def_write_funcs()
    
  
  def def_read_func_idx(self):
    self.read_func_idx = [
      0 if addr < 0x2000 else
      1 if 0x8000 <= addr < 0x10000 else
      2 if addr == 0x2002 else
      3 if addr == 0x2004 else
      4 if addr == 0x2007 else
      5 if addr == 0x4016 else
      6 if addr == 0x4017 else
      7 if 0x2008 <= addr < 0x4000 else
      8 if 0x4000 <= addr < 0x4016 else
      8
    for addr in range(0x10000)]
    self.read_func_idx = bytearray(self.read_func_idx)
  
  def def_read_funcs(self):
    self.read_funcs = [eval(f"self.read_fun_{i}") for i in range(9)]
  
  def read_fun_0(self, addr): return self.memory[addr & 0b00000111_11111111]
  def read_fun_1(self, addr): return self.cart.prg_rom[addr ^ 0x8000]
  def read_fun_2(self, addr): return self.ppu.read_status()
  def read_fun_3(self, addr): return self.ppu.oam_data[self.ppu.oam_addr]
  def read_fun_4(self, addr): return self.ppu.read_data()
  def read_fun_5(self, addr): return self.controllers[0].read() if self.controllers[0] else 0
  def read_fun_6(self, addr): return self.controllers[1].read() if self.controllers[1] else 0
  def read_fun_7(self, addr): return self.mem_read(addr & 0b00100000_00000111)
  def read_fun_8(self, addr): return 0

  def def_write_func_idx(self):
    self.write_func_idx = [
      0 if addr < 0x2000 else
      1 if addr == 0x2000 else
      2 if addr == 0x2001 else
      3 if addr == 0x2003 else
      4 if addr == 0x2004 else
      5 if addr == 0x2005 else
      6 if addr == 0x2006 else
      7 if addr == 0x2007 else
      8 if addr == 0x4014 else
      9 if addr == 0x4016 else
      10 if addr == 0x4017 else
      11 if 0x2008 <= addr < 0x4000 else
      12 if 0x8000 <= addr < 0x10000 else
      13 if 0x4000 <= addr < 0x4016 else
      8
    for addr in range(0x10000)]
    self.write_func_idx = bytearray(self.write_func_idx)

  def def_write_funcs(self):
    self.write_funcs = [eval(f"self.write_fun_{addr}") for addr in range(14)]
  
  def write_fun_0(self, addr, value): self.memory.__setitem__(addr & 0b00000111_11111111, value)
  def write_fun_1(self, addr, value): self.ppu.write_ctrl(value)
  def write_fun_2(self, addr, value): self.ppu.mask_set(value)
  def write_fun_3(self, addr, value): self.ppu.oam_addr = value
  def write_fun_4(self, addr, value): self.ppu.write_oam_data(value)
  def write_fun_5(self, addr, value): self.ppu.scrl_write(value)
  def write_fun_6(self, addr, value): self.ppu.addr_update(value)
  def write_fun_7(self, addr, value): self.ppu.write_data(value)
  def write_fun_8(self, addr, value): self.ppu.write_oam_dma(bytearray([self.mem_read(value << 8 | i) for i in range(256)]))
  def write_fun_9(self, addr, value): self.controllers[0].write(value) if self.controllers[0] else None
  def write_fun_10(self, addr, value): self.controllers[1].write(value) if self.controllers[1] else None
  def write_fun_11(self, addr, value): self.mem_write(addr & 0b00100000_00000111, value)
  def write_fun_12(self, addr, value): self.cart.write_prg_rom(addr ^ 0x8000, value)
  def write_fun_13(self, addr, value): pass

  def mem_read(self, addr):
    
    return self.read_funcs[self.read_func_idx[addr]](addr)

    if addr < 0x2000: return self.memory[addr & 0b00000111_11111111]
    if 0x8000 <= addr < 0x10000: return self.cart.prg_rom[addr ^ 0x8000]
    if addr == 0x2002: return self.ppu.read_status()
    if addr == 0x2004: return self.ppu.read_oam_data()
    if addr == 0x2007: return self.ppu.read_data()
    if addr == 0x4016: return self.controllers[0].read() if self.controllers[0] else 0
    if addr == 0x4017: return self.controllers[1].read() if self.controllers[1] else 0
    if 0x2008 <= addr < 0x4000: return self.mem_read(addr & 0b00100000_00000111)
    if 0x4000 <= addr < 0x4016: return 0
    
    # if addr in [0x2000, 0x2001, 0x2003, 0x2005, 0x2006, 0x4014]:
    #   print("Attempted to read from write-only PPU address: " + hex(addr))
    
    # else:
    #   print("Attempted to read from invalid memory location: " + hex(addr))
    
    return 0x00

  def mem_write(self, addr, value):
    
    self.write_funcs[self.write_func_idx[addr]](addr, value)
    
    return

    if addr < 0x2000: self.memory[addr & 0b00000111_11111111] = value
    elif addr == 0x2000: self.ppu.write_ctrl(value)
    elif addr == 0x2001: self.ppu.write_mask(value)
    elif addr == 0x2003: self.ppu.write_oam_addr(value)
    elif addr == 0x2004: self.ppu.write_oam_data(value)
    elif addr == 0x2005: self.ppu.write_scroll(value)
    elif addr == 0x2006: self.ppu.write_addr(value)
    elif addr == 0x2007: self.ppu.write_data(value)
    elif addr == 0x4014: self.ppu.write_oam_dma(bytearray([self.mem_read(value << 8 | i) for i in range(256)]))
    elif 0x4000 <= addr < 0x4016: pass
    elif addr == 0x4016: self.controllers[0].write(value) if self.controllers[0] else None
    elif addr == 0x4017: self.controllers[1].write(value) if self.controllers[1] else None
    elif 0x2008 <= addr < 0x4000: self.mem_write(addr & 0b00100000_00000111, value)
    elif 0x8000 <= addr < 0x10000: self.cart.write_prg_rom(addr ^ 0x8000, value)
    # elif addr == 0x2002: print("Attempted to write to read-only PPU address: " + hex(addr))
    # else: print("Attempted to write to invalid memory location: " + hex(addr))
  
  def reset(self):
    self.memory = bytearray(0x8000)
    self.cycles = 0

  def tick(self, cpu_cycles):
    cycles = cpu_cycles * 3
    self.cycles += cycles
    self.ppu.tick(cycles)

  def poll_nmi(self):
    return self.ppu.poll_nmi()
  
  def attach_cart(self, cart: Cart):
    self.cart = cart
    self.ppu.attach_cart(cart)

  def detach_cart(self):
    self.cart = None
    self.ppu.detach_cart()

  def attach_controller(self, controller: Controller, player=0):
    self.controllers[player] = controller
  
  def detach_controller(self, player=0):
    self.controllers[player] = None
