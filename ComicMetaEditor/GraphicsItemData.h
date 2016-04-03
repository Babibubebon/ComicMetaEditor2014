/*! \file
 *  \brief 画面に表示するためのグラフィックスアイテム用クラスと色設定
 *  \author Daisuke
 *  \date 2016/03/13    daisuke コメント編集
 */

#ifndef GRAPHICSITEMDATA_H
#define GRAPHICSITEMDATA_H
#include <QGraphicsPolygonItem>
#include <QBrush>
#include <QPen>

enum GraphicsItemDataColor{
    GraphicsItemDataColor_Blue,//!frame
    GraphicsItemDataColor_Red,//!character
    GraphicsItemDataColor_Green,//!dialog
    GraphicsItemDataColor_Purple,//!onomatopoeia
    GraphicsItemDataColor_Yellow,
    GraphicsItemDataColor_LightBlue,
};

/*!
 * \brief 枠等の表示で使用する色設定クラス
 */
class GraphicsItemColor{
public:
    GraphicsItemColor();
    GraphicsItemColor(GraphicsItemDataColor colType);
    void setColorPreset(GraphicsItemDataColor colType);
    QBrush brush_default;//!通常表示用
    QPen pen_default;
    QBrush brush_active;//!選択モードにおいて選択可能な場合
    QPen pen_active;
    QBrush brush_selected;//!選択モードに置いて選択された場合
    QPen pen_selected;
    QBrush brush_hidden;//!選択モード等において選択不可能な場合
    QPen pen_hidden;
    int pw_default;
    int pw_active;
    int pw_selected;
    void initpen();
};

/*!
 * \brief 画面に表示するためのグラフィックスアイテム用クラス
 * アイテムの実態生成もこのクラスで行う
 * 色設定についてはGraphicsItemColorを利用する
 */
class GraphicsItemData
{
public:
    GraphicsItemData();
    GraphicsItemData(GraphicsItemDataColor color);
    void setPolygon(QPolygonF &polygon, int width, int height);
    void setRelativePolygon(QPolygonF &polygon, int width, int height);
    QGraphicsPolygonItem* _item;
    QGraphicsPolygonItem* item();
    QPolygonF _relativePosition;
    void setGraphicsPolygonItem(QGraphicsPolygonItem* item);
    void colorDefault();
    void colorActive();
    void colorBrushSelected();//ブラシのみ選択状態に変更
    void colorSelected();
    void colorHidden();
    GraphicsItemColor col;
    void setColorPreset(GraphicsItemDataColor color);
private:
    void initbw();
};

QPolygonF calcRelativePosition(QPolygonF absolutePosition, int width, int height);
QPolygonF calcAbsolutePosition(QPolygonF relativePosition, int width, int height);

#endif // GRAPHICSITEMDATA_H
