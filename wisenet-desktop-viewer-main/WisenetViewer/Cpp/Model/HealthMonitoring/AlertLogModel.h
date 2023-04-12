#pragma once

#include <QObject>

class AlertLogModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(int objectType READ objectType WRITE setObjectType NOTIFY objectTypeChanged)
    Q_PROPERTY(long long occurrenceTime READ occurrenceTime WRITE setOccurrenceTime NOTIFY occurrenceTimeChanged)
    Q_PROPERTY(int errorType READ errorType WRITE setErrorType NOTIFY errorTypeChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QString recommendedAction READ recommendedAction WRITE setRecommendedAction NOTIFY recommendedActionChanged)

public:
    explicit AlertLogModel(QObject *parent = nullptr);

    QString uuid(){ return m_uuid; }
    int objectType(){ return m_objectType; }
    long long occurrenceTime(){ return m_occurrenceTime; }
    int errorType(){ return m_errorType; }
    QString errorMessage(){ return m_errorMessage; }
    QString recommendedAction(){ return m_recommendedAction; }

    void setUuid(QString uuid){ m_uuid = uuid; }
    void setObjectType(int objectType){ m_objectType = objectType; }
    void setOccurrenceTime(long long occurrenceTime){ m_occurrenceTime = occurrenceTime; }
    void setErrorType(int errorType){ m_errorType = errorType; }
    void setErrorMessage(QString errorMessage){ m_errorMessage = errorMessage; }
    void setRecommendedAction(QString recommendedAction){ m_recommendedAction = recommendedAction; }

signals:
    void uuidChanged();
    void objectTypeChanged();
    void occurrenceTimeChanged();
    void errorTypeChanged();
    void errorMessageChanged();
    void recommendedActionChanged();

private:
    QString m_uuid;
    int m_objectType = 0;
    long long m_occurrenceTime = 0;
    int m_errorType = 0;
    QString m_errorMessage;
    QString m_recommendedAction;
};
