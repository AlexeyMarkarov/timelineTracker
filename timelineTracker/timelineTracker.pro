QT += gui qml quick widgets charts

CONFIG += c++11 warn_on

CONFIG( debug, debug|release ) {
    BUILD_SUBDIR = debug
} else {
    BUILD_SUBDIR = release
}

DESTDIR = $${PWD}/bin/$${BUILD_SUBDIR}

HEADERS += \
    sources/MainController.h \
    sources/MainWindow.h \
    sources/TimelineModel.h \
    sources/ChartView.h \
    sources/TimeSpan.h \
    sources/Settings.h \
    sources/QmlPixmapProvider.h \
    sources/Util.h \
    sources/Logger.h \
    sources/Version.h \
    sources/BuildInfo.h

SOURCES += \
    sources/main.cpp \
    sources/MainController.cpp \
    sources/MainWindow.cpp \
    sources/TimelineModel.cpp \
    sources/ChartView.cpp \
    sources/TimeSpan.cpp \
    sources/Settings.cpp \
    sources/QmlPixmapProvider.cpp \
    sources/Util.cpp \
    sources/Logger.cpp

RESOURCES += resources/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_MESSAGELOGCONTEXT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

################################################################################
# Pre-build
################################################################################
versionTarget.target = sources/BuildInfo.h
versionTarget.depends = FORCE
versionTarget.commands = python $${PWD}/prebuild.py $${PWD} $${PWD}/sources/BuildInfo.h
PRE_TARGETDEPS += sources/BuildInfo.h
QMAKE_EXTRA_TARGETS += versionTarget

################################################################################
# Deploy
################################################################################
isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
} else {
    TARGET_CUSTOM_EXT = $${TARGET_EXT}
}
QMAKE_POST_LINK = python $${PWD}/postbuild.py $${DESTDIR} $${TARGET}$${TARGET_CUSTOM_EXT} $${PWD}/resources/qml
