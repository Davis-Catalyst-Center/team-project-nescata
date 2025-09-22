import pygame
import math
import json
from .cart import Cart
from .ppu import PPU

with open("config.json") as f:
  config = json.load(f)

SCALE = config["scale"]
PSCALE = config["pscale"]
XSCALE = SCALE * PSCALE
WIDTH = 256
HEIGHT = 240

SPRITE_MODE = 0
BG_MODE = 1


SYSTEM_PALETTE = [
  (0x80, 0x80, 0x80), (0x00, 0x3D, 0xA6), (0x00, 0x12, 0xB0), (0x44, 0x00, 0x96),
  (0xA1, 0x00, 0x5E), (0xC7, 0x00, 0x28), (0xBA, 0x06, 0x00), (0x8C, 0x17, 0x00),
  (0x5C, 0x2F, 0x00), (0x10, 0x45, 0x00), (0x05, 0x4A, 0x00), (0x00, 0x47, 0x2E),
  (0x00, 0x41, 0x66), (0x00, 0x00, 0x00), (0x05, 0x05, 0x05), (0x05, 0x05, 0x05),
  (0xC7, 0xC7, 0xC7), (0x00, 0x77, 0xFF), (0x21, 0x55, 0xFF), (0x82, 0x37, 0xFA),
  (0xEB, 0x2F, 0xB5), (0xFF, 0x29, 0x50), (0xFF, 0x22, 0x00), (0xD6, 0x32, 0x00),
  (0xC4, 0x62, 0x00), (0x35, 0x80, 0x00), (0x05, 0x8F, 0x00), (0x00, 0x8A, 0x55),
  (0x00, 0x99, 0xCC), (0x21, 0x21, 0x21), (0x09, 0x09, 0x09), (0x09, 0x09, 0x09),
  (0xFF, 0xFF, 0xFF), (0x0F, 0xD7, 0xFF), (0x69, 0xA2, 0xFF), (0xD4, 0x80, 0xFF),
  (0xFF, 0x45, 0xF3), (0xFF, 0x61, 0x8B), (0xFF, 0x88, 0x33), (0xFF, 0x9C, 0x12),
  (0xFA, 0xBC, 0x20), (0x9F, 0xE3, 0x0E), (0x2B, 0xF0, 0x35), (0x0C, 0xF0, 0xA4),
  (0x05, 0xFB, 0xFF), (0x5E, 0x5E, 0x5E), (0x0D, 0x0D, 0x0D), (0x0D, 0x0D, 0x0D),
  (0xFF, 0xFF, 0xFF), (0xA6, 0xFC, 0xFF), (0xB3, 0xEC, 0xFF), (0xDA, 0xAB, 0xEB),
  (0xFF, 0xA8, 0xF9), (0xFF, 0xAB, 0xB3), (0xFF, 0xD2, 0xB0), (0xFF, 0xEF, 0xA6),
  (0xFF, 0xF7, 0x9C), (0xD7, 0xE8, 0x95), (0xA6, 0xED, 0xAF), (0xA2, 0xF2, 0xDA),
  (0x99, 0xFF, 0xFC), (0xDD, 0xDD, 0xDD), (0x11, 0x11, 0x11), (0x11, 0x11, 0x11)
]

PS_NAMETABLE = {
  (Cart.MIRROR_VERTICAL, 0x2000):   (0x000, 0x400, 0x400, 0x800),
  (Cart.MIRROR_VERTICAL, 0x2800):   (0x000, 0x400, 0x400, 0x800),
  (Cart.MIRROR_HORIZONTAL, 0x2000): (0x000, 0x400, 0x400, 0x800),
  (Cart.MIRROR_HORIZONTAL, 0x2400): (0x000, 0x400, 0x400, 0x800),
  
  (Cart.MIRROR_VERTICAL, 0x2400):   (0x400, 0x800, 0x000, 0x400),
  (Cart.MIRROR_VERTICAL, 0x2c00):   (0x400, 0x800, 0x000, 0x400),
  (Cart.MIRROR_HORIZONTAL, 0x2800): (0x400, 0x800, 0x000, 0x400),
  (Cart.MIRROR_HORIZONTAL, 0x2c00): (0x400, 0x800, 0x000, 0x400)
}


