/*! \file
 *  \brief ソフトウェアの設定等を保存、起動時自動読み込みさせるための機能(ファイル拡張子フィルタ以外は未実装)
 *  \author Daisuke
 *  \date 2016/03/ daisuke コメント追加
 */

#ifndef COMICMETAEDITORSETTING_H
#define COMICMETAEDITORSETTING_H

#include "Common.h"
#include <QString>
#include <QFile>

class ComicMetaEditorSetting
{
public:
    ComicMetaEditorSetting();//!<コンストラクタ
    bool loadSetting(QString fileName);//!<未実装
    QString getFileDirectory() const;//!<未実装
    void setFileDirectory(QString directoryPath);//!<_fileDirectoryを返す
    bool saveSetting(QString fileName);//!<未実装
    QString getFilterForImage() const;//!<_filterForImageを返す
private:
    QString _fileDirectory;//!<デフォルトディレクトリまでの相対パスMac用とWindows用に対応
    QString _filterForImage;//!<画像読み込み時の設定(読み込み対象となる画像ファイルの設定)
};

#endif // COMICMETAEDITORSETTING_H

