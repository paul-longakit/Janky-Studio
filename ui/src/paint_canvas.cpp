#include <creature_studio/ui/widgets/paint_canvas.hpp>

#include <algorithm>
#include <cmath>

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

namespace creature_studio
{

PaintCanvas::PaintCanvas(
    painting::PaintDocument& document,
    QWidget* parent)
    : QWidget(parent)
    , m_document(document)
{
    setMinimumSize(400, 400);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void PaintCanvas::setTool(Tool tool)
{
    m_tool = tool;
}

PaintCanvas::Tool PaintCanvas::tool() const
{
    return m_tool;
}

void PaintCanvas::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::darkGray);

    if (m_document.layerCount() == 0)
    {
        return;
    }

    const auto& layer = m_document.layer(0);

    const double scaleX =
        static_cast<double>(width()) /
        static_cast<double>(m_document.width());

    const double scaleY =
        static_cast<double>(height()) /
        static_cast<double>(m_document.height());

    const double scale =
        std::min(scaleX, scaleY);

    const double canvasWidth =
        static_cast<double>(m_document.width()) * scale;

    const double canvasHeight =
        static_cast<double>(m_document.height()) * scale;

    const double offsetX =
        (static_cast<double>(width()) - canvasWidth) / 2.0;

    const double offsetY =
        (static_cast<double>(height()) - canvasHeight) / 2.0;

    for (std::size_t y = 0; y < layer.height(); ++y)
    {
        for (std::size_t x = 0; x < layer.width(); ++x)
        {
            const auto& pixel = layer.pixel(x, y);

            painter.fillRect(
                QRectF(
                    offsetX + static_cast<double>(x) * scale,
                    offsetY + static_cast<double>(y) * scale,
                    scale,
                    scale),
                QColor(
                    pixel.red,
                    pixel.green,
                    pixel.blue,
                    pixel.alpha));
        }
    }
}

void PaintCanvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    m_drawing = true;
    paintAt(event->position().toPoint());
}

void PaintCanvas::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_drawing)
    {
        return;
    }

    paintAt(event->position().toPoint());
}

void PaintCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    m_drawing = false;
}

void PaintCanvas::paintAt(const QPoint& position)
{
    if (m_document.layerCount() == 0)
    {
        return;
    }

    auto& layer = m_document.layer(0);

    const double scaleX =
        static_cast<double>(width()) /
        static_cast<double>(m_document.width());

    const double scaleY =
        static_cast<double>(height()) /
        static_cast<double>(m_document.height());

    const double scale =
        std::min(scaleX, scaleY);

    const double canvasWidth =
        static_cast<double>(m_document.width()) * scale;

    const double canvasHeight =
        static_cast<double>(m_document.height()) * scale;

    const double offsetX =
        (static_cast<double>(width()) - canvasWidth) / 2.0;

    const double offsetY =
        (static_cast<double>(height()) - canvasHeight) / 2.0;

    if (position.x() < offsetX ||
        position.y() < offsetY ||
        position.x() >= offsetX + canvasWidth ||
        position.y() >= offsetY + canvasHeight)
    {
        return;
    }

    const auto x =
        static_cast<std::size_t>(
            (static_cast<double>(position.x()) - offsetX) / scale);

    const auto y =
        static_cast<std::size_t>(
            (static_cast<double>(position.y()) - offsetY) / scale);

    if (x >= layer.width() || y >= layer.height())
    {
        return;
    }

    paintPixel(layer, x, y);

    update();
}

void PaintCanvas::paintPixel(
    painting::PaintLayer& layer,
    std::size_t x,
    std::size_t y)
{
    switch (m_tool)
    {
    case Tool::Pencil:
    {
        auto& pixel = layer.pixel(x, y);

        const auto& settings =
            m_brush.settings();

        pixel.red = settings.color.red;
        pixel.green = settings.color.green;
        pixel.blue = settings.color.blue;
        pixel.alpha = settings.color.alpha;

        break;
    }

    case Tool::Brush:
        m_brush.paint(layer, x, y);
        break;

    case Tool::Eraser:
    {
        auto& pixel = layer.pixel(x, y);

        pixel.red = 0;
        pixel.green = 0;
        pixel.blue = 0;
        pixel.alpha = 0;

        break;
    }
    }
}

} // namespace creature_studio