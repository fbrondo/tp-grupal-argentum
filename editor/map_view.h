#pragma once
#include <QGraphicsView>
#include <QPoint>

// QGraphicsView subclass that adds:
//   - Scroll wheel  → zoom in / zoom out centered on cursor
//   - Middle button → move the canvas by dragging
class MapView: public QGraphicsView {
    Q_OBJECT
public:
    explicit MapView(QGraphicsScene* scene, QWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPoint last_mov_post_;
};
