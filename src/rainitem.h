#pragma once

#include <QColor>
#include <QPixmap>
#include <QRectF>
#include <QQuickPaintedItem>
#include <QString>
#include <QTimer>
#include <QVector>

// The rain's renderer. Glyphs are rasterized once into a small pixmap atlas;
// each frame redraws the columns with blits instead of asking the text engine
// to shape thousands of characters. All state lives here so the QML file only
// describes the window.
class RainItem : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(int cell READ cell WRITE setCell NOTIFY cellChanged)
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(QString glyphs READ glyphs WRITE setGlyphs NOTIFY glyphsChanged)
    Q_PROPERTY(QColor accent READ accent WRITE setAccent NOTIFY accentChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
    explicit RainItem(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    int cell() const { return m_cell; }
    int speed() const { return m_speed; }
    QString glyphs() const { return m_glyphs; }
    QColor accent() const { return m_accent; }
    bool running() const { return m_running; }

    void setCell(int value);
    void setSpeed(int value);
    void setGlyphs(const QString &value);
    void setAccent(const QColor &value);
    void setRunning(bool value);

signals:
    void cellChanged();
    void speedChanged();
    void glyphsChanged();
    void accentChanged();
    void runningChanged();

private:
    void advance();
    void rebuildAtlas();
    void rebuildColumns();
    void clearColumn(QVector<int> &row) const;
    int glyphAt(int column, int row) const;

    QTimer m_timer;
    int m_cell = 18;
    int m_speed = 5;
    QString m_glyphs;
    QColor m_accent;
    bool m_running = false;
    bool m_atlasReady = false;

    QPixmap m_atlas;
    QVector<double> m_heads;
    QVector<double> m_progress;
    QVector<double> m_speeds;
    QVector<int> m_trails;
    QVector<QVector<int>> m_rows;
};
