#include "backend.h"

#include <QFileInfo>
#include <QSettings>

namespace {
const char *const kGlyphs =
    "アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

Backend::Backend(const QString &settingsPath, QObject *parent)
    : QObject(parent),
      m_settings(settingsPath.isEmpty()
                     ? std::make_unique<QSettings>("matrixrain", "matrixrain")
                     : std::make_unique<QSettings>(settingsPath, QSettings::IniFormat)),
      m_enabled(m_settings->value("enabled", true).toBool()),
      m_speed(m_settings->value("speed", 5).toInt()),
      m_density(m_settings->value("density", 6).toInt()),
      m_glyphs(QString::fromUtf8(kGlyphs)) {
    // A hand-edited config must not be able to freeze or blank the effect.
    m_speed = qBound(1, m_speed, 10);
    m_density = qBound(1, m_density, 10);

    // The config is also a control surface: editing it from a shell or a
    // script must reach the running app, or disabling it appears to do
    // nothing while the overlay keeps rendering. QFileSystemWatcher drops a
    // path when the file is replaced, so re-arm after every change.
    m_reloadTimer.setSingleShot(true);
    m_reloadTimer.setInterval(100);
    connect(&m_settingsWatcher, &QFileSystemWatcher::fileChanged, this, [this] { m_reloadTimer.start(); });
    connect(&m_settingsWatcher, &QFileSystemWatcher::directoryChanged, this, [this] { m_reloadTimer.start(); });
    connect(&m_reloadTimer, &QTimer::timeout, this, &Backend::reloadSettings);
    armWatcher();
}

void Backend::armWatcher() {
    const auto paths = m_settingsWatcher.files() + m_settingsWatcher.directories();
    if (!paths.isEmpty())
        m_settingsWatcher.removePaths(paths);
    const QString file = m_settings->fileName();
    if (QFileInfo::exists(file))
        m_settingsWatcher.addPath(file);
    const QString directory = QFileInfo(file).absolutePath();
    if (QFileInfo::exists(directory))
        m_settingsWatcher.addPath(directory);
}

void Backend::reloadSettings() {
    armWatcher();
    m_settings->sync();
    const bool enabled = m_settings->value("enabled", true).toBool();
    const int speed = qBound(1, m_settings->value("speed", 5).toInt(), 10);
    const int density = qBound(1, m_settings->value("density", 6).toInt(), 10);
    if (enabled != m_enabled) {
        m_enabled = enabled;
        emit enabledChanged();
    }
    if (speed != m_speed) {
        m_speed = speed;
        emit speedChanged();
    }
    if (density != m_density) {
        m_density = density;
        emit densityChanged();
    }
}

Backend::~Backend() = default;

void Backend::toggle() { setEnabled(!m_enabled); }

void Backend::setEnabled(bool value) {
    if (value == m_enabled)
        return;
    m_enabled = value;
    m_settings->setValue("enabled", value);
    m_settings->sync();
    emit enabledChanged();
}

void Backend::setSpeed(int value) {
    value = qBound(1, value, 10);
    if (value == m_speed)
        return;
    m_speed = value;
    m_settings->setValue("speed", value);
    m_settings->sync();
    emit speedChanged();
}

void Backend::setDensity(int value) {
    value = qBound(1, value, 10);
    if (value == m_density)
        return;
    m_density = value;
    m_settings->setValue("density", value);
    m_settings->sync();
    emit densityChanged();
}
