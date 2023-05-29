import shutil
import sys
import os

def copy_dir_same(first, second, first_dest, second_dest):
    os.mkdir(first_dest)
    os.mkdir(second_dest)

    first_files = dict()
    for root, _, files in os.walk(first):
        for file in files:
            first_files[file] = root;

    for root, _, files in os.walk(second):
        for file in files:
            if file in first_files:
                shutil.copy(os.path.join(first_files[file], file), os.path.join(first_dest, file))
                shutil.copy(os.path.join(root, file), os.path.join(second_dest, file))

    return 0


def main():
    if len(sys.argv) != 5:
        print('Usage: python3 terzo.py <dir> <dir> <dir> <dir>')
        return 1

    first = sys.argv[1]
    second = sys.argv[2]

    first_dest = sys.argv[3]
    second_dest = sys.argv[4]

    # check if first and second exist and are directories
    if not os.path.isdir(first):
        print('First argument is not a directory')
        return 1

    if not os.path.isdir(second):
        print('Second argument is not a directory')
        return 1

    # check if first dest or second exist
    if os.path.exists(first_dest):
        print('First destination already exists')
        return 1

    if os.path.exists(second_dest):
        print('Second destination already exists')
        return 1
    
    copy_dir_same(first, second, first_dest, second_dest)

    return 0


if __name__ == '__main__':
    main()
