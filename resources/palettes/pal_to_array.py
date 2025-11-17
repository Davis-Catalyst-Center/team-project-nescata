with open("FCEUX.pal", 'rb') as f:
	data = f.read()

colors = []

for i in range(0, len(data), 3):
	r = data[i]
	g = data[i+1]
	b = data[i+2]
	colors.append((r, g, b))

while len(colors) < 64:
	colors.append((0, 0, 0))

print("uint32 paletteARGB[64] = {", end='')
for i, (r, g, b) in enumerate(colors):
	if (i + 1) % 8 == 1:
		print("\n\t", end='')
	print(f"0xFF{r:02X}{g:02X}{b:02X}, ", end='')
print("\n};")
