/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-03-09
 * Description : LDR tonemapper batch tool.
 *
 * Copyright (C) 2010-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_LOCAL_CONTRAST_H
#define DIGIKAM_BQM_LOCAL_CONTRAST_H

// Local includes

#include "batchtool.h"

namespace Digikam
{

class LocalContrastSettings;

class LocalContrast : public BatchTool
{
    Q_OBJECT

public:

    explicit LocalContrast(QObject* const parent = 0);
    ~LocalContrast();

    BatchToolSettings defaultSettings();

    BatchTool* clone(QObject* const parent=0) const { return new LocalContrast(parent); };

    void registerSettingsWidget();

private:

    bool toolOperations();

private Q_SLOTS:

    void slotAssignSettings2Widget();
    void slotSettingsChanged();

private:

    LocalContrastSettings* m_settingsView;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_LOCAL_CONTRAST_H
