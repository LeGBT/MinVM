#!/usr/local/bin/python3
import sys

tl = []
i = 0


def token_list(s):
    tl = []
    line = 1
    i = 0
    len_ = len(s)
    while i < len_:
        if i == len_-1:
            tl.append(("eof", "eof", line))
        elif s[i] == '(':
            tl.append(("lpar", "(", line))
        elif ord(s[i]) >= ord('0') and ord(s[i]) <= ord('9'):
            n = 0
            while ord(s[i]) >= ord('0') and ord(s[i]) <= ord('9'):
                n *= 10
                n += ord(s[i]) - ord('0')
                i += 1
            i -= 1
            tl.append(("num", n, line))
        elif s[i] == ')':
            tl.append(("rpar", ")", line))
        elif s[i] == '#':
            tl.append(("diese", "#", line))
        elif s[i] == '\n':
            tl.append(("newline", "newline", line))
            line += 1
        else:
            id_ = ""
            while ((ord(s[i]) > 32) and (ord(s[i]) < 127)
                   and (s[i] not in ['(', ')', ' ', '#', '\n'])):
                id_ += s[i]
                i += 1
            if s[i] not in [')', '(', ' ', '#', '\n']:
                print(f"Err({line}) {s[i]} is not valid after an id")
            tl.append(("id", id_, line))
        i += 1
        while i < len_ and s[i] == ' ':
            i += 1
    return tl


def next_token():
    global i, tl
    print(f"Token {i} was parsed {tl[i][1]}")
    i += 1
    return tl[i]


def current_token():
    global i, tl
    return tl[i]


# valid return_codes: 0:    continue
#                     1:    eof
#                     2:    missing ')'
#                     3:    missing '('
#                     4:    unexpected token
def S():
    if current_token()[0] is "eof":
        return 1
    if current_token()[0] is "newline":
        next_token()
        return 0
    elif current_token()[0] is "lpar":
        next_token()  # eating (
        err = E()
        if err:
            return err
        if current_token()[0] is not "rpar":
            return 2
        else:
            next_token()
            return err
    else:
        print("Error in S")
        return 4


def E():
    if current_token()[0] == "rpar":  # ε
        return 0  # we dont eat there is nothing here
    else:
        err = F()
        if err:
            return err
        else:
            return E()


def F():
    if current_token()[0] is "num":
        next_token()  # eating num
        return 0
    elif current_token()[0] is "id":
        next_token()  # eating id
        return 0
    elif current_token()[0] is "string":
        next_token()  # eating string
        return 0
    elif current_token()[0] is "diese":
        if next_token()[0] is not "lpar":  # eating #
            return 3
        print("#(")
        next_token()  # eating (
        err = E()
        if err:
            return err
        if current_token()[0] is not "rpar":
            return 2
        else:
            next_token()  # eating )
            print(")")
            return 0
    elif current_token()[0] is "lpar":
        print("(")
        next_token()  # eating (
        err = E()
        if err:
            return err
        if current_token()[0] is not "rpar":  # eating )
            return 2
        else:
            next_token()
            print(")")
            return 0
    else:
        print(f"Error in F {current_token()}")
        return 4


if len(sys.argv) is not 2:
    print("A single argument must be given.")

filename = sys.argv[1]
if filename[-3:] != ".li":
    print("Filename must end with .li")

with open(filename, 'r') as f:
    raw_file = f.read()

tl = token_list(raw_file)

r = 0
i = 0
while r == 0:
    print("S")
    r = S()
    print("")
if r is 2:
    print(f"Missing ')', got {current_token()}")

if r is 3:
    print(f"Missing '(', got {current_token()}")

if r is 4:
    print(f"Unexpected {current_token()[1]}")
