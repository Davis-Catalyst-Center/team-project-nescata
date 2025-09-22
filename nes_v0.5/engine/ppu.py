from .ppu_registers import *
from .cart import Cart


NAMETABLE_LEN = 0x400
NAMETABLE_ADDR = 0x2000

MIRROR_TYPES = [
  (0, 0, 1, 1),
  (0, 1, 0, 1),
  (0, 1, 2, 3)
]


class PPU:

  def __init__(self, frame):
    
    self.frame = frame

    self.chr_rom = bytearray(2048)
    self.mirroring_type = Cart.MIRROR_HORIZONTAL
    self.mirroring = MIRROR_TYPES[self.mirroring_type]
    
    self.addr_value = [0, 0]
    self.addr_hi_ptr = 1
    
    self.ctrl_flag_V = 0
    self.ctrl_flag_P = 0
    self.ctrl_flag_H = 0
    self.ctrl_flag_B = 0
    self.ctrl_flag_S = 0
    self.ctrl_flag_I = 0
    self.ctrl_flag_N = 0
    
    self.mask_flag_B = 0
    self.mask_flag_G = 0
    self.mask_flag_R = 0
    self.mask_flag_s = 0
    self.mask_flag_b = 0
    self.mask_flag_M = 0
    self.mask_flag_m = 0
    self.mask_flag_g = 0
    
    self.scrl_scroll_x = 0
    self.scrl_scroll_y = 0
    self.scrl_latch = False
    
    self.stat_flag_V = 0
    self.stat_flag_S = 0
    self.stat_flag_O = 0
    
    self.vram = bytearray(2048)

    self.oam_addr = 0
    self.oam_data = bytearray(256)
    self.palette = bytearray(32)
    
    self.buffer = 0

    self.scanline = 0
    self.cycles = 0
    self.nmi_interrupt = False
  
  
  # address register
  
  def addr_set(self, data):
    self.addr_value = [data >> 8, data & 0xff]
  
  def addr_update(self, data):
    self.addr_hi_ptr = not self.addr_hi_ptr
    self.addr_value[self.addr_hi_ptr] = data
    self.addr_value[0] &= 0b00111111
  
  def addr_inc(self, inc):
    self.addr_value[1] += inc
    if self.addr_value[1] > 0xff:
      self.addr_value[1] &= 0xff
      self.addr_value[0] = (self.addr_value[0] + 1) & 0b00111111
  
  ### def addr_reset_latch(self):
  ###   self.addr_hi_ptr = 1
  
  ### def addr_get(self):
  ###   return self.addr_value[0] << 8 | self.addr_value[1]
  

  # control register
  
  def ctrl_nametable_addr(self):
    return 0x2400 if self.ctrl_flag_N else 0x2000
  
  def ctrl_vram_addr_inc(self):
    return 32 if self.ctrl_flag_I else 1
  
  def ctrl_sprt_pattern_addr(self):
    return 4096 if self.ctrl_flag_S else 0
  
  def ctrl_bknd_pattern_addr(self):
    return 4096 if self.ctrl_flag_B else 0
    
  def ctrl_sprt_size(self):
    return 16 if self.ctrl_flag_H else 8
    
  ### def ctrl_master_slave(self):
  ###   return self.ctrl_flag_H
   
  ### def ctrl_gen_vblank_nmi(self):
  ###   return self.ctrl_flag_V

  def ctrl_set(self, data):
    self.ctrl_flag_V = data & 0b10000000 != 0
    self.ctrl_flag_P = data & 0b01000000 != 0
    self.ctrl_flag_H = data & 0b00100000 != 0
    self.ctrl_flag_B = data & 0b00010000 != 0
    self.ctrl_flag_S = data & 0b00001000 != 0
    self.ctrl_flag_I = data & 0b00000100 != 0
    self.ctrl_flag_N = data & 0b00000011
  
  def ctrl_get(self):
    return (
      self.ctrl_flag_V << 7 |
      self.ctrl_flag_P << 6 |
      self.ctrl_flag_H << 5 |
      self.ctrl_flag_B << 4 |
      self.ctrl_flag_S << 3 |
      self.ctrl_flag_I << 2 |
      self.ctrl_flag_N
    )


  # mask register
  
  ### def mask_is_grayscale(self):
  ###   return self.mask_flag_g
  
  ### def mask_leftmost_8pxl_background(self):
  ###   return self.mask_flag_m
  
  ### def mask_leftmost_8pxl_sprite(self):
  ###   return self.mask_flag_M
  
  ### def mask_show_background(self):
  ###   return self.mask_flag_b
  
  ### def mask_show_sprites(self):
  ###   return self.mask_flag_s
  
  def mask_emphasize(self):
    return [
      self.mask_flag_R,
      self.mask_flag_G,
      self.mask_flag_B
    ]
  
  def mask_set(self, value):
    self.mask_flag_B = value & 0b10000000 != 0
    self.mask_flag_G = value & 0b01000000 != 0
    self.mask_flag_R = value & 0b00100000 != 0
    self.mask_flag_s = value & 0b00010000 != 0
    self.mask_flag_b = value & 0b00001000 != 0
    self.mask_flag_M = value & 0b00000100 != 0
    self.mask_flag_m = value & 0b00000010 != 0
    self.mask_flag_g = value & 0b00000001 != 0

  def mask_get(self):
    return (
      (self.mask_flag_B << 7) |
      (self.mask_flag_G << 6) |
      (self.mask_flag_R << 5) |
      (self.mask_flag_s << 4) |
      (self.mask_flag_b << 3) |
      (self.mask_flag_M << 2) |
      (self.mask_flag_m << 1) |
      (self.mask_flag_g << 0)
    )
  
  
  # scroll register
  
  def scrl_write(self, data):
    # self.frame.render(self, self.scanline)
    if self.scrl_latch:
      self.scrl_scroll_y = data
    else:
      self.scrl_scroll_x = data
    self.scrl_latch = not self.scrl_latch
  
  def scrl_get(self):
    return self.scrl_scroll_x | self.scrl_scroll_y << 5
  
  def scrl_set(self, data):
    # self.frame.render(self, self.scanline)
    self.scrl_scroll_x = data & 0b11111
    self.scrl_scroll_y = data >> 5
  
  
  # status register

  def stat_set_sprite_overflow(self, status):
    self.stat_flag_O = status

  def stat_get(self):
    return (
      self.stat_flag_V << 7 |
      self.stat_flag_S << 6 |
      self.stat_flag_O << 5
    )
  
  def stat_set(self, value):
    self.stat_flag_V = value & 0b10000000 != 0
    self.stat_flag_S = value & 0b01000000 != 0
    self.stat_flag_O = value & 0b00100000 != 0
  
  
  # PPU

  def tick(self, cycles):
    self.cycles += cycles

      
    if self.cycles < 341:
      return False
    
    if self.mask_flag_s and self.oam_data[0] == self.scanline and self.oam_data[3] <= self.cycles:
      self.stat_flag_S = 1
    self.cycles -= 341
    self.scanline += 1

    if self.scanline == 241:
      self.stat_flag_V = 1
      self.stat_flag_S = 0
      if self.ctrl_flag_V or True:
        self.nmi_interrupt = True

    elif self.scanline >= 262:
      self.scanline = 0
      self.nmi_interrupt = False
      self.stat_flag_S = 0
      self.stat_flag_V = 0
      return True
  
    return False

  def write_ctrl(self, data):
    prev_nmi = self.ctrl_flag_V
    self.ctrl_set(data)
    if (not prev_nmi) and self.ctrl_flag_V and self.stat_flag_V:
      self.nmi_interrupt = True

  def read_status(self):
    data = self.stat_get()
    self.stat_flag_V = 0
    self.addr_hi_ptr = 1
    self.scrl_latch = False
    return data

  def write_oam_data(self, data):
    self.oam_data[self.oam_addr] = data
    self.oam_addr = (self.oam_addr + 1) & 0xff

  def write_data(self, data):
    addr = self.addr_value[0] << 8 | self.addr_value[1]

    if 0x2000 <= addr < 0x3000:
      self.vram[(self.mirroring[addr - NAMETABLE_ADDR >> 10] << 10) + (addr & 0x3ff)] = data
    
    elif addr in [0x3f10, 0x3f14, 0x3f18, 0x3f1c]:
      self.palette[(addr ^ 0x10) & 0x1f] = data
    
    elif 0x3f00 <= addr < 0x4000:
      self.palette[addr & 0x1f] = data
    
    ### elif 0x3000 <= addr < 0x3f00:
    ###   print(f"PPU address {hex(addr)} is not supported")
    
    ### elif 0x0000 <= addr < 0x2000:
    ###   print(f"Attempted to write to CHR ROM space: {hex(addr)}")
    
    ### else:
    ###   print("Unexpected access to mirrored address: " + hex(addr))

    self.addr_inc(self.ctrl_vram_addr_inc())

  def read_data(self):
    addr = self.addr_value[0] << 8 | self.addr_value[1]
    self.addr_inc(self.ctrl_vram_addr_inc())

    if 0x0000 <= addr < 0x2000:
      return self.use_buffer(self.chr_rom[addr])

    elif 0x2000 <= addr < 0x3000:
      return self.use_buffer(self.vram[(self.mirroring[addr - NAMETABLE_ADDR >> 10] << 10) + (addr & 0x3ff)])
    
    elif 0x3f00 <= addr < 0x4000:
      return self.palette[addr & 0x1f]
    
    ### elif 0x3000 <= addr <= 0x3eff:
    ###   print(f"PPU address {hex(addr)} is not supported")
    
    ### else:
    ###   print("Unexpected access to mirrored address: " + hex(addr))
  
  def write_oam_dma(self, data):
    self.oam_data = (
      data[self.oam_addr:] +
      data[:self.oam_addr]
    )

  def use_buffer(self, expr):
    out, self.buffer = self.buffer, expr
    return out
  
  def attach_cart(self, cart: Cart):
    self.chr_rom = cart.chr_rom
    self.mirroring_type = cart.screen_mirroring
    self.mirroring = MIRROR_TYPES[self.mirroring_type]

  def detach_cart(self):
    # self.chr_rom = bytearray(2048)
    self.mirroring_type = Cart.MIRROR_HORIZONTAL
    self.mirroring = MIRROR_TYPES[self.mirroring_type]
    
  def reset(self):
    self.scanline = 0
    self.cycles = 0
    self.nmi_interrupt = False

  def poll_nmi(self):
    if self.nmi_interrupt:
      self.nmi_interrupt = False
      return True
    else:
      return False
