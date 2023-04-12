/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once

#include <QObject>
#include <memory>
#include <vector>

#include <dynlink_loader.h>

struct CudaDeviceSession
{
    CudaDeviceSession(CudaFunctions *cuda, CuvidFunctions *cuvid);
    ~CudaDeviceSession();

    CUdevice cuDevice = 0;
    CUcontext cuContext = nullptr;
    int majorVersion = 0;
    int minorVersion = 0;
    std::string deviceName;
    CudaFunctions *m_cuda = NULL;
    CuvidFunctions *m_cuvid = NULL;
};

typedef std::shared_ptr<CudaDeviceSession> CudaDeviceSessionPtr;

class CudaManager final : public QObject
{
    Q_OBJECT
public:
    static CudaManager &Instance()
    {
        static CudaManager instance;
        return instance;
    }

    bool Start();
    void Stop();
    bool IsSupport();
    CudaFunctions * cuda()
    {
        return m_cuda;
    }

    CuvidFunctions * cuvid()
    {
        return m_cuvid;
    }

    CudaDeviceSessionPtr GetCudaSession();
private:
    explicit CudaManager(QObject *parent = nullptr);
    ~CudaManager();

    Q_DISABLE_COPY_MOVE(CudaManager)
private:
    std::vector<CudaDeviceSessionPtr> m_cudaSessions;
    CudaFunctions *m_cuda = NULL;
    CuvidFunctions *m_cuvid = NULL;
};


