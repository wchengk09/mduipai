import os
L = int(1e6)
R = int(2e6)

while L < R:
    mid = (L + R) >> 1
    os.system('echo %s > in.txt' % mid)
    os.system('./a < in.txt > a.txt')
    os.system('./b < in.txt > b.txt')
    if os.system('diff -Z a.txt b.txt'): 
        R = mid
    else:
        L = mid + 1   
        
print(R)