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

#include "CoreService/CoreServiceRequestResponse.h"
#include "CoreService/CoreServiceEvent.h"
#include "DeviceClient/IDeviceClient.h"

namespace Wisenet
{
namespace Core
{

class ICoreService : public Wisenet::Device::IDeviceClient
{
public:
    virtual ~ICoreService() = default;

    virtual void Start(){}
    virtual void Stop(){}
    virtual bool IsRunning() { return true; }

    virtual void SetS1DaemonPort(unsigned short port){ boost::ignore_unused(port); }

    // Get service information
    // REQ :: GetServiceInformationRequest
    // RES :: GetServiceInformationResponse
    virtual void GetServiceInformation(RequestBaseSharedPtr const& request,
                                       ResponseBaseHandler const& responseHandler) = 0;

    // Setup service
    // REQ :: SetupServiceRequest
    // RES :: ResponseBase
    virtual void SetupService(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler) = 0;

    // Log in
    // REQ :: LoginRequest
    // RES :: ResponseBase
    virtual void Login(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) = 0;

    // Log out
    // REQ :: LogoutRequest
    virtual void Logout(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) = 0;

    // Start Event Service
    // REQ :: StartEventServiceRequest
    // RES :: ResponseBase
    virtual void StartEventService(RequestBaseSharedPtr const& request,
                                   ResponseBaseHandler const& responseHandler) = 0;


    // Add or update user group
    // REQ :: SaveUserGroupRequest
    // RES :: ResponseBase
    virtual void SaveUserGroup(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) = 0;

    // Remove user group
    // REQ :: RemoveUserGroupsRequest
    // RES :: ResponseBase
    virtual void RemoveUserGroups(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler) = 0;


    // Add or update a user
    // REQ :: SaveUserRequest
    // RES :: ResponseBase
    virtual void SaveUser(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) = 0;

    // Remove users
    // REQ :: RemoveUsersRequest
    // RES :: ResponseBase
    virtual void RemoveUsers(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) = 0;

    // Add or update a group
    // REQ :: SaveGroupRequest
    // RES :: ResponseBase
    virtual void SaveGroup(RequestBaseSharedPtr const&request,
                           ResponseBaseHandler const&responseHandler) = 0;

    // Remove groups
    // REQ :: RemoveGroupsRequest
    // RES :: ResponseBase
    virtual void RemoveGroups(RequestBaseSharedPtr const&request,
                             ResponseBaseHandler const&responseHandler) = 0;

    // Add group mapping
    // REQ :: AddGroupMappingRequest
    // RES :: ResponseBase
    virtual void AddGroupMapping(RequestBaseSharedPtr const&request,
                                 ResponseBaseHandler const&responseHandler) = 0;

    // Remove group mapping
    // REQ :: RemoveGroupMappingRequest
    // RES :: ResponseBase
    virtual void RemoveGroupMapping(RequestBaseSharedPtr const&request,
                                    ResponseBaseHandler const&responseHandler) = 0;

    // Update group mapping
    // REQ :: UpdateGroupMappingRequest
    // RES :: ResponseBase
    virtual void UpdateGroupMapping(RequestBaseSharedPtr const&request,
                                    ResponseBaseHandler const&responseHandler) = 0;

    // Add group with update group mapping
    // REQ :: AddGroupWithUpdateMappingRequest
    // RES :: ResponseBase
    virtual void AddGroupWithUpdateMapping(RequestBaseSharedPtr const&request,
                                           ResponseBaseHandler const&responseHandler) = 0;

    // Add or update a layout
    // REQ :: SaveLayoutRequest
    // RES :: ResponseBase
    virtual void SaveLayout(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) = 0;

    // Remove layouts
    // REQ :: RemoveLayoutsRequest
    // RES :: ResponseBase
    virtual void RemoveLayouts(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) = 0;

    // Add or update a sequence layout
    // REQ :: SaveSequenceLayoutRequest
    // RES :: ResponseBase
    virtual void SaveSequenceLayout(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler) = 0;

    // Remove sequence layouts
    // REQ :: RemoveSequenceLayoutsRequest
    // RES :: ResponseBase
    virtual void RemoveSequenceLayouts(RequestBaseSharedPtr const& request,
                                       ResponseBaseHandler const& responseHandler) = 0;

    // Add or update a webpage
    // REQ :: SaveWebpageRequest
    // RES :: ResponseBase
    virtual void SaveWebpage(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) = 0;

    // Remove sequence layouts
    // REQ :: RemoveWebpagesRequest
    // RES :: ResponseBase
    virtual void RemoveWebpages(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler) = 0;

    // Add a new device
    // REQ :: AddDeviceRequest
    // RES :: AddDeviceResponse
    virtual void AddDevice(RequestBaseSharedPtr const & request,
                           ResponseBaseHandler const & responseHandler) = 0;

