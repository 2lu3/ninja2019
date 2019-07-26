# Japanese

1, MapDataConverter.exeと同じフォルダに、"map_color_information.txt"を作成して、ペイントを使って、色の情報を定義してください。
形式は、
0 Red Green Blue
1 Red Green Blue
2 Red Geeen Blue
...
n 1000 1000 1000

nには0を含まない自然数を入れていください。
また、最後の行を入力した後も改行してください。

2, ペイントを使って、画像をbmp形式に変換してください。

3, MapDataConverter.exeを実行してください

4, MapDataConverter.exeと同じフォルダに、image_data_in_int_format.txtが生成されます。

生成する配列の大きさは、画像のピクセル数 / SIZE(main.cで定義されている)です。
質問があれば、issuesで質問してください。
また、バグを見つけたら同じくissuesで報告してもらえると嬉しいです。
新しい機能を付け加えたかたは、ここにアップロードできるようにプログラムを送っていただけると嬉しいです。

# English
1, Please define color information at "map_color_information.txt" by using paint in a directory which have MapDataConverter.
The format of this is

0 Red Green Blue
1 Red Green Blue
2 Red Geeen Blue
...
n 1000 1000 1000

Please enter a integer number above 1.
Also, Please press enter when you finish writting to this file.

2, Convert image format to bmp by using paint

3, Run MapDataConverter.exe

4, You can see image_data_in_int_format.txt in same directory.

The size of generated array is pixel number / SIZE(defined in main.c).

If you have any questions, Please use issues.
And I am very happy if you contribute to this project (If you find any bug, please commit it in issue which is a function of github. And if you add new function, please notify me with program. I will upload it here)
