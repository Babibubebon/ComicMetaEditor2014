/*!
 * \file
 */

#include "GraphicsItemData.h"

GraphicsItemColor::GraphicsItemColor()
{
    initpen();
}

GraphicsItemColor::GraphicsItemColor(GraphicsItemDataColor colType)
{
    initpen();
    setColorPreset(colType);
}

void GraphicsItemColor::initpen()
{
    pw_default = 2;
    pw_active = 2;
    pw_selected = 4;
}

/*!
 * \brief 表示用オブジェクトに使用する色セットを取得する
 * \param colType 色セットのタイプ
 */
void GraphicsItemColor::setColorPreset(GraphicsItemDataColor colType)
{
    switch (colType) {
    case GraphicsItemDataColor_Blue:
        brush_default = QBrush(QColor(100,100,255,50));
        pen_default = QPen(QColor(0,0,255,200));
        pen_default.setWidth(pw_default);
        brush_active = QBrush(QColor(100,100,255,150));
        pen_active = QPen(QColor(0,0,255,200));
        pen_active.setWidth(pw_active);
        brush_selected = QBrush(QColor(0,0,255,150));
        pen_selected = QPen(QColor(255,0,0,200));
        pen_selected.setWidth(pw_selected);
        brush_hidden = QBrush(QColor(150,150,255,50));
        pen_hidden = QPen(QColor(150,150,255,100));
        break;
    case GraphicsItemDataColor_Green:
        brush_default = QBrush(QColor(100,255,100,50));
        pen_default = QPen(QColor(0,255,0,200));
        pen_default.setWidth(pw_default);
        brush_active = QBrush(QColor(100,255,100,150));
        pen_active = QPen(QColor(0,255,0,200));
        pen_active.setWidth(pw_active);
        brush_selected = QBrush(QColor(0,255,0,150));
        pen_selected = QPen(QColor(0,0,255,200));
        pen_selected.setWidth(pw_selected);
        brush_hidden = QBrush(QColor(150,255,150,50));
        pen_hidden = QPen(QColor(150,255,150,100));
        break;
    case GraphicsItemDataColor_Red:
        brush_default = QBrush(QColor(255,100,0,50));
        pen_default = QPen(QColor(255,0,0,200));
        pen_default.setWidth(pw_default);
        brush_active = QBrush(QColor(255,100,100,150));
        pen_active = QPen(QColor(255,0,0,200));
        pen_active.setWidth(pw_active);
        brush_selected = QBrush(QColor(255,0,0,150));
        pen_selected = QPen(QColor(0,255,0,200));
        pen_selected.setWidth(pw_selected);
        brush_hidden = QBrush(QColor(255,150,150,50));
        pen_hidden = QPen(QColor(255,150,150,100));
        break;
    case GraphicsItemDataColor_Purple:
        brush_default = QBrush(QColor(255,100,255,50));
        pen_default = QPen(QColor(255,0,255,200));
        pen_default.setWidth(pw_default);
        brush_active = QBrush(QColor(255,100,255,150));
        pen_active = QPen(QColor(255,0,255,200));
        pen_active.setWidth(pw_active);
        brush_selected = QBrush(QColor(255,0,255,150));
        pen_selected = QPen(QColor(0,255,255,200));
        pen_selected.setWidth(pw_selected);
        brush_hidden = QBrush(QColor(255,150,255,50));
        pen_hidden = QPen(QColor(255,150,255,100));
        break;
    case GraphicsItemDataColor_Yellow:
        brush_default = QBrush(QColor(255,255,0,50));
        pen_default = QPen(QColor(255,255,0,200));
        pen_default.setWidth(pw_default);
        brush_active = QBrush(QColor(255,255,100,150));
        pen_active = QPen(QColor(255,255,0,200));
        pen_active.setWidth(pw_active);
        brush_selected = QBrush(QColor(255,255,0,150));
        pen_selected = QPen(QColor(255,0,255,200));
        pen_selected.setWidth(pw_selected);
        brush_hidden = QBrush(QColor(255,255,150,50));
        pen_hidden = QPen(QColor(255,255,150,100));
        break;
    case GraphicsItemDataColor_LightBlue://!light blue
        brush_default = QBrush(QColor(100,255,255,50));
        pen_default = QPen(QColor(0,255,255,200));
        pen_default.setWidth(pw_default);
        brush_active = QBrush(QColor(100,255,255,150));
        pen_active = QPen(QColor(0,255,255,200));
        pen_active.setWidth(pw_active);
        brush_selected = QBrush(QColor(0,255,255,150));
        pen_selected = QPen(QColor(255,255,0,200));
        pen_selected.setWidth(pw_selected);
        brush_hidden = QBrush(QColor(150,255,255,50));
        pen_hidden = QPen(QColor(150,255,255,100));
        break;
    default: //!Red
        brush_default = QBrush(QColor(255,100,0,50));
        pen_default = QPen(QColor(255,0,0,200));
        pen_default.setWidth(pw_default);
        brush_active = QBrush(QColor(255,100,100,150));
        pen_active = QPen(QColor(255,0,0,200));
        pen_active.setWidth(pw_active);
        brush_selected = QBrush(QColor(255,0,0,150));
        pen_selected = QPen(QColor(0,255,0,200));
        pen_selected.setWidth(pw_selected);
        brush_hidden = QBrush(QColor(255,150,150,50));
        pen_hidden = QPen(QColor(255,150,150,100));
        break;
    }
}

