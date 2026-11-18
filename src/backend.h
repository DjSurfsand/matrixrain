#pragma once

#include <QObject>
#include <QString>
#include <memory>

class QSettings;

// The app's one job: the on/off state of the rain and the two knobs that
// shape it. Values are remembered between launches.
class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(int density READ density WRITE setDensity NOTIFY densityChanged)
    Q_PROPERTY(QString glyphs READ glyphs CONSTANT)

public:
    // An empty path uses the normal user config; tests pass a temp file.
    explicit Backend(const QString &settingsPath = {}, QObject *parent = nullptr);
    ~Backend() override;

    bool enabled() const { return m_enabled; }
    int speed() const { return m_speed; }
    int density() const { return m_density; }
    QString glyphs() const { return m_glyphs; }

    Q_INVOKABLE void toggle();
    Q_INVOKABLE void setEnabled(bool value);
    Q_INVOKABLE void setSpeed(int value);
    Q_INVOKABLE void setDensity(int value);

signals:
    void enabledChanged();
    void speedChanged();
    void densityChanged();

private:
    std::unique_ptr<QSettings> m_settings;
    bool m_enabled = true;
    int m_speed = 5;
    int m_density = 6;
    QString m_glyphs;
};
