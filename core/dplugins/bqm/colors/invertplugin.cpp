/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to invert colors
 *
 * Copyright (C) 2018-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "invertplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "invert.h"

namespace Digikam
{

InvertPlugin::InvertPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

InvertPlugin::~InvertPlugin()
{
}

QString InvertPlugin::name() const
{
    return i18n("Invert Colors");
}

QString InvertPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon InvertPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("colorfx"));
}

QString InvertPlugin::description() const
{
    return i18n("A tool to invert image colors");
}

QString InvertPlugin::details() const
{
    return i18n("<p>This Batch Queue Manager tool can invert colors from images.</p>");
}

QList<DPluginAuthor> InvertPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QLatin1String("Gilles Caulier"),
                             QLatin1String("caulier dot gilles at gmail dot com"),
                             QLatin1String("(C) 2010-2019"))
            ;
}

void InvertPlugin::setup(QObject* const parent)
{
    Invert* const tool = new Invert(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace Digikam
