import tkinter as tk
from PIL import Image, ImageTk

image_width = 720
image_height = 540
margin = 20
button_margin = 120



button_labels = ["Red", "Cyan", "Black", "Yellow", "Wall", "Swampland", "Deposit", "SuperArea"]
button_values = [10, 15, 20, -2, -3, -1, 0, 1]
buttons_is_checked = None

def click(event):
    for i in range(len(button_labels)):
        print(buttons_is_checked[i].get())

    print("")

def onClickButton(event):
    pass
    # print(buttons_is_checked)

def setButtons(root):
    # global button_labels, button_values, buttons_is_checked
    button_width = 10
    for i in range(len(button_labels)):
        button = tk.Checkbutton(root, text=button_labels[i], variable=buttons_is_checked[i], width = button_width, anchor="w")
        button.bind("<Button-1>", onClickButton)
        button.place(x = image_width + margin * 2, y = margin * (i + 1))



def main():
    global button_labels, button_values, buttons_is_checked

    root = tk.Tk()

    # 名前
    root.title("Image2Array")

    buttons_is_checked = [tk.BooleanVar() for i in range(len(button_labels))]


    canvas = tk.Canvas(root, width=image_width + margin * 2 + button_margin, height=image_height + margin * 2, bg="white")
    canvas.pack()

    #
    setButtons(root)


    # 画像を配置
    img = Image.open("Background.bmp")
    tkimg = ImageTk.PhotoImage(img)
    canvas.create_image(margin, margin, image=tkimg, anchor=tk.NW)

    line_margin = 5
    # 外側の四角を作成
    canvas.create_rectangle(margin - line_margin / 2, margin - line_margin / 2, image_width + margin + line_margin / 2, image_height + margin + line_margin / 2, outline="black", width = line_margin)

    canvas.bind("<Button-1>", click)


    root.mainloop()

if __name__ == "__main__":
    main()
