/*! \file
 *  \brief 共通項目の機能関係設定用
 *  \author Daisuke
 *  \date 2016/03/06 daisuke コメント追加
 */


#ifndef COMMON_H
#define COMMON_H

//P_から始まるdefineは表示に関するもの
#define P_CONSTRUCT //!<MainWindowとFileUtilityのConstruct動作確認用
#define P_DESTRUCT //!<MainWindowとFileUtilityのConstruct動作確認用

//設定ファイルの場所
#define DEFAULT_SETTING_FILE "./setting.txt"
//!<実行した際の途中を読み込むため（実際は使用されていない）

//version
#define SOFTWARE_VERSION "Comic Meta Editor Alpha1.02"
#endif // COMMON_H
