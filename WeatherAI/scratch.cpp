#include <QCoreApplication>
#include <QUrl>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QUrl url("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=12345");
    qDebug() << url.toString();
    qDebug() << url.toEncoded();
    return 0;
}
