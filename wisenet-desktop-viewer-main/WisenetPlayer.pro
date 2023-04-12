TEMPLATE = subdirs

SUBDIRS += \
    WisenetCommonLibrary \
    WisenetStyle \
    WisenetMediaFramework \
    WisenetPlayer

# build the project sequentially as listed in SUBDIRS !
CONFIG += ordered

WisenetMediaFramework.depends = WisenetCommonLibrary WisenetStyle
WisenetPlayer.depends = WisenetCommonLibrary WisenetMediaFramework WisenetStyle

