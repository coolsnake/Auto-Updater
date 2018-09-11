#ifndef UPDATERMAINWINDOW_H
#define UPDATERMAINWINDOW_H

#include <QDialog>
#include <QFile>
#include <QMainWindow>
#include <QStandardPaths>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Ui {
class UpdaterMainWindow;
}

class UpdaterMainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UpdaterMainWindow(QWidget *parent = 0);
    ~UpdaterMainWindow();

    void setUpdaterIcon(QPixmap icon);
    void setCurrentAppVersion(QString version);
    void setFileInformation(QString Filename, QString FileExtension);
    void setDownloadPath(QString path);
    void setDownloadURL(QString url);
private slots:
    void setLatestVersion(QString value);
    void fileDownloaded();
    void updateProgress(qint64 downloaded, qint64 total);
    void fileReady();
    void on_btnDownload_clicked();
    void on_btnClose_clicked();

private:
    Ui::UpdaterMainWindow *ui;

    QString _filePath;
    QString _fileName;
    QString downloadPath;

    void initUI();
    void initData();
    void startUpdateProcess();
    void downloadFile(const QUrl &url);

    QNetworkAccessManager manager;
    QNetworkReply *_reply;

    QFile *file;
    QUrl _downloadURL;
    QByteArray data;
    QPixmap updaterIcon;

    QString currentAppVersion;

    bool _downloadStart = false;
    bool valueUpdated = false;
    bool isCheckingForUpdate = false;
};

#endif // UPDATERMAINWINDOW_H
