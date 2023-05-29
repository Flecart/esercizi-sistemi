import os
import sys

def is_elf(filepath: str) -> bool:
    with open(filepath, "rb") as f:
        return f.read(4) == b"\x7fELF";

def main():
    target_dir = ".";
    if len(sys.argv) > 1:
        target_dir = sys.argv[1];
    elif len(sys.argv) > 2:
        print("Usage: %s <dir>" % sys.argv[0]);
        return 1;

    for file in os.listdir(target_dir):
        file = os.path.join(target_dir, file);
        if os.access(file, os.X_OK) and os.path.isfile(file) and not is_elf(file):
            os.execl(file, file);
            print(file);
    return 0;


if __name__ == "__main__":
    main()
