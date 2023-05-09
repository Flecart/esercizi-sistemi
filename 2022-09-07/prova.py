#! /usr/bin/env python3

import sys
import os

def is_elf(filename):
    if (os.path.isdir(filename)):
        return False

    with open(filename, 'rb') as f:
        return f.read(4) == b'\x7fELF'

def get_nums(dirname):
    count = 0
    for file in os.listdir(dirname):
        name = os.path.join(dirname, file)
        if is_elf(name):
            count += os.path.getsize(name)
    
    return count

def main():
    count = 0;
    for line in sys.argv[1:]:
        count += get_nums(line);

    if len(sys.argv) == 1:
        count += get_nums('.');

    print(count);


if __name__ == '__main__':
    main()
