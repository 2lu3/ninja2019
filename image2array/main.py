import tkinter as tk
import time
from PIL import Image, ImageTk
import cv2

image_width = None
image_height = None
margin = 20
button_margin = 120

image_rgb = None
canvas = None
image_pil = None
tkimg = None
show_option = 0
listBox = None

# 0 : オブジェクトのみ
# 1 : 地形のみ
mode = 0

border_area_object = 5
button_labels = ["White", "Yellow", "Wall", "Swampland",  "Deposit", "SuperArea", "Red", "Cyan", "Black"]
button_values = [1, 2, 3, 4, 5, 6, 7, 8, 9]
registered_color = [
        [255, 255, 255], # white
        [255, 255,0], # yellow
        [221, 186, 151], # wall
        [166, 166, 166], # swampland
        [255, 192, 0], # deposit
        [0, 176, 240], # superarea
        [237, 28, 36], # red
        [63, 72, 204], # cyan
        [0, 0, 0] # black
    ]
buttons_is_checked = None
map_data = None

start_x = 0
start_y = 0
moving_x = 0
moving_y = 0
end_x = 0
end_y = 0

def reloadImage():
    global tkimg, image_rgb, image_pil, map_data, registered_color, canvas, show_option

    option = listBox.curselection()
    if option == 0: # マップ情報のみ
        for hi in range(image_height):
            for wj in range(image_width):
                image_rgb[hi][wj] = registered_color[map_data[hi][wj].index(1)]
    elif option == 1: # Redのみ
        red_label_index = button_labels.index("Red")
        white_label_index = button_labels.index("White")
        for hi in range(image_height):
            for wj in range(image_width):
                if map_data[hi][wj][red_label_index] == 1:
                    image_rgb[hi][wj] = registered_color[red_label_index]
                else:
                    image_rgb[hi][wj] = registered_color[white_label_index]





    image_pil = Image.fromarray(image_rgb)
    tkimg = ImageTk.PhotoImage(image_pil)
    canvas.create_image(margin, margin, image=tkimg, anchor=tk.NW)






def onClickOutput(event):
    print("finished")

def onImageClick(event):
    global start_x, start_y
    start_x = event.x
    start_y = event.y

def onImageMotion(event):
    global start_x, start_y, canvas, moving_x, moving_y
    canvas.delete("temp")
    moving_x = event.x
    moving_y = event.y
    canvas.create_rectangle(start_x, start_y, moving_x, moving_y, width = 1, outline = "black", tag = "temp")


def onImageRelease(event):

    global start_x, start_y, end_x, end_y, image_rgb, image_pil, tkimg


    canvas.delete("temp")

    end_x = event.x
    end_y = event.y

    start_x -= margin
    start_y -= margin
    end_x -= margin
    end_y -= margin


    for hi in range(start_y, end_y):
        for wj in range(start_x, end_x):
            if hi < 0 or hi >= image_height or wj < 0 or wj >= image_width:
                continue
            else:
                for k in range(len(buttons_is_checked)):
                    if buttons_is_checked[k].get() == True:
                        map_data[hi][wj][k] = 1
                        if k <= 5:
                            image_rgb[hi][wj] = registered_color[k]
                    else:
                        map_data[hi][wj][k] = 0

    reloadImage()

def onClickModeChange(event):
    reloadImage()



def setButtons(root):
    global listBox
    # global button_labels, button_values, buttons_is_checked
    button_width = 10
    for i in range(len(button_labels)):
        button = tk.Checkbutton(root, text=button_labels[i], variable=buttons_is_checked[i], width = button_width, anchor="w")
        # button.bind("<Button-1>", onClick)
        button.place(x = image_width + margin * 2, y = margin * (i + 1))

    listBox =tk.Listbox(root, height = 4)
    for line in ["床情報", "Red", "Cyan", "Black"]:
        listBox.insert(tk.END, line)
    listBox.select_set(0)
    listBox.bind("<Button-1>", onClickModeChange)
    listBox.place(x = image_width + margin * 2, y = margin * (len(button_labels) + 2))

    button = tk.Button(root, text="出力")
    button.bind("<Button-1>", onClickOutput)
    button.place(x = image_width + margin * 2, y = margin * (len(button_labels) + 8))

def formatImage():
    global map_data, image_rgb
    for hi in range(image_height):
        for wj in range(image_width):
            min_val = 0
            min_color_num = 0
            for i in range(border_area_object):
                if (registered_color[i][0] - image_rgb[hi][wj][0]) + (registered_color[i][1] - image_rgb[hi][wj][1]) + (registered_color[i][2] - image_rgb[hi][wj][2]) <= 20:
                    min_color_num = i
                    break
            map_data[hi][wj][min_color_num] = 1


def main():
    global button_labels, button_values, buttons_is_checked, image_rgb, canvas, map_data, image_width, image_height


    # 初期化
    root = tk.Tk()
    # 名前
    root.title("Image2Array")
    buttons_is_checked = [tk.BooleanVar() for i in range(len(button_labels))]



    # 画像の読み込み
    image_bgr = cv2.imread("Background.bmp")
    image_height, image_width,_ = image_bgr.shape
    print(str(image_height) + " " + str(image_width))

    # マップの情報を収納する変数を0初期化
    map_data = [[[0 for i in range(len(button_labels))] for j in range(image_width)] for k in range(image_height)]

    image_rgb = cv2.cvtColor(image_bgr, cv2.COLOR_BGR2RGB)
    formatImage()

    image_pil = Image.fromarray(image_rgb)
    tkimg = ImageTk.PhotoImage(image_pil)





    # メイン画面の配置
    canvas = tk.Canvas(root, width=image_width + margin * 2 + button_margin, height=image_height + margin * 2, bg="white")
    canvas.pack()

    # ボタンを配置
    setButtons(root)

    # 画像を表示
    reloadImage()


    # 画像の周りに線を表示
    line_margin = 1
    canvas.create_rectangle(margin - line_margin / 2, margin - line_margin / 2, image_width + margin + line_margin / 2, image_height + margin + line_margin / 2, outline="black", width = line_margin)

    canvas.bind("<Button-1>", onImageClick)
    canvas.bind("<ButtonRelease-1>", onImageRelease)
    canvas.bind("<B1-Motion>", onImageMotion)




    root.mainloop()


if __name__ == "__main__":
    main()