    // Update devices
    // REQ :: UpdateDevicesRequest
    // RES :: UpdateDeviceResponseBase
    virtual void UpdateDevices(RequestBaseSharedPtr const & request,
                               ResponseBaseHandler const & responseHandler) = 0;

    // Remove devices
    // REQ :: RemoveDevicesRequest
    // RES :: ResponseBase
    virtual void RemoveDevices(RequestBaseSharedPtr const & request,
                               ResponseBaseHandler const & responseHandler) = 0;

    // Discover devices
    // REQ :: DiscoverDevicesRequest
    // RES :: DiscoverDevicesResponse
    virtual void DiscoverDevices(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;

    // Discover IPv6 devices
    // REQ :: DiscoverDevicesRequest
    // RES :: DiscoverDevicesResponse
    virtual void DiscoverIPv6Devices(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;

    // IP install
    // REQ :: IpSettingDeviceRequest
    // RES :: IpSettingDeviceResponse
    virtual void IpSettingDevice(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;

    // Discover devices
    // REQ :: InitDevicePasswordRequest
    // RES :: ResponseBase
    virtual void InitDevicePassword(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler) = 0;

    virtual void GetAuditLog(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) = 0;

    // Get Event Log
    // REQ :: GetEventLogRequest
    // RES :: GetEventLogResponse
    virtual void GetEventLog(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) = 0;

    // Get System Log
    // REQ :: GetSystemLogRequest
    // RES :: GetSystemLogResponse
    virtual void GetSystemLog(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) = 0;

    // Add Log
    // REQ :: AddLogRequest
    // RES :: ResponseBase
    virtual void AddLog(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) = 0;

    virtual void GetDeviceProfile(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) = 0;

    // Add or update a bookmark
    // REQ :: SaveBookmarkRequest
    // RES :: ResponseBase
    virtual void SaveBookmark(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler) = 0;

    // Remove bookmarks
    // REQ :: RemoveBookmarksRequest
    // RES :: ResponseBase
    virtual void RemoveBookmarks(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;

    // Get EventRule
    // REQ :: GetEventRuleRequest
    // RES :: GetEventRuleResponse
    virtual void GetEventRule(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler) = 0;

    // Save EventRule
    // REQ :: SaveEventRuleRequest
    // RES :: ResponseBase
    virtual void SaveEventRule(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) = 0;

    // Remove EventRules
    // REQ :: RemoveEventRuleRequest
    // RES :: ResponseBase
    virtual void RemoveEventRules(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) = 0;

    virtual void SaveChannels(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) = 0;

    virtual void SaveDevicesCredential(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) = 0;

    virtual void BackupService(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler) = 0;
    virtual void RestoreService(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler) = 0;
    virtual void ResetService(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler) = 0;

    virtual void UpdateSoftwareService(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler) = 0;


    //virual void ChangeDeviceAuthentication

    // Save EventSchedule
    // REQ :: SaveEventScheduleRequest
    // RES :: ResponseBase
    virtual void SaveEventSchedule(RequestBaseSharedPtr const& request,
                                   ResponseBaseHandler const& responseHandler) = 0;

    // Remove EventSchedule
    // REQ :: RemoveEventSchedules
    // RES :: ResponseBase
    virtual void RemoveEventSchedules(RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler) = 0;
    // Save EventEmail
    // REQ :: SaveEventEmailRequest
    // RES :: ResponseBase
    virtual void SaveEventEmail(RequestBaseSharedPtr const& request,
                                   ResponseBaseHandler const& responseHandler) = 0;

    // Remove EventEmail
    // REQ :: RemoveEventEmail
    // RES :: ResponseBase
    virtual void RemoveEventEmail(RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler) = 0;

    // Get DashboardData
    // REQ :: GetDashboardDataRequest
    // RES :: ResponseBase
    /*virtual void GetDashboardData(RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler) = 0;*/

    // Update LDAP Setting
    // REQ :: UpdateLdapSettingRequest
    // RES :: ResponseBase
    virtual void UpdateLdapSetting(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) = 0;

    // Connect to a LDAP Server
    // REQ :: LdapConnectRequest
    // RES :: LdapConnectResponse
    virtual void LdapConnect(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) = 0;

    // Update Statistics
    // REQ :: UpdateStatisticsRequest
    // RES :: ResponseBase
    virtual void UpdateStatistics(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) = 0;

    // Activate License
    // REQ :: ActivateLicenseRequest
    // RES :: ResponseBase
    virtual void ActivateLicense(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;

};

typedef std::shared_ptr<ICoreService> ICoreServiceSharedPtr;
typedef void(ICoreService::*memFuncType)(RequestBaseSharedPtr const& request,
                                         ResponseBaseHandler const& responseHandler);

}
}
