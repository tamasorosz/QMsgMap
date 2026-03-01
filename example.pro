QT += core gui quickwidgets positioning network location
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
CONFIG += c++17

# --- Sources / headers ---
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/markermodel.cpp \
    src/receiver.cpp

HEADERS += \
    src/mainwindow.h \
    src/markermodel.h \
    src/receiver.h

FORMS += \
    ui/mainwindow.ui

# --- Resources (QML + images should be listed inside resources/resources.qrc) ---
RESOURCES += \
    resources/resources.qrc

# --- 3rd-party libs ---
LIBS += -L$$PWD/libs -lSimpleAmqpClient

# Linux: add rpath so the app can find libs next to the binary (build/run convenience)
unix:!macx: QMAKE_LFLAGS += -Wl,-rpath,'$$ORIGIN/libs'

# Optional: copy the runtime .so next to the built binary (only if you really ship it)
unix:!android {
    QMAKE_POST_LINK += mkdir -p $$OUT_PWD/libs && \
                       cp -f $$PWD/libs/libSimpleAmqpClient.so.7 $$OUT_PWD/libs/
}

# Keep distfiles for docs only (avoid committing .so if possible)
DISTFILES += \
    README.md