/*!
 * \file
 */
#include "FunctionalGraphicsView.h"

//-----------------------------------------------------------------------
//public functions
//-----------------------------------------------------------------------

FunctionalGraphicsView::FunctionalGraphicsView(QWidget *parent) :
   QGraphicsView(parent)
{
    _zoomRatio = 1.0;
    _zoomStep = 1.2;
#ifdef Q_OS_MAC
    _wheelZoomStep = 1.02;
#else
    _wheelZoomStep = 1.1;
#endif
}

void FunctionalGraphicsView::setScene(QSharedPointer<QGraphicsScene> scene)
{
    _scene = scene;
    this->QGraphicsView::setScene(_scene.data());
}

double FunctionalGraphicsView::zoom(double zoomRatio)
{
    //!現在のズーム率から、変化後のズーム率を計算
    double scaling = zoomRatio / _zoomRatio;

    //!ズーム率を変更する
    this->scale(scaling, scaling);
    _zoomRatio = zoomRatio;
    //!ズーム率が変更されたシグナルを発信
    emit signal_zoomed(_zoomRatio);
    return _zoomRatio;
}

double FunctionalGraphicsView::zoomIn()
{
    //!現在のズーム率から、変化後のズーム率を計算
    double nextZoomRatio = _zoomRatio * _zoomStep;
    //!1.0倍をまたぐ倍率の場合には強制的に1.0倍とする
    if(_zoomRatio < 1.0 && nextZoomRatio > 1.0) {
        nextZoomRatio = 1.0;
    }
    //!ズーム処理本体
    zoom(nextZoomRatio);
    _zoomRatio = nextZoomRatio;
    //!ズーム率が変更されたシグナルを発信
    emit signal_zoomed(_zoomRatio);
    return _zoomRatio;
}

double FunctionalGraphicsView::wheelZoomIn()
{
    //!現在のズーム率から、変化後のズーム率を計算
    double nextZoomRatio = _zoomRatio * _wheelZoomStep;
    //!1.0倍をまたぐ倍率の場合には強制的に1.0倍とする
    if(_zoomRatio < 1.0 && nextZoomRatio > 1.0) {
        nextZoomRatio = 1.0;
    }
    //!ズーム処理本体
    zoom(nextZoomRatio);
    _zoomRatio = nextZoomRatio;
    //!ズーム率が変更されたシグナルを発信
    emit signal_zoomed(_zoomRatio);
    return _zoomRatio;
}

double FunctionalGraphicsView::zoomOut()
{
    //!現在のズーム率から、変化後のズーム率を計算
    double nextZoomRatio = _zoomRatio / _zoomStep;
    //!1.0倍をまたぐ倍率の場合には強制的に1.0倍とする
    if(_zoomRatio > 1.0 && nextZoomRatio < 1.0) {
        nextZoomRatio = 1.0;
    }
    zoom(nextZoomRatio);
    //!ズーム処理本体
    _zoomRatio = nextZoomRatio;
    //!ズーム率が変更されたシグナルを発信
    emit signal_zoomed(_zoomRatio);
    return _zoomRatio;
}

double FunctionalGraphicsView::wheelZoomOut()
{
    //!現在のズーム率から、変化後のズーム率を計算
    double nextZoomRatio = _zoomRatio / _wheelZoomStep;
    //!1.0倍をまたぐ倍率の場合には強制的に1.0倍とする
    if(_zoomRatio > 1.0 && nextZoomRatio < 1.0) {
        nextZoomRatio = 1.0;
    }
    //!ズーム処理本体
    zoom(nextZoomRatio);
    _zoomRatio = nextZoomRatio;
    //!ズーム率が変更されたシグナルを発信
    emit signal_zoomed(_zoomRatio);
    return _zoomRatio;
}

double FunctionalGraphicsView::getZoomRatio()
{
    return _zoomRatio;
}

void FunctionalGraphicsView::setZoomStep(double zoomStep)
{
    _zoomStep = zoomStep;
    return;
}

void FunctionalGraphicsView::wheelEvent(QWheelEvent *event)
{
    emit SiMo_wheel(event);
}

void FunctionalGraphicsView::keyPressEvent(QKeyEvent *event)
{
    emit SiKy_press(event);
}

void FunctionalGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    emit SiKy_release(event);
}

void FunctionalGraphicsView::mousePressEvent(QMouseEvent *event)
{
    emit SiMo_press(event);
}

void FunctionalGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    emit SiMo_release(event);
}

void FunctionalGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    emit SiMo_move(event);
}

double FunctionalGraphicsView::getWheelZoomStep()
{
    return _wheelZoomStep;
}
