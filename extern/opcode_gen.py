import csv
from collections import defaultdict

if __name__ == "__main__":
    instructions  = defaultdict(list)
    with open("opcode.csv") as f:
        stream = csv.reader(f)
        for row in stream:
            if not row[1] or row[1] in ("CS", "DS", "ES", "SS"):
                continue
            instructions[row[1]].append(row[0])

    with open("./opcodes.h", "w") as out:
        out.write("enum class OPCODES {\n")
        for instruction in instructions:
            if len(instructions[instruction]) == 1:
                out.write("{}={},\n".format(instruction, instructions[instruction][0]))
            else:
                for opcode in instructions[instruction]:
                    opcode_postfix = opcode[-2:]
                    out.write("{}_{}={},\n".format(instruction, opcode_postfix, opcode))
        out.write("};")