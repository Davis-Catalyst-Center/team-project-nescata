import engine
import json
import pygame
import random
import sys


with open("config.json") as f:
  config = json.load(f)

with open("log.txt", "w") as f:
  pass
  

if len(sys.argv) == 2:
  cart_name = sys.argv[1]
elif "cart" in config:
  cart_name = config["cart"]
else:
  print("No cart specified, defaulting to mario.nes")
  cart_name = "carts/mario.nes"

print("Loading cart: " + cart_name)

nes_core = engine.Core()

cart = engine.Cart(cart_name)
controller = engine.Controller()
nes_core.attach_controller(controller)
nes_core.attach_cart(cart)

keep_running = True

def button_state(keys):
  return (
    keys[pygame.K_s]     << 0 |
    keys[pygame.K_a]     << 1 |
    keys[pygame.K_q]     << 2 |
    keys[pygame.K_w]     << 3 |
    keys[pygame.K_UP]    << 4 |
    keys[pygame.K_DOWN]  << 5 |
    keys[pygame.K_LEFT]  << 6 |
    keys[pygame.K_RIGHT] << 7
  )


def frame_interrupt():
  global keep_running

  for event in pygame.event.get():
    if event.type == pygame.QUIT:
      nes_core.cpu.killed = True
      keep_running = False
    if event.type == pygame.KEYDOWN:
      match event.key:
        case pygame.K_ESCAPE:
          nes_core.cpu.killed = not nes_core.cpu.killed
        case pygame.K_k:
          nes_core.cpu.killed = True
        case pygame.K_j:
          nes_core.cpu.killed = False
        case pygame.K_r:
          nes_core.cpu.reset()
        case pygame.K_m:
          for i in range(10):
            addr = random.randint(0, 0xffff)
            val = random.randint(0, 0xff)
            nes_core.bus.mem_write(addr, val)
        case pygame.K_o:
          with open("average_opcode_time.txt", "w") as f:
            for s in nes_core.cpu.calc_op_average():
              f.write(s + "\n")
        case pygame.K_h:
          cart = engine.Cart(input("\ncart path: "))
          nes_core.detach_cart()
          nes_core.attach_cart(cart)
          nes_core.bus.mem_write(addr, val)
        case pygame.K_n:
          addr = int(input("\naddress (0xXXXX): "), 16)
          val = int(input("value (0xXX): "), 16)
          nes_core

  nes_core.controllers[0].buttons = button_state(pygame.key.get_pressed())

  if nes_core.bus.frames % config["fps_report_rate"] == 0: print(f"\rFPS: {round(nes_core.fps, 2)}", end="")

  

nes_core.interrupt = frame_interrupt


for i in range(256):
  if nes_core.cpu.OPCODE_ADDR_MAP[i] == 2:
    print(f"{i:02X}: {i:08b}")


nes_core.reset()

while keep_running:
  nes_core.run_without_reset()
  frame_interrupt()
  nes_core.reset()
  # nes_core.cpu.save_log()
  # nes_core.cpu.dump()
