
# valid return_codes: 0:    continue
#                     1:    eof
#                     2:    missing ')'
#                     3:    missing '('
#                     4:    unexpected token


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


def S(i, tl):
    if tl[i][0] is "eof":
        return 1, i
    if tl[i][0] is "newline":
        i += 1
        return 0, i
    elif tl[i][0] is "lpar":
        i += 1
        err, i = E(i, tl)
        if err:
            return err
        if tl[i][0] is not "rpar":
            return 2
        else:
            return err, i+1
    else:
        print("Error in S")
        return 4, i


def E(i, tl):
    if tl[i][0] == "rpar":  # ε
        return 0, i  # we dont eat there is nothing here
    else:
        err, i = F(i, tl)
        if err:
            return err, i
        else:
            return E(i, tl)


def F(i, tl):
    if tl[i][0] is "num":
        return 0, i+1
    elif tl[i][0] is "id":
        return 0, i+1
    elif tl[i][0] is "string":
        return 0, i+1
    elif tl[i][0] is "diese":
        if tl[i+1][0] is not "lpar":
            return 3, i+1
        print("#(")
        err, i = E(i+1, tl)
        if err:
            return err
        if tl[i][0] is not "rpar":
            return 2, i
        else:
            print(")")
            return 0, i+1
    elif tl[i][0] is "lpar":
        print("(")
        err, i = E(i+1, tl)
        if err:
            return err, i
        if tl[i][0] is not "rpar":  # eating )
            return 2, i
        else:
            print(")")
            return 0, i+1
    else:
        print(f"Error in F {current_token()}")
        return 4, i
