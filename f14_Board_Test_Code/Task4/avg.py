n = 4

def avg(avg, num, nu):
	num = num + (avg * (nu - 1))
	avg = num / nu
	return avg

num = [8, 7, 3, 2]
a = 0
b = 0
sum = 0
i = 0

while(i != (n)):
	a = avg(a, num[i], i + 1)
	sum = sum + num[i]
	print(bin(a))
	i = i + 1

b = sum / n
print(bin(b))

input("Press Enter to continue...")