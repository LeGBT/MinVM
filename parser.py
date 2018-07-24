#!/usr/local/bin/python3
import sys
import lispc
import irc

conversions = {"ir": "bc",
               "li": "at"}


if len(sys.argv) is not 2:
    print("Need a single filename.")
    sys.exit()

filename = sys.argv[1]
if filename[-3:-2] is not ".":
    print("Not a valid file.")
    sys.exit()

try:
    with open(filename, 'r') as f:
        raw_file = f.read()
except IOError:
    print(f"Unable to open {filename}.")
    sys.exit()


ext = filename[-2:]
if ext == "ir":
    out = irc.parse(raw_file)
    with open(filename[0:-2]+"bc", 'wb') as f:
        f.write(bytes(out))
elif ext == "li":
    tl = lispc.token_list(raw_file)
    print(tl)

    r = 0
    i = 0
    while r == 0:
        print("S")
        r = lispc.S(0, tl)
        print("")
    if r is 2:
        print(f"Missing ')', got {current_token()}")

    if r is 3:
        print(f"Missing '(', got {current_token()}")

    if r is 4:
        print(f"Unexpected {current_token()[1]}")
else:
    print(f"{ext} format is not supported.")
