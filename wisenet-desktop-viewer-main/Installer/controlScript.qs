var logPath;
var logFile = "WisenetViewerInstallation.log";
var systemConfigFilePath = "/WisenetViewerInstaller.conf";
var versionKey = "prod.version";
var buildKey = "prod.build";
var osKey = "prod.plateform";
var installationPathKey = "prod.path";
var uuidKey = "prod.uuid";
var prodFolder = "/WisenetViewerInstaller";
var tempInstallationPath = "/tmp" + prodFolder;
var systemConfigFileFound = false;
var maintenanceToolFound = false;
var installationFound = false;
var installerInUpgradationMode = false;
var maintenanceTool = "WisenetViewerMaintenanceTool";
var _leaveAbortInstallationPageConnected = false;
var _onLeftAbortInstallationPageCalled = false;
//var autoClickDelay = 500;

function Controller() {
    // Set log, Config file path
    logPath = installer.value("InstallerDirPath");

    // data폴더 하위의 systemConfigFile을 먼저 참조하도록 수정 (20230127)
    systemConfigFilePath = installer.value("TargetDir") + "/data/WisenetViewerInstaller.conf";
    if (!installer.fileExists(systemConfigFilePath)) {
        systemConfigFilePath = installer.value("TargetDir") + "/WisenetViewerInstaller.conf";
    }

    log("Logging file : " + logPath + "/" + logFile);    
    log("Inside Wisenet Viewer Installer Controller()");

    populateInstallerInfo();
    log("Going to install Wisenet Viewer Installer version : " + installer.value("ProductInstallerVersion"));

    installer.setValue("logPath", logPath);
    installer.setValue("logFile", logFile);
    installer.setValue("systemConfigFilePath", systemConfigFilePath);
    installer.setValue("versionKey", versionKey);
    installer.setValue("buildKey", buildKey);
    installer.setValue("osKey", osKey);    
    installer.setValue("installationPathKey", installationPathKey);
    installer.setValue("uuidKey", uuidKey);
    installer.setValue("tempInstallationPath", tempInstallationPath);

    installer.setValue("AllUsers", "true"); // Always Install for all users
    installer.setValue("ProductUUID", "{DEFD9B22-9CC5-4140-9A64-9386A20B3310}");  // fix product UUID

    /*
    gui.interrupted.connect(onInstallationInterrupted);
    installer.installationStarted.connect(onInstallationStarted);
    installer.installationInterrupted.connect(onInstallationInterrupted);
    installer.uninstallationStarted.connect(this, onUninstallationStarted);
    installer.finishButtonClicked.connect(onLeftFinishedPage);
    */
    installer.installationFinished.connect(onInstallationFinished);
    installer.uninstallationFinished.connect(this, onUninstallationFinished);

    if (installer.isInstaller()) {
        installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
        installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
        installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
        installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
    }
}

Controller.prototype.IntroductionPageCallback = function () {
    log("Inside Controller.IntroductionPageCallback()");

    /*
    log("RootDir:" + installer.value("RootDir"));
    log("HomeDir:" + installer.value("HomeDir"));
    log("ApplicationsDir:" + installer.value("ApplicationsDir"));
    log("ApplicationsDirUser:" + installer.value("ApplicationsDirUser"));
    log("ApplicationsDirX86:" + installer.value("ApplicationsDirX86"));
    log("ApplicationsDirX64:" + installer.value("ApplicationsDirX64"));
    */

    var widget = gui.currentPageWidget();
    if (widget != null) {
        if (installer.isInstaller()) {
            // update message (Install)
            widget.title = "Setup - Wisenet Viewer Installer";
            widget.MessageLabel.setText("Welcome to the Wisenet Viewer Installer Installation Wizard.");
        } else {
            // hide unused radioButoon (Uninstall)
            var radioButton = widget.findChild("UpdaterRadioButton");
            radioButton.setVisible(false);
            radioButton = widget.findChild("PackageManagerRadioButton");
            radioButton.setVisible(false);
        }
    }

    if (installer.isInstaller()) {
        checkOldInstallation();
    }
}

