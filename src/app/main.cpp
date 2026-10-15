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
    parser.addOption(screenshotOption);
    parser.process(a);

    MainWindow w;
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
