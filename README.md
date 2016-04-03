# ComicMetaEditor2014
本プログラムはOpenSource版のQtを用いて作成されています。
Qtについては以下のサイトを参照ください。
http://www.qt.io/

本リポジトリではIcon画像を用意しておりません。
利用される方はComicMetaEditor/iconディレクトリ下にあるアイコン画像(png形式)を置きかえてください。
Config.png
Document.png
Glass.png
LeftArrow.png
Mouse.png
OpenFolder.png
RightArrow.png
ResetZoom.png
Save.png
Scroll.png
ZoomIn.png
ZoomOut.png

コードのコメントはDoxygenで読めるように記述してあります。
基本的なUI上の命令はほぼ全てMainWindowにて定義しています。
DoxygenのMainWindowクラスあるいMainWindow.h/.cppの項目をご参照ください。

画面の構成はMainWindow.uiにて定義しています
基本的な構造としては、UIでの操作によりprivate slotsの対応するものが呼ばれ、そこから各種関数を実行しています。
動作確認もしくは変更したい処理がある場合には、その操作に相当するslotから、呼び出されている関数を確認してください。
また、グラフィックスビュー上でなされたキーボード操作の取り扱い関連は、Sl_GVKy_press, Sl_GVKy_releaseに定義されています。
ショートカットキーに関連する動作を変更する場合にはそちらをご参照ください。