Controller.prototype.DynamicAbortInstallationPageCallback = function () {
    log("Inside Controller.prototype.DynamicAbortInstallationPageCallback()");

    var page = gui.pageWidgetByObjectName("DynamicAbortInstallationPage");

    /*
    //Once any of two options (Uninstall/Upgrade) selected, do not allow to change option
    if (page.upgrade.visible && (installerInUpgradationMode)) {
        gui.clickButton(buttons.NextButton, 5);
    }
    */

    if (installer.value("_installationFound")) {
        log("installation Found");
        page.uninstall.setVisible(false);

        if(systemConfigFileFound && maintenanceToolFound) {
            var lowerVersionInstalled = installer.versionMatches(version, "<"+installer.value("ProductInstallerVersion"));
            var sameVersionInstalled = installer.versionMatches(version, "="+installer.value("ProductInstallerVersion"));

            if (lowerVersionInstalled || (sameVersionInstalled && release < installer.value("WisenetViewerInstallerBuild"))) {
                log("Case 1 : If Conf file and Maintenance tool both present and lower version installed");
                var html = "Wisenet Viewer Installer already installed.<br/><br/>";
                html += "<b> - Installation Directory :</b> " + installer.value("TargetDir") + "<br/>";
                html += "<b> - Installer Version :</b> " + installer.value("ProductInstallerVersion") + "<br/><br/>";
                html += "An earlier version of Wisenet Viewer " + version + " is installed.<br/><br/>";
                //html += "Select appropriate action to be performed";
                html += "After starting the upgrade, the existing version will be uninstalled automatically.";
                page.upgrade.setVisible(true);
                page.abortDetails.html = html;
                page.upgrade.setChecked(true);
                enableUpgaradtionSteps();
            }
            else {
                log("Case 3 : If Conf file and Maintenance tool both present but installed version is same or higher");
                var html = "Wisenet Viewer Installer already installed.<br/><br/>";
                html += "<b> - Installation Directory :</b> " + installer.value("TargetDir") + "<br/>";
                html += "<b> - Installer Version :</b> " + installer.value("ProductInstallerVersion") + "<br/><br/>";
                html += "A higher or same version of Wisenet Viewer " + version + " is already installed.<br/><br/>";
                html += "To install an older or the same version of Wisenet Viewer, first uninstall the existing version.";
                page.upgrade.setVisible(false);
                page.abortDetails.html = html;
                skipAllSteps();
            }
        }
        else {
            // force update install
            log("Case 2 : systemConfigFile or maintenanceTool is not there");
            var html = "Wisenet Viewer Installer already installed.<br/><br/>";
            html += "<b> - Installation Directory :</b> " + installer.value("TargetDir") + "<br/>";
            html += "<b> - Installer Version :</b> " + installer.value("ProductInstallerVersion") + "<br/><br/>";
            html += "Wisenet Viewer installation has been found, but either Wisenet Viewer is not installed properly or some files were deleted.<br/><br/>";
            html += "After starting the upgrade, the existing version will be uninstalled automatically.";
            page.upgrade.setVisible(true);
            page.abortDetails.html = html;
            page.upgrade.setChecked(true);
            enableUpgaradtionSteps();
        }
    }
    else {
        //page.launchTool.setVisible(false);
        page.uninstall.setVisible(false);
        page.upgrade.setVisible(false);
        var html = "Installation aborted due to unknown reason";
        page.abortDetails.html = html;
        skipAllSteps();
    }
}

function onLeftAbortInstallationPage() {
    log("Inside onLeftAbortInstallationPage()");
    if (!_onLeftAbortInstallationPageCalled) {
        _onLeftAbortInstallationPageCalled = true;
    } else {
        return;
    }

    var widget = gui.pageWidgetByObjectName("DynamicAbortInstallationPage");
    if (widget != null) {
        if (widget.uninstall.checked) {
            log("User selected Uninstallation for Wisenet Viewer");
            /*
            // skipAllSteps();
            // installer.setUninstaller();
            delegatedUninstall = true;
            installerInUpgradationMode = false;
            installer.setValue("installerInUpgradationMode", "false");
            launchMaintenanceTool();
            */
        }
        if (widget.upgrade.checked) {
            log("User selected Upgradation for Wisenet Viewer");
            installerInUpgradationMode = true;
            installer.setValue("installerInUpgradationMode", "true");

            var components = installer.components();
            for (i = 0; i < components.length; ++i) {
                /*
                var installationRequested = getConfProperty(components[i].name);
                installer.setValue("installationRequested_" + components[i].name, installationRequested ? installationRequested : "false");
                */
                installer.setValue("installationRequested_" + components[i].name, "true");
            }

            if (installerInUpgradationMode) {
                /*
                var result = installer.executeDetached("/bin/sh", new Array("-c", "rm -Rf " + tempInstallationPath + "/" + maintenanceTool));
                result = installer.executeDetached("/bin/sh", new Array("-c", "rm -Rf " + tempInstallationPath + "/" + maintenanceTool + ".ini"));
                result = installer.executeDetached("/bin/sh", new Array("-c", "rm -Rf " + tempInstallationPath + "/" + maintenanceTool + ".dat"));
                result = installer.executeDetached("/bin/sh", new Array("-c", "rm -Rf /tmp/" + maintenanceTool + "*.lock"));
                result = installer.executeDetached("/bin/sh", new Array("-c", "rm -Rf " + tempInstallationPath));
                */
            }
            // enableUpgaradtionSteps();
        }
    }
}

