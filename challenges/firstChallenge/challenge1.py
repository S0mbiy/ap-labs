import sys
var = sys.argv[1]
if type(var) is not str:
    raise ValueError
words = {}
l = 0
tmp_l = 0
for c in var:
    if words.get(c) is not None:
        if tmp_l > l:
            l = tmp_l
        words = {}
        tmp_l = 0
    words[c] = True
    tmp_l += 1
if tmp_l > l:
    l = tmp_l
print(l)
