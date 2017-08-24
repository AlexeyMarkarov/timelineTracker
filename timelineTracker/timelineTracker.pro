QT += qml quick

CONFIG += c++11 warn_on

HEADERS += \
    sources/MainController.h \
    sources/MainWindow.h

SOURCES += sources/main.cpp \
    sources/MainController.cpp \
    sources/MainWindow.cpp

RESOURCES += resources/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

################################################################################
# Deploy
################################################################################

CONFIG( debug, debug|release ) {
    BUILD_SUBDIR = debug
} else {
    BUILD_SUBDIR = release
}

isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
    macx {
        TARGET_CUSTOM_EXT = .app
    }
} else {
    TARGET_CUSTOM_EXT = $${TARGET_EXT}
}

win32 {
    DEPLOY_COMMAND = windeployqt --compiler-runtime
}
macx {
    DEPLOY_COMMAND = macdeployqt
}

DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/$${BUILD_SUBDIR}/$${TARGET}$${TARGET_CUSTOM_EXT}))

QMAKE_POST_LINK = $${DEPLOY_COMMAND} \
    --qmldir $$shell_quote($$shell_path($${PWD}/resources/qml)) \
    $${DEPLOY_TARGET}
