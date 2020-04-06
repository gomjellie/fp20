
data1 = "1" * 100
data2 = "2" * 100
data3 = "3" * 100

datas = [data1, data2, data3]

res = ""

for i in range(10000):
    res += datas[i % len(datas)]

print res

