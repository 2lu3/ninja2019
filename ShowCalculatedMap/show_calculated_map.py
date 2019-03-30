import cv2 as cv
import os
from time import sleep
import numpy as np

# np.set_printoptions(threshold=np.inf)

# 0: 何もなし(白) 1:壁(茶) 2:罠(黄) 3:デポシット
color_list = [[255, 255, 255], [124, 96, 53], [255, 255, 0], [255, 183, 76]]
# r:b g:g b:r
for i, color in enumerate(color_list):
  color_list[i] = [color[2], color[1], color[0]]

# image = cv.imread("./image.JPG")
# print(type(image))
# print(image)

# exit()
sleep(10)

while(True):
  image = np.empty((92 * 5,122 * 5,3), dtype='int8')
  target_file = open("C:/Users/hikaru/Microsoft Robotics Dev Studio 4/calculated_map.txt", "r")


  for i, line in enumerate(target_file.readlines()):
    line = line.split()
    for j, char in enumerate(line):
      for k in range(3):
        image[i:i*5,j:j*5,k] = color_list[int(char)][k]

  # print(type(image))
  # print(image)
  # exit()

  # 配列を可視化した画像
  img = np.array(image).astype(np.uint8)

  # image = np.array(image)
  cv.imshow("image", img)
  print(img.shape)
  cv.waitKey(1)

exit()
