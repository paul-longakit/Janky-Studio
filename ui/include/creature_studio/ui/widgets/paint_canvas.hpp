#pragma once

#include <creature_studio/painting/brush.hpp>
#include <creature_studio/painting/paint_document.hpp>

#include <QWidget>

namespace creature_studio
{

class PaintCanvas final : public QWidget
{
public:
explicit PaintCanvas(
painting::PaintDocument& document,
QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void paintAt(const QPoint& position);

    painting::PaintDocument& m_document;
    painting::Brush m_brush;
    bool m_drawing{false};

};

} // namespace creature_studio
