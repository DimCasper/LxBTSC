#ifndef LYCHEEUTILS_H
#define LYCHEEUTILS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QImage>

class LycheeUtils : public QObject
{
    Q_OBJECT

public:
    LycheeUtils(QObject *parent = nullptr);
    ~LycheeUtils();
    bool upload(QString path);

    static void setUrl(QString url) { LycheeUtils::url = url; }
    static void setUsername(QString username) { LycheeUtils::username = username; }
    static void setPassword(QString password) { LycheeUtils::password = password; }

private:
    static QString url;
    static QString username;
    static QString password;

    QNetworkAccessManager qnam;
    QNetworkReply* reply;
    QString path;

    void setCommonHeader(QNetworkRequest &req);
    void init();
    void login();
    void upload();
    void getUrl(QString id);
    void logout();

    bool setError(bool forceError = false);

    void debugPrint(QString r);

signals:
    void finished(LycheeUtils*, QString picUrl);

private slots:
    void initFinished();
    void loginFinished();
    void uploadFinished();
    void getUrlFinished();
    void logoutFinished();
    void sslErrors(const QList<QSslError> &errors);
};

#endif // LYCHEEUTILS_H
