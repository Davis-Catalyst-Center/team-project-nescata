with open("expected1.txt", "r") as f:
  elines = f.read().split("\n")

with open("log.txt", "r") as f:
  llines = f.read().split("\n")

err_count = 0

for i in range(len(elines)):
  if elines[i][:16] + elines[i][20:57] != llines[i][:16] + llines[i][20:57]:
    err_count += 1
    print(f"mismatch at line {i+1} in log.txt:")
    print("expected:")
    print(f"  {elines[i-1]}")
    print(f"> {elines[i]}")
    print(f"  {elines[i+1]}")
    print("got:")
    print(f"  {llines[i-1]}")
    print(f"> {llines[i]}")
    print(f"  {llines[i+1]}")
    print()
    if err_count > 10:
      break