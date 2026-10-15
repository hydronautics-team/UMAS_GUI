#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QTimer>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Hydronautics UMAS Control Station"));
    parser.addHelpOption();
    QCommandLineOption screenshotOption(
        QStringList{QStringLiteral("s"), QStringLiteral("screenshot")},
        QStringLiteral("Save a UI screenshot and exit."), QStringLiteral("path"));
    QCommandLineOption windowSizeOption(
        QStringList{QStringLiteral("window-size")},
        QStringLiteral("Set the initial window size, for example 1366x768."),
        QStringLiteral("widthxheight"));
    parser.addOption(screenshotOption);
    parser.addOption(windowSizeOption);
    parser.process(a);

    MainWindow w;
    if (parser.isSet(windowSizeOption))
    {
        const QStringList dimensions = parser.value(windowSizeOption).split(QLatin1Char('x'));
        if (dimensions.size() == 2)
        {
            bool widthOk = false;
            bool heightOk = false;
            const int width = dimensions.at(0).toInt(&widthOk);
            const int height = dimensions.at(1).toInt(&heightOk);
            if (widthOk && heightOk && width > 0 && height > 0)
                w.resize(width, height);
        }
    }
    w.show();

    if (parser.isSet(screenshotOption))
    {
        const QString path = parser.value(screenshotOption);
        QTimer::singleShot(1200, &w,
                           [&w, path, &a]()
                           {
                               const bool saved = w.grab().save(path);
                               a.exit(saved ? 0 : 2);
                           });
    }
    return a.exec();
}
