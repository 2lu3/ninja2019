import glob
import subprocess
import os

# is_debug = True
is_debug = False

def printforDebug(message):
    if is_debug:
        print(message)

tcc_path_expectations = ["g++"]

code_path_expectations = ["./../code(c++)/", "./../code/", "./code(c++)/", "./code/"]

def main():
    code_path = None
    for investigating_code_path in code_path_expectations:
        if os.path.isdir(investigating_code_path):
            printforDebug("find source codefolder : " + investigating_code_path)
            code_path = investigating_code_path

    while code_path is None:
        print("I could not source code folder. Please write the correct path here")
        path_inputed = input()
        printforDebug("user_input : " + path_inputed)
        if os.path.isdir(path_inputed):
            code_path = path_inputed
            printforDebug("exist")
        else:
            printforDebug("not exist")


    if code_path is None:
        print("error code_path is None")
        exit()

    # .cファイル一覧を作成する
    fileList = glob.glob(code_path + "*.cpp")
    printforDebug(fileList)

    # dll作成コマンドを作成する
    command = "g++" + " -shared -O3 -static -std=c++17 -Wall -Wextra"
    for file_path in fileList:
        command = command + " " + file_path
    command = command + " -o " + "Ninja.dll"# + " & REM 2 > errors.txt strip --strip-unneeded Ninja.dll pause"
    printforDebug(command)

    # dll作成コマンドを実効する
    subprocess.Popen(command, shell=True)


if __name__ == '__main__':
    main()
