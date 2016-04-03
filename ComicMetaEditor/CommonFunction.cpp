/*!
 * \file
 */
#include "CommonFunction.h"
#include <cmath>

double calcPolygonAreaSize(QPolygonF polygon)
{
    double areaSize = 0.0;
    double x1, y1, x2, y2;
    for(int i=0; i<polygon.size(); i++){
        x1 = polygon.at(i).x();
        y1 = polygon.at(i).y();
        if(i == polygon.size() - 1){
            x2 = polygon.at(0).x();
            y2 = polygon.at(0).y();
        }
        else{
            x2 = polygon.at(i+1).x();
            y2 = polygon.at(i+1).y();
        }
        areaSize += (x1 * y2 - x2 * y1) / 2;
    }
    return fabs(areaSize);
}

double getDistance(QLineF line, QPointF pt)
{
    //! 線分lineと点ptとの距離Lを求める
    //! 参考：http://katahiromz.web.fc2.com/c/lineseg.html
    QPointF AB, AP;
    QPointF A = line.p1();
    QPointF B = line.p2();

    AB.setX(B.x() - A.x());
    AB.setY(B.y() - A.y());

    AP.setX(pt.x() - A.x());
    AP.setY(pt.y() - A.y());

    //! 線分の長さが0だった場合
    if(AB.x() == 0 && AB.y() == 0){
        return calcDistance(A, pt);
    }

    //! 通常の場合の計算
    double r2 = AB.x() * AB.x() + AB.y() * AB.y();
    double t = (AB.x() * AP.x() + AB.y() * AP.y()) / r2;

    if(t < 0){
        return calcDistance(A, pt);
    }
    else if(t > 1){
        return calcDistance(B, pt);
    }
    else{
        QPointF C;
        C.setX((1 - t) * A.x() + t * B.x());
        C.setY((1 - t) * A.y() + t * B.y());
        return calcDistance(C, pt);
    }
}

double calcDistance(QPointF pt1, QPointF pt2){
    return sqrt(pow((pt2.x() - pt1.x()), 2) + pow((pt2.y() - pt1.y()), 2));
}
double crossVector(QPointF vl, QPointF vr){
    return vl.x() * vr.y() - vl.y() * vr.x();
}


QRectF getRectBy2Pt(QPoint pt1, QPoint pt2)
{
    int ltx = std::min(pt1.x(), pt2.x());
    int width = fabs((double)pt1.x()-(double)pt2.x());
    int lty = std::min(pt1.y(), pt2.y());
    int height = fabs((double)pt1.y()-(double)pt2.y());
    QRectF rect(ltx,lty,width,height);
    return rect;
}

QPointF getPolygonCenter(QPolygonF polygon)
{
    QPolygon pt;
    double x = 0.0;
    double y = 0.0;
    for(int i=0; i<polygon.size(); i++){
        x += polygon.at(i).x();
        y += polygon.at(i).y();
    }
    x /= polygon.size();
    y /= polygon.size();
    return QPointF(x,y);
}
