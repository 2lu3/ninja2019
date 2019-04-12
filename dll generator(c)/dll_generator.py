import glob
import subprocess
import os

# is_debug = True
is_debug = False

def printforDebug(message):
    if is_debug:
        print(message)

tcc_path_expectations = [os.path.expanduser('~')+"/Microsoft Robotics Dev Studio 4/CS/CsBot/Uploader/lib/clrcc/tcc.exe", "C:/Microsoft Robotics Developer Studio 4/CS/CsBot/Uploader/lib/clrcc/tcc.exe", "D:/Microsoft Robotics Developer Studio 4/CS/CsBot/Uploader/lib/clrcc/tcc.exe"]

code_path_expectations = ["./../code(c)/", "./../code/", "./code(c)/", "./code/"]

def main():
    tcc_path = None
    investigating_tcc_path = None
    # print(os.path.isfile(""))
    for investigating_tcc_path in tcc_path_expectations:
        if os.path.isfile(investigating_tcc_path):
            printforDebug("find tcc.exe in " + investigating_tcc_path)
            tcc_path = investigating_tcc_path

    while tcc_path is None:
        print("I could not find Microsoft Robotics Dev Studio 4/CS/CsBot/Uploader/lib/clrcc/tcc.exe. Please write the correct path here")
        path_inputed = input()
        printforDebug("user_input : " + path_inputed)
        if os.path.isfile(path_inputed):
            tcc_path = path_inputed
            printforDebug("exist")
        else:
            printforDebug("not exist")

    if tcc_path is None:
        print("error tcc_path is None")
        exit()

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
    fileList = glob.glob(code_path + "*.c")
    printforDebug(fileList)

    # dll作成コマンドを作成する
    # -static : ライブラリをすべて含める
    # -Wall : 基本的な警告オプションをすべて有効にする
    # -Wextra 追加の警告オプションをonにする
    command = "\"" + "gcc" + "\"" + " -shared -static -Wall -Wextra"
    for file_path in fileList:
        command = command + " " + file_path
    command = command + " -o " + "Ninja.dll\""
    printforDebug(command)

    # dll作成コマンドを実効する
    subprocess.Popen(command, shell=True)


if __name__ == '__main__':
    main()
