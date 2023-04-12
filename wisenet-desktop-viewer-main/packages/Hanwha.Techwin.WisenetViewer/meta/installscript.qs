var systemConfigFilePath;
var versionKey;
var releaseKey;
var osKey;
var installationPathKey;
var uuidKey;
var dataExtracted = false;

function Component()
{
    // constructor
    log("Inside Wisenet Viewer Component()");
    systemConfigFilePath = installer.value("systemConfigFilePath");
    versionKey = installer.value("versionKey");
    releaseKey = installer.value("releaseKey");
    osKey = installer.value("osKey");
    installationPathKey = installer.value("installationPathKey");
    uuidKey = installer.value("uuidKey");

    component.loaded.connect(this, Component.prototype.loaded);

    if (installer.value("_installationFound") == 'true') {
        log("Going to launch AbortInstallationPage");
        if (installer.addWizardPage(component, "AbortInstallationPage", QInstaller.TargetDirectory)) {
            log("AbortInstallationPage added");
        }
    }
    else {
        log("Going to launch SupportedOsPage");
        if (installer.addWizardPage(component, "SupportedOsPage", QInstaller.TargetDirectory)) {
            log("SupportedOsPage added");
        }
    }

    installer.installationFinished.connect(onInstallationCompletion);
    installer.uninstallationFinished.connect(onUninstallationCompletion);
}

Component.prototype.loaded = function () {
    log("Inside Wisenet Viewer Component.prototype.loaded()");
}

Component.prototype.createOperationsForArchive = function (archive) {
    log("Inside Component.prototype.createOperationsForArchive()");
    try {
        component.createOperationsForArchive(archive);
        dataExtracted = true;
    } catch (e) {
        log(e);
    }
}

Component.prototype.createOperationsForPath = function (path) {
    log("Inside Component.prototype.createOperationsForPath()");
    try {
        component.createOperationsForPath(path);
    } catch (e) {
        log(e);
    }
}

Component.prototype.createOperations = function () {
    log("Inside Wisenet Viewer Component.prototype.createOperations()");
    try {
        component.createOperations();
    } catch (e) {
        log(e);
    }
    if (!dataExtracted) {
        log("Data Extraction status : " + dataExtracted);
        component.createOperationsForArchive(component.archives[0]);
    }

    if (installer.value("installerInUpgradationMode") == "true") {
        log("Inside Wisenet Viewer Component (Installer mode : Upgrade)");
        upgradeWisenetViewer();
    }
    else {
        if (systemInfo.productType === "windows") {
            // return value 3010 means it need a reboot, but in most cases it is not needed for running Qt application
            // return value 5100 means there's a newer version of the runtime already installed
            component.addElevatedOperation("Execute", "{0,3010,1638,5100}", "@TargetDir@/data/vc_redist.x64.exe", "/install", "/quiet", "/norestart");
        }
    }

    if (systemInfo.productType === "windows") {
        var publicPath = installer.environmentVariable("public");

        component.addOperation("CreateShortcut",
                               "@TargetDir@/data/WisenetViewer.exe", // target
                               "@AllUsersStartMenuProgramsPath@/Wisenet Viewer/Wisenet Viewer.lnk", // link-path
                               "workingDirectory=@TargetDir@", // working-dir
                               "iconPath=@TargetDir@/data/WisenetViewer_new.ico", "iconId=0", // icon
                               "description=Wisenet Viewer"); // description
        component.addOperation("CreateShortcut",
                               "@TargetDir@/data/WisenetViewer.exe", // target
                               publicPath + "\\Desktop\\Wisenet Viewer.lnk", // link-path
                               "workingDirectory=@TargetDir@", // working-dir
                               "iconPath=@TargetDir@/data/WisenetViewer_new.ico", "iconId=0", // icon
                               "description=Wisenet Viewer"); // description
        component.addOperation("CreateShortcut",
                               "@TargetDir@/data/WisenetViewer.exe", // target
                               "@TargetDir@/data/WisenetViewer.lnk", // link-path
                               "workingDirectory=@TargetDir@", // working-dir
                               "iconPath=@TargetDir@/data/WisenetViewer_new.ico", "iconId=0", // icon
                               "description=Wisenet Viewer"); // description
    }
}

function upgradeWisenetViewer() {
    log("Inside upgradeWisenetViewer()");
    var targetDir = installer.value("TargetDir");
    var installationPath = installer.value("existingInstallationPath");

    if (systemInfo.productType === "windows") {
        // delete old version shortcut
        component.addElevatedOperation("Delete", "@UserStartMenuProgramsPath@/Wisenet Viewer/Wisenet Viewer.lnk");
        component.addElevatedOperation("Delete", "@DesktopDir@/Wisenet Viewer.lnk");
    }

    // component.addElevatedOperation("Execute", "rm", "-Rf", installationPath + "/Module4/webapp");
    // component.addElevatedOperation("Execute", "rm", "-Rf", installationPath + "/Module4/lib/module4.war");
    // component.addElevatedOperation("Execute", "{0,1,2,3,4,5,15}", "cp", installationPath + "InstallationLog.txt", installationPath + "InstallationLog_old.txt");
}

function onInstallationCompletion() {
    log("Inside onInstallationCompletion()");
}

function onUninstallationCompletion() {
    log("Inside onUninstallationCompletion()");
}

function log(msg) {
    console.log(msg);
    /*
    if (systemInfo.productType == "windows") {
        installer.execute("cmd", new Array("/C", "echo " + msg + " >> " + installer.value("logPath").replace(/\//g, '\\') + "\\" + installer.value("logFile")));
    }
    else if(systemInfo.productType == "osx") {

    }
    else {

    }
    */
}