class Frame:

  def __init__(self, palette):

    self.window = pygame.display.set_mode((WIDTH * XSCALE, HEIGHT * XSCALE))#, vsync=1)
    self.buffer = pygame.Surface((WIDTH, HEIGHT))
    self.palette = palette
    self.cache = {}
    self.scanline = 0

  def render(self, ppu: PPU, scanline=262, cycle=256, new_frame=False):
    if scanline == self.scanline:
      return
    if new_frame:
      self.scanline = 0
      self.render_sprites(ppu, 1)
      # return
    
    if scanline < 240:
      lower_scr = self.buffer.subsurface((0, scanline, 256, 240 - scanline)).copy()
      lower_scr.fill((0, 0, 0, 0))
    
    # col = self.palette[self.ppu.palette[0]]

    scroll_x = ppu.scrl_scroll_x
    scroll_y = ppu.scrl_scroll_y
    

    ntbl_addr = PS_NAMETABLE[ppu.mirroring_type, ppu.ctrl_nametable_addr()]
    p_ntbl = ppu.vram[ntbl_addr[0]:ntbl_addr[1]]
    s_ntbl = ppu.vram[ntbl_addr[2]:ntbl_addr[3]]
    
    scan_range = range(self.scanline, scanline)

    
    self.render_name_table(
      ppu, p_ntbl,
      (scroll_x, scroll_y, 256, 240),
      -scroll_x, -scroll_y,
      scan_range
    )
    if scroll_x > 0:
      self.render_name_table(
        ppu, s_ntbl,
        (0, 0, scroll_x, 240),
        256 - scroll_x, 0,
        scan_range
      )
    elif scroll_y > 0:
      self.render_name_table(
        ppu, s_ntbl,
        (0, 0, 256, scroll_y),
        0, 240 - scroll_y,
        scan_range
      )
    
    self.render_sprites(ppu, 0)
    self.scanline = scanline

    if scanline < 240:
      self.buffer.blit(lower_scr, (0, scanline))



  def render_name_table(self, ppu: PPU, nametable, viewport, shift_x, shift_y, scan_range):

    bank = ppu.ctrl_bknd_pattern_addr()
    attribute_table = nametable[0x3c0:0x400]

    for i in range(0x03c0):

      tile_row = i >> 5
      if not (tile_row << 3) + shift_y in scan_range:
        return
      tile_col = i & 0b00011111
      tile_idx = nametable[i]
      tile_mem = bank | (tile_idx << 4)
  
      self.render_nt_tile(
        ppu.chr_rom[tile_mem : tile_mem + 16],
        self.bg_palette(ppu, attribute_table, tile_col, tile_row),
        tile_col << 3, tile_row << 3,
        viewport, shift_x, shift_y,
        tile_idx
      )
    
  

  def render_nt_tile(self, tile, palette, x_pos, y_pos, viewport, shift_x, shift_y, tile_idx):
    if x_pos < viewport[0] - 8\
    or x_pos >= viewport[2]:
      return
    if y_pos < viewport[1] - 8\
    or y_pos >= viewport[3]:
      return
    
    self.blit_tile(tile, palette, x_pos + shift_x, y_pos + shift_y, tile_idx)
    
    
    
  def render_sprites(self, ppu, priority):

    for i in range(0x00, len(ppu.oam_data), 0x04):
      
      attr = ppu.oam_data[i | 2]
      if (attr & 0b00100000) >> 5 != priority:
        continue
      
      tile_idx = ppu.oam_data[i | 1]
      # bank = (tile_idx & 0b00000001) << 16

      tile_pos = ppu.ctrl_sprt_pattern_addr() + (tile_idx << 4)

      self.blit_tile(
        ppu.chr_rom[tile_pos : tile_pos + 16],
        self.sprite_palette(ppu, attr & 0b00000011),
        ppu.oam_data[i | 3], ppu.oam_data[i], tile_idx,
        (attr & 0b01000000 != 0, attr & 0b10000000 != 0)
      )
  
  def blit_tile(self, tile, palette, x_pos, y_pos, tile_idx, flip=(0, 0)):
    tile = self.cache_return_tile(tile, palette, tile_idx, flip)
    if tile is None:
      return
    
    self.buffer.blit(tile, (x_pos, y_pos))
    
    
    
  def cache_return_tile(self, tile, palette, tile_idx, flip):
    name = hash((tile_idx, palette, flip))
    if name in self.cache:
      return self.cache[name]
    if not tile:
      self.cache[name] = None
      return None
    
    img = bytearray(256) # 8w x 8h x 4c
    for y in range(8):
      upper = tile[y + 8] << 1
      lower = tile[y]
      for x in range(8):
        value = (upper & 2) | (lower & 1)
        upper >>= 1
        lower >>= 1
        px = x if flip[0] else 7 - x
        pi = ((y << 3) | px) << 2
        if value == 0:
          img[pi:pi + 4] = [0, 0, 0, 0]
          continue
        img[pi:pi + 4] = list(self.palette[palette[value]]) + [255]
    img = pygame.image.frombytes(bytes(img), (8, 8), "RGBA", flip[1])
    self.cache[name] = img
    return img


  
  def sprite_palette(self, ppu: PPU, palette_idx):
    palette_start = 0x11 + (palette_idx << 2)
    return (
      ppu.palette[0],
      ppu.palette[palette_start],
      ppu.palette[palette_start + 1],
      ppu.palette[palette_start + 2]
    )
    
    

  def bg_palette(self, ppu: PPU, attr_table, tile_col, tile_row):
    palette_start = 1 + (((attr_table[(tile_row << 1 & 0b1111000) + (tile_col >> 2)] >> (((tile_row & 0b10) << 1) | (tile_col & 0b10))) & 0b11) << 2)
    
    return (
      ppu.palette[0],
      ppu.palette[palette_start],
      ppu.palette[palette_start + 1],
      ppu.palette[palette_start + 2]
    )



  def set_pixel(self, x, y, rgba):
    self.window.set_at((x, y), tuple(rgba))
  
  
  
  def fancyscale_buffer(self):
    
    if PSCALE == 1:
      surface = self.buffer
    else:
      surface = pygame.transform.scale(self.buffer, (WIDTH * PSCALE, HEIGHT * PSCALE))
    
    if SCALE == 1:
      return surface
    for i in range(max(0, math.ceil(math.log2(SCALE)))):
      surface = pygame.transform.scale2x(surface)
    
    surface = pygame.transform.scale(surface, (WIDTH * XSCALE, HEIGHT * XSCALE))
    
    return surface
      
  
  
  def update(self, ppu: PPU):
    self.scanline = 0
    self.window.blit(self.fancyscale_buffer(), (0, 0))
    pygame.display.flip()
    self.buffer.fill(self.palette[ppu.palette[0]])