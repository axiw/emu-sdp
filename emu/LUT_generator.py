#!/usr/bin/env python3
import sys

input_file = sys.argv[1]
output_file = sys.argv[2]
bits = sys.argv[3]
include_header = sys.argv[4]
func_table_name = sys.argv[5]
lookup_table_name = sys.argv[6]

in_list = open(input_file, "r")
out = open(output_file, "w")

out.write("// AUTO GENERATED //\n")
out.write("//   DO NOT EDIT  //\n\n")

out.write("#include <stddef.h>\n\n")
out.write("#include <stdint.h>\n\n")
out.write("#include \"" + include_header + "\"\n\n")
out.write("void* " + func_table_name + "[] = {\n")

line = in_list.readline()
count = 0
out.write("\t{}, // {:d}\n".format(line.rstrip("\r\n"), count)) # First one is just the invalid LUT handler
count += 1
line = in_list.readline()
while line:
	if (line.strip()[0] == "#"):
		line = in_list.readline()
		continue
	out.write("\t{}, // {:d}\n".format(line.split(" ")[1].rstrip("\r\n"), count))
	line = in_list.readline()
	count += 1
out.write("};\n\n")

in_list.seek(0)
in_list.readline() # Skip invalid LUT handler

opcode_masks = []
line = in_list.readline()
while line:
	if (line.strip()[0] == "#"):
		line = in_list.readline()
		continue
	opcode_masks.append(line.split(" ")[0])
	line = in_list.readline()

opcode_result_table = []
for x in range(0, 1 << int(bits)):
	index = 1
	handler = 0
	for mask in opcode_masks:
		for i in range(0, int(bits)):
			exp = ("{:0" + bits + "b}").format(x)
			if (exp[i] != mask[i] and mask[i] != 'x' and mask[i] != 'X'):
				break
			if (i == (int(bits) - 1)):
				if (handler == 0):
					handler = index
				else:
					print("LUT generation failed!")
					print("Overlapping lookups possible")
					exit(1)
		index += 1
	opcode_result_table.append(handler)

out.write("uint8_t " + lookup_table_name + "[] = {\n")
count = 0
for x in opcode_result_table:
	out.write(("\t{:d}, // {:0" + bits + "b}\n").format(x, count))
	count += 1
out.write("};\n")

in_list.close()
out.close()
