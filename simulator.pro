#-------------------------------------------------
#
# Project created by QtCreator 2017-11-07T14:13:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simulator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    elf_reader.cpp \
    if.cpp \
    id.cpp \
    func_sim.cpp \
    ex.cpp \
    mem.cpp \
    wb.cpp \
    multicycle_sim.cpp \
    pipeline_sim.cpp \
    ex2.cpp \
    cache.cpp \
    memory.cpp

HEADERS += \
    elf_reader.h \
    if.h \
    reg_def.h \
    id.h \
    func_sim.h \
    ex.h \
    mem.h \
    wb.h \
    multicycle_sim.h \
    lisence.h \
    pipeline_sim.h \
    ex2.h \
    cache.h \
    def.h \
    memory.h \
    storage.h

FORMS +=
