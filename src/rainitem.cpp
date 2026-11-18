#include "rainitem.h"

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QRandomGenerator>
#include <QtGlobal>

namespace {
// Bright head, pale second glyph, then progressively fainter tail cells.
const QVector<double> kTrailAlphas{0.85, 0.68, 0.50, 0.35, 0.22, 0.12};
const int kVariants = 2 + kTrailAlphas.size();
}

RainItem::RainItem(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_glyphs("アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン"
               "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
      m_accent("#FFD60A") {
    // An FBO keeps the blits on the GPU; the default Image target would upload
    // a full-screen QImage every frame.
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    m_timer.setInterval(33);
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &RainItem::advance);
    rebuildAtlas();
}

void RainItem::setCell(int value) {
    value = qBound(10, value, 30);
    if (value == m_cell)
        return;
    m_cell = value;
    emit cellChanged();
    rebuildAtlas();
    rebuildColumns();
}

void RainItem::setSpeed(int value) {
    value = qBound(1, value, 10);
    if (value == m_speed)
        return;
    m_speed = value;
    emit speedChanged();
}

void RainItem::setGlyphs(const QString &value) {
    if (value == m_glyphs || value.isEmpty())
        return;
    m_glyphs = value;
    emit glyphsChanged();
    rebuildAtlas();
    rebuildColumns();
}

void RainItem::setAccent(const QColor &value) {
    if (value == m_accent)
        return;
    m_accent = value;
    emit accentChanged();
    rebuildAtlas();
}

void RainItem::setRunning(bool value) {
    if (value == m_running)
        return;
    m_running = value;
    emit runningChanged();
    if (m_running) {
        rebuildColumns();
        m_timer.start();
    } else {
        m_timer.stop();
    }
    update();
}

void RainItem::clearColumn(QVector<int> &row) const {
    row.resize(48);
    for (int &glyph : row)
        glyph = QRandomGenerator::global()->bounded(m_glyphs.size());
}

void RainItem::rebuildColumns() {
    const int width = qMax(1, int(this->width()));
    const int height = qMax(1, int(this->height()));
    const int count = qBound(8, width / m_cell, 240);
    const int rowsToStart = height / m_cell + 4;

    m_heads.resize(count);
    m_progress.resize(count);
    m_speeds.resize(count);
    m_trails.resize(count);
    m_rows.resize(count);

    for (int i = 0; i < count; ++i) {
        m_heads[i] = QRandomGenerator::global()->bounded(rowsToStart) - 2;
        m_progress[i] = 0;
        m_speeds[i] = 0.55 + QRandomGenerator::global()->bounded(1.1);
        m_trails[i] = 12 + QRandomGenerator::global()->bounded(18);
        clearColumn(m_rows[i]);
    }
    update();
}

void RainItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) {
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    if (newGeometry.size() != oldGeometry.size())
        rebuildColumns();
}

void RainItem::rebuildAtlas() {
    if (m_glyphs.isEmpty() || m_cell <= 0)
        return;
    const int glyphCount = m_glyphs.size();
    const int w = glyphCount * m_cell;
    const int h = kVariants * m_cell;
    if (w <= 0 || h <= 0)
        return;

    QPixmap atlas(w, h);
    atlas.fill(Qt::transparent);
    QPainter painter(&atlas);
    QFont font(QStringLiteral("monospace"));
    font.setBold(true);
    font.setPixelSize(m_cell);
    painter.setFont(font);
    const int ascent = QFontMetrics(font).ascent();

    auto mixWhite = [](const QColor &c, double amount) {
        return QColor(int(c.red() + (255 - c.red()) * amount),
                      int(c.green() + (255 - c.green()) * amount),
                      int(c.blue() + (255 - c.blue()) * amount));
    };

    for (int variant = 0; variant < kVariants; ++variant) {
        QColor color;
        if (variant == 0)
            color = mixWhite(m_accent, 0.82);
        else if (variant == 1)
            color = mixWhite(m_accent, 0.4);
        else
            color = m_accent;
        color.setAlphaF(variant < 2 ? 1.0 : kTrailAlphas[variant - 2]);
        painter.setPen(color);
        for (int i = 0; i < glyphCount; ++i)
            painter.drawText(i * m_cell, variant * m_cell + ascent, m_glyphs.at(i));
    }
    painter.end();

    m_atlas = atlas;
    m_atlasReady = true;
    update();
}

int RainItem::glyphAt(int column, int row) const {
    const auto &stream = m_rows[column];
    return stream[((row % stream.size()) + stream.size()) % stream.size()];
}

void RainItem::advance() {
    if (m_heads.isEmpty())
        return;
    const int rowsPerSecond = 8 + m_speed * 4;
    const int viewRows = int(height()) / m_cell + 2;
    for (int i = 0; i < m_heads.size(); ++i) {
        m_progress[i] += rowsPerSecond * m_speeds[i] / 30.0;
        const int moved = int(m_progress[i]);
        if (moved > 0) {
            m_heads[i] += moved;
            m_progress[i] -= moved;
            m_rows[i][QRandomGenerator::global()->bounded(m_rows[i].size())] =
                QRandomGenerator::global()->bounded(m_glyphs.size());
        }
        if (m_heads[i] - m_trails[i] > viewRows) {
            m_heads[i] = -QRandomGenerator::global()->bounded(8) - 1;
            m_progress[i] = 0;
            m_speeds[i] = 0.55 + QRandomGenerator::global()->bounded(1.1);
            m_trails[i] = 12 + QRandomGenerator::global()->bounded(18);
        }
    }
    update();
}

void RainItem::paint(QPainter *painter) {
    painter->fillRect(boundingRect(), QColor(0, 0, 0, 0));
    if (!m_atlasReady || m_heads.isEmpty())
        return;

    const int cell = m_cell;
    const int glyphCount = m_glyphs.size();
    for (int i = 0; i < m_heads.size(); ++i) {
        const int head = int(m_heads[i]);
        const int trail = m_trails[i];
        for (int r = head - trail + 1; r <= head; ++r) {
            const int y = r * cell;
            if (y + cell < 0 || y > height())
                continue;
            int variant;
            if (r == head)
                variant = 0;
            else if (r == head - 1)
                variant = 1;
            else
                variant = 2 + qMin(kTrailAlphas.size() - 1,
                                   int(double(head - r - 2) / qMax(1, trail - 2) * kTrailAlphas.size()));
            const int glyph = glyphAt(i, r);
            if (glyph < 0 || glyph >= glyphCount)
                continue;
            painter->drawPixmap(QRectF(i * cell, y, cell, cell), m_atlas,
                                QRectF(glyph * cell, variant * cell, cell, cell));
        }
    }
}
