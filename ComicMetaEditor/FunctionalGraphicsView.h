/*! \file
 *  \brief 画像表示用　QGraphicsViewのHookクラス
 * 画像表示と、マウス関係、キーボード関係、ズームイン/アウト用の処理を追加
 * 必ず、SharedPointer形式で確保したQGraphicsSceneをセットして利用する
 *  \author Daisuke
 *  \date 2016/03/06    daisuke コメント編集
 */

#ifndef FUNCTIONALGRAPHICSVIEW_H
#define FUNCTIONALGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>

class FunctionalGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit FunctionalGraphicsView(QWidget *parent = 0);//!<コンストラクタ
    void setScene(QSharedPointer<QGraphicsScene> scene);//!<表示用シーンセット用関数
    double zoom(double zoomRatio);//!<ズーム操作
    double zoomIn();//!<固定倍率でのズームイン
    double wheelZoomIn();//!<マウスホイール操作時のズームイン
    double zoomOut();//!<固定倍率でのズームアウト
    double wheelZoomOut();//!<マウスホイール操作時のズームアウト
    double getZoomRatio();//!<現在のズーム率取得用関数
    double getWheelZoomStep();//!<マウスホイールでのズーム率を取得する関数
    void setZoomStep(double zoomStep);//!<固定のズーム倍率をセットする関数
private:
    QSharedPointer<QGraphicsScene> _scene; //!<画面表示用QGraphicsScene
    double _zoomRatio; //!<現在の拡大率
    double _zoomStep; //!<１回の拡大/縮小処理での拡大率変化量
    double _wheelZoomStep; //!<マウスホイールによる１回の拡大/縮小処理での拡大率変化量

    void wheelEvent(QWheelEvent *event);//!<マウスホイールが操作された際の動作
    void keyPressEvent(QKeyEvent *event);//!<キーボードが押下されたときの動作
    void keyReleaseEvent(QKeyEvent *event);//!<キーボード押下が解除されたときの動作
    void mousePressEvent(QMouseEvent *event);//!<マウスがクリックされたときの動作
    void mouseReleaseEvent(QMouseEvent *event);//!<マウスクリックが解除されたときの動作
    void mouseMoveEvent(QMouseEvent *event);//!<マウスが移動したときの動作

signals:    
    void signal_zoomed(double);
    //! SiMo:Signal for Mouse
    void SiMo_wheel(QWheelEvent *event);
    void SiMo_press(QMouseEvent *event);
    void SiMo_release(QMouseEvent *event);
    void SiMo_move(QMouseEvent *event);
    //! SiKy:Signal for Keyboard
    void SiKy_press(QKeyEvent *event);
    void SiKy_release(QKeyEvent *event);

public slots:

};

#endif // FUNCTIONALGRAPHICSVIEW_H
