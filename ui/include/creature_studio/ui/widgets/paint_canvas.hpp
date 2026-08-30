#pragma once

#include <creature_studio/painting/brush.hpp>
#include <creature_studio/painting/paint_document.hpp>

#include <QWidget>
#include <QImage>
#include <QPoint>

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

    void setBrushSize(std::size_t size);
    std::size_t brushSize() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void rebuildImage();
    
    void paintAt(const QPoint& position);

    void paintPixel(
        painting::PaintLayer& layer,
        std::size_t x,
        std::size_t y);

    painting::PaintDocument& m_document;
    painting::Brush m_brush;
    Tool m_tool{Tool::Brush};
    bool m_drawing{false};
    QPoint m_lastPaintPosition;

    QImage m_image;
    bool m_imageDirty{true};
};

} // namespace creature_studio