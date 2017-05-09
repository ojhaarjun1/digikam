/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PANO_WIZARD_H
#define PANO_WIZARD_H

// Qt includes

#include <QWidget>

// Local includes

#include "dwizarddlg.h"

namespace Digikam
{

class PanoManager;

class PanoWizard : public DWizardDlg
{

public:

    explicit PanoWizard(PanoManager* const mngr, QWidget* const parent=0);
    ~PanoWizard();

private:

    struct Private;
    Private* const d;
};

} // namespace Digikam

#endif // PANO_WIZARD_H
