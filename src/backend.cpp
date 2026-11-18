#include "backend.h"

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
