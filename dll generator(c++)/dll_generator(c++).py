import glob
import subprocess
import os
import sys

# is_debug = True
is_debug = False

is_lite_warming = True

def printforDebug(message):
    if is_debug:
        print(message)

code_path = None
code_path_expectations = ["./../code(c++)/", "./../code/", "./code(c++)/", "./code/"]

out_path = 'outcospace'
out_cospace_path_expectations = [os.path.expanduser('~') + '/Microsoft Robotics Dev Studio 4/CS/User/Rescue/CsBot/', 'C:/Microsoft Robotics Dev Studio 4/CS/User/Rescue/CsBot/', 'D:/Microsoft Robotics Dev Studio 4/CS/User/Rescue/CsBot/', 'C:/Microsoft Robotics Developer Studio 4/CS/User/Rescue/CsBot/']

optimisation_level = 0


def main():
    global code_path
    global out_path
    # Ninja.dllをどこに出力するかを決定する
    if out_path == None or out_path == 'outhere':
        out_path = './'
    elif out_path == 'outcospace':
        for investigating_out_path in out_cospace_path_expectations:
            if os.path.isdir(investigating_out_path):
                out_path = investigating_out_path
                printforDebug("find output folder : " + investigating_out_path)

    if out_path == "outcospace":
        out_path = "./"

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
    # -static : ライブラリをすべて含める
    # -Wall : 基本的な警告オプションをすべて有効にする
    # -Wextra 追加の警告オプションをonにする
    command = "\"" + "g++" + "\"" + " -shared -static "
    if optimisation_level != 0:
        command += '-O' + str(optimisation_level) + ' '
    if is_lite_warming:
        command = command + '-Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wendif-labels -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wmissing-include-dirs -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum -Wunsafe-loop-optimizations -Wvariadic-macros -Wwrite-strings'

    for file_path in fileList:
        command = command + " " + file_path
    command = command + " -o " +  '\"' + out_path + "Ninja.dll\""

    # command = command + " & REM 2> errors.txt strip --strip-unneeded \"" + out_path + "Ninja.dll\" pause"
    command = command + " &  strip --strip-unneeded \"" + out_path + "Ninja.dll\""
    printforDebug("command : " + command)

    # dll作成コマンドを実行する
    subprocess.run(command, shell=True)
    print("finished")
    sys.stdout.flush()
    if sys.argv[0].find('.exe') != -1:
        subprocess.run("pause", shell=True)



command_options = ['--help', '--debug', '--outhere', '--outcospace', '-O1', '-O2', '-O3']


if __name__ == '__main__':
    args = sys.argv
    for arg in args:
        if args.index(arg) == 0:
            continue
        elif arg == '--help':
            print(command_options)
            exit()
        elif arg == '--debug':
            print('mode : debug')
            is_debug = True
        elif arg == '--outhere':
            print('mode : outhere')
            out_path = 'outhere'
        elif arg == '--outcospace':
            print('mode : outcospace')
            out_path = 'outcospace'
        elif arg == '--no-lite-warming':
            print('mode : no lite warming')
            is_lite_warming = False
        elif arg == '-O1':
            print('mode : optimisation-level-1')
            optimisation_level = 1
        elif arg == '-O2':
            print('mode : optimisation-level-2')
            optimisation_level = 2
        elif arg == '-O3':
            print('mode : optimisation-level-3')
            optimisation_level = 3
        else:
            print(arg + ' is not correct option')

    main()
