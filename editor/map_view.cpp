#include "map_view.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QWheelEvent>

MapView::MapView(QGraphicsScene* scene, QWidget* parent): QGraphicsView(scene, parent) {}

void MapView::wheelEvent(QWheelEvent* event) {
    const double factor = event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;  // Zoom factor
    scale(factor, factor);
    event->accept();
}

void MapView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        last_mov_post_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void MapView::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::MiddleButton) {
        QPoint delta = event->pos() - last_mov_post_;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        last_mov_post_ = event->pos();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}
