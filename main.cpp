#include "PdfTool.h"

#include <QFile>
#include <QTranslator>
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QFile file(":/qss/silvery.css");
    if(file.open(QFile::ReadOnly))
    {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }

    QTranslator translator;
    translator.load(":/PdfTool_zh_CN.qm");
    app.installTranslator(&translator);

    QFont font("Microsoft YaHei", 9);
    app.setFont(font);

    PdfTool window;
    window.show();

    return app.exec();
}
