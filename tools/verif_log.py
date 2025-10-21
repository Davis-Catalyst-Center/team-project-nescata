with open("cpu.log", "r") as f:
	log_lines = f.read().split("\n")

with open("tools/nestest_placeholder.txt", "r") as f:
	verif_lines = f.read().split("\n")


mismatches = 0
for i in range(8992):
	if log_lines != verif_lines:
		mismatches += 1
		print(f"mismatch at line {i+1}:")
		print(f"log: {log_lines[i]}")
		print(f"ver: {verif_lines[i]}")
	if mismatches >= 10:
		print("too many errors")
		break