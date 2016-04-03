/*! \file
 *  \brief FileUtilityクラスヘッダファイル
 *  \author Daisuke
 *  \date 2014/10/05_01 daisuke 新規作成
 *        2014/10/05_02 daisuke 一応完成 正常動作確認
 *        2016/03/06    daisuke コメント編集
 */
#ifndef FILEUTILITY_H
#define FILEUTILITY_H

#ifdef P_FUNCTION_INFORMATION
#define P_FILEUTILITY
#endif
//#define P_FILEUTILITY_DEBUG

#include "Common.h"
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringList>
#include <iostream>
#include <QWidget>
#include <QFileDialog>

namespace IL{

/*-----------------------------------------------------------------------
クラスの宣言
-----------------------------------------------------------------------*/
/*!
 * \brief 前後のファイルに移動可能となる機能付きファイル名管理クラス
 * 利用の際には本体プログラムでファイルを読み込むごとに、setFile(QString fileName)にて
 * 現在のファイル名をセットする事
 */
class FileUtility
{
public:
    FileUtility();//!< コンストラクタ
    ~FileUtility();//!< デストラクタ

    /*!
     * \brief 使用したい拡張子をセットするための関数
     * （フォルダ内のファイルのうち、ここでセットされた拡張子のみが抽出される）
     * \param fileSuffixFilter 拡張子リスト
     */
    void setSuffixFilter(QStringList fileSuffixFilter);

    /*!
     * \brief setFileにファイル名を入れることで、本クラス内で当該ディレクトリ内のファイル群を扱えるようになる
     * \param fileName ファイル名
     */
    void setFile(QString fileName);//

    /*!
     * \brief setFileで与えられたファイルが含まれるフォルダにある、取り扱う拡張子のファイル数を得る
     * \return ファイル数
     */
    int size() const;

    /*!
     * \brief ディレクトリ中の、現在のファイルの順番を得る
     * \return 現在のファイルの順番
     */
    int getCurrentNumber() const;

    /*!
     * \brief 現在のファイル名を取得する /ab/cd.txt
     * \return 現在のファイル名
     */
    QString getCurrentFileName() const;

    /*!
     * \brief 現在のファイル名を取得する(ディレクトリパスを除いたファイル名) /ab/cd.txt => cd.txt
     * \return ディレクトリパスを除いたファイル名
     */
    QString getCurrentFileNameCore() const;

    /*!
     * \brief 現在のファイル名を取得する（ディレクトリパス・拡張子なし） /ab/cd.txt => cd
     * \return ディレクトリパス・拡張子なしのファイル名
     */
    QString getCurrentFileNameCore_WOExt() const;

    /*!
     * \brief 現在のディレクトリに含まれる次のファイル名を取得する /ab/ce.txt
     * \return ファイル名(/ab/ce.txt)
     */
    QString getNextFileName() const;

    /*!
     * \brief 現在のディレクトリに含まれる一つ前のファイル名を取得する /ab/cc.txt
     * \return ファイル名(/ab/cc.txt)
     */
    QString getPreviousFileName() const;

    /*!
     * \brief 現在のディレクトリ内にある指定された番号のファイル名を取得する
     * \param number
     * \return ファイル名
     */
    QString getFileName(int number) const;

private:
    QStringList _fileSuffixFilter; //!<ファイルリストの置き場所
    unsigned int _currentFileNumber; //!<現在のファイルの順番
    QFileInfoList _fileInfoList; //!<ディレクトリ内のファイルリスト
};

}

#endif // FILEUTILITY_H
