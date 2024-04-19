import random

SIZE = 1000000
N = 1000000

numbers = [7, 696, 12321]

while len(numbers) < SIZE:
    numbers.append(random.randint(1, N))

with open('random_numbers.txt', 'w') as file:
    for number in numbers:
        file.write(str(number) + '\n')
