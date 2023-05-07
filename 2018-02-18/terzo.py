import os

def count(process_id: int) -> int:
    dir_name = f"/proc/{process_id}/fd"
    
    try:
        dir_list = os.listdir(dir_name)
    except PermissionError:
        return -1
    
    fd_count = 0
    for path in dir_list:
        fd_count += 1
    return fd_count

if __name__ == "__main__":

    dir_name = "/proc/"
    
    files = []
    for process_number in os.listdir(dir_name):
        if process_number.isnumeric():
            fds = count(int(process_number))
            if fds != -1:
                files.append((fds, process_number))
    
    files = sorted(files)

    for file in files:
        print(f"process {file[1]} has {file[0]} fd open")
