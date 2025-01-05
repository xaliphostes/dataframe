from dataframe import math, Serie, forEach

help(math)

# s.forEach(lambda x, id: print(x)) # ERROR if itemSize = 1

s1 = Serie(1, [1,2,3,4])
s2 = Serie(1, [1,2,3,4])
s3 = Serie(1, [1,2,3,4])
s = math.add(s1, s2, s3)
forEach(lambda x, id: print(x), s)
print()

s = math.div(s1, s2)
forEach(lambda x, id: print(x), s)
print()

s1 = Serie(2, [1,2,3,4])
s2 = Serie(2, [1,2,3,4])
s3 = Serie(2, [1,2,3,4])
s = math.add(s1, s2, s3)
forEach(lambda x, id: print(x), s)
print()

s1 = Serie(4, [1,2,3,4])
s2 = Serie(4, [1,2,3,4])
s3 = Serie(4, [1,2,3,4])
s = math.add(s1, s2, s3)
forEach(lambda x, id: print(x), s)
print()

