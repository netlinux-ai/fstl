#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFileOpenEvent>

#include "app.h"
#include "canvas.h"
#include "window.h"

App::App(int& argc, char* argv[]) : QApplication(argc, argv), window(new Window())
{
    QCommandLineParser parser;
    parser.setApplicationDescription("A fast STL file viewer");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption isoOption(QStringList() << "iso" << "isometric",
                                 "Start with isometric view (Z up, Y away, X toward)");
    parser.addOption(isoOption);

    QCommandLineOption orthoOption(QStringList() << "ortho" << "orthographic",
                                   "Start in orthographic projection mode");
    parser.addOption(orthoOption);

    QCommandLineOption perspOption("perspective",
                                   "Start in perspective projection mode");
    parser.addOption(perspOption);

    QCommandLineOption screenshotOption(QStringList() << "screenshot",
                                        "Save screenshot to <file> and exit",
                                        "file");
    parser.addOption(screenshotOption);

    parser.addPositionalArgument("file", "STL file to open");

    parser.process(*this);

    if (parser.isSet(isoOption)) {
        window->setInitialView(isoview);
    }
    if (parser.isSet(orthoOption)) {
        window->setInitialProjection(false);
    } else if (parser.isSet(perspOption)) {
        window->setInitialProjection(true);
    }
    if (parser.isSet(screenshotOption)) {
        window->setScreenshotPath(parser.value(screenshotOption));
    }

    const QStringList positional = parser.positionalArguments();
    if (!positional.isEmpty()) {
        QString filename = positional.first();
        if (filename.startsWith("~")) {
            filename.replace(0, 1, QDir::homePath());
        }
        window->load_stl(filename);
    } else {
        window->load_stl(":gl/sphere.stl");
    }
    window->show();
}

App::~App()
{
    delete window;
}

bool App::event(QEvent* e)
{
    if (e->type() == QEvent::FileOpen) {
        window->load_stl(static_cast<QFileOpenEvent*>(e)->file());
        return true;
    } else {
        return QApplication::event(e);
    }
}
