TEMPLATE = app
TARGET = qemuctl
QT += core \
    gui \
    network
CONFIG += debug \
    console
HEADERS += QHexSpinbox.h \
    serialconnector.h \
    addmedia.h \
    debugmonitor.h \
    ConsoleInput.h \
    QemuComunication.h \
    qemuctl.h
SOURCES += QHexSpinbox.cpp \
    serialconnector.cpp \
    addmedia.cpp \
    debugmonitor.cpp \
    ConsoleInput.cpp \
    QemuComunication.cpp \
    main.cpp \
    qemuctl.cpp
FORMS += serialconnector.ui \
    addmedia.ui \
    debugmonitor.ui \
    qemuctl.ui
RESOURCES += qemuctl.qrc
INCLUDEPATH += /usr/include/ \
    /usr/include/X11/
LIBS += -lX11 \
    -lXext

target.path += $$[QT_INSTALL_BINS]
INSTALLS += target
    