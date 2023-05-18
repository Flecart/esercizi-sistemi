import os
import sys
def main():
    if len(sys.argv) != 2:
        print("Usage: python terzo.py <dirname>")
        return 1 

    dirname = sys.argv[1]
    if not os.path.isdir(dirname):
        print("Error: <dirname> must be a directory")
        return 1

    dic = {}
    for filename in os.listdir(dirname):
        result = os.popen("file " + dirname + "/" + filename).read().split(":")[1].strip()
        if result not in dic:
            dic[result] = []
        dic[result].append(filename)

    for r in dic:
        print(r + ":")
        for filename in dic[r]:
            print("\t" + filename)
    return 0;


if __name__ == '__main__':
    main()