Controller.prototype.DynamicSupportedOsPageCallback = function () {
    log("Inside Controller.prototype.DynamicSupportedOsPageCallback()");

    var page = gui.pageWidgetByObjectName("DynamicSupportedOsPage");

    var html = "Supported Operating Systems<br/><br/>";
    html += "<b> - Windows 8.1 64bit</b><br/>";
    html += "<b> - Windows 10 64bit</b><br/>";
    html += "<b> - Windows 11 64bit</b><br/><br/>";
    html += "Before installation, please check the supported OSs.<br/><br/>";
    html += "Normal program operation cannot be guaranteed on an unsupported OS.";

    page.SupportedOsDetails.html = html;
}

Controller.prototype.ReadyForInstallationPageCallback = function () {
    log("Inside Controller.ReadyForInstallationPageCallback()");

    // ReadyForInstallationPage 페이지 진입 시 WisenetViewer.exe 프로세스 강제종료 추가
    installer.execute("C:/Windows/System32/taskkill", new Array("/im", "WisenetViewer.exe", "/f"));

    if (installer.isUninstaller()) {
        var components = installer.components();
        for (i = 0; i < components.length; ++i) {
            installer.setValue("uninstallationRequested_" + components[i].name, "true");
        }
    }
    if (installerInUpgradationMode) {
        var widget = gui.currentPageWidget();
        if (widget != null) {
            widget.title = "Ready to upgrade";
            widget.MessageLabel.setText("Setup is now ready to begin upgrading Wisenet Viewer Installer on your computer. Do not interrupt process, as interruption may corrupt existing installation.");
        }
    }
}

Controller.prototype.PerformInstallationPageCallback = function () {
    log("Inside Controller.PerformInstallationPageCallback()");
    if(installer.isUninstaller() || installerInUpgradationMode) {
        installer.execute("C:/Windows/System32/taskkill", new Array("/im", "WisenetViewer.exe", "/f"));
    }
    if (installerInUpgradationMode) {
        /*
        log("old version auto_uninstall");
        log("maintenanceTool Path : " + installer.value("TargetDir") + "/" + maintenanceTool + ".exe");
        installer.execute(installer.value("TargetDir") + "/" + maintenanceTool + ".exe", "--script=" + installer.value("TargetDir") + "/data/auto_uninstall.qs");
        */
    }
}

function onInstallationFinished() {
    log("Inside onInstallationFinished()");

    // force delete old version TargetDir
    var oldTargetDir = installer.value("TargetDir") + "/../../HanwhaTechwin/WisenetViewer"
    installer.performOperation("Delete", oldTargetDir);

    writeSystemConfigFile();

    if (systemInfo.productType == "windows") {
        // delete old version registry
        var batPath = installer.value("TargetDir") + "/data/deleteOldRegistry.bat";
        var ret = installer.execute("cmd", ["/c", batPath]);
        log("execute deleteOldRegistry cmd returns : " + ret);

        // Update Uninstall Path
        batPath = installer.value("TargetDir") + "/data/updateUninstallPath.bat";
        var productUUID = installer.value("ProductUUID");
        log("updateUninstallPath : " + batPath);
        log("productUUID : " + productUUID);
        ret = installer.execute("cmd", ["/c", batPath, productUUID]);
        log("execute updateUninstallPath cmd returns : " + ret);
    }
}

function onUninstallationFinished() {
    log("Inside onUninstallationFinished()");
    if (systemInfo.productType == "windows") {
        var publicPath = installer.environmentVariable("public");
        var dbPath = publicPath + "/Documents/WisenetViewer";
        log("dbPath : " + dbPath);
        installer.performOperation("Delete", dbPath);
    }
}

function log(msg) {
    console.log(msg);
    /*
    if (systemInfo.productType == "windows") {
        installer.execute("cmd", new Array("/C", "echo " + msg + " >> " + logPath.replace(/\//g, '\\') + "\\" + logFile))
    }
    else if(systemInfo.productType == "osx") {

    }
    else {

    }
    */
}

