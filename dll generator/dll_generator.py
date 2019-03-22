import glob
import subprocess

# tcc_path = "\"%USERPROFILE%/Microsoft Robotics Dev Studio 4/CS/CsBot/Uploader/lib/clrcc/tcc.exe\""
tcc_path = "\"C:/Microsoft Robotics Developer Studio 4/CS/CsBot/Uploader/lib/clrcc/tcc.exe\""

def main():
    # .cファイル一覧を作成する
    fileList = glob.glob(".\\..\\code\\*.c")
    # print(fileList)

    # dll作成コマンドを作成する
    command = tcc_path + " -shared"
    for file_path in fileList:
        command = command + " " + file_path
    command = command + " -o " + "\"C:\\Microsoft Robotics Developer Studio 4\\CS\\User\\Rescue\\CsBot\\" + "Ninja.dll\""
    # print(command)

    # dll作成コマンドを実効する
    subprocess.Popen(command, shell=True)


if __name__ == '__main__':
    main()