/*!
 * \brief 引数なしのコンストラクタでは（赤がせっとされる）
 * コンストラクタにてQGraqphicsPolygonItemの実体をnewする
 */
GraphicsItemData::GraphicsItemData()
{
    _item = NULL;
    _item = new QGraphicsPolygonItem();
    setColorPreset(GraphicsItemDataColor_Red);
}

/*!
 * \brief 色指定付きのコンストラクタ
 * \param color
 */
GraphicsItemData::GraphicsItemData(GraphicsItemDataColor color)
{
    _item = NULL;
    _item = new QGraphicsPolygonItem();
    setColorPreset(color);
}

/*!
 * \brief QGraphicsPolygonItem付きで生成される場合にはそのアイテムを使用する
 * \param item
 */
void GraphicsItemData::setGraphicsPolygonItem(QGraphicsPolygonItem* item)
{
    _item = item;
}

/*!
 * \brief 渡されたポリゴンを、クラスで保持しているQGraphicsPolygonItemに反映する
 * \param polygon 画像上の座標列
 * \param width 画像幅
 * \param height 画像高さ
 */
void GraphicsItemData::setPolygon(QPolygonF &polygon, int width, int height)
{
    if(_item == NULL) return;
    _item->setPolygon(polygon);
    //!画像に対する相対座標を計算
    _relativePosition = calcRelativePosition(polygon, width, height);
}

/*!
 * \brief GraphicsItemData::setRelativePolygon
 * \param polygon 画像幅・高さに対する相対表現の座標列
 * \param width 画像幅
 * \param height 画像高さ
 */
void GraphicsItemData::setRelativePolygon(QPolygonF &polygon, int width, int height)
{
    if(_item == NULL) return;
    _relativePosition = polygon;
    //!相対表現の座標列を絶対座標の座標列に変換してセットする
    _item->setPolygon(calcAbsolutePosition(polygon, width, height));
}

QGraphicsPolygonItem* GraphicsItemData::item()
{
    return _item;
}

/*!
 * \brief アイテムを通常状態の色に設定
 */
void GraphicsItemData::colorDefault()
{
    if(_item == NULL) return;
    _item->setBrush(col.brush_default);
    _item->setPen(col.pen_default);
}

/*!
 * \brief アイテムをアクティブ状態の色に設定
 */
void GraphicsItemData::colorActive()
{
    if(_item == NULL) return;
    _item->setBrush(col.brush_active);
    _item->setPen(col.pen_active);
}
/*!
 * \brief アイテムを隠ぺい状態の色に設定
 */
void GraphicsItemData::colorHidden()
{
    if(_item == NULL) return;
    _item->setBrush(col.brush_hidden);
    _item->setPen(col.pen_hidden);
}
/*!
 * \brief 選択状態の色に設定
 */
void GraphicsItemData::colorBrushSelected()
{
    if(_item == NULL) return;
    _item->setBrush(col.brush_selected);
    _item->setPen(col.pen_default);
}

/*!
 * \brief 選択状態の色に設定
 */
void GraphicsItemData::colorSelected()
{
    if(_item == NULL) return;
    _item->setBrush(col.brush_selected);
    _item->setPen(col.pen_selected);
}





void GraphicsItemData::setColorPreset(GraphicsItemDataColor color)
{
    col.setColorPreset(color);
}


/*!
 * \brief 直接座標から（画像の縦横に対して）相対的な表現の座標に変換する
 * \param absolutePosition 座標列
 * \param width 画像幅
 * \param height 画像高さ
 * \return 相対表現の座標列
 */
QPolygonF calcRelativePosition(QPolygonF absolutePosition, int width, int height)
{
    QPolygonF relativePosition;
    QPointF absolutePoint;
    QPointF relativePoint;
    for(int i=0; i<absolutePosition.size(); i++){
        absolutePoint = absolutePosition.at(i);
        relativePoint.setX(absolutePoint.x()/width);
        relativePoint.setY(absolutePoint.y()/height);
        relativePosition.push_back(relativePoint);
    }
    return relativePosition;
}

/*!
 * \brief calcAbsolutePosition （画像の縦横に対して）相対的な表現の座標から直接座標に変換する
 * \param relativePosition　相対表現の座標列
 * \param width 画像幅
 * \param height 画像高さ
 * \return 直接座標列
 */
QPolygonF calcAbsolutePosition(QPolygonF relativePosition, int width, int height)
{
    QPolygonF absolutePosition;
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

