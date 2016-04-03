/*! \file
 *  \brief メインウィンドウ+処理内容本体
 *  \author Daisuke
 *  \date 2014/10/05_01 daisuke 新規作成
 *        2014/10/05_02 daisuke 一応完成 正常動作確認
 *        2016/03/06    daisuke コメント編集
 *        2016/03/13    daisuke コメント編集
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Common.h"
#include <QMainWindow>
#include <QImage>
#include <QGraphicsScene>
#include <QPixmap>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QGraphicsItem>
#include <QList>
#include <QRadialGradient>
#include "FileUtility.h"
#include "ComicMetaEditorSetting.h"
#include "CommonFunction.h"
#include "ComicMetadata.h"
#include <QListWidget>
#include <QTextDocument>

#define SCROLL_KEY Qt::Key_Space

//!if IMAGE SIZE CONVERSION == true
//!loaded image size is converted to fit it's longer side
//!to TARGET_IMAGE_LENGTH
#define IMAGE_SIZE_CONVERSION true;
#define TARGET_IMAGE_LENGTH 1500;


namespace Ui {
class MainWindow;
}
class MainWindowPrivateData;
class MainWindowGraphicsViewData;

//!メインウィンドウの各機能用インデックス
enum MainWindowTabType{
    MainWindowTab_Viewer,
    MainWindowTab_Info,
    MainWindowTab_Frame,
    MainWindowTab_Character,
    MainWindowTab_Dialog,
    MainWindowTab_Onomatopoeia,
    MainWindowTab_Item,
    MainWindowTab_UserDefined
};

/*!
 * \brief 画面関連処理＋各種動作すべて\n
 * <strong>基本的な処理はほぼすべてこのMainWindowクラスで持っています。本項目の詳細を参照してください。</strong>\n
 * \attention 基本的な処理はほぼすべてこのMainWindowクラスで持っています。本項目の詳細を参照してください。
 * \note 画面の構成はMainWindow.uiにて定義している\n
 * 基本的な構造としては、UIでの操作によりprivate slotsの対応するものが呼ばれ、\n
 * そこから各種関数を実行しています。\n
 * 動作確認もしくは変更したい処理がある場合には、その操作に相当するslotから、呼び出されている関数を確認してください。\n
 * また、グラフィックスビュー上でなされたキーボード操作の取り扱い関連は、Sl_GVKy_press, Sl_GVKy_releaseに定義されています。\n
 * ショートカットキーに関連する動作を変更する場合にはこちらをご参照ください。\n
　* \n
 * メタデータ増加時には画像を切り替える際の下記3関数についても忘れずに修正すること\n
 * releaseSpecificItems();//!<選択状態である可能性のある全てのアイテムをリリースする(-1にセットする)\n
 * void metaDataUIClear();//!<UI上の各メタデータ要素のクリア（リスト以外)\n
 * void metaDataListClear();//!<UI上のメタデータリストのクリア
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // 画像を開く
    bool openImageFile(QString fileName, bool loadMetadataStatus = true);

    // 画面下部のステータスバーにメッセージを表示するための関数
    void setStatusBarMessage(QString message, int time = 5000);

    // brief ウィンドウにマウスポインタの座標を表示するための関数
    void displayMousePosition(QPoint pt);

    // シーン本体と、保持している各種GraphicsItemを消去する
    void clearScene();

    // 画像の表示倍率をウィンドウサイズに合わせて調整する
    void fitScale();

    //画像上のマウス座標を計算して返却する
    QPoint calcMousePointOnImage();

    //メタデータ出力用関数
    bool writeMetaData();

private slots:
    //!Openボタンが押された際の動作
    void on_actionOpen_triggered();
    //!Nextボタンが押された際の動作
    void on_actionNext_triggered();
    //!Previousボタンが押された際の動作
    void on_actionPrevious_triggered();
    //!ZoomInボタンが押された際の動作
    void on_actionZoomIn_triggered();
    //!ZoomOutボタンが押された際の動作
    void on_actionZoomOut_triggered();
    //!ResetZoomボタンが押された際の動作
    void on_actionResetZoom_triggered();

    //!画像表示エリアでマウスが動いた際の動作
    void Sl_GVMo_move(QMouseEvent* event);
    //!画像表示エリアでマウスがクリックされた際の動作
    void Sl_GVMo_press(QMouseEvent* event);
    //!画像表示エリアでマウスクリックが解除された際の動作
    void Sl_GVMo_release(QMouseEvent* event);
    //!画像表示エリアでマウスホイールが操作された際の動作
    void Sl_GVMo_wheel(QWheelEvent* event);
    //!画像表示エリアでキーボードが押下された際の動作
    void Sl_GVKy_press(QKeyEvent* event);
    //!画像表示エリアでキーボードが押下が解除された際の動作
    void Sl_GVKy_release(QKeyEvent* event);
    //!画像のズーム率が変更された際の動作
    void Sl_GV_zoomed(double);


    void on_functionTab_currentChanged(int index);
    void on_actionSaveMetaData_triggered();
    void on_AddItem_Rect_clicked();
    void on_actionClearMetaData_triggered();

    //Info
    void on_Info_ComicTitle_LineEdit_textChanged(const QString &arg1);
    void on_Info_EpisodeNumber_LineEdit_textChanged(const QString &arg1);
    void on_Info_PageNumber_textChanged(const QString &arg1);
    void on_Info_CharacterListWidget_currentRowChanged(int currentRow);
    void on_Info_SelectedCharacterDelete_clicked();
    void on_Info_SelectedCharacterRename_clicked();

    //Frame
    void on_AddFrame_Rect_clicked();
    void on_SetOrder_Frame_clicked();
    void on_ShowOrder_Frame_clicked();
    void on_Select_Frame_clicked();
    void on_Delete_Frame_clicked();
    void on_listWidget_Frame_currentRowChanged(int currentRow);
    void on_Frame_SceneChange_CheckBox_toggled(bool checked);

    //Character
    void on_AddCharacter_Rect_clicked();
    void on_AddNewCharacter_PushButton_clicked();
    void on_Character_ComboBox_currentIndexChanged(int index);
    void on_SetOrder_Character_clicked();
    void on_ShowOrder_Character_clicked();
    void on_ListWidget_Character_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_Select_Character_clicked();
    void on_Character_FrameComboBox_currentIndexChanged(int index);
    void on_Character_DeleteButton_clicked();

    //Dialog
    void on_AddDialog_Rect_clicked();
    void on_SetOrder_Dialog_clicked();
    void on_ShowOrder_Dialog_clicked();
    void on_ListWidget_Dialog_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_TextEdit_Dialog_textChanged();
    void on_Select_Dialog_clicked();
    void on_DialogType_Dialog_clicked();
    void on_DialogType_Narration_clicked();
    void on_Dialog_FrameComboBox_currentIndexChanged(int index);
    void on_Dialog_SpeakerComboBox_currentIndexChanged(int index);
    void on_Dialog_DeleteButton_clicked();

    //Onomatopoeia
    void on_AddOnomatopoeia_Rect_clicked();
    void on_SetOrder_Onomatopoeia_clicked();
    void on_ShowOrder_Onomatopoeia_clicked();
    void on_TextEdit_Onomatopoeia_textChanged();
    void on_Select_Onomatopoeia_clicked();
    void on_ListWidget_Onomatopoeia_currentRowChanged(int currentRow);
    void on_Onomatopoeia_FrameComboBox_currentIndexChanged(int index);
    void on_Onomatopoeia_DeleteButton_clicked();

    //Item
    void on_ListWidget_CItem_currentRowChanged(int currentRow);
    void on_Item_Class_LineEdit_textChanged(const QString &arg1);
    void on_TextEdit_CItem_textChanged();
    void on_Item_FrameComboBox_currentIndexChanged(int index);
    void on_Item_DelereButton_clicked();

private:
    void connectGraphicsView();
    Ui::MainWindow *ui;
    QScopedPointer<MainWindowPrivateData> const _pdata; //!< 画像等のデータ
    QScopedPointer<MainWindowGraphicsViewData> const _gv;//!< UIの表示領域関連のデータ
    QSharedPointer<QImage> _image; //!< _imageと_sceneはmain本体で持っておく事とする
    QSharedPointer<QGraphicsScene> _scene; //!< 表示用のGraphicsScene
    IL::FileUtility _fileUtility; //!<　画像ファイル名等のハンドリング用ユーティリティー
    ComicMetaEditorSetting _setting; //!<　本アプリケーションの設定格納場所
    ComicMetadata _metadata; //!< メタデータ格納場所
    bool _commonMetadataEdit; //!< 共通メタデータが編集された場合のフラグ
    bool _pageMetadataEdit; //!< ページメタデータが編集された場合のフラグ

    //create polygon and rect
    bool _crossCursor; //!<現在十字型のカーソルになっている場合のフラグ
    bool _createPolygonMode; //!<ポリゴン生成モードになっている場合のフラグ
    GraphicsItemColor _editColor; //!編集中の表示色
    int _cornerNumber; //!< ポリゴン編集時に使用 ポリゴンの角数
    int _cornerCount; //!< ポリゴン編集時に使用
    int _polygonCircleSize; //!< ポリゴン編集時に使用
    int _selectItemMinimumDistance; //!< ポリゴン編集時に使用
    QGraphicsEllipseItem *_startCircle; //!< ポリゴン編集時に使用
    QGraphicsLineItem *_newline;//!< ポリゴン編集時に使用
    QPointF _startPoint;//!< ポリゴン編集時に使用
    QVector<QGraphicsLineItem*> _editPolygonLine;//!< ポリゴン編集時に使用

    ComicMetadataType _targetType; //!< 編集対象となっているメタデータタイプの格納場所

    //create polygon
    void startCreateMode_Polygon(ComicMetadataType type);
    void addPointForEditingPolygonItem(QPoint pt);
    void terminateCreatePolygon();
    void cancelCurrentLine(QPoint pt);
    void cancelCreatePolygon();
    void deleteLineItems(QVector<QGraphicsLineItem*> &lines);
    void createPolygonModeMouseMove();

    //create rect
    void startCreateMode_Rect(ComicMetadataType type); //!<矩形オブジェクト関連
    void cancelCreateRect();//!<矩形オブジェクト関連
    void createRectMousePress(QPoint pt);//!<矩形オブジェクト関連
    void createRectMouseMove(QPoint pt);//!<矩形オブジェクト関連
    void createRectMouseRelease();//!<矩形オブジェクト関連
    void terminateCreateRect();//!<矩形オブジェクト関連
    bool _createRectMode;//!<矩形オブジェクト生成モード時にTrue
    QGraphicsRectItem *_editRectItem;
    bool _rectCreating;//!矩形生成モード実行中の時にTrue
    bool _setRectItem;
    void cancelAllMode();
    QPoint _rectPT1;

    //edit mode
    bool _isEditMode; //!< 編集モードである場合のフラグ
    bool _nowEditing; //!< 編集モードでかつ編集作業が行われている場合のフラグ
    //ComicMetadataType _editTargetType;
    //int _editTargetNumber;
    QVector<QGraphicsEllipseItem*> _editModeCornerList; //!< 角編集時の表示用円データ
    GraphicsItemData _editModeItem;//!< 編集モード関連
    int _editCornerNumber;//!< 編集モード関連
    int _editCircleSize;//!< 編集モード関連
    void startEditMode(ComicMetadataType type, int number);// 編集モード関連
    void startEditMode(GraphicsItemData target);// 編集モード関連
    void editModeMouseClick(QPoint pt);// 編集モード関連
    void editModeMouseMove(QPoint pt);// 編集モード関連
    void editModeMouseRelease(QPoint pt);// 編集モード関連
    void cancelEditMode();// 編集モード関連

    //select mode
    bool _isSelectMode; //!< 選択モードである場合のフラグ
    ComicMetadataType _selectTargetType; //!< 選択モードの処理対象ターゲットメタデータタイプ
    void startSelectMode(ComicMetadataType type);
    QVector<GraphicsItemData> _selectTarget;//!< 選択モード関連
    QVector<double> _selectItemPolygonSizeList;//!< 選択モード関連
    int _selectedItemNumber;//!< 選択モード関連
    bool _selectLock;//!< 選択モード関連
    int selectItem(QPoint pt);//!< 選択モード関連
    ///void cancelSelectMode();
    void hideAll();
    void showAll();
    void selectModeMouseClick(QPoint pt);// 選択モード関連
    void selectModeMouseMove(QPoint pt);// 選択モード関連
    void selectModeMouseDoubleClick(QPoint pt);// 選択モード関連
    void selectModeRightClick();// 選択モード関連
    void selectModeDelete();// 選択モード関連
    void selectModeSelect(int selectNumber);// 選択モード関連
    void selectModeLock();// 選択モード関連 //現在選択されているItemの番号でロックする
    void selectModeLockRelease();// 選択モード関連
    void selectModeRelease();// 選択モード関連
    void selectModeCancel();// 選択モード関連

    // create lator: set Order Mode //!<順番設定モード関連
    void setOrderModeStart(ComicMetadataType type);//!<順番設定モード関連
    bool _isSetOrderMode;///!<順番設定モード関連
    ComicMetadataType _setOrderTargetType;//!<順番設定モード関連
    QVector<GraphicsItemData> _setOrderTarget;//!<順番設定モード関連
    QVector<double> _isSetOrderModePolygonSizeList;//!<順番設定モード関連
    QVector<int> _isSetOrderModeSelectedList;//!<順番設定モード関連
    QVector<QGraphicsRectItem*> _orderNumberRect;//!<順番設定モード関連
    QVector<QGraphicsTextItem*> _orderNumberText;//!<順番設定モード関連
    void setOrderModeCancel();// 順番設定モード関連
    void setOrderModeMouseClick(QPoint pt);// 順番設定モード関連
    void setOrderModeMouseRightClick();// 順番設定モード関連
    void setOrderModeTerminate();// 順番設定モード関連
    void setOrderModeMouseMove(QPoint pt);// 順番設定モード関連
    int selectItem(QVector<GraphicsItemData>& target,
                   QVector<double> &sizeList, QPoint pt, QVector<int> &ignore);// 順番設定モード関連

    bool _isShowOrderMode;//!<順番確認モード関連
    void showOrder(ComicMetadataType type);//!<順番確認モード関連
    void showOrder(QVector<GraphicsItemData> GIData);//!<順番確認モード関連
    QVector<QGraphicsRectItem*> _showNumberRect;//!<順番確認モード関連
    QVector<QGraphicsTextItem*> _showNumberText;//!<順番確認モード関連
    void showOrderCancel();// 順番確認モード関連

    MainWindowTabType _currentTabType;//!< 現在のメインウィンドウの右側タブモードの状態を保持する変数

    int _currentFrameNumber; //!< 現在選択されているFrameのインデックス
    void addFrame(QPolygonF polygon, QString mangaPath = "",
                  bool sceneBoundery = false);
    void specifyFrame(int number = -1);
    void clearFrameUI();

    int _currentCharacterNumber;//!< 現在選択されているCharacterのインデックス
    void addCharacter
        (QPolygonF polygon, QString mangaPath = "", QString characterName = "--",
         int characterID = 0, int targetFrame = 0);
    void specifyCharacter(int number = -1);
    void clearCharacterUI();

    int _currentDialogNumber;//!< 現在選択されているDialogのインデックス
    void addDialog
        (QPolygonF polygon, QString mangaPath = "", QString text = "", int fontsize = 3,
         bool narration = false, int targetCharacterID = 0,
         int targetFrame = 0, QString characterName = "--");
    void specifyDialog(int number = -1);
    void clearDialogUI();

    int _currentOnomatopoeiaNumber;//!< 現在選択されているOnomatopoeiaのインデックス
    OnomatopoeiaData _currentOnomatopoeia;//!< 現在選択されているオノマトペデータ
    void addOnomatopoeia
    (QPolygonF polygon, QString mangaPath = "", QString text = "",
     int fontsize = 3, bool targetFrame = 0);
    void specifyOnomatopoeia(int number = -1);
    void clearOnomatopoeiaUI();

    //画像を切り替える際には下記3つを実施する
    //メタデータ増加時要修正
    void releaseSpecificItems();//!<選択状態である可能性のある全てのアイテムをリリースする(-1にセットする)
    void metaDataUIClear();//!<UI上の各メタデータ要素のクリア（リスト以外)
    void metaDataListClear();//!<UI上のメタデータリストのクリア

    //Item
    int _currentCItemNumber;//!< 現在選択されているItemのインデックス
    ItemData _currentItem;//!< 現在選択されているItemのデータ
    void addItem
    (QPolygonF polygon, QString mangaPath = "", QString itemClass = "Item",
     QString description = "", int targetFrame = 0);
    void specifyItem(int number = -1);
    void clearItemUI();

    //Character Name
    int _selectedCharacterNameNumber;//!< 選択されている登場人物のインデックス
    void deleteCharacterList(int number);
    void resetCharacterList();
    void initCharacterList();

    void initListWidget_Frame();

    void removeFrame(int number);
    void removeCharacter(int number);
    void removeDialog(int number);
    void removeOnomatopoeia(int number);
    void removeCItem(int number);
    QPolygonF calcAbsolutePosition(QPolygonF relativePosition);
    void loadMetadata();
    QString _metadataDirectoryName;//!< メタデータを格納するディレクトリ名
    void clearMetadata();

    void editFramePolygon(int number, QPolygonF polygon);
    void editCharacterPolygon(int number, QPolygonF polygon);
    void editDialogPolygon(int number, QPolygonF polygon);
    void editOnomatopoeiaPolygon(int number, QPolygonF polygon);
    void editItemPolygon(int number, QPolygonF polygon);

    bool _isRefreshingNow; //!<Refresh動作中にtrueとなる
    void refresh_Frame_ListWidget(int currentIndex = -1);
    void refresh_Character_ListWidget(int currentIndex = -1);
    void refresh_Dialog_ListWidget(int currentIndex = -1);
    void refresh_Onomatopoeia_ListWidget(int currentIndex = -1);
    void refresh_Item_ListWidget(int currentIndex = -1);
    void refresh_ALL_ListWidget();
    QString getFirstLine(QString str);
    QString cvtSingleLine(QString str);

    bool _isSpecifyed; //!< 現在何らかのメタデータが指定状態であるかのフラグ

signals:
    void signal_setStatusBarMessage(QString, int);

};


#endif // MAINWINDOW_H
