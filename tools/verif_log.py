with open("cpu.log", "r") as f:
	# splitlines avoids keeping a final empty string if file ends with a newline
	log_lines = f.read().splitlines()

with open("tools/nestest_placeholder.txt", "r") as f:
	verif_lines = f.read().splitlines()

mismatches = 0
max_lines = max(len(log_lines), len(verif_lines))
for i in range(max_lines):
	log_line = log_lines[i] if i < len(log_lines) else "<MISSING>"
	ver_line = verif_lines[i] if i < len(verif_lines) else "<MISSING>"
	if log_line != ver_line:
		mismatches += 1
		print(f"\nmismatch at line {i+1}:")
		for j in range(-5, 2):
			index = i + j
			log_context = log_lines[index] if 0 <= index < len(log_lines) else "<MISSING>"
			ver_context = verif_lines[index] if 0 <= index < len(verif_lines) else "<MISSING>"
			prefix = ">> " if j == 0 else "   "
			print(f"{prefix}log: {log_context}")
			if j >= 0: print(f"{prefix}ver: {ver_context}")
	if mismatches >= 3:
		print("too many errors")
		break