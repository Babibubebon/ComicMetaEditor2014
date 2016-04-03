/*! \file
 *  \brief メインウィンドウ+処理内容本体の処理記述部
 *  \author Daisuke
 *  \date 2016/03/13    daisuke コメント編集
 */


#include "MainWindow.h"
#include "ui_MainWindow.h"
#ifdef Q_OS_MAC
#include <math.h>
#else
#include <cmath>
#endif

using namespace std;

/*!
 * \brief 画像そのものと、SharedPointer型の画像・シーン保持用クラス
 * MainWindowのデータが多くなりすぎるため分割
 */
class MainWindowPrivateData{
public:
    //ComicMetaEditorSetting _setting;
    QImage _originalImage; //!< 読み込んだデータ
    int _targetImageLength; //!< 画像解像度の縮小が行われる場合（大きい画像用）の縮小ターゲットサイズ
    QSharedPointer<QImage> _image; //!< 実際に表示する画像の本体
    QSharedPointer<QGraphicsScene> _scene;//!< 表示用Graphics Scene
    MainWindowPrivateData();
    ~MainWindowPrivateData();
    double calcOriginalImageSizeRatio();
};

/*!
 * \brief MainWindowPrivateData::MainWindowPrivateData
 * 画像の目標最大長さを設定する。
 * IMAGE_SIZE_CONVERSIONがdefineされている場合
 * 画像読み込み時に上記サイズに変換される
 */
MainWindowPrivateData::MainWindowPrivateData()
{
    _targetImageLength = TARGET_IMAGE_LENGTH;
}

/*!
 * \brief デストラクタ
 */
MainWindowPrivateData::~MainWindowPrivateData()
{

}

/*!
 * \brief 元画像との画像サイズ比を計算する
 * \return 元画像を1.0としたときの、表示に使用するターゲット画像のサイズ比
 */
double MainWindowPrivateData::calcOriginalImageSizeRatio()
{
    double originalImageSizeRatio = 1.0;
    bool sizeConversion = IMAGE_SIZE_CONVERSION;
    if(sizeConversion){
        if(_image.data()->isNull() || _originalImage.isNull()){
            originalImageSizeRatio = 0.0;
        }
        else{
            double dx = (double)_targetImageLength / _image.data()->width();
            double dy = (double)_targetImageLength / _image.data()->height();
            originalImageSizeRatio = min(dx,dy);
        }
    }
    else{
        originalImageSizeRatio = 1.0;
    }
    return originalImageSizeRatio;
}

/*!
 * \brief グラフィックスビューと関連データを詰め合せたクラス
 */
class MainWindowGraphicsViewData
{
public:
    FunctionalGraphicsView *_graphicsView;
    QSharedPointer<QImage> _image;
    bool _keyLock;
    bool _isScrollKeyPressed;
    Qt::Key _scrollKey;
    bool _crossCursor;
    bool _scrollModeSW; //!<スクロールモードがONになっている際に使用するSW
    bool _scrollProcess; //!<スクロール処理がONになっている際に使用するSW
    QPoint _mpoint_W; //!< Mouse Point On Widget
    QPoint _map;
    QPoint _mpoint_WP; //!< Mouse Point On Widget (Previous)
    QPoint _mpoint_I; //!< Mouse Point On Image
    QRgb _rgb;
    MainWindowGraphicsViewData();
    void init();
    QPoint calcMousePointOnImage();
    void mouseMove(QMouseEvent *event);
    void mousePress(QMouseEvent *event);
    void mouseRelease(QMouseEvent *event);
    void wheelZoom(QWheelEvent *event);
    void keyPress(QKeyEvent *event);
    void keyRelease(QKeyEvent *event);
};

/*!
 * \brief 画像上のマウス座標を計算して返却する
 * \return 画像上のマウス座標
 */
QPoint MainWindowGraphicsViewData::calcMousePointOnImage()
{
    QPoint _mpoint_I;
    if(_image.data()->isNull()) return QPoint(0,0);
    _mpoint_I.setX((_mpoint_W.x() - _map.x()) / _graphicsView->getZoomRatio());
    _mpoint_I.setY((_mpoint_W.y() - _map.y()) / _graphicsView->getZoomRatio());
    if(_mpoint_I.x() < 0) _mpoint_I.setX(0);
    if(_mpoint_I.y() < 0) _mpoint_I.setY(0);
    if(_mpoint_I.x() >= _image.data()->width()) _mpoint_I.setX(_image.data()->width()-1);
    if(_mpoint_I.y() >= _image.data()->height()) _mpoint_I.setY(_image.data()->height()-1);
    return _mpoint_I;
}

/*!
 * \brief MainWindowGraphicsViewData::mouseMove
 * \brief マウスが移動した際の動作
 * \param event
 */
void MainWindowGraphicsViewData::mouseMove(QMouseEvent *event)
{
    //! 画像がセットされているならば、座標位置を取得する
    if(!_image.data()->isNull()){
        _mpoint_W = event->pos();
        _map = _graphicsView->mapFromScene(0,0);
        _mpoint_I = calcMousePointOnImage();
        //_rgb = _image.data()->pixel(_mpoint_I);
    }

    //! スクロールモードの場合に、実際にスクロールされる状態（ドラッグされている）であれば、スクロール動作を実行する
    if(_scrollModeSW || _isScrollKeyPressed){
        if(_scrollProcess){
            //transformationAnchorは（フォームで）必ずNoAnchorにセットしておくこと
            double zoomRatio = _graphicsView->getZoomRatio();
            _graphicsView->translate((_mpoint_W.x() - _mpoint_WP.x())/zoomRatio,
                                    (_mpoint_W.y() - _mpoint_WP.y())/zoomRatio);
         }
        _mpoint_WP = _mpoint_W;
        return;
    }
    _mpoint_WP = _mpoint_W;
}

/*!
 * \brief MainWindowGraphicsViewData::mousePress
 * \brief マウスがクリックされたとき（スクロールモード対応のため）
 * \param event
 */
void MainWindowGraphicsViewData::mousePress(QMouseEvent *event)
{
    //!スクロールモードの場合には、実際のスクロール処理をONにする
    if(_scrollModeSW || _isScrollKeyPressed){
        _graphicsView->viewport()->setCursor(Qt::ClosedHandCursor);
        _scrollProcess = true;
        return;
    }
}

/*!
 * \brief MainWindowGraphicsViewData::mouseRelease
 * \brief マウスのクリックが解除されたとき
 * \param event
 */
void MainWindowGraphicsViewData::mouseRelease(QMouseEvent *event)
{
    //!<スクロールモードである場合、実際のスクロール処理をOFFにする
    if(_scrollModeSW || _isScrollKeyPressed){
      _graphicsView->viewport()->setCursor(Qt::OpenHandCursor);
      _scrollProcess = false;
      return;
    }
}

/*!
 * \brief MainWindowGraphicsViewData::wheelZoom
 * \brief ホイールを回した際のズーム動作
 * \param event
 */
void MainWindowGraphicsViewData::wheelZoom(QWheelEvent *event)
{
    //!現在のズーム率と、画面中心に対するマウスの位置を取得する
    double currentRatio = _graphicsView->getZoomRatio();
    double distanceToCenterX = (double)(_mpoint_W.x() - _graphicsView->width()/2)/currentRatio;
    double distanceToCenterY = (double)(_mpoint_W.y() - _graphicsView->height()/2)/currentRatio;

    //!Wheelの回転方向により、あらかじめ設定されたズーム率で倍率変更ののち、画像の表示位置を調整する
    if(_image.isNull()) return;
    if(event->delta() > 0){//手前に回転
        double scalingRatio = currentRatio * _graphicsView->getWheelZoomStep();
        _graphicsView->wheelZoomIn();
        _graphicsView->centerOn((_mpoint_I.x() - distanceToCenterX / scalingRatio),
                 (_mpoint_I.y() - distanceToCenterY / scalingRatio));
    }
    else {//奥向きに回転
        double scalingRatio = currentRatio / _graphicsView->getWheelZoomStep();
        _graphicsView->wheelZoomOut();
        _graphicsView->centerOn((_mpoint_I.x() - distanceToCenterX / scalingRatio),
                 (_mpoint_I.y() - distanceToCenterY / scalingRatio));
    }

    return;
}

/*!
 * \brief MainWindowGraphicsViewData::keyPress
 * \brief スクロール用のキーが押された際にスクロールモードをONにする
 * \param event
 */
void MainWindowGraphicsViewData::keyPress(QKeyEvent *event)
{
    if(!_keyLock){
        if(event->key() == _scrollKey){
            _isScrollKeyPressed = true;
            _graphicsView->viewport()->setCursor(Qt::OpenHandCursor);
            _keyLock = true;
//            ui->actionScroll->setDisabled(true);
        }
    }
}

/*!
 * \brief MainWindowGraphicsViewData::keyRelease
 * \brief スクロール用のキーが解除された際にスクロールモードをOFFにする
 * \param event
 */
void MainWindowGraphicsViewData::keyRelease(QKeyEvent *event)
{
    if(_keyLock){
        if(_isScrollKeyPressed){
            _graphicsView->viewport()->setCursor(Qt::ArrowCursor);
            if(_crossCursor){
                _graphicsView->viewport()->setCursor(Qt::CrossCursor);
            }
            _isScrollKeyPressed = false;
            _scrollModeSW = false;
//            ui->actionScroll->setEnabled(true);
        }
        _keyLock = false;
    }
}


MainWindowGraphicsViewData::MainWindowGraphicsViewData()
{
    init();
}

/*!
 * \brief MainWindowGraphicsViewData::init
 * \brief 初期化処理
 */
void MainWindowGraphicsViewData::init()
{
    _keyLock = false;
    _isScrollKeyPressed = false;
    _keyLock = false;
    _isScrollKeyPressed = false;
    _scrollKey = SCROLL_KEY;
    _crossCursor = false;
    _scrollModeSW = false;
    _scrollProcess = false;
    _mpoint_W = QPoint(0,0);
    _map = QPoint(0,0);
    _mpoint_WP = QPoint(0,0);
    _mpoint_I = QPoint(0,0);
    _rgb = qRgb(0,0,0);
}





















//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// MainWindow:: public function
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


/*!
 * \brief MainWindow::MainWindow
 * 本体の生成\n
 * 生成と同時にui, Privatedata, GraphicsviewDataの生成を行う
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _pdata(new MainWindowPrivateData),
    _gv(new MainWindowGraphicsViewData)
{
#ifdef P_CONSTRUCT //動作確認用
    cout << "P_CONSTRUCT > MainWindow::MainWindow(QWidget *parent)" << endl;
#endif

    //!メインウィンドウにフォームで作成したUIをセットする
    ui->setupUi(this);

    this->setWindowTitle(SOFTWARE_VERSION);
    _isRefreshingNow = false;
    _isSpecifyed = false;

    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);
    _image = QSharedPointer<QImage>(new QImage);
    _scene = QSharedPointer<QGraphicsScene>(new QGraphicsScene);
    _pdata.data()->_image = _image;
    _pdata.data()->_scene = _scene;
    _gv.data()->_graphicsView = ui->graphicsView;
    _gv.data()->_image = _image;

    //for create polygon/rect mode
    _createPolygonMode = false;
    _cornerNumber = 4;
    _polygonCircleSize= 25;
    _selectItemMinimumDistance = 25;
    _crossCursor = false;
    _createRectMode = false;
    _editRectItem = NULL;
    _setRectItem = false;
    _rectCreating = false;

    //for edit mode
    _isEditMode = false;
    _nowEditing = false;
    _editCircleSize = 30;

    //for select mode
    _isSelectMode = false;

    //for setOrder mode
    _isSetOrderMode = false;

    //for showOrder mode
    _isShowOrderMode = false;

    //????
    _currentCharacterNumber = -1;

    connectGraphicsView();
    connect(this, SIGNAL(signal_setStatusBarMessage(QString, int)),
            ui->statusBar, SLOT(showMessage(QString, int)));
    displayMousePosition(QPoint(0,0));

    //Info
    initCharacterList();


    metaDataListClear();

    //!メタデータ格納用のディレクトリパスを設定する
    _metadataDirectoryName = "metadata";
    _commonMetadataEdit = false;
    _pageMetadataEdit = false;
    clearScene();
}


MainWindow::~MainWindow()
{
    cancelAllMode();
    delete ui;
#ifdef P_DESTRUCT
    cout << "P_DESTRUCT  > MainWindow::~MainWindow()" << endl;
#endif
}



/*!
 * \brief openImageFile
 * \param fileName 画像ファイル名
 * \param loadMetadataStatus 画像open時に合わせてメタデータを読み込むかどうかのフラグ
 * \return
 */
