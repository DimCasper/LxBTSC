#ifndef LYCHEEUTILS_H
#define LYCHEEUTILS_H

#include <QObject>
#include <QNetworkAccessManager>

class LycheeUtils : public QObject
{
    Q_OBJECT

public:
    LycheeUtils(QString url, QString username, QString password, QObject *parent = nullptr);
    bool upload(QString path);

    void setUrl(QString url) { this->url = url; }
    void setUsername(QString username) { this->username = username; }
    void setPassword(QString password) { this->password = password; }

private:
    QNetworkAccessManager qnam;
    QNetworkReply* reply;
    QString url;
    QString path;
    QString username;
    QString password;

    void setCommonHeader(QNetworkRequest &req);
    void init();
    void login();
    void upload();
    void getUrl(QString id);
    void logout();

    bool setError();

    void debugPrint(QString r);

signals:
    void finished(QString picUrl);

private slots:
    void initFinished();
    void loginFinished();
    void uploadFinished();
    void getUrlFinished();
    void logoutFinished();
    void sslErrors(const QList<QSslError> &errors);
};

#endif // LYCHEEUTILS_H
