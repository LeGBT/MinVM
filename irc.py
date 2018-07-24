import sys
import string


def get_bytecode_map():
    list_map = []
    with open("vm2.h") as f:
        list_map = f.readlines()
    list_map = [l.split() for l in list_map]
    list_map = [l[1:] for l in list_map]
    list_map = [(a, int(b)) for a, b in list_map]
    return dict(list_map)


def eat_spaces(text, i):
    while (text[i] is " ") or (text[i] is "\n"):
        if i == len(text) - 1:
            return "eof"
        else:
            i += 1
    return i


def first_pass(text):
    tokens = []
    i = eat_spaces(text, 0)
    while i is not "eof":
        j = i
        while text[j] not in [" ", "\n", ":"]:
            j += 1
        if text[j] is ":":
            j += 1
        tokens.append(text[i:j])
        i = j
        i = eat_spaces(text, i)
    tokens.append("EOF")
    return tokens


def get_id(token):
    if token == "EOF":
        print("Error, got EOF instead of a variable name.")
        sys.exit()
    if ":" in token:
        print("Error, : is forbidden in a variable name.")
        sys.exit()
    if token[0] in string.digits:
        print("A variable can't start with a digit.")
        sys.exit()
    for c in token:
        if (c not in string.ascii_lowercase) and (c not in string.digits):
            print(f"Unvalid character \"{c}\" in variable name.")
            sys.exit()
    return token


def get_num(token):
    if token == "0":
        return 0
    else:
        if token[0] == "0":
            print("Error, non zero numbers can't begin with a 0.")
            sys.exit()
        i = 0
        for c in token:
            if c not in string.digits:
                print(f"Error, expected a number, instead got {token}")
                sys.exit()
            i *= 10
            i += int(c)
        return i


def E(tokens, out):
    if tokens[0] == "EOF":
        out.append(("EOF",))
        return None, out
    tokens, out = F(tokens, out)
    tokens, out = E(tokens, out)
    return tokens, out


def F(tokens, out):
    if tokens[0] in ["NOP", "EXIT", "NOT", "POP", "DUP" "INC",
                     "DEC", "PUTC", "ADD", "SUB", "MUL", "EQ"]:
        out.append((tokens[0],))
        del tokens[0]
        return tokens, out
    if tokens[0][-1] == ":":
        out.append(("label", tokens[0][:-1]))
        del tokens[0]
        return tokens, out
    if tokens[0] in ["STORE", "LOAD"]:
        t = tokens[0]
        del tokens[0]
        out.append((t, get_id(tokens[0])))
        del tokens[0]
        return tokens, out
    if tokens[0] in ["JUMP", "JIF"]:
        t = tokens[0]
        del tokens[0]
        out.append((t, get_id(tokens[0])))
        del tokens[0]
        return tokens, out
    if tokens[0] == "PUSH":
        t = tokens[0]
        del tokens[0]
        out.append((t, get_num(tokens[0])))
        del tokens[0]
        return tokens, out
    print(f"Unknown token {tokens[0]}")
    sys.exit()


def transform(tl):
    variable_map = {}
    variable_count = 0
    for token in tl:
        if token[0] == "STORE":
            if token[1] not in variable_map:
                variable_map[token[1]] = variable_count
                variable_count += 1
    ntl = []
    for token in tl:
        if token[0] in ["STORE", "LOAD"]:
            ntl.append((token[0], variable_map[token[1]]))
        elif token[0] == "PUSH":
            if token[1] < 2**8:
                ntl.append(("PUSHB", token[1]))
            elif token[1] < 2**16:
                i = token[1]
                b0 = i & 255
                i >>= 8
                b1 = i & 255
                ntl.append(("PUSHW", b1, b0))
            elif token[1] < 2**32:
                i = token[1]
                b0 = i & 255
                i >>= 8
                b1 = i & 255
                i >>= 8
                b2 = i & 255
                i >>= 8
                b3 = i & 255
                ntl.append(("PUSHD", b3, b2, b1, b0))
            elif token[1] < 2**64:
                i = token[1]
                b0 = i & 255
                i >>= 8
                b1 = i & 255
                i >>= 8
                b2 = i & 255
                i >>= 8
                b3 = i & 255
                i >>= 8
                b4 = i & 255
                i >>= 8
                b5 = i & 255
                i >>= 8
                b6 = i & 255
                i >>= 8
                b7 = i & 255
                ntl.append(("PUSHQ", b7, b6, b5, b4, b3, b2, b1, b0))
            else:
                print(f"Error, {token[1]} is bigger than 2**64-1")
                sys.exit()
        else:
            ntl.append(token)
    label_map = {}
    instr_count = 0
    for token in ntl:
        if token[0] == "label":
            label_map[token[1]] = instr_count
        elif token[0] in ["STORE", "LOAD", "PUSHB"]:
            instr_count += 2
        elif token[0] in ["JUMP", "JIF", "PUSHW"]:
            instr_count += 3
        elif token[0] == "PUSHD":
            instr_count += 5
        elif token[0] == "PUSHQ":
            instr_count += 9
        else:
            instr_count += 1
    nt = []
    for token in ntl:
        if token[0] in ["JUMP", "JIF"]:
            if token[1] not in label_map:
                print(f"Can't find label {token[1]} !")
                sys.exit()
            addr = label_map[token[1]]
            if addr > 2**16 - 1:
                print("Error out of memory address.")
                sys.exit()
            addr1 = addr >> 8
            addr0 = addr & 255
            nt.append((token[0], addr1, addr0))
        elif token[0] == "label":
            pass
        else:
            nt.append(token)
    instructions = []
    mapping = get_bytecode_map()
    for token in nt:
        if token[0] in ["JUMP", "JIF", "PUSHW"]:
            instructions.append(mapping[token[0]])
            instructions.append(token[1])
            instructions.append(token[2])
        elif token[0] in ["STORE", "LOAD", "PUSHB"]:
            instructions.append(mapping[token[0]])
            instructions.append(token[1])
        elif token[0] == "PUSHD":
            instructions.append(mapping[token[0]])
            instructions.append(token[1])
            instructions.append(token[2])
            instructions.append(token[3])
            instructions.append(token[4])
        elif token[0] == "PUSHQ":
            instructions.append(mapping[token[0]])
            instructions.append(token[1])
            instructions.append(token[2])
            instructions.append(token[3])
            instructions.append(token[4])
            instructions.append(token[5])
            instructions.append(token[6])
            instructions.append(token[7])
            instructions.append(token[8])
        elif token[0] == "EOF":
            pass
        else:
            instructions.append(mapping[token[0]])
    return instructions


def parse(raw):
    _, tokens = E(first_pass(raw), [])
    tokens = transform(tokens)
    return tokens
