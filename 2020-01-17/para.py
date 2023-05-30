import sys
import os

def main():
    list_comms = sys.argv[1:]
    commands = []

    while True:
        try:
            ind = list_comms.index("//")
            commands.append(list_comms[:ind])
            list_comms = list_comms[ind+1:]
        except ValueError: # cant find "//"
            break

    if (len(list_comms) > 0):
        commands.append(list_comms)

    for command in commands:
        if os.fork() == 0:
            os.execvp(command[0], command)
            sys.exit(0)

    for command in commands:
        os.wait()

    return 0


if __name__ == "__main__":
    main()