function populateInstallerInfo() {
    log("Inside Controller populateInstallerInfo()");

    // Read ProductVersion
    // ProductVersion format in config.xml => major.minor.patch|build
    var data = installer.value("ProductVersion");
    if (data) {
        var parts = data.split("\|");
        if (parts.length >= 1) {
            var versionParts = parts[0].split("\.");
            if (versionParts.length > 2) {
                installer.setValue("WisenetViewerInstallerMajorVersion", versionParts[0]);
                installer.setValue("WisenetViewerInstallerMinorVersion", versionParts[1]);
                installer.setValue("WisenetViewerInstallerPatchVersion", versionParts[2]);
            }
            installer.setValue("ProductInstallerVersion", parts[0]);
            log("InstallerVersion : " + parts[0]);

            if(parts.length > 1) {
                installer.setValue("WisenetViewerInstallerBuild", parts[1]);
                log("InstallerBuild : " + parts[1]);
            }
        }
    }
}

function checkOldInstallation() {
    log("Inside Controller checkOldInstallation()");
    log("Looking if an Wisenet Viewer installation already exists on this machine ....");

    installer.setValue("_oldProductUuid", "");

    if (installer.fileExists(systemConfigFilePath)) {
        systemConfigFileFound = true;
        log("Found an existing Wisenet Viewer systemConfigFile.");
        log("Configuration file : " + systemConfigFilePath + " exists");
        log("Operating System : " + (getConfProperty(osKey)));
        log("Installed Version : " + (version = getConfProperty(versionKey)));
        log("Installed Release : " + (release = getConfProperty(buildKey)));
        log("Installed productUUID : " + (uuid = getConfProperty(uuidKey)));

        if(uuid) {
            log("Found an existing Wisenet Viewer productUuid. " + uuid);
            installer.setValue("_oldProductUuid", uuid);
        }
        else {
            log("Not found an existing Wisenet Viewer productUuid.");
        }
    }

    if(installer.fileExists(installer.value("TargetDir") + "/" + maintenanceTool + ".exe")) {
        maintenanceToolFound = true;
        log("Found an existing Wisenet Viewer maintenanceTool.");
    }

    installationFound = systemConfigFileFound || maintenanceToolFound;
    installer.setValue("_installationFound", installationFound);

    if(installationFound) {
        log("Found an existing Wisenet Viewer Installation.");
        log("Wisenet Viewer Component will launch AbortInstallationPage");
        installer.setValue("existingInstallationPath", installer.value("TargetDir"));
    }
    else {
        log("Configuration file : " + systemConfigFilePath + " or maintenanceTool not exists");
    }
}

function getConfProperty(key) {
    var line = "";

    if (systemInfo.productType == "windows") {
        var ConfPath = systemConfigFilePath.replace(/\//g, '\\');
        line = installer.execute("findstr", new Array(key, ConfPath))[0];
    }
    else if(systemInfo.productType == "osx") {

    }
    else {
        //line = installer.execute("grep", new Array(key, systemConfigFilePath))[0];
    }

    if (line) {
        var parts = line.split(":");
        if (parts.length > 1) {
            return (parts[1]).replace(/\r?\n|\r/g, "");
        }
    }

    return undefined;
}

function writeSystemConfigFile() {
    log("Inside Controller writeSystemConfigFile()");
    if (systemInfo.productType == "windows") {
        var configFilePath = installer.value("TargetDir") + "/data/WisenetViewerInstaller.conf";
        log("configFilePath : " + configFilePath);
        var ret = installer.execute("xcopy", ["/y", configFilePath.replace(/\//g, '\\'), installer.value("TargetDir").replace(/\//g, '\\')]);
        log("execute cmd returns : " + ret);
    }
    else if(systemInfo.productType == "osx") {

    }
    else {

    }
}

function skipAllSteps() {
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
    installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
    installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, false);
    installer.setDefaultPageVisible(QInstaller.PerformInstallation, false);
}

function enableUpgaradtionSteps() {
    // 업데이트 설치 페이지 진입 시 WisenetViewer.exe 프로세스 강제종료 추가
    installer.execute("C:/Windows/System32/taskkill", new Array("/im", "WisenetViewer.exe", "/f"));

    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
    installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
    installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, true);
    installer.setDefaultPageVisible(QInstaller.PerformInstallation, true);

    /*
    var components = installer.components();
    for (i = 0; i < components.length; ++i) {
        if (components[i].name == 'com.org.product') {
            installer.addWizardPage(components[i], "InstallationDetailsPage", QInstaller.ReadyForInstallation);
            log("WizardPage InstallationDetailsPage added.");
            break;
        }
    }
    */

    if (!_leaveAbortInstallationPageConnected) {
        _leaveAbortInstallationPageConnected = true;
        log("Connecting onLeftAbortInstallationPage on entered InstallationDetailsPage");
        //gui.pageByObjectName("DynamicInstallationDetailsPage").entered.connect(onLeftAbortInstallationPage);
        var widget = gui.pageById(QInstaller.ReadyForInstallation);
        if (widget != null) {
            widget.entered.connect(onLeftAbortInstallationPage);
        }
    }
}
