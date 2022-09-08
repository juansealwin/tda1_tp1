import sys
import os.path

POS_ARGS = 1
POS_N = 0
POS_FILENAME = 1
POS_ACQUAINTANCES = 2
MIN_ACQUAINTANCES = 4
COMMA_CHAR = ","

def check_arguments(args):
    return args[POS_N].isnumeric() & os.path.isfile(args[POS_FILENAME]) 

# Obtain and check the arguments
args = sys.argv[POS_ARGS:]

if check_arguments(args) == False:
    print("Invalid arguments\n")
    
n = int(args[POS_N])
file_name = args[POS_FILENAME]

file = open(file_name, 'r')
for line in file:
    
    if (n <= 0):
        break
    
    splited_line = line.split(COMMA_CHAR, POS_ACQUAINTANCES)
    acquaintances = splited_line[-1]
    number_of_acquaintances = acquaintances.split(COMMA_CHAR, MIN_ACQUAINTANCES - 1)
    
    if len(number_of_acquaintances) >= MIN_ACQUAINTANCES:
        guest = COMMA_CHAR.join(splited_line[:POS_ACQUAINTANCES])
        print(guest)
        n = n - 1
  
# Close file
file.close()