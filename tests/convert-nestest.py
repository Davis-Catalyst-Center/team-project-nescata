#!/usr/bin/env python3



with open("tests/nestest.log", "r") as f:
	og_lines = f.read().split("\n")[:-1]


print(og_lines[0])

# C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD PPU:  0, 21 CYC:7

new_lines = []
for i, line in enumerate(og_lines):

	addr = line[0:4]
	if i == 0: print(f"address: {addr}")

	assembly_bytes = line[6:14]
	if i == 0: print(f"assembly bytes: {assembly_bytes}")

	assembly = line[16:48]
	if i == 0: print(f"assembly: {assembly}")

	a_reg = line[50:52]
	if i == 0: print(f"A reg: {a_reg}")

	x_reg = line[55:57]
	if i == 0: print(f"X reg: {x_reg}")

	y_reg = line[60:62]
	if i == 0: print(f"Y reg: {y_reg}")

	p_reg = line[65:67]
	if i == 0: print(f"P reg: {p_reg}")

	sp_reg = line[71:73]
	if i == 0: print(f"SP reg: {sp_reg}")

	ppu_scanline = line[78:81]
	if i == 0: print(f"PPU scanline: {ppu_scanline}")

	ppu_cycle = line[82:85]
	if i == 0: print(f"PPU cycle: {ppu_cycle}")

	cycle = line[90:]
	if i == 0: print(f"Cycle: {cycle}")

	new_line = f"{addr} {assembly_bytes} a:{a_reg} x:{x_reg} y:{y_reg} p:{int(p_reg, 16):08b} sp:{sp_reg} cyc:{cycle}"
	new_lines.append(new_line)

	if i == 0:
		print("Generated line:")
		print(new_line)

with open("tools/nestest-convert.txt", "w") as f:
	f.write("\n".join(new_lines) + "\n")