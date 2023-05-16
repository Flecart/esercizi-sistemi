import os
import sys

def find_all_file():
    files = []
    for filename in os.listdir('.'):
        if os.path.isfile(filename):
            files.append(filename)

    return files;

def same_content(file1: str, file2: str):
    if os.path.samefile(file1, file2):
        return True
    if os.path.getsize(file1) != os.path.getsize(file2):
        return False

    with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
        while True:
            b1 = f1.read(4096)
            b2 = f2.read(4096)
            if b1 != b2:
                return False

            # read returns 0 on end of file, or None, so this checks is for sure an exit.
            if not b1:
                return True

def return_same(files: list[str]):
    same = []
    for file in files:
        for file2 in files:
            if file != file2:
                if same_content(file, file2):
                    same.append((file, file2))
    return same

def fix_files(couples: list[(str, str)]):
    for couple in couples:
        os.remove(couple[0])
        os.link(couple[1], couple[0])

def main():
    if (len(sys.argv) != 2):
        print("Usage: python3 terzo.py <dirname>")
        return 1
    # check if input is dir
    if not os.path.isdir(sys.argv[1]):
        print ("Error: input is not a directory")
        return 1
    
    os.chdir(sys.argv[1])
    files = find_all_file()
    same = return_same(files)
    fix_files(same)

    return 0

if __name__ == '__main__':
    main()
