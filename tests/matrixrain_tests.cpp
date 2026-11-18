#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include "backend.h"
#include "rainitem.h"
#include "theme.h"
#include "translator.h"

class MatrixRainTests : public QObject {
    Q_OBJECT

private slots:
    void toggleRoundTrip()
    {
        QTemporaryDir dir;
        const QString file = dir.filePath("settings.conf");
        Backend backend(file);
        QVERIFY(backend.enabled());
        backend.toggle();
        QVERIFY(!backend.enabled());
        backend.toggle();
        QVERIFY(backend.enabled());

        Backend again(file);
        QVERIFY(again.enabled());
    }

    void knobsAreClamped()
    {
        QTemporaryDir dir;
        Backend backend(dir.filePath("settings.conf"));
        backend.setSpeed(99);
        QCOMPARE(backend.speed(), 10);
        backend.setDensity(-4);
        QCOMPARE(backend.density(), 1);
        QVERIFY(!backend.glyphs().isEmpty());
    }

    void rainItemDefaultsAndClamps()
    {
        RainItem item;
        QCOMPARE(item.cell(), 18);
        QCOMPARE(item.speed(), 5);
        QVERIFY(!item.glyphs().isEmpty());
        QVERIFY(!item.running());
        item.setSpeed(99);
        QCOMPARE(item.speed(), 10);
        item.setCell(99);
        QCOMPARE(item.cell(), 30);
    }

    void themeFallsBack()
    {
        QTemporaryDir dir;
        Theme theme(dir.path());
        QCOMPARE(theme.accent(), QString("#FFD60A"));
        QCOMPARE(theme.accentForeground(), QString("black"));
    }

    void themeReadsAccent()
    {
        QTemporaryDir dir;
        QDir(dir.path()).mkpath("theme");
        QFile colors(dir.path() + "/theme/colors.toml");
        QVERIFY(colors.open(QIODevice::WriteOnly));
        colors.write("accent = \"#112233\"\n");
        colors.close();

        Theme theme(dir.path());
        QCOMPARE(theme.accent(), QString("#112233"));
        QCOMPARE(theme.accentForeground(), QString("white"));
    }

    void themeFollowsHyprlandRounding()
    {
        QTemporaryDir dir;
        const QString response = dir.filePath("response.json"), command = dir.filePath("hyprctl");
        QVERIFY(write(command, "#!/bin/sh\ncat '" + response.toUtf8() + "'\n"));
        QVERIFY(QFile::setPermissions(command, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner));
        QVERIFY(write(response, "{\"int\":0}"));

        Theme theme(dir.path(), nullptr, command);
        QCOMPARE(theme.radius(), 0);
        QVERIFY(write(response, "{\"int\":6}"));
        QTRY_COMPARE(theme.radius(), 6);
    }

    void translatorFallsBackToEnglish()
    {
        QTemporaryDir dir;
        Translator translator(dir.path() + "/language", dir.path());
        QCOMPARE(translator.language(), QString("en"));
        QVERIFY(translator.translate("Main", "Save").isNull());
    }

    void translatorFollowsTheLanguageFile()
    {
        QTemporaryDir dir;
        write(dir.path() + "/language", "pt-BR\n");
        write(dir.path() + "/pt.json", R"({"Save": "Guardar", "Open": "Abrir"})");
        write(dir.path() + "/pt-BR.json", R"({"Save": "Salvar"})");

        Translator translator(dir.path() + "/language", dir.path());
        QCOMPARE(translator.language(), QString("pt-BR"));
        QCOMPARE(translator.translate("Main", "Save"), QString("Salvar"));
        QCOMPARE(translator.translate("Main", "Open"), QString("Abrir"));
        QVERIFY(translator.translate("Main", "Quit").isNull());
    }

private:
    static bool write(const QString &path, const QByteArray &contents)
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly))
            return false;
        return file.write(contents) == contents.size();
    }
};

QTEST_MAIN(MatrixRainTests)
#include "matrixrain_tests.moc"
