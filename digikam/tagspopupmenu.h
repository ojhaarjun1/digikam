/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-09-07
 * Description : a pop-up menu implementation to display a
 *               hierarchical view of digiKam tags.
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef TAGSPOPUPMENU_H
#define TAGSPOPUPMENU_H

// Qt includes.

#include <QList>

// KDE includes.

#include <kmenu.h>

namespace Digikam
{

class Album;
class TAlbum;
class TagsPopupMenuPriv;

class TagsPopupMenu : public KMenu
{
    Q_OBJECT

public:

    enum Mode
    {
        ASSIGN = 0,
        REMOVE,
        DISPLAY      // Used by "GoTo Tag" feature
    };

    TagsPopupMenu(qlonglong selectedImageId, Mode mode);
    TagsPopupMenu(const QList<qlonglong>& selectedImageIDs, Mode mode);
    ~TagsPopupMenu();

signals:

    void signalTagActivated(int id);

private slots:

    void slotAboutToShow();
    void slotToggleTag(QAction *);
    void slotAddTag(QAction *);
    void slotTagThumbnail(Album *, const QPixmap&);

private:

    void         setup(Mode mode);
    void         clearPopup();
    KMenu*       buildSubMenu(int tagid);
    void         iterateAndBuildMenu(KMenu *menu, TAlbum *album);
    void         setAlbumIcon(QAction *action, TAlbum *album);

private:

    TagsPopupMenuPriv* const d;
};

}  // namespace Digikam

#endif /* TAGSPOPUPMENU_H */
