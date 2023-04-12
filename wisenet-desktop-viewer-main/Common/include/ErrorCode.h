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

#include <string>

#define WISENET_UNUSED_PARAM(x) (void)x;

namespace Wisenet
{

enum class ErrorCode
{
    /// No Error
    NoError = 0,

    MovedPermanently,

    /// Invalid Request
    InvalidRequest,

    /// Did not response in time
    NoResponse,

    /// Authentication failure
    UnAuthorized,

    /// Network error
    NetworkError,

    /// Not support device
    InCompatibleDevice,

    /// Exceeded the maximum number of login attempts
    AccountBlocked,

    /// Managemenet Database transaction error
    ManagementDatabaseError,

    /// log Database transaction error
    LogDatabaseError,

    StrongPasswordRequired,

    /// Can not access resource currently
    UserFull,

    /// user doesn't have permission to access the resource
    PermissionDenied,

    //Firmware update or Config file is wrong
    InCompatibleFile,
    //Firmware update or Config is failed
    UpdateFail,

    DDNSError,

    // Failed to connect to the Cloud.
    CloudConnectError,
    CloudInvalidServerValue,
    CloudAddingUserError,
    CloudAddingDeviceError,

    // Failed to connect in P2P mode.
    P2PConnectError,
    RELAYConnectError,

    BackupServiceError,
    RestoreServiceError,
    ResetServiceError,
    DeviceMismatch, //등록된 장비와 연결된 장비가 채널수 or 타입이 불일치
    AlreadyRegistered,
    SystemMenuUsed,

    FwupdateMediaError,

    MutualAuthenticationError,
    FwupdateCanceled,

    NotExistID,

    LdapUser,
    LdapSuccess,
    LdapConnectionError,
};

inline std::string GetErrorString(const ErrorCode ec)
{
    switch (ec) {
    case ErrorCode::NoError :           return "Success";
    case ErrorCode::MovedPermanently:   return "Moved Permanently";
    case ErrorCode::InvalidRequest:     return "Invalid request";
    case ErrorCode::NoResponse:         return "No response";
    case ErrorCode::UnAuthorized:       return "Authentication failed";
    case ErrorCode::NetworkError:       return "Network error";
    case ErrorCode::InCompatibleDevice: return "Not support device";
    case ErrorCode::AccountBlocked:     return "Account Blocked";
    case ErrorCode::ManagementDatabaseError: return "Management database Error";
    case ErrorCode::StrongPasswordRequired: return "Strong Password is required";
    case ErrorCode::UserFull:           return "User Full";
    case ErrorCode::PermissionDenied:           return "Permission Denied";
    case ErrorCode::InCompatibleFile:           return "InCompatible File";
    case ErrorCode::UpdateFail:           return "UpdateFail";
    case ErrorCode::DDNSError:           return "DDNS Error";
    case ErrorCode::CloudConnectError:              return "Failed to connect to Wisenet Cloud";
    case ErrorCode::CloudInvalidServerValue:        return "Failed to get P2P serverinformation from Wisenet Cloud.";
    case ErrorCode::CloudAddingUserError:           return "Failed to add user to Wisenet Cloud";
    case ErrorCode::CloudAddingDeviceError:         return "Failed to add device to Wisenet Cloud";
    case ErrorCode::P2PConnectError:                return "Failed to connect via P2P";
    case ErrorCode::BackupServiceError:           return "Backup Service Error";
    case ErrorCode::RestoreServiceError:           return "Restore Service Error";
    case ErrorCode::ResetServiceError:           return "Reset Service Error";
    case ErrorCode::DeviceMismatch:           return "Device mismatch Error";
    case ErrorCode::AlreadyRegistered:           return "Already Registered";
    case ErrorCode::SystemMenuUsed:           return "System Menu Used";
    case ErrorCode::FwupdateMediaError:       return "Fwupdate Media Error";
    case ErrorCode::MutualAuthenticationError:       return "Mutual Authentication Error";
    case ErrorCode::FwupdateCanceled:       return "Fwupdate canceled";
    case ErrorCode::NotExistID:             return "ID does not exist.";
    default:                            return "Unrecognized error";
    }
}

}
