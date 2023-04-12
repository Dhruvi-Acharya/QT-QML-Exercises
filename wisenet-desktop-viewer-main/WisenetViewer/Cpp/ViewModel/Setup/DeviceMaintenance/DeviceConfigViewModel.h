/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#pragma once

#include <QAbstractTableModel>
#include <set>
#include <QObject>
#include <QtNetwork>
#include <QUrl>

namespace Wisenet
{
namespace Device
{
    struct Device;
    struct UpdatingChannel;
    struct UpdatingDevice;
}
};

class DeviceConfigViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int highlightRow READ highlightRow NOTIFY highlightRowChanged)
    Q_PROPERTY(int highlightRowCount READ highlightRowCount NOTIFY highlightRowCountChanged)
    Q_PROPERTY(int selectedRowCount READ selectedRowCount NOTIFY selectedRowCountChanged)
    Q_PROPERTY(bool isFirmwareUpdating READ isFirmwareUpdating NOTIFY isFirmwareUpdatingChanged)
public:
    enum DeviceConfigTitle {
        DeviceConfigDeviceId = 0,
        DeviceConfigCheck,
        DeviceConfigModel,
        DeviceConfigDeviceName,
        DeviceConfigChannelId,
        DeviceConfigChannelModel,
        DeviceConfigChannelName,
        DeviceConfigIp,
        DeviceConfigVersion,
        DeviceConfigPath,
        DeviceConfigServerUpdate,
        DeviceConfigStatus
    };
    Q_ENUM(DeviceConfigTitle);

    enum DeviceConfigViewType{
        FirmwareUpdateViewType,
        ConfigRestoreViewType,
        ConfigBackupViewType,
        PasswordChangeViewType,
    };
    Q_ENUM(DeviceConfigViewType);


    enum Role{
        HighlightRole = Qt::UserRole + 1,
        HoveredRole,
        PaintRole,
        ServerUpdateRole,
    };

    Q_INVOKABLE void reset(DeviceConfigViewType type);
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    Q_INVOKABLE void startUpdate(int row);
    Q_INVOKABLE void startFirmwareUpdate();
    Q_INVOKABLE void cancelFirmwareUpdate();

    Q_INVOKABLE bool isServerUpdateMode(int row);
    Q_INVOKABLE void setServerUpdateMode(int rowNum, bool isServerUpdateMode);
    Q_INVOKABLE void fillRecentFwVersion(int rowNum);

    Q_INVOKABLE void startConfigBackup(QString path, QString password);
    Q_INVOKABLE void startConfigRestore(bool excludeNetworkSettings, QString password);
    Q_INVOKABLE void startChangePasswordSetting(QString currentPassword, QString newPassword);

    Q_INVOKABLE void setSameFile(bool bSameChecked, int row, QString path);
    Q_INVOKABLE void deletePath(int row);
    Q_INVOKABLE void setHighlightRow(int rowNum, int modifiers);
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE void setPaintRow(int rowNum, int modifiers);
    Q_INVOKABLE bool setCheckState(int rowNum, bool checked);
    Q_INVOKABLE QVector<int> getHighlightRows() const;
    Q_INVOKABLE bool setCellValue(int rowNum, int columnNum, const QVariant& data, bool isServerUpdate = false);
    Q_INVOKABLE bool setAllCheckState(bool checked);
    Q_INVOKABLE QString translate(QVariant display);
    Q_INVOKABLE double convertType(QVariant display);
    Q_INVOKABLE bool isConfigBackupRequirePassword();
    Q_INVOKABLE bool isConfigRestoreRequirePassword();
    Q_INVOKABLE bool isSetFilePath();

    DeviceConfigViewModel(QObject* parent = nullptr);

    int highlightRow() const;
    int highlightRowCount(){ return (int)m_highlightRows.size(); }
    int selectedRowCount();
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int getCurrentRow(int row){
        for(int i=0; i< m_index.size();i++){
            if(m_index[i] == row)
                return i;
        }
        return 0;
    }
    bool isFirmwareUpdating();
    QPair<QString,QString> getDeviceRecentVersion(QString deviceModelName);
    QVariant getDownloadProgress() { return m_downloadProgress;}
    void setDownloadProgress(QVariant &downloadProgress);
    void removeDownloadedFirmware();

signals:
    void highlightRowChanged();
    void highlightRowCountChanged();
    void selectedRowCountChanged();
    void isFirmwareUpdatingChanged();
    void paintRowChanged();
    void paintRowCountChanged();

private slots:
    void httpFinished();
    void httpReadyRead();
    void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);

private:
    void UpdateSortData(int column, Qt::SortOrder order);
    QVariant getDataDisplayRole(const QModelIndex &index) const;
    QVariant getServerUpdateRole(const QModelIndex &index) const;
    void deviceFirmwareUpdate(std::string deviceId, std::string path, int row, bool stop = false);
    void channelFirmwareUpdate(bool stop=false);
    void configBackup(std::string deviceId, std::string path, std::string password, int row);
    void configRestore(std::string deviceId, std::string path, std::string password, int row, bool excludeNetworkSettings);

    void DisconnectDevice(Wisenet::Device::Device &device);
    void SaveCredential(Wisenet::Device::Device &device, int row, QString password);
    void ConnectDevice(Wisenet::Device::Device &device, int row);
    void checkAllDevicePasswordExpiry();

    void setFirmwareDownloading(bool flag);
    void getUpdateFwInfoFile();
    bool internalServerUpdateMode(int row);

    QVector<QVector<QVariant>> m_data;
    QVector<int> m_index;
    QVector<int> m_sorted;
    QVector<int> m_roles;

    QStringList m_columnNames;
    QStringList m_columnConfigBackupNames;
    QStringList m_columnChangePasswordNames;

    std::map<std::string, std::shared_ptr<Wisenet::Device::UpdatingDevice>> m_deviceFirmwareUpdatingMap;
    std::map<std::string, std::vector<std::shared_ptr<Wisenet::Device::UpdatingChannel>> > m_channelFirmwareUpdatingMap; // key:<devID> val:<chID,path>

    int m_hoveredRow;
    int m_highlightRow;
    int m_lastHighlightRow;
    int m_paintRow;
    int m_lastPaintRow;
    std::set<int> m_highlightRows;
    std::set<int> m_paintRows;
    DeviceConfigViewType m_viewType;

    QMap<QString, bool> m_deviceServerUpdateModeMap; // <key:devID or chID, val:isServerUpdateMode>
    QNetworkAccessManager qnam;
    QNetworkReply *reply = nullptr;
    std::unique_ptr<QFile> openFileForWrite(const QString &fileName);
    std::unique_ptr<QFile> file;
    QVariant m_downloadProgress;
    QString m_deviceUpdatefileName;
    QFuture<void> m_future;
    int m_row = 0;
    bool m_cancelUpdating = false;
    bool m_firmwareDownloading = false;
};

