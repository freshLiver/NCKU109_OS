from random import randint

with open("input.cvs",'a') as file:
    lines = 4900000
    for i in range(lines):
        line = ""
        if ( i % 100000 == 0 ):
            print(f"{i}/{lines}")

        for j in range(20):
            line += f"{randint(-2147483648,2147483647)}{'|' if j != 19 else ''}"
        file.write(f"{line}\n")
