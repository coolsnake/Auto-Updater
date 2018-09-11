# Auto-Updater

A Qt C++ project that allows application to download updated installer and install it automatically.

## Built with
[Qt5](https://www.qt.io/developers/) - Cross-platform software development for embedded & desktop

## Getting Started
These instructions will help you add this plugin to your project.

### Prerequisites
Things needed for this plugin:
* libeay32.dll or libeay64.dll
* ssleay32.dll or ssleay64.dll
* openssl.exe

### Adding plugin to project
You also need to add "network" in your project's .pro
```
QT += network
```
Then add the Auto-Updater plugin to your project. Copy the this plugin to your project's destination.
```
include(<project-destination>/Auto_Updater.pri)
```
Run QMake and you will now see the Auto-Updater folder included in your project

# Public Functions
* setDownloadPath(QString) - Downloaded file will be saved in this path.
* setUpdaterIcon(QPixmap) - Set your application's icon.
* setFileInformation(QString, QString) - Set the Filename and File Extension of the downloaded file respectively.
* setDownloadURL(QString) - Set the download URL of the file.

### Sample Implementation
```
Auto_Updater autoUpdate = new Auto_Updater();
autoUpdate->setDownloadURL("<download-url>");
autoUpdate->setDownloadPath("<downloaded-file-destination>");
autoUpdate->setFileInformation("<filename>", "<file-extension>");
autoUpdate->show();
