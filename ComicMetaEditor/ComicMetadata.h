/*! \file
 *  \brief マンガメタデータ用クラス群
 *  \author Daisuke
 *  \date 2016/03/ daisuke コメント追加
 */

#ifndef COMICMETADATA_H
#define COMICMETADATA_H
#include "GraphicsItemData.h"

#include <QVector>
#include <QString>
#include <QTextDocument>
#include <QtXml>

/*!
 * \brief メタデータの種類定義用enum
 */
enum ComicMetadataType{
    ComicMetadata_Frame,
    ComicMetadata_Character,
    ComicMetadata_Dialog,
    ComicMetadata_Onomatopoeia,
    ComicMetadata_Item,
    ComicMetadata_All
};

/*!
 * \brief コマ用メタデータクラス
 */
class FrameData
{
public:
    FrameData();
    bool sceneBoundary;//!<シーンの切り替えフラグ
    GraphicsItemData GIData;//!<画面上に表示する枠の情報(Qt)
    QString mangaPath;//!<マンガパス式保持用
};

/*!
  * \brief 登場人物用メタデータクラス
  */
 class CharacterData
{
public:
    CharacterData();
    QString characterName;
    int characterID;//!<登場人物ID
    int targetFrame;//!<対象とするコマのインデックス、コマを並べ替えた際には必ず変更する事、ターゲットとするコマ無しの場合には-1
    GraphicsItemData GIData;//!<画面上に表示する枠の情報(Qt)
    QString mangaPath;//!<マンガパス式保持用
};

/*!
 * \brief セリフ用メタデータクラス
 */
class DialogData
{
public:
    DialogData();
    QString text;
    int fontSize;//!<フォントサイズを5段階で定義 1:very small, 2:small 3:normal 4:large 5:very large
    bool narration;//!<ナレーションの場合にはtrueとする
    int targetCharacterID;//!<ナレーション以外の場合には対応する登場人物IDを保持する
    QString characterName;//!<要修正　targetCharacterIDから自動取得できる様にする必要が有る
    //!<話者の選択（キャラクターメタデータ(どのシーンの誰なのか？)話者を並べ替えた際には必ず更新する事
    int targetFrame;//!<対象とするコマのインデックス、コマを並べ替えた際には必ず変更する事、ターゲットとするコマ無しの場合には-1
    GraphicsItemData GIData;//!<画面上に表示する枠の情報(Qt)
    QString mangaPath;//!<マンガパス式保持用
};

/*!
 * \brief オノマトペ用メタデータクラス
 */
class OnomatopoeiaData
{
public:
    OnomatopoeiaData();
    void setText(QString str);
    QString text;//!<読み情報
    int fontSize;//!<1:very small, 2:small 3:normal 4:large 5:very large
    int targetFrame;//!<対象とするコマのインデックス、コマを並べ替えた際には必ず変更する事、ターゲットとするコマ無しの場合には-1
    GraphicsItemData GIData;//!<画面上に表示する枠の情報(Qt)
    QString mangaPath;//!<マンガパス式保持用
};

/*!
 * \brief The ItemData class
 * 物等記述用メタデータクラス
 */
class ItemData
{
public:
    ItemData();
    QString itemClass;
    QString description;//!<記述された内容
    int targetFrame;//!<対象とするコマのインデックス、コマを並べ替えた際には必ず変更する事、ターゲットとするコマ無しの場合には-1
    GraphicsItemData GIData;//!<画面上に表示する枠の情報(Qt)
    QString mangaPath;//!<マンガパス式保持用
};

/*!
 * \brief 1ページに対応するメタデータ保持用クラス
 * 上記各メタデータクラスの保持とその取扱いを行う
 */
class ComicMetadata{
public:
    int indent;
    ComicMetadata(); //!<コンストラクタ
    QString workTitle; //!<作品名
    int episodeNumber; //!<話数
    int pageNumber; //!<ページ番号
    QString imageFileName; //!<画像ファイル名
    int imageWidth; //!<画像の幅
    int imageHeight; //!<画像の高さ
    QVector<QString> characterName; //!<キャラクター名リスト
    QSharedPointer<QVector<FrameData> > frame; //!<コマリスト
    QSharedPointer<QVector<CharacterData> > character; //!<キャラクターリスト
    QSharedPointer<QVector<DialogData> > dialog; //!<セリフリスト
    QSharedPointer<QVector<OnomatopoeiaData> > onomatopoeia; //!<オノマトペリスト
    QSharedPointer<QVector<ItemData> > item; //!<その他のアイテムリスト

    /*!
     * \brief 指定したタイプの枠情報取得用関数
     * \param type メタデータの種類
     * \return
     */
    QVector<GraphicsItemData> getGraphicsItemList(ComicMetadataType type);

