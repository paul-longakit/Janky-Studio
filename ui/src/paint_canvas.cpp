#include <creature_studio/ui/widgets/paint_canvas.hpp>

#include <algorithm>
#include <cmath>

#include <QImage>
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

    if (m_imageDirty)
    {
        rebuildImage();
    }

    if (m_image.isNull())
    {
        return;
    }

    const double scaleX =
        static_cast<double>(width()) /
        static_cast<double>(m_image.width());

    const double scaleY =
        static_cast<double>(height()) /
        static_cast<double>(m_image.height());

    const double scale =
        std::min(scaleX, scaleY);

    const int canvasWidth =
        static_cast<int>(
            static_cast<double>(m_image.width()) * scale);

    const int canvasHeight =
        static_cast<int>(
            static_cast<double>(m_image.height()) * scale);

    const int offsetX =
        (width() - canvasWidth) / 2;

    const int offsetY =
        (height() - canvasHeight) / 2;

    painter.drawImage(
        QRect(
            offsetX,
            offsetY,
            canvasWidth,
            canvasHeight),
        m_image);
}

void PaintCanvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    m_drawing = true;
    m_lastPaintPosition = event->position().toPoint();

    paintAt(m_lastPaintPosition);

    m_imageDirty = true;
    update();
}

void PaintCanvas::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_drawing)
    {
        return;
    }

    const QPoint currentPosition =
        event->position().toPoint();

    const QPoint delta =
        currentPosition - m_lastPaintPosition;

    const int distance =
        std::max(
            std::abs(delta.x()),
            std::abs(delta.y()));

    if (distance <= 0)
    {
        return;
    }

    for (int step = 1; step <= distance; ++step)
    {
        const double t =
            static_cast<double>(step) /
            static_cast<double>(distance);

        const QPoint interpolatedPosition(
            static_cast<int>(
                std::round(
                    m_lastPaintPosition.x() +
                    delta.x() * t)),
            static_cast<int>(
                std::round(
                    m_lastPaintPosition.y() +
                    delta.y() * t)));

        paintAt(interpolatedPosition);
    }

    m_lastPaintPosition = currentPosition;

    m_imageDirty = true;
    update();
}

void PaintCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    m_drawing = false;
}

void PaintCanvas::rebuildImage()
{
    if (m_document.layerCount() == 0)
    {
        m_image = QImage();
        m_imageDirty = false;
        return;
    }

    const auto& layer = m_document.layer(0);

    m_image = QImage(
        static_cast<int>(layer.width()),
        static_cast<int>(layer.height()),
        QImage::Format_RGBA8888);

    for (std::size_t y = 0; y < layer.height(); ++y)
    {
        auto* scanLine =
            m_image.scanLine(static_cast<int>(y));

        for (std::size_t x = 0; x < layer.width(); ++x)
        {
            const auto& pixel = layer.pixel(x, y);

            const auto offset = x * 4;

            scanLine[offset + 0] = pixel.red;
            scanLine[offset + 1] = pixel.green;
            scanLine[offset + 2] = pixel.blue;
            scanLine[offset + 3] = pixel.alpha;
        }
    }

    m_imageDirty = false;
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

    const auto& pixel = layer.pixel(x, y);

    m_image.setPixelColor(
        static_cast<int>(x),
        static_cast<int>(y),
        QColor(
            pixel.red,
            pixel.green,
            pixel.blue,
            pixel.alpha));
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

void PaintCanvas::setBrushSize(std::size_t size)
{
    auto settings = m_brush.settings();
    settings.size = size;
    m_brush.setSettings(settings);
}

std::size_t PaintCanvas::brushSize() const
{
    return m_brush.settings().size;
}

} // namespace creature_studio