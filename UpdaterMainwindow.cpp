#include "UpdaterMainwindow.h"
#include "ui_UpdaterMainwindow.h"

#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QMessageBox>
#include <QProcess>

UpdaterMainWindow::UpdaterMainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdaterMainWindow)
{
    ui->setupUi(this);

    initUI();

}

UpdaterMainWindow::~UpdaterMainWindow()
{
    delete ui;
}

void UpdaterMainWindow::initUI()
{
#ifdef Q_OS_MACX
    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
#else
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif
    this->setModal(true);
    this->setFixedSize(372,416);
    this->setWindowTitle("Application Update");

    ui->progressBar->setVisible(false);

#ifdef Q_OS_MACX
    ui->label->setVisible(false);
    ui->label->setAlignment(Qt::AlignCenter);
#endif
}

void UpdaterMainWindow::initData()
{
    downloadPath = _filePath + _fileName;

    file = new QFile(downloadPath);
    if (!file->open(QIODevice::WriteOnly)) {
        qCritical() << file->errorString();
        return;
    }
}

void UpdaterMainWindow::downloadFile(const QUrl &url)
{
    _downloadStart = true;

    if (!isCheckingForUpdate) {
        ui->btnDownload->setEnabled(false);
        ui->progressBar->setVisible(true);
        ui->label->setVisible(true);
    }

    _reply = manager.get(QNetworkRequest(url));

    if (isCheckingForUpdate) {
        connect(_reply, SIGNAL(finished()), this, SLOT(fileDownloaded()));
    } else {
        initData();
        connect(_reply, SIGNAL(readyRead()), this, SLOT(fileReady()));
        connect(_reply, SIGNAL(finished()), this, SLOT(fileDownloaded()));
        connect(_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    }
}

void UpdaterMainWindow::fileDownloaded()
{
    qInfo() << "Network Reply:" << _reply->error();
    if (_reply->error() == QNetworkReply::NoError) {
        int replyCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        qInfo() << "Reply Code:" << _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (replyCode == 301 || replyCode == 302) {
            ui->label->setText("Redirecting...");
            QUrl redirectURL = _reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            downloadFile(redirectURL);

        } else {
                _reply->deleteLater();
                file->close();

                ui->label->setText("File Downloaded");

                startUpdateProcess();
        }
    } else {
        ui->label->hide();
        ui->progressBar->hide();
        ui->btnDownload->setEnabled(true);
        _downloadStart = false;
    }
}

void UpdaterMainWindow::updateProgress(qint64 downloaded, qint64 total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setRange(0,total);
    ui->progressBar->setFormat("%p%");
    ui->progressBar->setValue(downloaded);

    double dlProgress = ((double)downloaded / (double)total) * 100;

    QString msg;
    if (dlProgress < 10.00) {
        msg = QString::number(dlProgress, 'G', 3) + "%";
    } else {
        msg = QString::number(dlProgress, 'G', 4) + "%";
    }

    if (dlProgress > 0) {
        ui->label->setText(msg);
        qDebug() << "downloading" << msg;
    }
}

void UpdaterMainWindow::fileReady()
{
    if (_reply->error() == QNetworkReply::NoError) {
        data = _reply->readAll();
        file->write(data);
        data.clear();
    }
}

void UpdaterMainWindow::on_btnDownload_clicked()
{
    if (_fileName.isEmpty()) {
        QMessageBox::information(this, "Application Update", "No Filename Detected!");
    } else if (_filePath.isEmpty()) {
        QMessageBox::information(this, "Application Update", "No File Path Detected!");
    } else {
       downloadFile(_downloadURL);
    }
}

void UpdaterMainWindow::on_btnClose_clicked()
{
    this->close();
}


void UpdaterMainWindow::setDownloadURL(QString url)
{
    _downloadURL = QUrl(url);
}

void UpdaterMainWindow::setLatestVersion(QString value)
{
    ui->lblLatestVersion->setText(value);
    ui->lblLatestVersion->setAlignment(Qt::AlignCenter);
}

void UpdaterMainWindow::setFileInformation(QString Filename, QString FileExtension)
{
    _fileName = "/" + Filename + FileExtension;
}

void UpdaterMainWindow::setDownloadPath(QString path)
{
    _filePath = path;
}

void UpdaterMainWindow::setUpdaterIcon(QPixmap icon)
{
    updaterIcon = icon;
    this->setWindowIcon(icon);

    int width = ui->lblIcon->width();
    int height = ui->lblIcon->height();
    ui->lblIcon->setPixmap(icon.scaled(width,height,Qt::IgnoreAspectRatio));
}

void UpdaterMainWindow::startUpdateProcess()
{
    QDir exePath;
    if (exePath.exists(downloadPath)) {
        QProcess *process = new QProcess();
#ifdef Q_OS_MACX
        process->startDetached(QString("open %1").arg(downloadPath));
        process->waitForFinished();

#elif defined(Q_OS_WIN)
        process->startDetached(downloadPath);
        delete process;

#elif defined(Q_OS_LINUX)

        delete process;
#endif
        qApp->exit(2000);
    }
}
