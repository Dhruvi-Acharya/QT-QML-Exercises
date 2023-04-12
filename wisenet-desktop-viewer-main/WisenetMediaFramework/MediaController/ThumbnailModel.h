/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
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

#ifndef THUMBNAILMODEL_H
#define THUMBNAILMODEL_H

#include <QObject>
#include <QImage>
#include <QMap>
#include <QDebug>
#include <QTimer>

struct ThumbnailParam {
    // 타임라인 시작~끝시간 범위의 섬네일 한장에 대한 설정 파라미터
    int thumbnailWidth = 0;   // 섬네일 한장의 width
    int thumbnailHeight = 0;  // 섬네일 한장의 height
    int column = 0;   // front&rear 섬네일에 포함 된 단일 섬네일의 수
    qint64 thumbnailTimeRange = 0;    // 섬네일 구간의 시작시간~끝시간 범위 (msec). m_visibleTimeRange보다 조금 더 길다.
};

class ThumbnailModel : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged)
    Q_PROPERTY(QImage frontThumbnail READ frontThumbnail WRITE setFrontThumbnail NOTIFY frontThumbnailChanged)
    Q_PROPERTY(QImage rearThumbnail READ rearThumbnail WRITE setRearThumbnail NOTIFY rearThumbnailChanged)
    Q_PROPERTY(QImage hoveredThumbnail READ hoveredThumbnail WRITE setHoveredThumbnail NOTIFY hoveredThumbnailChanged)
    Q_PROPERTY(float frontThumbnailPosition READ frontThumbnailPosition WRITE setFrontThumbnailPosition NOTIFY frontThumbnailPositionChanged)
    Q_PROPERTY(float rearThumbnailPosition READ rearThumbnailPosition WRITE setRearThumbnailPosition NOTIFY rearThumbnailPositionChanged)
    Q_PROPERTY(bool hoveredThumbnailVisible READ hoveredThumbnailVisible WRITE setHoveredThumbnailVisible NOTIFY hoveredThumbnailVisibleChanged)
    Q_PROPERTY(int thumbnailWidth READ thumbnailWidth WRITE setThumbnailWidth NOTIFY thumbnailWidthChanged)
    Q_PROPERTY(int thumbnailHeight READ thumbnailHeight WRITE setThumbnailHeight NOTIFY thumbnailHeightChanged)    
    Q_PROPERTY(float mouseX READ mouseX WRITE setMouseX)
    Q_PROPERTY(float selectRectanglePosition READ selectRectanglePosition WRITE setSelectRectanglePosition NOTIFY selectRectanglePositionChanged)
    Q_PROPERTY(bool selectRectangleVisible READ selectRectangleVisible WRITE setSelectRectangleVisible NOTIFY selectRectangleVisibleChanged)
    Q_PROPERTY(bool isLoading READ isLoading WRITE setIsLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(bool thumbnailCap READ thumbnailCap WRITE setThumbnailCap NOTIFY thumbnailCapChanged)

public:
    ThumbnailModel();
    ~ThumbnailModel();

    //QString key() const;
    QImage frontThumbnail() const;
    QImage rearThumbnail() const;
    QImage hoveredThumbnail() const;
    float frontThumbnailPosition() const;
    float rearThumbnailPosition() const;
    bool hoveredThumbnailVisible() const;
    int thumbnailWidth() const;
    int thumbnailHeight() const;
    float mouseX() const;
    float selectRectanglePosition() const;
    bool selectRectangleVisible() const;
    bool isLoading() const;
    bool thumbnailCap() const;

    //void setKey(const QString& key);
    void setFrontThumbnail(const QImage &image);
    void setRearThumbnail(const QImage &image);
    void setHoveredThumbnail(const QImage &image);
    void setFrontThumbnailPosition(const float position);
    void setRearThumbnailPosition(const float position);
    void setHoveredThumbnailVisible(const bool visible);
    void setThumbnailWidth(const int width);
    void setThumbnailHeight(const int height);
    void setMouseX(const float mouseX);
    void setSelectRectanglePosition(const float position);
    void setSelectRectangleVisible(const bool visible);
    void setIsLoading(const bool isLoading);
    void setThumbnailCap(const bool cap);

    Q_INVOKABLE void refreshTimelineThumbnail(const QString& deviceID, const QString& channelID, const int overlappedId, qint64 visibleStartTime, qint64 visibleEndTime,
                                              float timelineWidth, int thumbnailHeight);
    Q_INVOKABLE void refreshHoveredThumbnail(const QString& deviceID, const QString& channelID, const int overlappedId,
                                             qint64 time, bool useThumbnailParam);
    Q_INVOKABLE void checkThumbnailCap(const QString& deviceID);

private:
    void clearThumbnail(QString key = "");
    void getAdditionalTimelineThumbnail(QString key, QList<qint64> timeList);
    void onRequestTimerTimeout();

signals:
    //void keyChanged();
    void frontThumbnailChanged();
    void rearThumbnailChanged();
    void hoveredThumbnailChanged();
    void frontThumbnailPositionChanged();
    void rearThumbnailPositionChanged();
    void hoveredThumbnailVisibleChanged();
    void thumbnailWidthChanged();
    void thumbnailHeightChanged();
    void selectRectanglePositionChanged();
    void selectRectangleVisibleChanged();
    void additionalThumbnailLoaded(QString key);    // 추가 섬네일 로드 signal -> qml에서 받아서 현재 표시범위 기준으로 redraw 요청
    void isLoadingChanged();
    void thumbnailCapChanged();

private:
    QString m_key = ""; // 현재 선택 된 key(DeviceID_ChannelID_OverlappedID)
    qint64 m_visibleTimeRange = 0;  // 타임라인 표시 범위(msec)
    QMap<QString, QMap<qint64, QImage>> m_thumbnailMap;  // firstKey:DeviceID_ChannelID_OverlappedID, secondKey:startTime
    QMap<QString, ThumbnailParam> m_thumbnailParamMap;   // key:DeviceID_ChannelID_OverlappedID
    QImage m_frontThumbnail;    // 타임라인 시작 시간에 걸친 이미지 (섬네일 여러장 가로 배치)
    QImage m_rearThumbnail;     // 타임라인 끝 시간에 걸친 이미지 (섬네일 여러장 가로 배치)
    QImage m_hoveredThumbnail;  // 타임라인 MouseOver시 표시 할 이미지 (섬네일 1장)
    float m_frontThumbnailPosition = 0; // frontThumbnail의 표시 위치 (x좌표)
    float m_rearThumbnailPosition = 0;  // rearThumbnail의 표시 위치 (x좌표)
    bool m_hoveredThumbnailVisible = false; // hovered Thumbnail 표시 여부
    int m_thumbnailWidth = 0;   // 섬네일 한장의 width
    int m_thumbnailHeight = 0;  // 섬네일 한장의 height
    int m_sequenceNumber = 0;   // 섬네일 요청 응답 전 섬네일이 초기화 된 경우, 기존 응답을 무시하기 위한 시퀀스 번호
    QList<qint64> m_requestedTimeList;  // 장비에 섬네일 요청 후 응답 대기중인 시간 리스트
    QList<qint64> m_waitTimeList;   // 장비에 섬네일 요청 전 대기중인 시간 리스트
    bool m_hoveredRequested = false;    // HoveredThumbnail 요청 후 응답 대기상태
    qint64 m_hoveredTime = 0;   // HoveredThumbnail 요청 시간
    float m_mouseX = 0;
    float m_selectRectanglePosition = 0;
    bool m_selectRectangleVisible = false;
    bool m_isLoading = false;
    bool m_thumbnailCap = false;

    QTimer m_requestTimer;
};

#endif // THUMBNAILMODEL_H
