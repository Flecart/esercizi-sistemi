#! /bin/env python

import os
import sys

def find_names_dirs(dirname: str):
    names_dirs = []
    for name in os.listdir(dirname):
        filename = os.path.join(dirname, name)
        if os.path.isdir(filename):
            subdir_names_dirs = find_names_dirs(filename)
            names_dirs.extend(subdir_names_dirs)
        elif os.path.isfile(filename):
            names_dirs.append((name, dirname))
    return names_dirs

def main():
    if (len(sys.argv) != 2):
        print("Usage: python3 terzo.py <dirname>")
        return 1
    # check if input is dir
    if not os.path.isdir(sys.argv[1]):
        print("Error: input is not a directory")
        return 1

    os.chdir(sys.argv[1])
    names_dirs = find_names_dirs(".")
    names_dirs.sort(key=lambda x: x[0])
    
    if (len(names_dirs) > 0):
        print(f"{names_dirs[0][0]}: {names_dirs[0][1]}", end="")
        for i in range(1, len(names_dirs)):
            if names_dirs[i][0] != names_dirs[i-1][0]:
                print(f"\n{names_dirs[i][0]}: {names_dirs[i][1]}", end="")
            else:
                print(f" {names_dirs[i][1]}", end="")

        print()

    return 0

if __name__ == '__main__':
    main()