bool MainWindow::openImageFile(QString fileName, bool loadMetadataStatus)
{
    if(_commonMetadataEdit || _pageMetadataEdit){
        writeMetaData();
    }

    //!画像を開く前に各種アイテム類を初期化する
    cancelAllMode();
    clearScene();
    metaDataUIClear();
    metaDataListClear();
    refresh_ALL_ListWidget();

    //!画像ファイルを読み込む
    QString msg = tr("open image file : ") + fileName + tr(" ... ");
    if(_pdata.data()->_image.data()->load(fileName)){
        msg += "success!";
        setStatusBarMessage(msg);
        ui->label_FileName->setText(fileName);
    }
    else{
        msg += "fail!";
        setStatusBarMessage(msg);
        return false;
    }

    //!画像サイズ変換が有効であった場合、一定サイズまで画像サイズを変更する
    //読み込んだ画像が大きすぎて表示に時間がかかる場合に対応したもの
    bool status = IMAGE_SIZE_CONVERSION;
    if(status){
        _pdata.data()->_originalImage = *_pdata.data()->_image.data();
        double sizeRatio = _pdata.data()->calcOriginalImageSizeRatio();
        int convertedWidth = sizeRatio * _pdata.data()->_image.data()->width();
        int convertedHeight = sizeRatio * _pdata.data()->_image.data()->height();
        //cout << "image size convertion => convertedWidth:"
            //<< convertedWidth << " convertedHeight:" << convertedHeight << endl;
        *_pdata.data()->_image.data() = _pdata.data()->_originalImage.scaled
                (convertedWidth, convertedHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else{
    }

    //!画像が読み込めたらファイルユーティリティーに名前をセットする
    _fileUtility.setFile(fileName);

    //!画像の表示
    QPixmap pixmap = QPixmap::fromImage(*_pdata.data()->_image.data());
    _pdata.data()->_scene.data()->addPixmap(pixmap);
    fitScale();

    //!メタデータを読み込む設定であれば読み込み処理を行う
    if(loadMetadataStatus){
        loadMetadata();
    }

    _metadata.imageFileName = _fileUtility.getCurrentFileNameCore();
    //_metadata.imageFileName = _fileUtility.getCurrentFileNameCore_WOExt();//拡張子なし
    _metadata.imageWidth = _pdata.data()->_originalImage.width();
    _metadata.imageHeight = _pdata.data()->_originalImage.height();

    return true;
}

/*!
 * \brief 画面下部のステータスバーにメッセージを表示するための関数
 * \param message メッセージ内容
 * \param time メッセージ表示時間
 */
void MainWindow::setStatusBarMessage(QString message, int time)
{
    emit signal_setStatusBarMessage(message, time);
}


/*!
 * \brief ウィンドウにマウスポインタの座標を表示するための関数
 * \param pt マウスの座標（画像上の座標）
 */
void MainWindow::displayMousePosition(QPoint pt)
{
    QString textX = QString("%1%2").arg("X:").arg(pt.x());
    QString textY = QString("%1%2").arg("Y:").arg(pt.y());
    ui->label_MouseX->setText(textX);
    ui->label_MouseY->setText(textY);
}

/*!
 * \brief シーン本体と、保持している各種GraphicsItemを消去する
 */
void MainWindow::clearScene()
{
    //シーン本体のリセット
    _scene.data()->clear();
    //各種Itemのリセット
    _metadata.clear();
    releaseSpecificItems();
    metaDataUIClear();
}

/*!
 * \brief 画像の表示倍率をウィンドウサイズに合わせて調整する
 */
void MainWindow::fitScale()
{
    if(_image.isNull()) return;
    //!<倍率を計算する
    double ratioX, ratioY, ratio;
    ratioX = ((double)ui->graphicsView->width()-2) / _image.data()->width();
    ratioY = ((double)ui->graphicsView->height()-2) / _image.data()->height();
    ratio = min(ratioX, ratioY);
    ui->graphicsView->zoom(ratio);
}




//-----------------------------------------------------------------------
// MainWindow:: private slots with GUI (FORM)
//-----------------------------------------------------------------------
void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName
        (this, tr("Load Image"), _setting.getFileDirectory(), _setting.getFilterForImage());
    if(!fileName.isEmpty()){
        openImageFile(fileName);
    }
    return;
}

void MainWindow::on_actionNext_triggered()
{
    //変更が有る場合保存のチェック
    this->openImageFile(_fileUtility.getNextFileName());
}


void MainWindow::on_actionPrevious_triggered()
{
    //変更が有る場合保存のチェック
    this->openImageFile(_fileUtility.getPreviousFileName());
}

void MainWindow::on_actionZoomIn_triggered()
{
    if(_image.isNull()) return;
    ui->graphicsView->zoomIn();
}

void MainWindow::on_actionZoomOut_triggered()
{
    if(_image.isNull()) return;
    ui->graphicsView->zoomOut();
}

void MainWindow::on_actionResetZoom_triggered()
{
    fitScale();
}

//-----------------------------------------------------------------------
// MainWindow:: private slots with Graphics View
//-----------------------------------------------------------------------

void MainWindow::Sl_GVMo_move(QMouseEvent *event)
{
    //! 処理開始
    //! 画像が入っていなければ何もしないで終了
    if(_image.data()->isNull()) return;

    //! マウス位置の計算と、スクロールモード時のスクロール動作
    _gv.data()->mouseMove(event);

    //! マウス位置の表示
    displayMousePosition(_gv.data()->_mpoint_I);

    //! 各モード用のマウス動作時関数を呼び出す
    if(_createPolygonMode){
        createPolygonModeMouseMove();
    }
    else if(_rectCreating){
        createRectMouseMove(_gv.data()->_mpoint_I);
    }
    else if(_isEditMode){
        editModeMouseMove(_gv.data()->_mpoint_I);
    }
    else if(_isSelectMode){
        selectModeMouseMove(_gv.data()->_mpoint_I);
    }
    else if(_isSetOrderMode){
        setOrderModeMouseMove(_gv.data()->_mpoint_I);
    }
    //! 処理終了
}


void MainWindow::Sl_GVMo_press(QMouseEvent *event)
{
    //! 処理開始
    if(_image.data()->isNull()) return;
    //! クリックされたボタンにより処理変更
    if( event->button() == Qt::LeftButton){//左クリック時
        _gv.data()->mousePress(event);
        //! - 各モード用のマウス左クリック時動作を呼び出す
        if(_createPolygonMode){
            addPointForEditingPolygonItem(_gv.data()->_mpoint_I);
        }
        if(_createRectMode){
            createRectMousePress(_gv.data()->_mpoint_I);
        }
        if(_isEditMode){
            editModeMouseClick(_gv.data()->_mpoint_I);
        }
        if(_isSelectMode){
            selectModeMouseClick(_gv.data()->_mpoint_I);
        }
        if(_isSetOrderMode){
            setOrderModeMouseClick(_gv.data()->_mpoint_I);
        }

    }
    else{//右クリック時
        //! - 各モード用のマウス右クリック時動作を呼び出す
        if(_createPolygonMode){
            cancelCurrentLine(_gv.data()->_mpoint_I);
        }
        else if(_createRectMode && _setRectItem){
            cancelCreateRect();
        }
        else if(_isEditMode){
            cancelEditMode();
            //editmodeは基本的にselectModeのみから呼ばれるため、selectmodeに復帰する
            startSelectMode(_selectTargetType);
            selectModeSelect(selectItem(_gv.data()->_mpoint_I));
        }
        else if(_isSelectMode){
            selectModeRightClick();
        }
        else if(_isSetOrderMode){
            setOrderModeMouseRightClick();
        }
    }
    //! 処理終了
}

void MainWindow::Sl_GVMo_release(QMouseEvent *event)
{
    if(_image.data()->isNull()) return;

    _gv.data()->mouseRelease(event);

    if(_rectCreating){
        createRectMouseRelease(); //矩形生成モード時の動作
    }
    else if(_isEditMode){ //Editモード時の操作
        editModeMouseRelease(_gv.data()->_mpoint_I);
    }
}


//!拡大時にマウスカーソルに追随する様に改良 2014/10/09 daisuke
void MainWindow::Sl_GVMo_wheel(QWheelEvent *event)
{
    _gv.data()->wheelZoom(event);
}

/*!
 * \brief GraphicsViewにフォーカスしている際に、キーボード操作がされた場合の処理
 * \brief MainWindow::Sl_GVKy_press
 * \param event
 */
void MainWindow::Sl_GVKy_press(QKeyEvent *event)
{
    //! 処理開始
    _gv.data()->keyPress(event);
    //! ESCが押された際の動作（各モードごとの処理を実行）
    if(event->key() == Qt::Key_Escape){
        if(_createPolygonMode){//! - ポリゴン生成モードをキャンセルする
            cancelCreatePolygon();
        }
        if(_createRectMode){//! - 矩形生成モードをキャンセルする
            cancelCreateRect();
        }
        if(_isEditMode){ //! - エディットモードをキャンセルする
            cancelEditMode();
        }
        if(_isSelectMode){//! - 選択モードをキャンセルする
            selectModeCancel();
        }
    }
    //! Shiftが押された際の動作
    if(event->key() == Qt::Key_Shift){
        if(_createRectMode && _setRectItem){
            terminateCreateRect();//! - 矩形生成モード時/矩形設定時には、矩形生成を終了する
        }
    }

    //! Deleteが押された際の動作
    if(event->key() == Qt::Key_Delete){
        if(_isSelectMode){
            selectModeDelete();//! - 選択モード時には、現在選択されているアイテムを削除する
        }
    }
    //! 処理終了
}

void MainWindow::Sl_GVKy_release(QKeyEvent *event)
{
    _gv.data()->keyRelease(event);
}


/*!
 * \brief ズーム率が変更された際の動作
 * \brief MainWindow::Sl_GV_zoomed
 * \param zoomRatio 変更後のズーム率
 */
void MainWindow::Sl_GV_zoomed(double zoomRatio)
{
    //! 処理開始
    QString text;
    //! ズーム率を小数点以下3桁で文字変換する
    text.setNum(zoomRatio, 3, 3);
    text += "x";
    //! 倍率を表示する
    if(zoomRatio > 0.0) {
        ui->label_ZoomRatio->setText(text);
    }
    //! 処理終了
}



//-----------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------

/*!
 * \brief GraphicsViewの各種SignalとMainWindowのSlotを接続する処理（GraphicsView生成時用）
 * \brief MainWindow::connectGraphicsView
 */
void MainWindow::connectGraphicsView()
{
    ui->graphicsView->setScene(_pdata.data()->_scene);
    //! mouse関連機能の接続
    connect(ui->graphicsView, SIGNAL(SiMo_move(QMouseEvent*)),
            this, SLOT(Sl_GVMo_move(QMouseEvent*)));
    connect(ui->graphicsView, SIGNAL(SiMo_wheel(QWheelEvent*)),
            this, SLOT(Sl_GVMo_wheel(QWheelEvent*)));
    connect(ui->graphicsView, SIGNAL(SiMo_press(QMouseEvent*)),
            this, SLOT(Sl_GVMo_press(QMouseEvent*)));
    connect(ui->graphicsView, SIGNAL(SiMo_release(QMouseEvent*)),
            this, SLOT(Sl_GVMo_release(QMouseEvent*)));

    //! keyboard関連機能の接続
    connect(ui->graphicsView, SIGNAL(SiKy_press(QKeyEvent*)),
            this, SLOT(Sl_GVKy_press(QKeyEvent*)));
    connect(ui->graphicsView, SIGNAL(SiKy_release(QKeyEvent*)),
            this, SLOT(Sl_GVKy_release(QKeyEvent*)));

    //! ズーム結果通知用機能の接続
    connect(ui->graphicsView, SIGNAL(signal_zoomed(double)),
            this, SLOT(Sl_GV_zoomed(double)));
}


/*!
 * \brief 指定されたメタデータの順番を変更する処理を開始する関数
 * \brief MainWindow::setOrderModeStart
 * \param type 順番設定するメタデータのタイプ
 *
 */
void MainWindow::setOrderModeStart(ComicMetadataType type)
{
    //! 現在進行しているすべての作業を中止
    cancelAllMode();
    //! 一旦すべての表示されているアイテムを隠す
    hideAll();

    //! SetOrderModeのフラグをOnにする
    _isSetOrderMode = true;

    //! 取り扱うメタデータの種類をセットする
    _setOrderTargetType = type;

    //! 順序入れ替え用データのストック場所をクリアする
    _setOrderTarget.clear();


    //! 取り扱うメタデータの枠部分の情報を、_setOrderTargetにセットする
    GraphicsItemData GIData;
    switch(type){
    case ComicMetadata_Frame:
        for(int i=0; i<_metadata.frame.data()->size(); i++){
            GIData = _metadata.frame.data()->at(i).GIData;
            GIData.colorActive();
            _setOrderTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_Character:
        for(int i=0; i<_metadata.character.data()->size(); i++){
            GIData = _metadata.character.data()->at(i).GIData;
            GIData.colorActive();
            _setOrderTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_Dialog:
        for(int i=0; i<_metadata.dialog.data()->size(); i++){
            GIData = _metadata.dialog.data()->at(i).GIData;
            GIData.colorActive();
            _setOrderTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_Onomatopoeia:
        for(int i=0; i<_metadata.onomatopoeia.data()->size(); i++){
            GIData = _metadata.onomatopoeia.data()->at(i).GIData;
            GIData.colorActive();
            _setOrderTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_Item:
        for(int i=0; i<_metadata.item.data()->size(); i++){
            GIData = _metadata.item.data()->at(i).GIData;
            GIData.colorActive();
            _setOrderTarget.push_back(GIData);
        }
    case ComicMetadata_All:
        _isSetOrderMode = false;
        break;
    default:
        _isSetOrderMode = false;
        break;
    }

    //! 種類変更できないメタデータタイプが指定された場合には処理をキャンセルして終了する
    if(!_isSetOrderMode) {
        setOrderModeCancel();
        return;
    }

    //! 順序変更用ターゲットに入っている各ポリゴンの面積を計算しておく
    _isSetOrderModePolygonSizeList.clear();
    for(int i=0; i<_setOrderTarget.size(); i++){
        GraphicsItemData GIData = _setOrderTarget.at(i);
        QPolygonF polygon = GIData.item()->polygon();
        _isSetOrderModePolygonSizeList.push_back(calcPolygonAreaSize(polygon));
    }
    //! 終了
}

/*!
 * \brier setOrderModeを終了する（ここではデータの反映は行わない）
 * \brief MainWindow::setOrderModeCancel
 */
void MainWindow::setOrderModeCancel()
{
    //! setOrderModeのフラグをOFFにする
    _isSetOrderMode = false;

    //! 順序入れ替え用データをクリアする
    _setOrderTarget.clear();
    _isSetOrderModeSelectedList.clear();
    _isSetOrderModePolygonSizeList.clear();

    //! 順序情報表示用GraphicsItemを削除する
    for(int i=0; i<_orderNumberRect.size(); i++){
        _scene.data()->removeItem(_orderNumberRect.at(i));
    }
    _orderNumberRect.clear();
    for(int i=0; i<_orderNumberText.size(); i++){
        _scene.data()->removeItem(_orderNumberText.at(i));
    }
    _orderNumberText.clear();

    //! メタデータの表示を元に戻す
    showAll();

    //! 終了
}

/*!
 * \brief setOrderModeでの変更を確定して終了する
 * \brief MainWindow::setOrderModeTerminate
 * \note 現時点ではコマの情報のみ順序変更対応
 * 順序入れ替え結果を反映する際には\n
 * 各データを、新しい順番で一時ストック用データ置き場に保持し、一旦元データはクリア\n
 * その後ストックしておいたデータをコピーするという手順で行う
 */
void MainWindow::setOrderModeTerminate()
{
    //! 指定されているデータを、新しい順番に入れ替える
    switch(_setOrderTargetType){//現時点ではコマのみ対応
    case ComicMetadata_Frame:{
        QVector<FrameData> newFrameData;//一時ストック用
        for(int i=0; i<_isSetOrderModeSelectedList.size(); i++){
            newFrameData.push_back(_metadata.frame.data()->at(_isSetOrderModeSelectedList.at(i)));
        }
        _metadata.frame.data()->clear();
        for(int i=0; i < newFrameData.size(); i++){
            _metadata.frame.data()->push_back(newFrameData.at(i));
        }
        refresh_Frame_ListWidget();
        break;
    }
    case ComicMetadata_Character:
        break;
    case ComicMetadata_Dialog:
        break;
    case ComicMetadata_Onomatopoeia:
        break;
    case ComicMetadata_Item:
        break;
    case ComicMetadata_All:
        break;
    default:
        break;
    }

    //! データの反映が終了したのちに、setOrderModeを終了する処理を実行
    setOrderModeCancel();
}

/*!
 * \brief setOrderModeでマウスが左クリックされたときの動作
 * \brief MainWindow::setOrderModeMouseClick
 * \param pt クリックされた際のマウス座標
 *
 */
void MainWindow::setOrderModeMouseClick(QPoint pt)
{
    //! もしすべて選択済みの状態でクリックされた場合には、変更を確定して終了
    if(_isSetOrderModeSelectedList.size() == _setOrderTarget.size()){
        setOrderModeTerminate();
        return;
    }
    //! もし未選択のアイテム上でクリックされた場合には、選択されたアイテムの番号をselectedListに追加し、番号を表示する
    int selected = selectItem(_setOrderTarget, _isSetOrderModePolygonSizeList, pt, _isSetOrderModeSelectedList);
    bool selectedStatus = false;
    for(int i=0; i<_isSetOrderModeSelectedList.size(); i++){
        if(selected == _isSetOrderModeSelectedList.at(i)){
            selectedStatus = true;
        }
    }
    //! 既に選択されているアイテムの上であった場合か、何の上でもない場合には終了
    if(selectedStatus || selected < 0){
        return;
    }

    //! 選択されていないアイテムの上であった場合、選択状態に移行する
    _isSetOrderModeSelectedList.push_back(selected);
    GraphicsItemData GIData = _setOrderTarget.at(selected);
    GIData.colorDefault();
    _selectedItemNumber = -1;

    QPolygonF polygon = GIData._item->polygon();
    QPointF center = getPolygonCenter(polygon);
    center.setX(center.x() - 30);
    center.setY(center.y() - 30);
    int digit = (int)log10(_isSetOrderModeSelectedList.size()) + 1;
    QGraphicsRectItem *el = new QGraphicsRectItem;
    el->setRect(center.x()-20, center.y()-10, 80+20*(digit-1), 80);
    el->setBrush(QBrush(QColor(255,255,255,150)));
    el->setPen(QPen(QBrush(QColor(200,200,200,255)),5));
    _scene->addItem(el);
    QGraphicsTextItem *io = new QGraphicsTextItem;
    io->setPos(center);
    io->setPlainText(QString("%1").arg(_isSetOrderModeSelectedList.size()));
    io->setFont(QFont("Helvetica", 40));
    _scene->addItem(io);
    _orderNumberRect.push_back(el);
    _orderNumberText.push_back(io);

}

/*!
 * \brief setOrderMode時に右クリックされた際の動作
 * \brief MainWindow::setOrderModeMouseRightClick
 */
void MainWindow::setOrderModeMouseRightClick()
{
    //! 処理開始
    //! 順序変更用Listが空の場合にはsetOrderModeをキャンセルして終了
    if(_isSetOrderModeSelectedList.isEmpty()){
        setOrderModeCancel();
        return;
    }

    //! 現在設定している一つ前の順番を取得する
    int previous = _isSetOrderModeSelectedList.at(_isSetOrderModeSelectedList.size()-1);
    //! 一つ前のアイテムを取得し、変更対象に設定する
    GraphicsItemData item;
    item = _setOrderTarget.at(previous);
    item.colorActive();

    //! 設定済みであった最後のアイテム（一つ前に相当する）を除去する
    _isSetOrderModeSelectedList.removeLast();
   if(!_orderNumberRect.isEmpty()){
        _scene.data()->removeItem(_orderNumberRect.at(_orderNumberRect.size()-1));
        _orderNumberRect.removeLast();
    }
    if(!_orderNumberText.isEmpty()){
        _scene.data()->removeItem(_orderNumberText.at(_orderNumberText.size()-1));
        _orderNumberText.removeLast();
    }
    //! 処理終了
}

/*!
 * \brief setOrderModeにおけるマウス移動時の処理
 * \brief MainWindow::setOrderModeMouseMove
 * \param pt マウス座標
 */
void MainWindow::setOrderModeMouseMove(QPoint pt)
{
    //! 処理開始

    //!　現在のマウス座標をもとに、選択されているアイテム番号を取得する
    int selected = selectItem(_setOrderTarget, _isSetOrderModePolygonSizeList, pt, _isSetOrderModeSelectedList);

    //! 選択されている対象が、変更となる場合には以下を実行する
    if(selected != _selectedItemNumber){
        //! - 現在のマウス下がすでに選択されている番号であった場合には何もしない
        for(int i=0; i<_isSetOrderModeSelectedList.size(); i++){
            if(_isSetOrderModeSelectedList.at(i)== selected){
                return;
            }
        }

        //! - 現在選択状態となっている物があり、かつ別の番号の上に移動した場合には、一旦現在選択状態の物を開放する
        if(_selectedItemNumber >= 0){
            GraphicsItemData prevItemData = _setOrderTarget.at(_selectedItemNumber);
            prevItemData.colorActive();
            _selectedItemNumber = -1;
        }

        //! - 現在選択されている番号のアイテムと、その表示を選択状態に変更する
        _selectedItemNumber = selected;
        if(_selectedItemNumber >= 0){
            GraphicsItemData GIData = _setOrderTarget.at(_selectedItemNumber);
            GIData.colorBrushSelected();
        }
    }
    //! 処理終了
}

/*!
 * \brief ポリゴン生成モードの開始処理
 * \brief MainWindow::startCreateMode_Polygon
 * \param type 本処理で追加するメタデータの種類
 */
void MainWindow::startCreateMode_Polygon(ComicMetadataType type)
{
    //! 処理開始

    //! すでに始まっているポリゴン生成処理があればキャンセル
    cancelCreatePolygon();

    //! ポリゴン生成モード用の設定を行う
    _createPolygonMode = true;
    _targetType = type;
    _cornerNumber = 4; //!< ポリゴンモードでの最大角数を設定（基本的に矩形のためここでは4に設定）

    //!作成するアイテムの色を設定する（作成するメタデータタイプに応じた色を設定）
    switch(type){
    case ComicMetadata_Frame:
        _editColor.setColorPreset(GraphicsItemDataColor_Blue);
        break;
    case ComicMetadata_Character:
        _editColor.setColorPreset(GraphicsItemDataColor_Red);
        break;
    case ComicMetadata_Dialog:
        _editColor.setColorPreset(GraphicsItemDataColor_Green);
        break;
    case ComicMetadata_Onomatopoeia:
        _editColor.setColorPreset(GraphicsItemDataColor_Purple);
        break;
    case ComicMetadata_Item:
        _editColor.setColorPreset(GraphicsItemDataColor_LightBlue);
    default:
        break;
    }

    //! ポリゴン新規作成用初期化
    _cornerCount = 0;
    _createPolygonMode = true;
    _crossCursor = true;
    _newline = NULL;

    //! マウス表示の変更
    ui->graphicsView->viewport()->setCursor(Qt::CrossCursor);

    //! 処理終了
}

/*!
 * \brief ポリゴン編集モード時に、点を追加する処理
 * \brief MainWindow::addPointForEditingPolygonItem
 * \param pt
 */
void MainWindow::addPointForEditingPolygonItem(QPoint pt)
{
    //! 処理開始

    //! 画像がない場合には何もせず終了
    if(_image.isNull()){
        return;
    }

    //! 線の有無により最初の点の場合の処理と２番目以降の処理を切り分ける
    if(_newline==NULL)
    {
        //! 最初の点の処理
        //! - 初期点アイテムを新規生成する
        _startPoint = pt;
        _startCircle = new QGraphicsEllipseItem
                (pt.x() - _polygonCircleSize, pt.y() - _polygonCircleSize,
                 _polygonCircleSize * 2, _polygonCircleSize * 2);
        _startCircle->setBrush(_editColor.brush_active);
        _startCircle->setPen(_editColor.pen_active);
        _scene.data()->addItem(_startCircle);
    }
    else{
        //! 2番目の点以降の処理
        //! - 始点付近をクリックした場合には連結してポリゴン生成モードを終了する
        if(fabs((double)(_startPoint.x() - pt.x())) < _polygonCircleSize
                && fabs((double)(_startPoint.y() - pt.y())) < _polygonCircleSize){
            terminateCreatePolygon();
            return;
        }
        else{//! - 始点付近以外をクリックした際には、点を追加する
            QLineF line = _newline->line();
            line.setP2(pt);
            _newline->setLine(line);
            _editPolygonLine.push_back(_newline);
        }
    }

    //! 直前の点とマウス位置をつなぐ線アイテムを生成、表示する
    _newline = new QGraphicsLineItem(QLine(pt, pt));
    _newline->setPen(QPen(_editColor.brush_active,4));
    _scene.data()->addItem(_newline);

    //! 処理終了
}

/*!
 * \brief 生成したポリゴンを確定させる処理
 * \brief MainWindow::terminateCreatePolygon
 */
void MainWindow::terminateCreatePolygon()
{
    //! 処理開始
    QLineF line = _newline->line();
    line.setP2(_startPoint);
    _newline->setLine(line);
    _editPolygonLine.push_back(_newline);
    _newline = NULL;

    QVector<QPointF> polygonCorner;
    for(int i=0; i<_editPolygonLine.size() ; i++){
        polygonCorner.push_back(_editPolygonLine.at(i)->line().p1());
        _scene->removeItem(_editPolygonLine.at(i));
        delete _editPolygonLine.at(i);
    }
    _editPolygonLine.clear();
    _scene->removeItem(_startCircle);
    delete _startCircle;//!< delete removed item
    _startCircle = NULL;

    //! 新しいメタデータとして追加処理を行う（各メタデータごとの処理）
    QPolygonF polygon(polygonCorner);
    switch(_targetType){
    case ComicMetadata_Frame:
    {
        FrameData newframe;
        newframe.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
        newframe.GIData.colorDefault();
        _scene.data()->addItem(newframe.GIData.item());
        _metadata.frame.data()->push_back(newframe);
        _metadata.renewMangaPath_Frame(_metadata.frame.data()->size());
    }
        break;
    case ComicMetadata_Character:
    {
        CharacterData newcharacter;
        newcharacter.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
        newcharacter.GIData.colorDefault();
        _scene.data()->addItem(newcharacter.GIData.item());
        _metadata.character.data()->push_back(newcharacter);
        _metadata.renewMangaPath_Character(_metadata.character.data()->size()-1);
    }
        break;
    case ComicMetadata_Dialog:
    {
        DialogData newdialog;
        newdialog.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
        newdialog.GIData.colorDefault();
        _scene.data()->addItem(newdialog.GIData.item());
        _metadata.dialog.data()->push_back(newdialog);
        _metadata.renewMangaPath_Dialog(_metadata.dialog.data()->size()-1);
    }
        break;
    case ComicMetadata_Onomatopoeia:
    {
        OnomatopoeiaData newonomatopoeia;
        newonomatopoeia.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
        newonomatopoeia.GIData.colorDefault();
        _scene.data()->addItem(newonomatopoeia.GIData.item());
        _metadata.onomatopoeia.data()->push_back(newonomatopoeia);
        _metadata.renewMangaPath_Onomatopoeia(_metadata.onomatopoeia.data()->size()-1);
    }
        break;
    case ComicMetadata_Item:
    {
        ItemData newitem;
        newitem.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
        newitem.GIData.colorDefault();
        _scene.data()->addItem(newitem.GIData.item());
        _metadata.item.data()->push_back(newitem);
        _metadata.renewMangaPath_Item(_metadata.item.data()->size()-1);
    }
    default:
        break;
    }

    //! 設定が終わったらポリゴン生成モードを一旦終了する
    cancelCreatePolygon();

    //! 次のポリゴン生成モードに移行する
    startCreateMode_Polygon(_targetType);

    //! 処理終了
}

/*!
 * \brief 一つ前に設定した点をキャンセルする
 * \brief MainWindow::cancelCurrentLine
 * \param pt
 */
void MainWindow::cancelCurrentLine(QPoint pt)
{
    if(_image.isNull() || _scene == NULL) return;

    if(_createPolygonMode == true){
        if(_editPolygonLine.size() > 0){
            _scene->removeItem(_newline);
            delete _newline;
            _newline = _editPolygonLine.at(_editPolygonLine.size()-1);
            _editPolygonLine.removeLast();
            QLineF line = _newline->line();
            line.setP2(pt);
            _newline->setLine(line);
        }
        else{
            cancelCreatePolygon();
        }
    }
}

/*!
 * \brief ポリゴン生成モードをキャンセルして終了する
 * \brief MainWindow::cancelCreatePolygon
 */
void MainWindow::cancelCreatePolygon()
{
    //! 処理開始
    //! 各種生成してある仮アイテムを除去して終了する
    if(_scene==NULL || _image.isNull()) return;
    if(!_createPolygonMode) {
        return;
    }
    _createPolygonMode = false;

    if(_newline){
        _scene->removeItem(_newline);
        delete _newline;
        _newline = NULL;
    }
    if(!_editPolygonLine.isEmpty()){
        deleteLineItems(_editPolygonLine);
    }
    if(_startCircle){
        _scene->removeItem(_startCircle);
        delete _startCircle;
        _startCircle = NULL;
    }
    //! カーソルを元に戻す
    ui->graphicsView->viewport()->setCursor(Qt::ArrowCursor);

    //! 処理終了
}

/*!
 * \brief ポリゴン生成モード等で作成された線（複数）を取り消す処理
 * \brief MainWindow::deleteLineItems
 * \param lines
 */
void MainWindow::deleteLineItems(QVector<QGraphicsLineItem *> &lines)
{
    if(lines.isEmpty()){
        return;
    }
    else {
        for(int i=0; i<lines.size();i++){
            _scene.data()->removeItem(lines.at(i));
            delete lines.at(i);
        }
        lines.clear();
    }
}

/*!
 * \brief ポリゴン生成モードでマウスが移動した場合の処理
 * \brief MainWindow::createPolygonModeMouseMove
 */
void MainWindow::createPolygonModeMouseMove()
{
    //! 最初の1点を打った後であれば、lineが存在しているため、そのlineの終点を移動させる
    if(_newline != NULL){
        QLineF line = _newline->line();
        line.setP2(_gv.data()->_mpoint_I);
        _newline->setLine(line);
        //! マウスの位置がすでにチェックした点に近い場合にはカーソルの表示を変える
        if(fabs((double)(_startPoint.x() - _gv.data()->_mpoint_I.x())) < _polygonCircleSize &&
           fabs((double)(_startPoint.y() - _gv.data()->_mpoint_I.y())) < _polygonCircleSize)
        {
            if(_crossCursor){
                ui->graphicsView->viewport()->setCursor(Qt::ArrowCursor);
                _crossCursor = false;
            }
        }
        else{
            if(!_crossCursor){
                ui->graphicsView->viewport()->setCursor(Qt::CrossCursor);
                _crossCursor = true;
            }
        }
    }
}

/*!
 * \brief 矩形生成用のボタンをクリックされた際の動作
 * \brief MainWindow::on_AddFrame_Rect_clicked
 */
void MainWindow::on_AddFrame_Rect_clicked()
{
    //! 矩形生成モードを開始する
    startCreateMode_Rect(ComicMetadata_Frame);
}


/*!
 * \brief 矩形生成モードの開始処理
 * \brief MainWindow::startCreateMode_Rect
 * \param type 本処理で追加するメタデータの種類
 */
void MainWindow::startCreateMode_Rect(ComicMetadataType type)
{
    //! 処理開始
    //! 画像がセットされていない場合には何もせず処理終了
    if(_image.data()->isNull()) return;

    //! 本モードを含む全モードをキャンセル
    cancelAllMode();

    //! 矩形生成モード用の設定を行う
    _createRectMode = true;
    _targetType = type;
    _cornerNumber = 4; //!< ポリゴンモードでの最大角数
    _editRectItem = new QGraphicsRectItem;

    //! 作成するアイテムの色を設定する（作成するメタデータタイプに応じた色を設定）
    switch(type){
    case ComicMetadata_Frame:
        _editColor.setColorPreset(GraphicsItemDataColor_Blue);
        break;
    case ComicMetadata_Character:
        _editColor.setColorPreset(GraphicsItemDataColor_Red);
        break;
    case ComicMetadata_Dialog:
        _editColor.setColorPreset(GraphicsItemDataColor_Green);
        break;
    case ComicMetadata_Onomatopoeia:
        _editColor.setColorPreset(GraphicsItemDataColor_Purple);
        break;
    case ComicMetadata_Item:
        _editColor.setColorPreset(GraphicsItemDataColor_LightBlue);
    default:
        break;
    }

    //! 矩形新規作成用初期化
    _editRectItem->setBrush(_editColor.brush_active);
    _editRectItem->setPen(_editColor.pen_active);
    _createRectMode = true;
    _crossCursor = true;
    _rectCreating = false;

    //! マウス表示の変更
    ui->graphicsView->viewport()->setCursor(Qt::CrossCursor);  

    //! 処理終了
}

/*!
 * \brief 矩形生成モードをキャンセルして終了する
 * \brief MainWindow::cancelCreateRect
 */
void MainWindow::cancelCreateRect()
{
    //! 各種キャンセル処理
    if(_image.data()->isNull()) return;

    if(!_createRectMode) {
        return;
    }
    _createRectMode = false;
    _rectCreating = false;

    if(_setRectItem){
        _scene->removeItem(_editRectItem);
        _setRectItem = false;
        delete _editRectItem;
        _editRectItem = NULL;
    }

    //! マウス表示を元に戻す
    ui->graphicsView->viewport()->setCursor(Qt::ArrowCursor);

    //! 処理終了
}

/*!
 * \brief 矩形生成モードでマウスがクリックされた際の動作
 * \brief MainWindow::createRectMousePress
 * \param pt
 */
void MainWindow::createRectMousePress(QPoint pt)
{
    _rectPT1 = pt;
    if(!_setRectItem){
        //! 矩形を追加する
        _scene.data()->addItem(_editRectItem);
        _setRectItem = true;
    }
    //! 矩形が生成されていることを示すフラグを立てる
    _rectCreating = true;
}

/*!
 * \brief 矩形生成モードでマウスが移動した際の動作
 * \brief MainWindow::createRectMouseMove
 * \param pt
 */
void MainWindow::createRectMouseMove(QPoint pt)
{
    //! 矩形が生成されている場合には、その領域を変更する
    if(_rectCreating){
        _editRectItem->setRect(getRectBy2Pt(_rectPT1, pt));
    }
}

/*!
 * \brief 矩形生成の確定処理
 * \brief MainWindow::terminateCreateRect
 */
void MainWindow::terminateCreateRect()
{
    //ここで、点数が5点になっている可能性があるため注意
    QPolygonF polygon(_editRectItem->rect());
    if(polygon.size() == 5){
        polygon.removeFirst();
    }
    //cout << polygon.size() << endl;
    //! 新しいメタデータとして追加処理を行う
    switch(_targetType){
    case ComicMetadata_Frame:
    {
        addFrame(polygon);
    }
        break;
    case ComicMetadata_Character:
    {
        addCharacter(polygon);
    }
        break;
    case ComicMetadata_Dialog:
    {
        addDialog(polygon);
    }
        break;
    case ComicMetadata_Onomatopoeia:
    {
        addOnomatopoeia(polygon);
    }
        break;
    case ComicMetadata_Item:
    {
        addItem(polygon);
    }
    default:
        break;
    }
    //!　一旦矩形生成モードを終了
    cancelCreateRect();

    //! 次の矩形生成を開始
    startCreateMode_Rect(_targetType);
}


/*!
 * \brief 矩形生成モードでマウスが離された際の動作
 * \brief MainWindow::createRectMouseRelease
 */
void MainWindow::createRectMouseRelease()
{
    _rectCreating = false;
}

/*!
 * \brief すべてのモードを一括キャンセルする
 * \brief MainWindow::cancelAllMode
 * \note キャンセルされるモード一覧\br
 * - ポリゴン生成モード createPolygonMode
 * - 矩形生成モード createRectMode
 * - 編集モード editMode
 * - 選択モード selectMode
 * - 順番設定モード setOrderMode
 * - 順番表示
 */
void MainWindow::cancelAllMode()
{
    cancelCreatePolygon();
    cancelCreateRect();
    cancelEditMode();
    selectModeCancel();
    setOrderModeCancel();
    showOrderCancel();
    if(!_isSpecifyed){//現状ではCharacterがうまく選択できない為入れてあるスイッチ
        releaseSpecificItems();
    }
}

/*!
 * \brief AddCharacter(Rect)ボタンが押された際の動作
 * \brief MainWindow::on_AddCharacter_Rect_clicked
 */
void MainWindow::on_AddCharacter_Rect_clicked()
{
    startCreateMode_Rect(ComicMetadata_Character);
}

/*!
 * \brief AddDialogボタンが押された際の動作
 * \brief MainWindow::on_AddDialog_Rect_clicked
 */
void MainWindow::on_AddDialog_Rect_clicked()
{
    startCreateMode_Rect(ComicMetadata_Dialog);
}

/*!
 * \brief AddOnomatopoeiaボタンが押された際の動作
 * \brief MainWindow::on_AddOnomatopoeia_Rect_clicked
 */
void MainWindow::on_AddOnomatopoeia_Rect_clicked()
{
    startCreateMode_Rect(ComicMetadata_Onomatopoeia);
}

/*!
 * \brief 編集モードの開始（すでに作成されたアイテムを編集する）
 * \brief MainWindow::startEditMode
 * \param type メタデータの種類
 * \param number 何番目のデータを編集するかの指定
 */
void MainWindow::startEditMode(ComicMetadataType type, int number)
{
    //! 一旦全モードをキャンセル
    cancelAllMode();

    //! 画像がセットされていない場合は何もせず終了
    if(_image.data()->isNull()) return;

    //! メタデータの種類に応じて処理振り分け
    //! 指定されたメタデータの情報を取り出す
    switch(type){
    case ComicMetadata_Frame:
        if(number >= _metadata.frame.data()->size()) return;
        _editModeItem = _metadata.frame.data()->at(number).GIData;
        break;
    case ComicMetadata_Character:
        if(number >= _metadata.character.data()->size()) return;
        _editModeItem = _metadata.character.data()->at(number).GIData;
        break;
    case ComicMetadata_Dialog:
        if(number >= _metadata.dialog.data()->size()) return;
        _editModeItem = _metadata.dialog.data()->at(number).GIData;
        break;
    case ComicMetadata_Onomatopoeia:
        if(number >= _metadata.onomatopoeia.data()->size()) return;
        _editModeItem = _metadata.onomatopoeia.data()->at(number).GIData;
        break;
    case ComicMetadata_Item:
        if(number >= _metadata.item.data()->size()) return;
        _editModeItem = _metadata.item.data()->at(number).GIData;
        break;
    default:
        return;
        break;
    }

    _selectedItemNumber = number;
    cout << "startEditMode_A" << endl;

    //! 取り出したメタデータに対して、編集モード本体を起動する
    startEditMode(_editModeItem);

    //! 処理終了
}

/*!
 *　\brief 編集モード本体
 * \brief MainWindow::startEditMode
 * \param target 実際に編集するアイテム情報
 */
void MainWindow::startEditMode(GraphicsItemData target)
{
    cout << "startEditMode_B" << endl;
    int number = _selectedItemNumber;
    cancelAllMode();
    _selectedItemNumber = number;
    _isEditMode = true;
    _editModeItem = target;
    target.colorSelected();
    QPolygonF polygon = _editModeItem.item()->polygon();
    QVector<QPointF> _editPts;
    for(int i=0; i<polygon.size(); i++){
        QPointF pt = polygon.at(i);
        _editPts.push_back(pt);
        QGraphicsEllipseItem * circle =
                new QGraphicsEllipseItem(pt.x()-_editCircleSize/2,pt.y()-_editCircleSize/2
                                         ,_editCircleSize, _editCircleSize);
        circle->setPen(target.col.pen_selected);
        _editModeCornerList.push_back(circle);
        _scene.data()->addItem(circle);
    }
}

/*!
 * \brief 編集モードをキャンセルして終了する（確定されていない処理についてはキャンセル）
 * \brief MainWindow::cancelEditMode
 */
void MainWindow::cancelEditMode()
{
    if(_isEditMode){
        _editModeItem.colorDefault();
        if(_image.data()->isNull()) return;
        if(!_editModeCornerList.isEmpty()){
            for(int i=0; i<_editModeCornerList.size(); i++){
                _scene.data()->removeItem(_editModeCornerList.at(i));
                delete _editModeCornerList.at(i);
            }
            _editModeCornerList.clear();
        }
    }
    _isEditMode = false;
    _nowEditing = false;
}

/*!
 * \brief 編集モードでマウスをクリックされた際の動作
 * MainWindow::editModeMouseClick
 * \param pt
 */
void MainWindow::editModeMouseClick(QPoint pt)
{
    if(_image.data()->isNull()) return;
    if(!_isEditMode) return;
    QPolygonF polygon = _editModeItem.item()->polygon();
    double mindistance = calcDistance(QPointF(pt), polygon.at(0));
    int nearest = 0;
    for(int i=1; i<polygon.size();i++){
        double distance = calcDistance(QPointF(pt), polygon.at(i));
        if(mindistance > distance){
            mindistance = distance;
            nearest = i;
        }
    }

    if(mindistance < _editCircleSize){
        _nowEditing = true;
        _editCornerNumber = nearest;
    }
}

/*!
 * \brief 編集モードでマウスが移動した際の動作
 * \param pt マウスの座標
 */
void MainWindow::editModeMouseMove(QPoint pt)
{
    //! 処理開始
    //! 以下の場合には何もせず終了
    //! - 画像がセットされていない場合には何もせず終了
    if(_image.data()->isNull()) return;

    //! - モード違いの場合には何もせず終了
    if(!_isEditMode) return;

    //! - 編集中でない場合には何もせず終了
    if(!_nowEditing) return;

    //! - 設定されている値がおかしい場合には何もせず終了
    if(_editCornerNumber < 0 || _editCornerNumber > _editModeCornerList.size()) return;

    //! 編集中の点を現在のマウス座標に変更してRectに反映
    QGraphicsEllipseItem* corner = _editModeCornerList.at(_editCornerNumber);
    corner->setRect(pt.x()-_editCircleSize/2,pt.y()-_editCircleSize/2
                    ,_editCircleSize, _editCircleSize);

    //! 処理終了
}

/*!
 * \brief 編集モードでマウスクリックが解除された際の処理
 * MainWindow::editModeMouseRelease
 * \param pt
 */
void MainWindow::editModeMouseRelease(QPoint pt)
{

    //!編集中である場合には以下の処理を実行する
    if(_nowEditing){
        //! - 変更された形状を取得する
        _nowEditing = false;
        QPolygonF polygon = _editModeItem.item()->polygon();
        QPolygonF newPolygon;
        for(int i=0; i<polygon.size(); i++){
            if(i != _editCornerNumber){
                newPolygon.push_back(polygon.at(i));
            }
            else{
                newPolygon.push_back(pt);
            }
        }
        _editModeItem.setPolygon(newPolygon, _image.data()->width(), _image.data()->height());

        //! - 各メタデータに対して現在の形状を反映する
        switch(_selectTargetType){
        case ComicMetadata_Frame:
            editFramePolygon(_selectedItemNumber, newPolygon);
            break;
        case ComicMetadata_Character:
            editCharacterPolygon(_selectedItemNumber, newPolygon);
            break;
        case ComicMetadata_Dialog:
            editDialogPolygon(_selectedItemNumber, newPolygon);
            break;
        case ComicMetadata_Onomatopoeia:
            editOnomatopoeiaPolygon(_selectedItemNumber, newPolygon);
            break;
        case ComicMetadata_Item:
            editItemPolygon(_selectedItemNumber, newPolygon);
            break;
        }
    }
    //! 処理終了
}

/*!
 * \brief コマの形状を変更する処理
 * MainWindow::editFramePolygon
 * \param number
 * \param polygon
 */
void MainWindow::editFramePolygon(int number, QPolygonF polygon)
{
    if(number < 0 && number >= _metadata.frame.data()->size()){
        return;
    }
    FrameData buf = _metadata.frame.data()->at(number);
    buf.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    _metadata.frame.data()->replace(number, buf);
}

/*!
 * \brief 登場人物の形状を変更する処理
 * MainWindow::editCharacterPolygon
 * \param number
 * \param polygon
 */
void MainWindow::editCharacterPolygon(int number, QPolygonF polygon)
{
    if(number < 0 && number >= _metadata.character.data()->size()){
        return;
    }
    CharacterData buf = _metadata.character.data()->at(number);
    buf.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    _metadata.character.data()->replace(number, buf);
}

/*!
 * \brief セリフの形状を変更する処理
 * MainWindow::editDialogPolygon
 * \param number
 * \param polygon
 */
void MainWindow::editDialogPolygon(int number, QPolygonF polygon)
{
    if(number < 0 && number >= _metadata.dialog.data()->size()){
        return;
    }
    DialogData buf = _metadata.dialog.data()->at(number);
    buf.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    _metadata.dialog.data()->replace(number, buf);
}

/*!
 * \brief オノマトペの形状を変更する処理
 *  MainWindow::editOnomatopoeiaPolygon
 * \param number
 * \param polygon
 */
void MainWindow::editOnomatopoeiaPolygon(int number, QPolygonF polygon)
{
    if(number < 0 && number >= _metadata.onomatopoeia.data()->size()){
        return;
    }
    OnomatopoeiaData buf = _metadata.onomatopoeia.data()->at(number);
    buf.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    _metadata.onomatopoeia.data()->replace(number, buf);
}

/*!
 * \brief アイテムの形状を変更する処理
 *  MainWindow::editItemPolygon
 * \param number
 * \param polygon
 */
void MainWindow::editItemPolygon(int number, QPolygonF polygon)
{
    if(number < 0 && number >= _metadata.item.data()->size()){
        return;
    }
    ItemData buf = _metadata.item.data()->at(number);
    buf.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    _metadata.item.data()->replace(number, buf);
}


/*!
 * \brief SelectModeの開始処理
 * MainWindow::startSelectMode
 * \param type
 */
void MainWindow::startSelectMode(ComicMetadataType type)
{
    //! 処理開始

    //! 一旦すべてのモードをキャンセル
    cancelAllMode();

    //! 全部のデータを隠す
    hideAll();

    _isSelectMode = true;
    _selectTargetType = type;
    GraphicsItemData GIData;

    //選択されたメタデータタイプについて、selectTargetにストックし、表示をActiveに変更
    switch(type){
    case ComicMetadata_Frame:
        for(int i=0; i<_metadata.frame.data()->size(); i++){
            GIData = _metadata.frame.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_Character:
        for(int i=0; i<_metadata.character.data()->size(); i++){
            GIData = _metadata.character.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_Dialog:
        for(int i=0; i<_metadata.dialog.data()->size(); i++){
            GIData = _metadata.dialog.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_Onomatopoeia:
        for(int i=0; i<_metadata.onomatopoeia.data()->size(); i++){
            GIData = _metadata.onomatopoeia.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_Item:
        for(int i=0; i<_metadata.item.data()->size(); i++){
            GIData = _metadata.item.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        break;
    case ComicMetadata_All:
        /*
        for(int i=0; i<_metadata.frame.data()->size(); i++){
            GIData = _metadata.frame.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        for(int i=0; i<_metadata.character.data()->size(); i++){
            GIData = _metadata.character.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        for(int i=0; i<_metadata.dialog.data()->size(); i++){
            GIData = _metadata.dialog.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        for(int i=0; i<_metadata.onomatopoeia.data()->size(); i++){
            GIData = _metadata.onomatopoeia.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        for(int i=0; i<_metadata.item.data()->size(); i++){
            GIData = _metadata.item.data()->at(i).GIData;
            GIData.colorActive();
            _selectTarget.push_back(GIData);
        }
        */
        break;
    default:
        break;
    }

    //! 各ポリゴンの面積を計算しておく
    for(int i=0; i<_selectTarget.size(); i++){
        GraphicsItemData GIData = _selectTarget.at(i);
        QPolygonF polygon = GIData.item()->polygon();
        _selectItemPolygonSizeList.push_back(calcPolygonAreaSize(polygon));
    }

    //! 処理終了
}


/*!
 * \brief 全てのメタデータタイプについて、表示をhiddenに変更する
 *  MainWindow::hideAll
 */
void MainWindow::hideAll()
{
    GraphicsItemData GIData;
    for(int i=0; i<_metadata.frame.data()->size(); i++){
        GIData = _metadata.frame.data()->at(i).GIData;
        GIData.colorHidden();
    }
    for(int i=0; i<_metadata.character.data()->size(); i++){
        GIData = _metadata.character.data()->at(i).GIData;
        GIData.colorHidden();
    }
    for(int i=0; i<_metadata.dialog.data()->size(); i++){
        GIData = _metadata.dialog.data()->at(i).GIData;
        GIData.colorHidden();
    }
    for(int i=0; i<_metadata.onomatopoeia.data()->size(); i++){
        GIData = _metadata.onomatopoeia.data()->at(i).GIData;
        GIData.colorHidden();
    }
    for(int i=0; i<_metadata.item.data()->size(); i++){
        GIData = _metadata.item.data()->at(i).GIData;
        GIData.colorHidden();
    }
}

/*!
 * \brief すべてのメタデータタイプの表示をDefaultに変更する
 *  MainWindow::showAll
 */
void MainWindow::showAll()
{
    GraphicsItemData GIData;
    for(int i=0; i<_metadata.frame.data()->size(); i++){
        GIData = _metadata.frame.data()->at(i).GIData;
        GIData.colorDefault();
    }
    for(int i=0; i<_metadata.character.data()->size(); i++){
        GIData = _metadata.character.data()->at(i).GIData;
        GIData.colorDefault();
    }
    for(int i=0; i<_metadata.dialog.data()->size(); i++){
        GIData = _metadata.dialog.data()->at(i).GIData;
        GIData.colorDefault();
    }
    for(int i=0; i<_metadata.onomatopoeia.data()->size(); i++){
        GIData = _metadata.onomatopoeia.data()->at(i).GIData;
        GIData.colorDefault();
    }
    for(int i=0; i<_metadata.item.data()->size(); i++){
        GIData = _metadata.item.data()->at(i).GIData;
        GIData.colorDefault();
    }
}

/*!
 * \brief マウス座標に応じて_selectTargetに入っているデータのうち、選択状態にするインデックスを取得する
 *  MainWindow::selectItem
 * \param mouse
 * \return 選択状態にするアイテムのインデックス
 */
int MainWindow::selectItem(QPoint mouse)
{
    //! 処理開始
    int nearestNumber = -1;
    QList<int> onList;

    //! 現在のマウス座標を内包するポリゴンのリストを取得する
    for(int i=0; i<_selectTarget.size(); i++){
        GraphicsItemData GIData = _selectTarget.at(i);
        QPolygonF polygon = GIData.item()->polygon();
        if(polygon.containsPoint(mouse, Qt::WindingFill)){
            onList.push_back(i);
        }
    }

    //! 現在のマウス座標を内包するポリゴンが複数ある場合に、どれが選択状態となるかを決定する（小さいほう優先）
    if(onList.size() >= 2){
        double minArea = _selectItemPolygonSizeList.at(onList.at(0));
        nearestNumber = onList.at(0);
        for(int i = 1; i<onList.size(); i++){
            double areaSize = _selectItemPolygonSizeList.at(onList.at(i));
            if(minArea > areaSize){
                minArea = areaSize;
                nearestNumber = onList.at(i);
            }
        }
    }
    else if(!onList.isEmpty()){
        nearestNumber = onList.at(0);
    }
    //! 結果を返す
    return nearestNumber;

    //! 処理終了
}

/*!
 * \brief selectItemのターゲット指定付き関数
 * MainWindow::selectItem
 * \param target 対象となるデータ
 * \param sizeList 対象領域の面積リスト
 * \param mouse マウス座標
 * \param ignore　無視するインデックスのリスト
 * \return 選択状態にするアイテムのインデックス
 */
int MainWindow::selectItem(QVector<GraphicsItemData>& target,
                           QVector<double> &sizeList, QPoint mouse, QVector<int> &ignore)
{
    int nearestNumber = -1;
    QList<int> onList;
    for(int i=0; i<target.size(); i++){
        GraphicsItemData GIData = target.at(i);
        QPolygonF polygon = GIData.item()->polygon();
        bool status = true;
        //今回の処理対象が無視リストに入っている場合にはonListへの追加処理を行わない
        for(int j=0; j<ignore.size(); j++){
            if(i == ignore.at(j)){
                status = false;
            }
        }
        if(status && polygon.containsPoint(mouse, Qt::WindingFill)){
            onList.push_back(i);
        }
    }

    if(onList.size() >= 2){
        double minArea = sizeList.at(onList.at(0));
        nearestNumber = onList.at(0);
        for(int i = 1; i<onList.size(); i++){
            double areaSize = sizeList.at(onList.at(i));
            if(minArea > areaSize){
                minArea = areaSize;
                nearestNumber = onList.at(i);
            }
        }
    }
    else if(!onList.isEmpty()){
        nearestNumber = onList.at(0);
    }
    return nearestNumber;
}

/*!
 * \brief 選択モードでマウスが移動した際の動作
 * MainWindow::selectModeMouseMove
 * \param pt
 */
void MainWindow::selectModeMouseMove(QPoint pt)
{
    if(_selectLock) return;//ロックがかかっている場合何もしない

    //int selected = selectItem(pt);
    int selected = selectItem(pt);
    if(selected != _selectedItemNumber){
        selectModeSelect(selected);
    }
}

/*!
 * \brief 選択モードでマウスがクリックされた際の動作
 *  MainWindow::selectModeMouseClick
 * \param pt
 */
void MainWindow::selectModeMouseClick(QPoint pt)
{

    //!すでにlockされている場合にはeditmodeへ移行する
    if(_selectLock && _selectedItemNumber == selectItem(pt)){
        startEditMode(_selectTarget.at(_selectedItemNumber));
    }
    else if(_selectedItemNumber >= 0){
        //! 選択されたアイテムをlock状態に移行する
        selectModeLock();
    }
}

/*!
 * \brief 選択されたアイテムをlock状態に移行する
 * MainWindow::selectModeLock
 * \note lock状態になったものに対して編集等の処理が行われることになる
 */
void MainWindow::selectModeLock()
{

    switch (_selectTargetType) {
    case ComicMetadata_Frame:
        specifyFrame(_selectedItemNumber);
        break;
    case ComicMetadata_Character:
        specifyCharacter(_selectedItemNumber);
        break;
    case ComicMetadata_Dialog:
        specifyDialog(_selectedItemNumber);
        break;
    case ComicMetadata_Onomatopoeia:
        specifyOnomatopoeia(_selectedItemNumber);
        break;
    case ComicMetadata_Item:
        specifyItem(_selectedItemNumber);
        break;
        //ユーザ定義の場合は未作成
    default:
        break;
    }
}

/*!
 * \brief 選択モードでロック状態を解除する
 *  MainWindow::selectModeLockRelease
 */
void MainWindow::selectModeLockRelease()
{
    _selectLock = false;
    int selected = selectItem(_gv.data()->_mpoint_I);
    selectModeSelect(selected);
}

/*!
 * \brief 選択モードで右クリックされた際の動作 （キャンセル系の処理）
 *  MainWindow::selectModeRightClick
 */
void MainWindow::selectModeRightClick(){
    if(_selectLock){//すでに選択状態であればそれを解除する
        selectModeLockRelease();
    }
    else{//何も選択されていない状態であれば、selectモードを抜ける
        selectModeCancel();
    }
}

/*!
 * \brief 選択モードで、実際に選択が行われた場合の処理
 * MainWindow::selectModeSelect
 * \param selectNumber
 */
void MainWindow::selectModeSelect(int selectNumber)
{
    //! 処理開始
    //! すでに選択されている場合で異なるインデックスが指定された場合選択を解除する
    if(_selectedItemNumber != selectNumber){
        selectModeRelease();
    }

    //! 選択処理
    //! 選択された番号の取得
    _selectedItemNumber = selectNumber;
    if(_selectedItemNumber >= 0){
        //! 表示色の変更
        GraphicsItemData GIData = _selectTarget.at(_selectedItemNumber);
        GIData.colorBrushSelected();
    }
    //! 処理終了
}

/*!
 * \brief 選択モードで、選択解除時の処理
 *  MainWindow::selectModeRelease
 */
void MainWindow::selectModeRelease()
{
    //release selected color
    if(_selectedItemNumber >= 0){
        GraphicsItemData prevItemData = _selectTarget.at(_selectedItemNumber);
        prevItemData.colorActive();
        _selectLock = false;
        _selectedItemNumber = -1;
        releaseSpecificItems();
    }
}

/*!
 * \brief 選択モード時の、アイテム削除処理
 *  MainWindow::selectModeDelete
 */
void MainWindow::selectModeDelete()
{
    cout << "selectModeDelete" << endl;
    if(!_selectLock) return;
    _selectLock = false;
    //GraphicsItemData GIData;
    //!現在編集されているデータごとにデータの削除処理を行う
    switch(_selectTargetType){
    case ComicMetadata_Frame:
        removeFrame(_currentFrameNumber);
        return;
        break;
    case ComicMetadata_Character:
        removeCharacter(_currentCharacterNumber);
        break;
    case ComicMetadata_Dialog:
        removeDialog(_currentDialogNumber);
        break;
    case ComicMetadata_Onomatopoeia:
        removeOnomatopoeia(_currentOnomatopoeiaNumber);
        break;
    case ComicMetadata_Item:
        removeCItem(_currentCItemNumber);
    case ComicMetadata_All:
        break;
    default:
        break;
    }
}

/*!
 * \brief コマデータの削除処理
 *  MainWindow::removeFrame
 * \param number 削除するコマデータのインデックス
 */
void MainWindow::removeFrame(int number)
{
    //! 処理開始
    //! 範囲外が指定された場合には何もせず終了
    if(number < 0) return;
    if(number > _metadata.frame.data()->size()) return;

    //! 削除処理
    GraphicsItemData GIData;
    GIData = _metadata.frame.data()->at(number).GIData;
    _scene.data()->removeItem(GIData.item());
    _metadata.deleteItem(ComicMetadata_Frame, number);//消去時には一つずらす

    //! コマのリストを最新の状態に変更する
    refresh_Frame_ListWidget();

    //! 選択状態となるコマを変更する
    if(number == 1 && !_metadata.frame.data()->isEmpty()){
        specifyFrame(1);
    }
    else if(_metadata.frame.data()->isEmpty()){
        specifyFrame(-1);
    }
    else{
        specifyFrame(number -1);
    }

    //! 処理終了
}

/*!
 * \brief 登場人物データの削除処理
 * MainWindow::removeCharacter
 * \param number
 */
void MainWindow::removeCharacter(int number)
{
    //! 処理開始
    //! 範囲外が指定された場合には何もせず終了
    if(number < 0) return;
    if(number > _metadata.character.data()->size()) return;

    //! 削除処理
    GraphicsItemData GIData;
    GIData = _metadata.character.data()->at(number).GIData;
    _scene.data()->removeItem(GIData.item());
    _metadata.deleteItem(ComicMetadata_Character, number);

    //! 登場人物のリストを最新の状態に変更する
    refresh_Character_ListWidget();

    //! 選択状態となる登場人物を変更する
    if(number == 0 && !_metadata.character.data()->isEmpty()){
        specifyCharacter(0);
    }
    else{
        specifyCharacter(number -1);
    }
    //! 処理終了
}

/*!
 * \brief セリフデータの削除処理
 *  MainWindow::removeDialog
 * \param number
 */
void MainWindow::removeDialog(int number)
{
    //! 処理開始
    //! 範囲外が指定された場合には何もせず終了
    if(number < 0) return;
    if(number > _metadata.dialog.data()->size()) return;

    //! 削除処理
    GraphicsItemData GIData;
    GIData = _metadata.dialog.data()->at(number).GIData;
    _scene.data()->removeItem(GIData.item());
    _metadata.deleteItem(ComicMetadata_Dialog, number);

    //! セリフのリストを最新の状態に変更する
    refresh_Dialog_ListWidget();

    //! 選択状態となるセリフを変更する
    if(number == 0 && !_metadata.dialog.data()->isEmpty()){
        specifyDialog(0);
    }
    else{
        specifyDialog(number -1);
    }
    //! 処理終了
}

/*!
 * \brief オノマトペデータの削除処理
 * MainWindow::removeOnomatopoeia
 * \param number
 */
void MainWindow::removeOnomatopoeia(int number)
{
    //! 処理開始
    //! 範囲外が指定された場合には何もせず終了
    if(number < 0) return;
    if(number > _metadata.onomatopoeia.data()->size()) return;

    //! 削除処理
    GraphicsItemData GIData;
    GIData = _metadata.onomatopoeia.data()->at(number).GIData;
    _scene.data()->removeItem(GIData.item());
    _metadata.deleteItem(ComicMetadata_Onomatopoeia, number);

    //! オノマトペのリストを最新の状態に変更する
    refresh_Onomatopoeia_ListWidget();

    //! 選択状態となるオノマトペを変更する
    if(number == 0 && !_metadata.onomatopoeia.data()->isEmpty()){
        specifyOnomatopoeia(0);
    }
    else{
        specifyOnomatopoeia(number -1);
    }

    //! 処理終了
}

/*!
 * \brief アイテムデータの削除処理
 *  MainWindow::removeCItem
 * \param number
 */
void MainWindow::removeCItem(int number)
{
    //! 処理開始
    //! 範囲外が指定された場合には何もせず終了
    if(number < 0) return;
    if(number > _metadata.item.data()->size()) return;

    //! 削除処理
    GraphicsItemData GIData;
    GIData = _metadata.item.data()->at(number).GIData;
    _scene.data()->removeItem(GIData.item());
    _metadata.deleteItem(ComicMetadata_Item, number);

    //! アイテムのリストを最新の状態に変更する
    refresh_Item_ListWidget();

    //! 選択状態となるアイテムを変更する
    if(number == 0 && !_metadata.item.data()->isEmpty()){
        specifyItem(0);
    }
    else{
        specifyItem(number -1);
    }
}

/*!
 * \brief 選択モードを抜ける
 * MainWindow::selectModeCancel
 */
void MainWindow::selectModeCancel()
{
    _isSelectMode = false;
    _selectTarget.clear();
    _selectItemPolygonSizeList.clear();
    _selectedItemNumber = -1;
    _selectLock = false;

    //とりあえずキャンセル部分を以下の様にセットしておく
    releaseSpecificItems();

    showAll();
}

/*!
 * \brief コマ選択ボタンをクリックされた際の動作
 *  MainWindow::on_Select_Frame_clicked
 */
void MainWindow::on_Select_Frame_clicked()
{
    startSelectMode(ComicMetadata_Frame);
}

/*!
 * \brief メイン画面右側のfunctionTabが切り替えられた際の動作
 *  MainWindow::on_functionTab_currentChanged
 * \param index
 */
void MainWindow::on_functionTab_currentChanged(int index)
{
    //!　すべてのモードをキャンセルする
    cancelAllMode();

    //! 選択されているアイテムがある場合には開放する
    releaseSpecificItems();

    //! モード名を変更する
    switch(index){
    case 0:
        _currentTabType = MainWindowTab_Info;
//        cout << "Info" << endl;
        break;
    case 1:
        _currentTabType = MainWindowTab_Frame;
//        cout << "Frame" << endl;
        break;
    case 2:
        _currentTabType = MainWindowTab_Character;
//        cout << "Character" << endl;
        break;
    case 3:
        _currentTabType = MainWindowTab_Dialog;
//        cout << "Dialog" << endl;
        break;
    case 4:
        _currentTabType = MainWindowTab_Onomatopoeia;
//        cout << "Onomatopoeia" << endl;
        break;
    case 5:
        _currentTabType = MainWindowTab_Item;
//        cout << "Item" << endl;
        break;
    default:
        _currentTabType = MainWindowTab_Viewer;
        break;
    }
}

////////////////add item///////////////////

/*!
 * \brief コマを追加する処理
 * MainWindow::addFrame
 * \param polygon ポリゴン情報
 * \param mangaPath マンガパス
 * \param sceneBoundery シーン切り替え情報
 */
void MainWindow::addFrame
(QPolygonF polygon, QString mangaPath, bool sceneBoundery)
{
    //! 新規エントリ作成とセット
    FrameData newframe;
    newframe.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    newframe.GIData.colorDefault();
    newframe.mangaPath = mangaPath;
    newframe.sceneBoundary = sceneBoundery;
    _scene.data()->addItem(newframe.GIData.item());
    _metadata.frame.data()->push_back(newframe);

    int size = _metadata.frame.data()->size();
    QString itemName(QString("ID:%1 Frame").arg(size));
    ui->listWidget_Frame->addItem(itemName);

    //!表示情報を最新の状態に変更する
    refresh_Frame_ListWidget();
    _metadata.renewMangaPath_Frame(_metadata.frame.data()->size()-1);
}

/*!
 * \brief 登場人物の追加処理
 * MainWindow::addCharacter
 * \param polygon ポリゴン情報
 * \param mangaPath マンガパス
 * \param characterName 登場人物名
 * \param characterID 登場人物ID
 * \param targetFrame 対応するコマのインデックス
 */
void MainWindow::addCharacter
(QPolygonF polygon, QString mangaPath,
 QString characterName, int characterID, int targetFrame)
{
    //! 新規エントリ作成とセット
    CharacterData newCharacter;
    newCharacter.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    newCharacter.GIData.colorDefault();
    newCharacter.mangaPath = mangaPath;
    newCharacter.characterName = characterName;
    newCharacter.characterID = characterID;
    newCharacter.targetFrame = targetFrame;
    _scene.data()->addItem(newCharacter.GIData.item());
    _metadata.character.data()->push_back(newCharacter);

    //コメントアウトの理由忘却 FIXME!
    //あとでcreate character name という様な関数を作って統一したい FIXME!
    //int size = _metadata.character.data()->size();
    //QString itemName(QString("%1: ID:%2 %3").arg(size).arg(0, 3, 10, QChar('0')).arg(_metadata.characterName.at(0)));
    //ui->ListWidget_Character->addItem(itemName);

    //!表示情報を最新の状態に変更する
    _metadata.renewMangaPath_Character(_metadata.character.data()->size()-1);
    refresh_Character_ListWidget();
}

/*!
 * \brief セリフの追加処理
 * MainWindow::addDialog
 * \param polygon ポリゴン情報
 * \param mangaPath マンガパス
 * \param text セリフ内容
 * \param fontsize フォントサイズ
 * \param narration ナレーションかどうかのフラグ
 * \param targetCharacterID 対応する登場人物ID
 * \param targetFrame 対応するコマのインデックス
 * \param characterName 対応する登場人物名
 */
void MainWindow::addDialog
(QPolygonF polygon, QString mangaPath, QString text,
 int fontsize, bool narration, int targetCharacterID,
 int targetFrame, QString characterName)
{
    //! 新規エントリ作成とセット
    DialogData newDialog;
    newDialog.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    newDialog.mangaPath = mangaPath;
    newDialog.GIData.colorDefault();
    newDialog.fontSize = fontsize;
    newDialog.narration = narration;
    newDialog.targetCharacterID = targetCharacterID;
    newDialog.characterName = characterName;
    newDialog.targetFrame = targetFrame;
    newDialog.text = text;
    _scene.data()->addItem(newDialog.GIData.item());
    _metadata.dialog.data()->push_back(newDialog);

    //!表示情報を最新の状態に変更する
    _metadata.renewMangaPath_Dialog(_metadata.dialog.data()->size()-1);
    refresh_Dialog_ListWidget();
}

/*!
 * \brief オノマトペの追加処理
 * MainWindow::addOnomatopoeia
 * \param polygon ポリゴン情報
 * \param mangaPath マンガパス
 * \param text オノマトペの内容
 * \param fontsize フォントサイズ
 * \param targetFrame 対応するコマのインデックス
 */
void MainWindow::addOnomatopoeia
(QPolygonF polygon, QString mangaPath, QString text,
 int fontsize, bool targetFrame)
{
    //! 新規エントリ作成とセット
    OnomatopoeiaData newOnomatopoeia;
    newOnomatopoeia.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    newOnomatopoeia.mangaPath = mangaPath;
    newOnomatopoeia.GIData.colorDefault();
    newOnomatopoeia.fontSize = fontsize;
    newOnomatopoeia.targetFrame = targetFrame;
    newOnomatopoeia.text = text;
    _scene.data()->addItem(newOnomatopoeia.GIData.item());
    _metadata.onomatopoeia.data()->push_back(newOnomatopoeia);

    //コメントアウトの理由忘却 FIXME!
    //int size = _metadata.onomatopoeia.data()->size();
    //QString itemName(QString("onomatopoeia%1").arg(size));
    //ui->ListWidget_Onomatopoeia->addItem(itemName);

    //!表示情報を最新の状態に変更する
    _metadata.renewMangaPath_Onomatopoeia(_metadata.onomatopoeia.data()->size()-1);
    refresh_Onomatopoeia_ListWidget();
}

/*!
 * \brief アイテムの追加処理
 *  MainWindow::addItem
 * \param polygon ポリゴン情報
 * \param mangaPath マンガパス
 * \param itemClass アイテムの属性
 * \param description　記述内容
 * \param targetFrame 対応するコマのインデックス
 */
void MainWindow::addItem
(QPolygonF polygon, QString mangaPath, QString itemClass,
 QString description, int targetFrame)
{
    //! 新規エントリ作成とセット
    ItemData newItem;
    newItem.GIData.setPolygon(polygon, _image.data()->width(), _image.data()->height());
    newItem.GIData.colorDefault();
    newItem.mangaPath = mangaPath;
    newItem.itemClass = itemClass;
    newItem.description = description;
    newItem.targetFrame = targetFrame;
    _scene.data()->addItem(newItem.GIData.item());
    _metadata.item.data()->push_back(newItem);

    //コメントアウトの理由忘却 FIXME!
    //int size = _metadata.item.data()->size();
    //QString itemName(QString("%1: Item").arg(size, 3, 10, QChar('0')));
    //ui->ListWidget_CItem->addItem(itemName);

    //!表示情報を最新の状態に変更する
    _metadata.renewMangaPath_Item(_metadata.item.data()->size()-1);
    refresh_Item_ListWidget();
}

/*!
 * \brief 順番表示関数（メタデータタイプの指定）
 *  MainWindow::showOrder
 * \param type 順番表示するメタデータタイプ
 */
void MainWindow::showOrder(ComicMetadataType type)
{
    QVector<GraphicsItemData> GIData;
    switch(type){
    case ComicMetadata_Frame:
        for(int i=0; i<_metadata.frame.data()->size(); i++){
            GIData.push_back(_metadata.frame.data()->at(i).GIData);
        }
        break;
    case ComicMetadata_Character:
        for(int i=0; i<_metadata.character.data()->size(); i++){
            GIData.push_back(_metadata.character.data()->at(i).GIData);
        }
        break;
    case ComicMetadata_Dialog:
        for(int i=0; i<_metadata.dialog.data()->size(); i++){
            GIData.push_back(_metadata.dialog.data()->at(i).GIData);
        }
        break;
    case ComicMetadata_Onomatopoeia:
        for(int i=0; i<_metadata.onomatopoeia.data()->size(); i++){
            GIData.push_back(_metadata.onomatopoeia.data()->at(i).GIData);
        }
        break;
    case ComicMetadata_Item:
        for(int i=0; i<_metadata.item.data()->size(); i++){
            GIData.push_back(_metadata.item.data()->at(i).GIData);
        }
        break;
    default:
        return;
        break;
    }

    showOrder(GIData);
}

/*!
 * \brief 番号表示関数（渡されたデータ列に対して順番を表示する）
 *  MainWindow::showOrder
 * \param GIData 表示するデータ列
 */
void MainWindow::showOrder(QVector<GraphicsItemData> GIData)
{
    //! 処理開始

    //! すでに順番表示モードであった場合には、番号表示モードを抜ける
    if(_isShowOrderMode){
        showOrderCancel();
        return;
    }

    //! 全モードをキャンセルする
    cancelAllMode();

    //! 渡されたデータが空であった場合何もせず終了する
    if(GIData.isEmpty()){
        return;
    }

    //! 渡されたデータ全体について番号を表示する
    for(int i=0; i<GIData.size(); i++){
        //! - 表示する番号を算出（１から始まる）
        int number = i+1;

        //! - 数字の表示場所を計算する
        QPolygonF polygon = GIData.at(i)._item->polygon();
        QPointF center = getPolygonCenter(polygon);
        center.setX(center.x() - 30);
        center.setY(center.y() - 30);
        int digit = (int)log10(i) + 1;

        //! - 数字の背景用矩形を生成する
        QGraphicsRectItem *el = new QGraphicsRectItem;
        el->setRect(center.x()-20, center.y()-10, 80+20*(digit-1), 80);
        el->setBrush(QBrush(QColor(255,255,255,150)));
        el->setPen(QPen(QBrush(QColor(200,200,200,255)),5));
        //! - 数字の背景を表示する
        _scene->addItem(el);

        //! - 番号を生成する
        QGraphicsTextItem *io = new QGraphicsTextItem;
        io->setPos(center);
        io->setPlainText(QString("%1").arg(number));
        io->setFont(QFont("Helvetica", 40));
        //! - 番号を表示する
        _scene->addItem(io);

        //! - 番号とその背景をそれぞれ表示中リストに加える
        _showNumberRect.push_back(el);
        _showNumberText.push_back(io);
    }

    //! 番号表示モードのフラグをたてる
    _isShowOrderMode = true;

    //! 処理終了
}

/*!
 * \brief 番号表示モードを終了する
 *  MainWindow::showOrderCancel
 * \note 本関数内での消去は、clearの前に対象をdeleteしておいたほうが良い可能性があります。\n
 * removeItem時に自動消去される?　かもしれないため、要チェック。
 */
void MainWindow::showOrderCancel()
{
    //! 番号表示モードのフラグをOffにする
    _isShowOrderMode = false;

    //! 番号表示モードで作成したアイテム群を除去する
    for(int i=0; i<_showNumberRect.size(); i++){
        _scene.data()->removeItem(_showNumberRect.at(i));
    }
    _showNumberRect.clear();
    for(int i=0; i<_showNumberText.size(); i++){
        _scene.data()->removeItem(_showNumberText.at(i));
    }
    _showNumberText.clear();
    //! 処理終了
}

///////////////Frame//////////////////

/*!
 * \brief FrameタブにあるSetOrderボタンがクリックされたときの動作\n
 * コマの順番を変更する
 * MainWindow::on_SetOrder_Frame_clicked
 */
void MainWindow::on_SetOrder_Frame_clicked()
{
    setOrderModeStart(ComicMetadata_Frame);
}

/*!
 * \brief FrameタブにあるShowOrderボタンがクリックされたときの動作\n
 * コマの順番を表示する
 * MainWindow::on_ShowOrder_Frame_clicked
 */
void MainWindow::on_ShowOrder_Frame_clicked()
{
    showOrder(ComicMetadata_Frame);
}

////////////////Character///////////////////

/*!
 * \brief CharacterタブのSetOrderボタンがクリックされたときの動作(Disable)
 * \brief MainWindow::on_SetOrder_Character_clicked
 */
void MainWindow::on_SetOrder_Character_clicked()
{
    setOrderModeStart(ComicMetadata_Character);
}

/*!
 * \brief CharacterタブのShowOrderボタンがクリックされたときの動作(Disable)
 * \brief MainWindow::on_ShowOrder_Character_clicked
 */
void MainWindow::on_ShowOrder_Character_clicked()
{
    showOrder(ComicMetadata_Character);
}

/*!
 * \brief CharacterタブのSelectItemボタンがクリックされたときの動作
 * \brief MainWindow::on_Select_Character_clicked
 */
void MainWindow::on_Select_Character_clicked()
{
    startSelectMode(ComicMetadata_Character);
}

/*!
 * \brief Characterタブのリストで選択されているアイテムが変更されたときの動作
 * \brief MainWindow::on_ListWidget_Character_currentItemChanged
 * \param current
 * \param previous
 */
void MainWindow::on_ListWidget_Character_currentItemChanged
    (QListWidgetItem *current, QListWidgetItem *previous)
{
    //! 指定アイテムを選択状態に変更
    specifyCharacter(ui->ListWidget_Character->currentRow());
}


////////////////Dialog////////////////////

/*!
 * \brief DialogタブのSetOrderボタンがクリックされたときの動作(Disable)
 * \brief MainWindow::on_SetOrder_Dialog_clicked
 */
void MainWindow::on_SetOrder_Dialog_clicked()
{
    setOrderModeStart(ComicMetadata_Dialog);
}

/*!
 * \brief DialogタブのShowOrderボタンがクリックされたときの動作(Disable)
 * \brief MainWindow::on_ShowOrder_Dialog_clicked
 */
void MainWindow::on_ShowOrder_Dialog_clicked()
{
    showOrder(ComicMetadata_Dialog);
}

/*!
 * \brief DialogタブのSelectItemボタンがクリックされたときの動作
 * \brief MainWindow::on_Select_Dialog_clicked
 */
void MainWindow::on_Select_Dialog_clicked()
{
    startSelectMode(ComicMetadata_Dialog);
}

/*!
 * \brief Dialogタブのリストで選択されているアイテムが変更されたときの動作
 * \brief MainWindow::on_ListWidget_Dialog_currentItemChanged
 * \param current
 * \param previous
 */
void MainWindow::on_ListWidget_Dialog_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    //! 指定アイテムを選択状態に変更
    specifyDialog(ui->ListWidget_Dialog->currentRow());
}

/*!
 * \brief DialogタブのTextで文字編集された際の動作
 * \brief MainWindow::on_TextEdit_Dialog_textChanged
 */
void MainWindow::on_TextEdit_Dialog_textChanged()
{
    //! 選択されているDialogアイテムの番号が正しいかチェック
    if(_currentDialogNumber < 0 || _metadata.dialog.data()->size() < _currentDialogNumber){
        return;
    }
    //! 新しいデータを作成し、選択されているデータと入れ替える
    DialogData newData = _metadata.dialog.data()->at(_currentDialogNumber);
    newData.text = ui->TextEdit_Dialog->document()->toPlainText();
    _metadata.dialog.data()->replace(_currentDialogNumber, newData);

//    refresh_Dialog_ListWidget(_currentDialogNumber);
}


////////////////Onomatopoeia//////////////////
/*!
 * \brief OnomatopoeiaタブのSetOrderボタンがクリックされたときの動作(Disable)
 * \brief MainWindow::on_SetOrder_Onomatopoeia_clicked
 */
void MainWindow::on_SetOrder_Onomatopoeia_clicked()
{
    setOrderModeStart(ComicMetadata_Onomatopoeia);
}
/*!
 * \brief OnomatopoeiaタブのShowOrderボタンがクリックされたときの動作(Disable)
 * \brief MainWindow::on_ShowOrder_Onomatopoeia_clicked
 */
void MainWindow::on_ShowOrder_Onomatopoeia_clicked()
{
    showOrder(ComicMetadata_Onomatopoeia);
}

/*!
 * \brief OnomatopoeiaタブのSelectItemボタンがクリックされたときの動作
 * \brief MainWindow::on_Select_Onomatopoeia_clicked
 */
void MainWindow::on_Select_Onomatopoeia_clicked()
{
    startSelectMode(ComicMetadata_Onomatopoeia);
}

/*!
 * \brief OnomatopoeiaタブのTextで文字編集された際の動作
 * \brief MainWindow::on_TextEdit_Onomatopoeia_textChanged
 */
void MainWindow::on_TextEdit_Onomatopoeia_textChanged()
{
    if(_currentOnomatopoeiaNumber < 0
            || _currentOnomatopoeiaNumber >= _metadata.onomatopoeia.data()->size()){
        return;
    }
    _currentOnomatopoeia.text = ui->TextEdit_Onomatopoeia->document()->toPlainText();
    _metadata.onomatopoeia.data()->replace(_currentOnomatopoeiaNumber, _currentOnomatopoeia);
//    refresh_Onomatopoeia_ListWidget(_currentOnomatopoeiaNumber);
}

/*!
 * \brief numberのコマを選択状態にする
 * \brief MainWindow::specifyFrame
 * \param number
 */
void MainWindow::specifyFrame(int number)
{
    if(!_isRefreshingNow){
        refresh_Frame_ListWidget();
    }

    if(number == _currentFrameNumber) return;

    if(_metadata.frame.data()->isEmpty() || number < 0){
        clearFrameUI();
        return;
    }

    else if(number >= _metadata.frame.data()->size()){
        number = 0;
    }
    startSelectMode(ComicMetadata_Frame);

    GraphicsItemData GIData = _metadata.frame.data()->at(number).GIData;

    //! 選択確定状態にする
    _selectLock = true;

    //! アイテムの色を選択状態に変更する
    GIData.colorSelected();

    ui->Delete_Frame->setEnabled(true);
    ui->Frame_MangaPath->setText(_metadata.frame.data()->at(number).mangaPath);
    ui->Frame_SceneChange_CheckBox->setChecked(_metadata.frame.data()->at(number).sceneBoundary);
    ui->Frame_SceneChange_CheckBox->setEnabled(true);
    _currentFrameNumber = number;
    _selectedItemNumber = number;
}

/*!
 * \brief 選択されたframeに関連して表示されるUI上の情報をクリアする
 * \brief MainWindow::clearFrameUI
 */
void MainWindow::clearFrameUI()
{
    ui->Frame_MangaPath->clear();
    _currentFrameNumber = -1;
    ui->Delete_Frame->setEnabled(false);
    ui->Frame_SceneChange_CheckBox->setChecked(false);
    ui->Frame_SceneChange_CheckBox->setEnabled(false);
}

/*!
 * \brief numberで指定されたcharacterを選択状態にする
 * \brief MainWindow::specifyCharacter
 * \param number
 */
void MainWindow::specifyCharacter(int number)
{
    if(!_isRefreshingNow && !_isSpecifyed){
        //refresh_Character_ListWidget();
    }
    if(_isSpecifyed){
        return;
    }
    _isSpecifyed = true;

    if(_metadata.character.data()->isEmpty() || number < 0){
        clearCharacterUI();
        _isSpecifyed = false;
        return;
    }

    if(number != ui->ListWidget_Character->currentRow()){
        ui->ListWidget_Character->setCurrentRow(number);
        _isSpecifyed = false;
        return;
    }
    if(number >= _metadata.character.data()->size()){
        number = 0;
    }
    startSelectMode(ComicMetadata_Character);
    GraphicsItemData GIData = _metadata.character.data()->at(number).GIData;

    //! アイテムの色を選択状態に変更する
    GIData.colorSelected();

    //! 選択確定状態にする
    _selectLock = true;

    //! UIの内容を選択されたものに変更
    _selectedItemNumber = number;
    _currentCharacterNumber = number;
    ui->Character_DeleteButton->setEnabled(true);
    ui->Character_ComboBox->setCurrentIndex(_metadata.character.data()->at(number).characterID);
    ui->Character_ComboBox->setEnabled(true);
    ui->Character_FrameComboBox->setCurrentIndex(_metadata.character.data()->at(number).targetFrame);
    ui->Character_FrameComboBox->setEnabled(true);
    ui->Character_MangaPath->setText(
                _metadata.character.data()->at(number).mangaPath);
    _currentCharacterNumber = number;
    _isSpecifyed = false;
}

/*!
 * \brief 選択されたCharacterに関連して表示されるUI上の情報をクリアする
 * \brief MainWindow::clearCharacterUI
 */
void MainWindow::clearCharacterUI()
{
    _currentCharacterNumber = -1;
    ui->Character_MangaPath->clear();
    ui->ListWidget_Character->clearFocus();
    ui->Character_ComboBox->setCurrentIndex(0);
    ui->Character_DeleteButton->setEnabled(false);
    ui->Character_ComboBox->setEnabled(false);
    ui->Character_FrameComboBox->setEnabled(false);
}

/*!
 * \brief numberで指定されたdialogを選択状態にする
 * \brief MainWindow::specifyDialog
 * \param number
 */
void MainWindow::specifyDialog(int number)
{
    if(!_isRefreshingNow){
        refresh_Dialog_ListWidget();
    }

    if(_metadata.dialog.data()->isEmpty() || number < 0){//メタデータが無い場合
        clearDialogUI();
        return;
    }
    if(number >= _metadata.dialog.data()->size()){//範囲外
        number = 0;
    }
    /*
    if(number != ui->ListWidget_Dialog->currentRow()){//正しい場合
        ui->ListWidget_Dialog->setCurrentRow(number);
    }*/

    startSelectMode(ComicMetadata_Dialog);

    //! 各種情報をUIにセットする（一部セリフかナレーションかで内容を切り替える）
    ui->TextEdit_Dialog->setEnabled(true);
    ui->TextEdit_Dialog->setText(_metadata.dialog.data()->at(number).text);

    ui->DialogType_Dialog->setEnabled(true);
    ui->DialogType_Narration->setEnabled(true);
    ui->Dialog_FrameComboBox->setEnabled(true);
    ui->Dialog_FrameComboBox->setCurrentIndex(_metadata.dialog.data()->at(number).targetFrame);
    if(_metadata.dialog.data()->at(number).narration){//ナレーションの場合
        ui->DialogType_Narration->setChecked(true);
        ui->DialogType_Dialog->setChecked(false);
        ui->Dialog_SpeakerComboBox->setCurrentIndex(-1);
        ui->Dialog_SpeakerComboBox->setEnabled(false);
    }
    else{//セリフの場合
        ui->DialogType_Dialog->setChecked(true);
        ui->DialogType_Narration->setChecked(false);
        ui->Dialog_SpeakerComboBox->setEnabled(true);
        ui->Dialog_SpeakerComboBox->setCurrentIndex(_metadata.dialog.data()->at(number).targetCharacterID);
    }
    GraphicsItemData GIData = _metadata.dialog.data()->at(number).GIData;

    //! 選択確定状態にする
    _selectLock = true;
    GIData.colorSelected();

    //! 残りの情報をUIにセットする
    _currentDialogNumber = number;
    _selectedItemNumber = number;
    ui->Dialog_MangaPath->setText(
                _metadata.dialog.data()->at(number).mangaPath);

}

/*!
 * \brief 選択されたDialogに関連して表示されるUI上の情報をクリアする
 * \brief MainWindow::clearDialogUI
 */
void MainWindow::clearDialogUI()
{
    _currentDialogNumber = -1;
    ui->Dialog_MangaPath->clear();
    ui->ListWidget_Dialog->clearFocus();
    ui->TextEdit_Dialog->clear();
    ui->TextEdit_Dialog->setEnabled(false);
    ui->DialogType_Dialog->setChecked(false);
    ui->DialogType_Dialog->setEnabled(false);
    ui->DialogType_Narration->setChecked(false);
    ui->DialogType_Narration->setEnabled(false);
    ui->Dialog_SpeakerComboBox->setCurrentIndex(-1);
    ui->Dialog_SpeakerComboBox->setEnabled(false);
    ui->Dialog_FrameComboBox->setCurrentIndex(-1);
    ui->Dialog_FrameComboBox->setEnabled(false);
}

/*!
 * \brief numberで指定されたonomatopoeiaを選択状態にする
 * \brief MainWindow::specifyOnomatopoeia
 * \param number
 */
void MainWindow::specifyOnomatopoeia(int number)
{
    if(!_isRefreshingNow){
        refresh_Onomatopoeia_ListWidget();
    }
    if(_metadata.onomatopoeia.data()->isEmpty() || number < 0){
        clearOnomatopoeiaUI();
        return;
    }
    if(number >= _metadata.onomatopoeia.data()->size() || number < 0){
        number = 0;
    }
    /*
    if(number != ui->ListWidget_Onomatopoeia->currentRow()){
        ui->ListWidget_Dialog->setCurrentRow(number);
    }*/

    startSelectMode(ComicMetadata_Onomatopoeia);
    GraphicsItemData GIData = _metadata.onomatopoeia.data()->at(number).GIData;
    _selectLock = true;
    GIData.colorSelected();
    _currentOnomatopoeiaNumber = number;
    _currentOnomatopoeia = _metadata.onomatopoeia.data()->at(number);
    _selectedItemNumber = number;
    ui->TextEdit_Onomatopoeia->setText(_currentOnomatopoeia.text);
    ui->Onomatopoeia_FrameComboBox->setEnabled(true);
    ui->Onomatopoeia_FrameComboBox->setCurrentIndex(_currentOnomatopoeia.targetFrame);
    ui->TextEdit_Onomatopoeia->setEnabled(true);
    ui->Onomatopoeia_MangaPath->setText(
                _metadata.onomatopoeia.data()->at(number).mangaPath);

}

/*!
 * \brief 選択されたDialogに関連して表示されるUI上の情報をクリアする
 * \brief MainWindow::clearOnomatopoeiaUI
 */
void MainWindow::clearOnomatopoeiaUI()
{
    _currentOnomatopoeiaNumber = -1;
    ui->Onomatopoeia_MangaPath->clear();
    ui->ListWidget_Onomatopoeia->clearFocus();
    ui->TextEdit_Onomatopoeia->clear();
    ui->TextEdit_Onomatopoeia->setEnabled(false);
    ui->Onomatopoeia_FrameComboBox->setCurrentIndex(-1);
    ui->Onomatopoeia_FrameComboBox->setEnabled(false);
}

/*!
 * \brief numberで指定されたitemを選択状態にする
 * \brief MainWindow::specifyItem
 * \param number
 */
void MainWindow::specifyItem(int number)
{
    if(!_isRefreshingNow){
        refresh_Item_ListWidget();
    }
    if(_metadata.item.data()->isEmpty() || number < 0){
        clearItemUI();
        return;
    }
    if(number >= _metadata.item.data()->size() || number < 0){
        number = 0;
    }

    startSelectMode(ComicMetadata_Item);
    GraphicsItemData GIData = _metadata.item.data()->at(number).GIData;
    _selectLock = true;
    GIData.colorSelected();
    _currentCItemNumber = number;
    _currentItem = _metadata.item.data()->at(number);
    _selectedItemNumber = number;
    ui->Item_Class_LineEdit->setText(_currentItem.itemClass);
    ui->Item_Class_LineEdit->setEnabled(true);
    ui->TextEdit_CItem->setText(_currentItem.description);
    ui->TextEdit_CItem->setEnabled(true);
    ui->Item_FrameComboBox->setCurrentIndex(_currentItem.targetFrame);
    ui->Item_FrameComboBox->setEnabled(true);
    ui->Item_MangaPath->setText(
                _metadata.item.data()->at(number).mangaPath);
}

/*!
 * \brief 選択されたitemに関連して表示されるUI上の情報をクリアする
 * \brief MainWindow::clearItemUI
 */
void MainWindow::clearItemUI()
{
    _currentCItemNumber = -1;
    ui->Item_MangaPath->clear();
    ui->ListWidget_CItem->clearFocus();
    ui->Item_Class_LineEdit->clear();
    ui->Item_Class_LineEdit->setEnabled(false);
    ui->TextEdit_CItem->clear();
    ui->TextEdit_CItem->setEnabled(false);
    ui->Item_FrameComboBox->setCurrentIndex(-1);
    ui->Item_FrameComboBox->setEnabled(false);

}

/*!
 * \brief セリフの種別にて、セリフが選択されたときの動作
 * \brief MainWindow::on_DialogType_Dialog_clicked
 */
void MainWindow::on_DialogType_Dialog_clicked()
{
    if(_metadata.dialog.data()->isEmpty()){
        return;
    }
    if(_currentDialogNumber >= _metadata.dialog.data()->size() || _currentDialogNumber < 0){
        return;
    }
    DialogData newDialog = _metadata.dialog.data()->at(_currentDialogNumber);
    ui->DialogType_Dialog->setChecked(true);
    ui->DialogType_Narration->setChecked(false);
    newDialog.narration = false;
    newDialog.characterName = "--";
    newDialog.targetCharacterID = 0;
    ui->Dialog_SpeakerComboBox->setEnabled(true);
    _metadata.dialog.data()->replace(_currentDialogNumber, newDialog);

    refresh_Dialog_ListWidget(_currentDialogNumber);
}

/*!
 * \brief セリフの種別にて、ナレーションが選択されたときの動作
 * \brief MainWindow::on_DialogType_Narration_clicked
 */
void MainWindow::on_DialogType_Narration_clicked()
{
    if(_metadata.dialog.data()->isEmpty()){
        return;
    }
    if(_currentDialogNumber >= _metadata.dialog.data()->size() || _currentDialogNumber < 0){
        return;
    }
    ui->DialogType_Dialog->setChecked(false);
    ui->DialogType_Narration->setChecked(true);
    DialogData newDialog = _metadata.dialog.data()->at(_currentDialogNumber);
    newDialog.narration = true;
    newDialog.characterName = "Narration";
    ui->Dialog_SpeakerComboBox->setEnabled(false);
    ui->Dialog_SpeakerComboBox->setCurrentIndex(-1);
    _metadata.dialog.data()->replace(_currentDialogNumber, newDialog);
    refresh_Dialog_ListWidget(_currentDialogNumber);
}

/*!
 * \brief 現在選択されているメタデータアイテムを”全て”選択解除する
 * \brief MainWindow::releaseSpecificItems
 */
void MainWindow::releaseSpecificItems()
{
    specifyFrame();
    specifyCharacter();
    specifyDialog();
    specifyOnomatopoeia();
    specifyItem();
}

/*!
 * \brief 現在設定されているUI上のメタデータを”全て”クリアする
 * \brief MainWindow::metaDataUIClear
 */
void MainWindow::metaDataUIClear()
{
    clearFrameUI();
    clearCharacterUI();
    clearDialogUI();
    clearOnomatopoeiaUI();
    clearItemUI();
}

/*!
 * \brief 現在保持している（ページに対する）メタデータを”全て”クリアする
 * \brief MainWindow::metaDataListClear
 */
void MainWindow::metaDataListClear()
{
    initListWidget_Frame();
    ui->ListWidget_Character->clear();
    ui->ListWidget_Dialog->clear();
    ui->ListWidget_Onomatopoeia->clear();
    ui->ListWidget_CItem->clear();
}

/*!
 * \brief コマの情報を初期化する(コマはID:000があるため、クリア後には初期化が必要)
 * \brief MainWindow::initListWidget_Frame
 */
void MainWindow::initListWidget_Frame()
{
    ui->listWidget_Frame->clear();
    QString defaultFrame;
    ui->listWidget_Frame->addItem("ID:000 Default");
}

/*!
 * \brief 保存ボタンが押された際の動作
 * \brief MainWindow::on_actionSaveMetaData_triggered
 */
void MainWindow::on_actionSaveMetaData_triggered()
{
    writeMetaData();
}

/*!
 * \brief メタデータを保存する
 * \note作品+登場人物リストからなるCommonメタデータ部分と、各ページメタデータに分けて出力する
 * 現時点では基本すべて上書き
 * 初めにCommonメタデータを出力する
 * 次に該当するページメタデータを出力する
 * \return
 */
bool MainWindow::writeMetaData()
{
    QFileInfo imageFileName = QFileInfo(_fileUtility.getCurrentFileName());

    //ファイルが開かれていない場合には何もせず終了
    if(imageFileName.absoluteFilePath().size() <= 0) return false;

    QDir dir = imageFileName.absoluteDir();

//Windows Mac Linux
#ifdef Q_OS_WIN
    QDir metadataDir(QString("%1/%2").arg(dir.absolutePath()).arg(_metadataDirectoryName));
#else
    QDir metadataDir(QString("/%1/%2").arg(dir.absolutePath()).arg(_metadataDirectoryName));
#endif

    if(!metadataDir.exists()){
        cout << "mkdir: " << metadataDir.absolutePath().toStdString() << "..." << endl;

        if(metadataDir.mkdir(metadataDir.absolutePath())){
            cout << "success" << endl;
        }
        else{
            cout << "false" << endl;
        }
    }

    //! 共通メタデータのファイル名を設定
    QString commonXMLFileName = QString("%1/ComicMetadata.xml").arg(metadataDir.absolutePath());

    //! 共通メタデータを出力する
    _metadata.writeMetadata_Common(commonXMLFileName);

    //! ページメタデータのファイル名を設定
    QString pageXMLFileName = QString("%1/").arg(metadataDir.absolutePath());
    pageXMLFileName += QString("%1.xml").arg(imageFileName.baseName());

    //! ページメタデータを出力する
    _metadata.writeMetadata_Page(pageXMLFileName);

    _commonMetadataEdit = false;
    _pageMetadataEdit = false;
    return true;
}

/*!
 * \brief InfoタブにあるAdd New Characterボタンをクリックされた際の動作　共通メタデータの、登場人物を追加する
 * \brief MainWindow::on_AddNewCharacter_PushButton_clicked
 */
void MainWindow::on_AddNewCharacter_PushButton_clicked()
{
    //!Add New CharacterのText編集部分に何も入力されていなければ何もせず終了
    if(ui->Info_NewCharacterName_LineEdit->text().isEmpty()){
        return;
    }

    //!Add New CharacterのText編集部分に何か入力されていたら、登場人物を追加する
    cout << "addCharacter_" << _metadata.characterName.size() << endl;
    if(_selectedCharacterNameNumber >= 0 && _metadata.characterName.size()-1 > _selectedCharacterNameNumber){
        _metadata.characterName.insert(_selectedCharacterNameNumber+1, ui->Info_NewCharacterName_LineEdit->text());
    }
    else{
        _metadata.characterName.push_back(ui->Info_NewCharacterName_LineEdit->text());
    }
    cout << "addCharacter" << _metadata.characterName.size() << endl;
    ui->Info_NewCharacterName_LineEdit->clear();
    resetCharacterList();
}

/*!
 * \brief Infoタブの登場人物リストで選択されているものが変更された場合の動作
 * \brief MainWindow::on_Info_CharacterListWidget_currentRowChanged
 * \param currentRow
 */
void MainWindow::on_Info_CharacterListWidget_currentRowChanged(int currentRow)
{
    //! 選択された番号が正しければ、その番号を、現在選択状態の登場人物番号に設定する
    if(currentRow < 0) return;
    if(_selectedCharacterNameNumber == currentRow) return;

    if(currentRow >= _metadata.characterName.size()){
        ui->Info_SelectedCharacterName->clear();
        ui->Info_SelectedCharacterDelete->setEnabled(false);
        ui->Info_SelectedCharacterRename->setEnabled(false);
        return;
    }

    //! 設定された番号の登場人物情報をUIに反映する
    ui->Info_SelectedCharacterName->setText(_metadata.characterName.at(currentRow));

    if(currentRow == 0){
        ui->Info_SelectedCharacterRename->setEnabled(false);
        ui->Info_SelectedCharacterDelete->setEnabled(false);
    }
    else{
        ui->Info_SelectedCharacterRename->setEnabled(true);
        ui->Info_SelectedCharacterDelete->setEnabled(true);
    }

    _selectedCharacterNameNumber = currentRow;
}

/*!
 * \brief 登場人物を削除する
 * \brief MainWindow::deleteCharacterList
 * \param number
 */
void MainWindow::deleteCharacterList(int number)
{
    //デフォルト：0番は消さない
    if(number < _metadata.characterName.size() && number > 0){
        _metadata.characterName.removeAt(number);
        resetCharacterList();
        if(number >= _metadata.characterName.size()) --number;
        ui->Info_CharacterListWidget->setCurrentRow(number);
    }
}

/**
 * @brief MainWindow::resetCharacterList
 * 1:現在登録されている名前のリストにしたがってリネームする
 * 2:登録された情報を、すべてのキャラクター選択箇所に反映する
 */
void MainWindow::resetCharacterList()
{
    ui->Info_CharacterListWidget->clear();
    QString name;
    QVector<QString> nameList;
    //!ID番号付きの名前リストを作成する
    if(_metadata.characterName.isEmpty()) initCharacterList();

    for(int i=0; i<_metadata.characterName.size(); i++){
        name = QString("ID:%1 ").arg(i, 3, 10, QChar('0'));
        name += _metadata.characterName.at(i);
        nameList.push_back(name);
    }
    //! 1:現在登録されている名前のリストにしたがってリネームする
    for(int i=0; i<_metadata.characterName.size(); i++){
        ui->Info_CharacterListWidget->addItem(nameList.at(i));
    }

    //! 2:登録された情報を、すべてのキャラクター選択箇所に反映する
    ui->Character_ComboBox->clear();
    ui->Dialog_SpeakerComboBox->clear();
    for(int i=0; i<_metadata.characterName.size(); i++){
        ui->Character_ComboBox->addItem(nameList.at(i));
        ui->Dialog_SpeakerComboBox->addItem(nameList.at(i));
    }
}

/**
 * \brief 登場人物リストをすべて削除して、デフォルト000番のみの状態にする
 * \brief MainWindow::clearCharacterList
 */
void MainWindow::initCharacterList()
{
    _metadata.clearCharacterName();
    resetCharacterList();
}

/*!
 * \brief InfoタブのSelectedCharacterDeleteボタンがクリックされた際の動作
 * \brief MainWindow::on_Info_SelectedCharacterDelete_clicked
 */
void MainWindow::on_Info_SelectedCharacterDelete_clicked()
{
    deleteCharacterList(_selectedCharacterNameNumber);
}

/*!
 * \brief InfoタブのRenameボタンがクリックされた際の動作
 * \brief MainWindow::on_Info_SelectedCharacterRename_clicked
 */
void MainWindow::on_Info_SelectedCharacterRename_clicked()
{
    //!選択されている登場人物番号が範囲外である場合には無視
    if(_selectedCharacterNameNumber < 0
            || _selectedCharacterNameNumber >= _metadata.characterName.size()){
        ui->Info_SelectedCharacterName->clear();
        return;
    }

    //!通常のリネーム処理
    _metadata.characterName.replace
            (_selectedCharacterNameNumber, ui->Info_SelectedCharacterName->text());
    resetCharacterList();
    return;
}

/*!
 * \brief CharacterタブのCharacter IDコンボボックスでデータが選択された際の動作
 * \brief MainWindow::on_Character_ComboBox_currentIndexChanged
 * \param index
 */
void MainWindow::on_Character_ComboBox_currentIndexChanged(int index)
{
    //! 選択されている登場人物番号が範囲外の場合には何もしないで終了
    if(_currentCharacterNumber < 0
            || _currentCharacterNumber >= _metadata.character.data()->size()) return;
    if(index < 0 || index >= _metadata.characterName.size()) return;

    //! 選択されたデータを取得する
    CharacterData character_buff = _metadata.character.data()->at(_currentCharacterNumber);
    character_buff.characterID = index;
    character_buff.characterName = _metadata.characterName.at(index);

    //! 現在のデータで置き換える
    _metadata.character.data()->replace(_currentCharacterNumber, character_buff);
    if(!_isRefreshingNow && !_isSpecifyed){
        refresh_Character_ListWidget(_currentCharacterNumber);
    }
}

/*!
 * \brief FrameタブでDeleteボタンがクリックされた際の動作
 * \brief MainWindow::on_Delete_Frame_clicked
 */
void MainWindow::on_Delete_Frame_clicked()
{
    if(!_selectLock)
        return;
    selectModeDelete();
}

/*!
 * \brief CharacterタブのFrameコンボボックスでデータが選択された際の動作
 * \brief MainWindow::on_Character_FrameComboBox_currentIndexChanged
 * \param index
 */
void MainWindow::on_Character_FrameComboBox_currentIndexChanged(int index)
{
    if(_isSpecifyed) return;

    if(_currentCharacterNumber < 0
            || _currentCharacterNumber >= _metadata.character.data()->size()) return;
    if(index < 0 || index > _metadata.frame.data()->size()) return;

    CharacterData buff = _metadata.character.data()->at(_currentCharacterNumber);
    buff.targetFrame = index;
    _metadata.character.data()->replace(_currentCharacterNumber, buff);
    _metadata.renewMangaPath_Character(_currentCharacterNumber);
    ui->Character_MangaPath->setText(
                _metadata.character.data()->at(_currentCharacterNumber).mangaPath);
    if(!_isRefreshingNow && !_isSpecifyed){
        refresh_Character_ListWidget(_currentCharacterNumber);
    }
}

/*!
 * \brief DialogタブのFrameコンボボックスでデータが選択された際の動作
 * \brief MainWindow::on_Dialog_FrameComboBox_currentIndexChanged
 * \param index
 */
void MainWindow::on_Dialog_FrameComboBox_currentIndexChanged(int index)
{
    if(_currentDialogNumber < 0
            || _currentDialogNumber >= _metadata.dialog.data()->size()) return;
    if(index < 0 || index > _metadata.frame.data()->size()) return;

    DialogData buff = _metadata.dialog.data()->at(_currentDialogNumber);
    buff.targetFrame = index;
    _metadata.dialog.data()->replace(_currentDialogNumber, buff);
    _metadata.renewMangaPath_Dialog(_currentDialogNumber);
    ui->Dialog_MangaPath->setText(
                _metadata.dialog.data()->at(_currentDialogNumber).mangaPath);
    if(!_isRefreshingNow){
        refresh_Dialog_ListWidget(_currentDialogNumber);
    }
}

/*!
 * \brief DialogタブのSpeakerコンボボックスでデータが選択された際の動作
 * \brief MainWindow::on_Dialog_SpeakerComboBox_currentIndexChanged
 * \param index
 */
void MainWindow::on_Dialog_SpeakerComboBox_currentIndexChanged(int index)
{
    //選択されているキャラクターデータが範囲外の場合には何もしない
    if(_currentDialogNumber < 0
            || _currentDialogNumber >= _metadata.dialog.data()->size()) return;
    if(index < 0 || index >= _metadata.characterName.size()) return;

    DialogData buff = _metadata.dialog.data()->at(_currentDialogNumber);
    if(buff.narration) return;
    buff.targetCharacterID = index;
    buff.characterName = _metadata.characterName.at(index);
    _metadata.dialog.data()->replace(_currentDialogNumber, buff);
    _metadata.renewMangaPath_Dialog(_currentDialogNumber);
    ui->Dialog_MangaPath->setText(
                _metadata.dialog.data()->at(_currentDialogNumber).mangaPath);
    if(!_isRefreshingNow){
        refresh_Dialog_ListWidget(_currentDialogNumber);
    }
}

/*!
 * \brief ItemタブのAdd Itemがクリックされた際の動作
 * \brief MainWindow::on_AddItem_Rect_clicked
 */
void MainWindow::on_AddItem_Rect_clicked()
{
    startCreateMode_Rect(ComicMetadata_Item);
}


/*!
 * \brief ItemタブのItemListでアイテムが選択された際の動作
 * \brief MainWindow::on_ListWidget_CItem_currentRowChanged
 * \param currentRow
 */
void MainWindow::on_ListWidget_CItem_currentRowChanged(int currentRow)
{
    /*
    if(currentRow < 0) return;
    if(_selectedItemNumber == currentRow) return;

    if(currentRow >= _metadata.item.data()->size()){
        clearItemUI();
    }
    */
    specifyItem(currentRow);
}

/*!
 * \brief Itemタブのクラス部に文字入力された際の動作
 * \brief MainWindow::on_Item_Class_LineEdit_textChanged
 * \param arg1
 */
void MainWindow::on_Item_Class_LineEdit_textChanged(const QString &arg1)
{
    if(_currentCItemNumber < 0 || _metadata.item.data()->size() <= _currentCItemNumber){
        return;
    }
    ItemData buff = _metadata.item.data()->at(_currentCItemNumber);
    buff.itemClass = ui->Item_Class_LineEdit->text();
    _metadata.item.data()->replace(_currentCItemNumber, buff);
}

/*!
 * \brief ItemタブのDescriptionが編集された際の動作
 * \brief MainWindow::on_TextEdit_CItem_textChanged
 */
void MainWindow::on_TextEdit_CItem_textChanged()
{
    if(_currentCItemNumber < 0 || _metadata.item.data()->size() <= _currentCItemNumber){
        return;
    }
    ItemData buff = _metadata.item.data()->at(_currentCItemNumber);
    buff.description = ui->TextEdit_CItem->document()->toPlainText();
    _metadata.item.data()->replace(_currentCItemNumber, buff);
//    refresh_Item_ListWidget();
}

/*!
 * \brief Itemタブのframeコンボボックスでデータが選択された際の動作
 * \brief MainWindow::on_Item_FrameComboBox_currentIndexChanged
 * \param index
 */
void MainWindow::on_Item_FrameComboBox_currentIndexChanged(int index)
{
    if(_currentCItemNumber < 0 || _metadata.item.data()->size() <= _currentCItemNumber){
        return;
    }
    if(index < 0 || index > _metadata.frame.data()->size()) return;
    ItemData buff = _metadata.item.data()->at(_currentCItemNumber);
    buff.targetFrame = index;
    _metadata.item.data()->replace(_currentCItemNumber, buff);
    _metadata.renewMangaPath_Item(_currentCItemNumber);
    ui->Item_MangaPath->setText(
                _metadata.item.data()->at(_currentCItemNumber).mangaPath);
    if(!_isRefreshingNow){
        refresh_Item_ListWidget();
    }
}

/*!
 * \brief OnomatopoeiaタブのOnomatopoeiaListでアイテムが選択された際の動作
 * \brief MainWindow::on_ListWidget_Onomatopoeia_currentRowChanged
 * \param currentRow
 */
void MainWindow::on_ListWidget_Onomatopoeia_currentRowChanged(int currentRow)
{
    specifyOnomatopoeia(ui->ListWidget_Onomatopoeia->currentRow());
}

/*!
 * \brief Onomatopoeiaタブのframeコンボボックスでデータが選択された際の動作
 * \brief MainWindow::on_Onomatopoeia_FrameComboBox_currentIndexChanged
 * \param index
 */
void MainWindow::on_Onomatopoeia_FrameComboBox_currentIndexChanged(int index)
{
    if(_currentOnomatopoeiaNumber < 0
            || _currentOnomatopoeiaNumber >= _metadata.onomatopoeia.data()->size()) return;
    if(index < 0 || index > _metadata.frame.data()->size()) return;

    OnomatopoeiaData buff = _metadata.onomatopoeia.data()->at(_currentOnomatopoeiaNumber);
    buff.targetFrame = index;
    _metadata.onomatopoeia.data()->replace(_currentOnomatopoeiaNumber, buff);
    _metadata.renewMangaPath_Onomatopoeia(_currentOnomatopoeiaNumber);
    ui->Onomatopoeia_MangaPath->setText(
                _metadata.onomatopoeia.data()->at(_currentOnomatopoeiaNumber).mangaPath);
    if(!_isRefreshingNow){
        refresh_Onomatopoeia_ListWidget(_currentOnomatopoeiaNumber);
    }
}

/*!
 * \brief CharacterタブのDeleteボタンがクリックされた際の動作
 * \brief MainWindow::on_Character_DeleteButton_clicked
 */
void MainWindow::on_Character_DeleteButton_clicked()
{
    removeCharacter(_currentCharacterNumber);
}

/*!
 * \brief DialogタブのDeleteボタンがクリックされた際の動作
 * \brief MainWindow::on_Dialog_DeleteButton_clicked
 */
void MainWindow::on_Dialog_DeleteButton_clicked()
{
    removeDialog(_currentDialogNumber);
}

/*!
 * \brief OnomatopoeiaタブのDeleteボタンがクリックされた際の動作
 * \brief MainWindow::on_Onomatopoeia_DeleteButton_clicked
 */
void MainWindow::on_Onomatopoeia_DeleteButton_clicked()
{
    removeOnomatopoeia(_currentOnomatopoeiaNumber);
}

/*!
 * \brief ItemタブのDeleteボタンがクリックされた際の動作
 * \brief MainWindow::on_Item_DelereButton_clicked
 */
void MainWindow::on_Item_DelereButton_clicked()
{
    removeCItem(_currentCItemNumber);
}

/*!
 * \brief InfoタブのComicTitleが編集された際の動作
 * \brief MainWindow::on_Info_ComicTitle_LineEdit_textChanged
 * \param arg1
 */
void MainWindow::on_Info_ComicTitle_LineEdit_textChanged(const QString &arg1)
{
    _metadata.workTitle = arg1;
    _metadata.renewAllMangaPath();
}

/*!
 * \brief InfoタブのEpisodeNumberが編集された際の動作
 * \brief MainWindow::on_Info_EpisodeNumber_LineEdit_textChanged
 * \param arg1
 */
void MainWindow::on_Info_EpisodeNumber_LineEdit_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()){
        _metadata.episodeNumber = -1;
        ui->Info_EpisodeNumber_LineEdit->clear();
        return;
    }

    int num = arg1.toInt();
    if(num >= 0){
        _metadata.episodeNumber = num;
        _metadata.renewAllMangaPath();
    }
    else{
        _metadata.episodeNumber = -1;
        ui->Info_EpisodeNumber_LineEdit->clear();
    }
}

/*!
 * \brief InfoタブのPage Numberが編集された際の動作
 * \brief MainWindow::on_Info_PageNumber_textChanged
 * \param arg1
 */
void MainWindow::on_Info_PageNumber_textChanged(const QString &arg1)
{
    _metadata.pageNumber = arg1.toInt();
    _metadata.renewAllMangaPath();
}

/*!
 * \brief FrameタブのFrameListでアイテムが選択された際の動作
 * \brief MainWindow::on_listWidget_Frame_currentRowChanged
 * \param currentRow
 */
void MainWindow::on_listWidget_Frame_currentRowChanged(int currentRow)
{
    if(currentRow == 0){
        if(_metadata.frame.data()->isEmpty()) return;
        selectModeRelease();
        specifyFrame(-1);
        ui->Frame_MangaPath->setText(_metadata.MangaPath_title_episode_page_frame(0));
        return;//0番はデフォルトの為何もしない
    }
    specifyFrame(currentRow-1);
}

/*!
 * \brief FileメニューのClear Meta Data がクリックされた際の動作
 * \brief MainWindow::on_actionClearMetaData_triggered
 */
void MainWindow::on_actionClearMetaData_triggered()
{
    clearMetadata();
}

/*!
 * \brief ページメタデータを削除する
 * \brief MainWindow::clearMetadata
 */
void MainWindow::clearMetadata()
{
    //!メタデータをクリアする際には、メタデータ情報なしでファイルを開きなおす
    openImageFile(_fileUtility.getCurrentFileName(), false);
}

/*!
 * \brief ｍ多データ情報を開く
 * \brief MainWindow::loadMetadata
 */
void MainWindow::loadMetadata()
{
    _commonMetadataEdit = true;
    _pageMetadataEdit = true;

    QFileInfo imageFileName = QFileInfo(_fileUtility.getCurrentFileName());

    //!画像ファイルが開かれていない場合には何もせず終了
    if(imageFileName.absoluteFilePath().size() <= 0) return;

    _metadata.clear();

    //!Common Metadataを開く
    QDir dir_base = imageFileName.absoluteDir();
    QDir dir(QString("%1/%2").arg(dir_base.absolutePath()).arg(_metadataDirectoryName));
    QString commonXMLFileName = QString("%1/ComicMetadata.xml").arg(dir.absolutePath());
    if(_metadata.loadMetadata_Common(commonXMLFileName)){
        ui->Info_ComicTitle_LineEdit->setText(_metadata.workTitle);
        resetCharacterList();
    }

    //! Page Metadataを開く
    QString pageXMLFileName = QString("%1/").arg(dir.absolutePath());
    pageXMLFileName += QString("%1.xml").arg(imageFileName.baseName());
    _metadata.loadMetadata_Page(pageXMLFileName);

    //! 読み込んだページの情報を，UIに反映する
    ui->Info_EpisodeNumber_LineEdit->setText
            (QString("%1").arg(_metadata.loadEpisodeNumber, 4, 10,QChar('0')));
    ui->Info_PageNumber->setText
            (QString("%1").arg(_metadata.loadPageNumber, 4, 10, QChar('0')));

    _metadata.imageFileName = _metadata.loadImageFileName;

    for(int i=0; i<_metadata.loadFrame.size(); i++){
        QPolygonF polygon = calcAbsolutePosition(_metadata.loadFrameCoordinate.at(i));
        FrameData data = _metadata.loadFrame.at(i);
        addFrame(polygon, data.mangaPath, data.sceneBoundary);
    }
    for(int i=0; i<_metadata.loadCharacter.size(); i++){
        QPolygonF polygon = calcAbsolutePosition(_metadata.loadCharacterCoordinate.at(i));
        CharacterData data = _metadata.loadCharacter.at(i);
        addCharacter(polygon, data.mangaPath, data.characterName, data.characterID, data.targetFrame);
    }
    for(int i=0; i<_metadata.loadDialog.size(); i++){
        QPolygonF polygon = calcAbsolutePosition(_metadata.loadDialogCoordinate.at(i));
        DialogData data = _metadata.loadDialog.at(i);
        addDialog(polygon, data.mangaPath,data.text, data.fontSize,
                  data.narration, data.targetCharacterID, data.targetFrame, data.characterName);
    }
    for(int i=0; i<_metadata.loadOnomatopoeia.size();i++){
        QPolygonF polygon = calcAbsolutePosition(_metadata.loadOnomatopoeiaCoordinate.at(i));
        OnomatopoeiaData data = _metadata.loadOnomatopoeia.at(i);
        addOnomatopoeia(polygon, data.mangaPath, data.text, data.fontSize, data.targetFrame);
    }
    for(int i=0; i<_metadata.loadItem.size(); i++){
        QPolygonF polygon = calcAbsolutePosition(_metadata.loadItemCoordinate.at(i));
        ItemData data = _metadata.loadItem.at(i);
        addItem(polygon, data.mangaPath, data.itemClass, data.description, data.targetFrame);
    }
}

/*!
 * \brief 相対表現座標系から絶対表現座標系への変換
 * \brief MainWindow::calcAbsolutePosition
 * \param relativePosition
 * \return
 */
QPolygonF MainWindow::calcAbsolutePosition(QPolygonF relativePosition)
{
    QPolygonF absolutePosition;
    if(_image.isNull()) return absolutePosition;
    if(_image.data()->isNull()) return absolutePosition;
    double width = _image.data()->width();
    double height = _image.data()->height();
    QPointF relativePoint;
    QPointF absolutePoint;
    for(int i=0; i<relativePosition.size(); i++){
        relativePoint = relativePosition.at(i);
        absolutePoint.setX(relativePoint.x() * width);
        absolutePoint.setY(relativePoint.y() * height);
        absolutePosition.push_back(absolutePoint);
    }
    return absolutePosition;
}

/*!
 * \brief frameタブのScene Changeのチェックが変更された際の動作
 * \brief MainWindow::on_Frame_SceneChange_CheckBox_toggled
 * \param checked
 */
void MainWindow::on_Frame_SceneChange_CheckBox_toggled(bool checked)
{
    if(_currentFrameNumber < 0 || _currentFrameNumber >= _metadata.frame.data()->size())
    {
        return;
    }
    FrameData buf = _metadata.frame.data()->at(_currentFrameNumber);
    buf.sceneBoundary = checked;
    _metadata.frame.data()->replace(_currentFrameNumber, buf);
    if(!_isRefreshingNow){
        refresh_Frame_ListWidget(_currentFrameNumber);
    }
}

/*!
 * \brief FrameタブのFrameListコマの情報（各メタデータのタブ:frame部に表示されるものを含む）を最新の状態に変更する
 * \brief MainWindow::refresh_Frame_ListWidget
 * \param currentIndex
 */
void MainWindow::refresh_Frame_ListWidget(int currentIndex)
{
    _isRefreshingNow = true;
    //!他のウィジェットに最新のフレーム数を反映する
    //Signal->slot方式に変更したいFIXME

    //! リセットとデフォルトフレームのセット
    QString defaultFrameText = ("Frame000");
    ui->listWidget_Frame->clear();
    ui->listWidget_Frame->addItem(defaultFrameText);
    ui->Character_FrameComboBox->clear();
    ui->Character_FrameComboBox->addItem(defaultFrameText);
    ui->Dialog_FrameComboBox->clear();
    ui->Dialog_FrameComboBox->addItem(defaultFrameText);
    ui->Onomatopoeia_FrameComboBox->clear();
    ui->Onomatopoeia_FrameComboBox->addItem(defaultFrameText);
    ui->Item_FrameComboBox->clear();
    ui->Item_FrameComboBox->addItem(defaultFrameText);

    //! 入力されているフレーム情報を適用する
    for(int i=0; i<_metadata.frame.data()->size(); i++){
        FrameData f = _metadata.frame.data()->at(i);
        QString title = QString("Frame%1 ").arg(i+1,3,10,QChar('0'));
        if(f.sceneBoundary){
            title += " s ";
        }
        else{
            title += " - ";
        }
        QPolygonF polygon = f.GIData.item()->polygon();
        for(int j=0; j< polygon.size(); j++){
            QPointF pt = polygon.at(j);
            title += QString("(%1,%2) ").arg(pt.x()).arg(pt.y());
        }
        ui->listWidget_Frame->addItem(title);
        ui->Character_FrameComboBox->addItem(title);
        ui->Dialog_FrameComboBox->addItem(title);
        ui->Onomatopoeia_FrameComboBox->addItem(title);
        ui->Item_FrameComboBox->addItem(title);
    }

    if(currentIndex >= 0 && currentIndex <= _metadata.frame.data()->size()){
        ui->listWidget_Frame->setCurrentRow(currentIndex);
    }
    _isRefreshingNow = false;
}

/*!
 * \brief CharacterタブのCharacter Listに表示される情報を最新の状態に変更する
 * \brief MainWindow::refresh_Character_ListWidget
 * \param currentIndex
 */
void MainWindow::refresh_Character_ListWidget(int currentIndex)
{
    _isRefreshingNow = true;
    ui->ListWidget_Character->clear();
    for(int i=0; i<_metadata.character.data()->size(); i++){
        CharacterData buf = _metadata.character.data()->at(i);
        QString text = QString("%1:").arg(i,3,10,QChar('0'));
        text += QString(" Frame:%1").arg(buf.targetFrame);
        text += QString(" ID:%1 %2").arg(buf.characterID).arg(buf.characterName);
        ui->ListWidget_Character->addItem(text);
    }
    if(currentIndex >= 0 && currentIndex < _metadata.character.data()->size()){
        ui->ListWidget_Character->setCurrentRow(currentIndex);
    }
    _isRefreshingNow = false;
}

/*!
 * \brief DialogタブのDialog Listに表示される情報を最新の状態に変更する
 * \brief MainWindow::refresh_Dialog_ListWidget
 * \param currentIndex
 */
void MainWindow::refresh_Dialog_ListWidget(int currentIndex)
{
    _isRefreshingNow = true;
    ui->ListWidget_Dialog->clear();
    for(int i=0; i<_metadata.dialog.data()->size(); i++){
        DialogData buf = _metadata.dialog.data()->at(i);
        QString text = QString("Dialog%1:").arg(i,3,10,QChar('0'));
        text += QString(" Frame:%1").arg(buf.targetFrame);
        if(!buf.narration){
            text += QString(" Dialog ");
            text += QString(" CharacterID:%1 %2 ")
                    .arg(buf.targetCharacterID).arg(buf.characterName);
        }
        else{
            text += " Narration ";
        }
//        text += getFirstLine(buf.text);
        text += cvtSingleLine(buf.text);
        ui->ListWidget_Dialog->addItem(text);
    }
    if(currentIndex >= 0 && currentIndex < _metadata.dialog.data()->size()){
        ui->ListWidget_Dialog->setCurrentRow(currentIndex);
    }
    _isRefreshingNow = false;
}

/*!
 * \brief OnomatopoeiaタブのOnomatopoeia Listに表示される情報を最新の状態に変更する
 * \brief MainWindow::refresh_Onomatopoeia_ListWidget
 * \param currentIndex
 */
void MainWindow::refresh_Onomatopoeia_ListWidget(int currentIndex)
{
    _isRefreshingNow = true;
    ui->ListWidget_Onomatopoeia->clear();
    for(int i=0; i<_metadata.onomatopoeia.data()->size(); i++){
        OnomatopoeiaData buf = _metadata.onomatopoeia.data()->at(i);
        QString text = QString("%1:").arg(i,3,10,QChar('0'));
        text += QString(" Frame:%1 ").arg(buf.targetFrame);
        //text += getFirstLine(buf.text);
        text += cvtSingleLine(buf.text);
        ui->ListWidget_Onomatopoeia->addItem(text);
    }
    if(currentIndex >= 0 && currentIndex < _metadata.onomatopoeia.data()->size()){
        ui->ListWidget_Onomatopoeia->setCurrentRow(currentIndex);
    }
    _isRefreshingNow = false;
}

/*!
 * \brief ItemタブのItem Listに表示される情報を最新の状態に変更する
 * \brief MainWindow::refresh_Item_ListWidget
 * \param currentIndex
 */
void MainWindow::refresh_Item_ListWidget(int currentIndex)
{
    _isRefreshingNow = true;
    ui->ListWidget_CItem->clear();
    for(int i=0; i<_metadata.item.data()->size(); i++){
        ItemData buf = _metadata.item.data()->at(i);
        QString text = QString("%1:").arg(i, 3, 10, QChar('0'));
        text += QString(" Frame:%1").arg(buf.targetFrame);
        text += QString(" %1").arg(cvtSingleLine(buf.itemClass));
        text += QString(" %1").arg(cvtSingleLine(buf.description));
        ui->ListWidget_CItem->addItem(text);
    }
    if(currentIndex >= 0 && currentIndex < _metadata.item.data()->size()){
        ui->ListWidget_CItem->setCurrentRow(currentIndex);
    }
    _isRefreshingNow = false;
}

void MainWindow::refresh_ALL_ListWidget()
{
    refresh_Frame_ListWidget();
    refresh_Character_ListWidget();
    refresh_Dialog_ListWidget();
    refresh_Onomatopoeia_ListWidget();
    refresh_Item_ListWidget();
}

QString MainWindow::getFirstLine(QString str)
{
    QString firstLine;
    for(int i=0;i<str.size();i++){
        if(str.at(i) == '\n'){
            //cout << "改行コード発見" << endl;
            break;
        }
        else{
            firstLine.push_back(str.at(i));
        }
    }
    return firstLine;
}

QString MainWindow::cvtSingleLine(QString str)
{
    QString newStr;
    for(int i=0; i<str.size(); i++){
        if(str.at(i) == '\n' || str.at(i) == '\t'){
            newStr.push_back(QChar(' '));
        }
        else{
            newStr.push_back(str.at(i));
        }
    }
    return newStr;
}
