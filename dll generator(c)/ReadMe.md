# dll generatorの使い方

dll generatorは、Cospace用のdllファイルを自動で作成するツールである。

原理は、`code`というフォルダーに入っている拡張子が`.c`のファイルを検索し、それらすべてをまとめてコンパイルする。
そのため、Cospaceに関係ない`.c`のファイルはcodeのフォルダーの中に入れてはならない。

## Pythonをインストールしている場合

dll_generator.pyをコピーして、以下のようなフォルダー構造にしなければならない。

> 適当なフォルダー/<br>
> 　├ code/<br>
> 　│　└ main.c<br>
> 　│　└ tools.c<br>
> 　│　└ tools.h<br>
> 　└ dll_generator.py

`python dll_generator.py`とコマンドプロンプトで実行する。

## Pythonをインストールしていない場合

dll_generator.exeをコピーして、以下のようなフォルダー構造にしなければならない。

> 適当なフォルダー/<br>
> 　├ code/<br>
> 　│　└ main.c<br>
> 　│　└ tools.c<br>
> 　│　└ tools.h<br>
> 　└ dll_generator.exe

その後、dll_generator.exeを実行すればよい。

## 最終更新日時

* ReadMe.md : 2019/02/17
* ReadMe.pdf : 2019/02/17
* dll_generator.py : 2019/02/17
* dll_generator.exe : 2019/02/17
