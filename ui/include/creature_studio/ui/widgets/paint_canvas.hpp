#pragma once

#include <creature_studio/painting/brush.hpp>
#include <creature_studio/painting/paint_document.hpp>

#include <QWidget>

namespace creature_studio
{

class PaintCanvas final : public QWidget
{
public:
    enum class Tool
    {
        Pencil,
        Brush,
        Eraser
    };

    explicit PaintCanvas(
        painting::PaintDocument& document,
        QWidget* parent = nullptr);

    void setTool(Tool tool);
    Tool tool() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void paintAt(const QPoint& position);

    void paintPixel(
        painting::PaintLayer& layer,
        std::size_t x,
        std::size_t y);

    painting::PaintDocument& m_document;
    painting::Brush m_brush;
    Tool m_tool{Tool::Brush};
    bool m_drawing{false};
};

} // namespace creature_studio