    /*!
     * \brief 複数ページ共通のメタデータ（書籍情報、登場人物リスト）を読み込む関数
     * \param fileName メタデータファイル名
     * \return
     */
    bool loadMetadata_Common(QString fileName);

    /*!
     * \brief 複数ページ共通のメタデータを出力する関数
     * \param fileName 出力先メタデータファイル名
     * \return
     */
    bool writeMetadata_Common(QString fileName);

    /*!
     * @brief ページメタデータ読み込み用関数
     * @param fileName ファイル名
     * @param targetPageNumber 直接読み込むページを指定する場合使う -1ならば指定なし
     * @return 読み込みの成否
     * 読み込み時は一度loadエリアにストアしておき後にMainWindowからloadエリアの各データを実際のメタデータとして生成する
     * ->画像サイズがセットされていない為
     */
    bool loadMetadata_Page(QString fileName, int targetPageNumber = -1);

    /*!
     * \brief ページメタデータ出力用関数
     * \param fileName 出力先メタデータファイル名
     * \return
     */
    bool writeMetadata_Page(QString fileName);

    /*!
     * \brief タイプと番号で指定されたメタデータをリストから削除する
     * \param target メタデータの種類
     * \param number メタデータの番号
     */
    void deleteItem(ComicMetadataType target, int number);

    /*!
     * \brief 現在保持しているCommonメタデータと、ページメタデータをすべて消去する
     */
    void clear();

    /*!
     * \brief 現在保持しているCommonメタデータを消去する
     */
    void clearCommon();

    /*!
     * \brief 現在保持しているページメタデータを消去する
     */
    void clearPageMetadata();

    /*!
     * \brief キャラクター情報を全消去する
     */
    void clearCharacterName();


    /*!
     * \brief マンガパス式を再構築する
     */
    void renewAllMangaPath();
    void renewMangaPath_Frame(int number);//!<マンガパス式を再構築
    void renewMangaPath_Character(int number);//!<マンガパス式を再構築
    void renewMangaPath_Dialog(int number);//!<マンガパス式を再構築
    void renewMangaPath_Onomatopoeia(int number);//!<マンガパス式を再構築
    void renewMangaPath_Item(int number);//!<マンガパス式を再構築

    //マンガパス式を得る関数
    QString MangaPath_title_episode();//!<マンガパス式を取得するための関数
    QString MangaPath_page();//!<マンガパス式を取得するための関数
    QString MangaPath_frame(int number);//!<マンガパス式を取得するための関数
    QString MangaPath_title_episode_page_frame(int frameNumber);//!<マンガパス式を取得するための関数

    //以下はXML生成時に利用する関数
    void XMLCreate_Coordinage(QDomElement &element, GraphicsItemData GIData);//!<XML生成用
    void XMLCreate_Frame(QDomElement &element);//!<XML生成用
    void XMLCreate_Character(QDomElement &element);//!<XML生成用
    void XMLCreate_Dialog(QDomElement &element);//!<XML生成用
    void XMLCreate_Onomatopoeia(QDomElement &element);//!<XML生成用
    void XMLCreate_Item(QDomElement &element);//!<XML生成用

    //以下はXML読み込み時に利用する関数
    void XMLPurse_CharacterList(QDomElement &element);//!<XML読み込み用
    void XMLPurse_Frame(QDomElement &element);//!<XML読み込み用
    void XMLPurse_Character(QDomElement &element);//!<XML読み込み用
    void XMLPurse_Dialog(QDomElement &element);//!<XML読み込み用
    void XMLPurse_Onomatopoeia(QDomElement &element);//!<XML読み込み用
    void XMLPurse_Item(QDomElement &element);//!<XML読み込み用
    QPolygonF XMLPurse_Coordinate(QDomElement &element);//!<XML読み込み用

    //以下は読み込まれたデータの保持場所
    int loadEpisodeNumber;//!<読み込んだデータ用
    int loadPageNumber;//!<読み込んだデータ用
    QString loadImageFileName;//!<読み込んだデータ用
    QVector<FrameData> loadFrame;//!<読み込んだデータ用
    QVector<QPolygonF> loadFrameCoordinate;//!<読み込んだデータ用
    QVector<CharacterData> loadCharacter;//!<読み込んだデータ用
    QVector<QPolygonF> loadCharacterCoordinate;//!<読み込んだデータ用
    QVector<DialogData> loadDialog;//!<読み込んだデータ用
    QVector<QPolygonF> loadDialogCoordinate;//!<読み込んだデータ用
    QVector<OnomatopoeiaData> loadOnomatopoeia;//!<読み込んだデータ用
    QVector<QPolygonF> loadOnomatopoeiaCoordinate;//!<読み込んだデータ用
    QVector<ItemData> loadItem;//!<読み込んだデータ用
    QVector<QPolygonF> loadItemCoordinate;//!<読み込んだデータ用
};
#endif // COMICMETADATA_H
