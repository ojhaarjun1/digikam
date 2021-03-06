#
# Copyright (c) 2010-2019 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif (POLICY CMP0063)

include_directories(
    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

set(albummodeltest_SRCS
    albummodeltest.cpp
    ../modeltest/modeltest.cpp
)

add_executable(albummodeltest ${albummodeltest_SRCS})
ecm_mark_nongui_executable(albummodeltest)

target_link_libraries(albummodeltest
                      digikamcore
                      digikamgui

                      Qt5::Gui
                      Qt5::Test
                      Qt5::Sql
                      Qt5::Xml

                      KF5::I18n
                      KF5::XmlGui

                      ${OpenCV_LIBRARIES}
)
