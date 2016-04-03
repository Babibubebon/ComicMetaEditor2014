/*! \file
 *  \brief 各種座標計算系実装部
 *  \author Daisuke
 *  \date 2016/03/ daisuke コメント追加
 */

#ifndef COMMONFUNCTION_H
#define COMMONFUNCTION_H
#include <QPolygonF>
#include <QLineF>
#include <QRectF>

/*!
 * \brief ポリゴンの面積を計算する関数
 * \param ポリゴン（頂点情報）
 * \return
 */
double calcPolygonAreaSize(QPolygonF polygon);
/*!
 * \brief 点と直線との距離を計算する関数
 * \param 基準となる直線
 * \param 点
 * \return
 */
double getDistance(QLineF line, QPointF pt);

/*!
 * \brief 2点間の距離計算関数
 * \param 点1
 * \param 点2
 * \return
 */
double calcDistance(QPointF pt1, QPointF pt2);

/*!
 * \brief (0,0)を基準とした2つのベクトルの内積計算関数
 * \param ベクトル1（点）
 * \param ベクトル2（点）
 * \return
 */
double crossVector(QPointF vl, QPointF vr);

/*!
 * \brief 2点の情報をQtのQRectF形式に変換する関数
 * \param 点1
 * \param 点2
 * \return
 */
QRectF getRectBy2Pt(QPoint pt1, QPoint pt2);

/*!
 * \brief 渡されたポリゴンの中心座標を計算する関数
 * \param ポリゴン（頂点情報）
 * \return
 */
QPointF getPolygonCenter(QPolygonF polygon);

#endif // COMMONFUNCTION_H
