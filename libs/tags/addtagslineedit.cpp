/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-06-12
 * Description : Special line edit for adding or creatingtags
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 1997      by Sven Radej (sven.radej@iname.com)
 * Copyright (c) 1999      by Patrick Ward <PAT_WARD@HP-USA-om5.om.hp.com>
 * Copyright (c) 1999      by Preston Brown <pbrown@kde.org>
 * Copyright (c) 2000-2001 by Dawit Alemayehu <adawit@kde.org>
 * Copyright (c) 2000-2001 by Carsten Pfeiffer <pfeiffer@kde.org>
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

#include "addtagslineedit.h"

// Qt includes

#include <QKeyEvent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "tagscompleter.h"
#include "tageditdlg.h"
#include "album.h"
#include "albummodel.h"
#include "albumfiltermodel.h"
#include "albumtreeview.h"
#include "tagscache.h"
#include "taggingactionfactory.h"
#include "databaseaccess.h"
#include "albumdb.h"

namespace Digikam
{

class AddTagsLineEdit::Private
{
public:

    Private()
        : completer(0),
          tagView(0),
          tagFilterModel(0),
          parentTagId(0)
    {
    }

    TagCompleter*       completer;
    TagTreeView*        tagView;
    AlbumFilterModel*   tagFilterModel;
    TaggingAction       currentTaggingAction;
    int                 parentTagId;
};

AddTagsLineEdit::AddTagsLineEdit(QWidget* const parent)
    : QLineEdit(parent),
      d(new Private)
{
    d->completer = new TagCompleter(this);
    d->completer->setMaxVisibleItems(15);
    d->completer->setWidget(this);
    setCompleter(d->completer);

    connect(this, &QLineEdit::returnPressed, this, &AddTagsLineEdit::slotReturnPressed);
    connect(this, &QLineEdit::editingFinished, this, &AddTagsLineEdit::slotEditingFinished);
    connect(this, &QLineEdit::textChanged, this, &AddTagsLineEdit::slotTextChanged);

    connect(d->completer, SIGNAL(activated(TaggingAction)), this, SLOT(completerActivated(TaggingAction)));
    connect(d->completer, SIGNAL(highlighted(TaggingAction)), this, SIGNAL(taggingActionSelected(TaggingAction)));
}

AddTagsLineEdit::~AddTagsLineEdit()
{
    delete d;
}

void AddTagsLineEdit::setSupportingTagModel(TagModel* model)
{
    d->completer->setSupportingTagModel(model);
}

void AddTagsLineEdit::setFilterModel(AlbumFilterModel* model)
{
    d->tagFilterModel = model;
    d->completer->setTagFilterModel(d->tagFilterModel);
}

void AddTagsLineEdit::setModel(TagModel* model, TagPropertiesFilterModel* filteredModel, AlbumFilterModel* filterModel)
{
    if (filterModel)
    {
        setFilterModel(filterModel);
    }
    else if (filteredModel)
    {
        setFilterModel(filteredModel);
    }
    setSupportingTagModel(model);
}

void AddTagsLineEdit::setTagTreeView(TagTreeView* view)
{
    if (d->tagView)
    {
        disconnect(d->tagView, &TagTreeView::currentAlbumChanged, this, &AddTagsLineEdit::setParentTag);
    }

    d->tagView = view;

    if (d->tagView)
    {
        connect(d->tagView, &TagTreeView::currentAlbumChanged, this, &AddTagsLineEdit::setParentTag);
        setParentTag(d->tagView->currentAlbum());
    }
}

void AddTagsLineEdit::setCurrentTag(TAlbum* album)
{
    setCurrentTaggingAction(album ? TaggingAction(album->id()) : TaggingAction());
    setText(album ? album->title() : QString());
}

void AddTagsLineEdit::setParentTag(Album* album)
{
    d->parentTagId = album ? album->id() : 0;
    d->completer->setContextParentTag(d->parentTagId);
}

void AddTagsLineEdit::setAllowExceedBound(bool value)
{
    Q_UNUSED(value);
    // -> the pop-up is allowed to be bigger than the line edit widget
    // Currently unimplemented, QCompleter calculates the size automatically.
    // Idea: intercept show event via event filter on completer->popup(); from there, change width.
}

void AddTagsLineEdit::focusInEvent(QFocusEvent* f)
{
    QLineEdit::focusInEvent(f);

    // NOTE: Need to disconnect completer from QLineEdit, otherwise
    // we won't be able to clear completion after tag was added
    // See QLineEdit::focusInEvent(QFocusEvent *e) where this connection is made

    disconnect(d->completer, SIGNAL(activated(QString)),
               this, SLOT(setText(QString)));
}

// Tagging action is used by facemanagement and assignwidget

void AddTagsLineEdit::slotReturnPressed()
{
    //qCDebug(DIGIKAM_GENERAL_LOG) << "slot return pressed";

    if (text().isEmpty())
    {
        //focus back to mainview
        emit taggingActionFinished();
        return;
    }

    if (d->currentTaggingAction.shallAssignTag() && d->completer->popup()->isVisible())
    {
        setText(d->currentTaggingAction.newTagName());
    }

    emit taggingActionActivated(currentTaggingAction());
}

void AddTagsLineEdit::slotEditingFinished()
{
    d->currentTaggingAction = TaggingAction();
}

void AddTagsLineEdit::slotTextChanged(const QString &text)
{
    if (text.isEmpty())
    {
        d->currentTaggingAction = TaggingAction();
    }
}

void AddTagsLineEdit::completerActivated(const TaggingAction &action)
{
    setCurrentTaggingAction(action);
    emit taggingActionActivated(action);
}

void AddTagsLineEdit::completerHighlighted(const TaggingAction &action)
{
    setCurrentTaggingAction(action);
}

void AddTagsLineEdit::setCurrentTaggingAction(const TaggingAction& action)
{
    if (d->currentTaggingAction == action)
    {
        return;
    }

    d->currentTaggingAction = action;
    emit taggingActionSelected(action);
}

TaggingAction AddTagsLineEdit::currentTaggingAction() const
{
    if (d->currentTaggingAction.isValid())
    {
        return d->currentTaggingAction;
    }
    else if(!text().isEmpty())
    {
        return TaggingActionFactory::defaultTaggingAction(text(), d->parentTagId);
    }
    else
    {
        return TaggingAction();
    }
}

} // namespace Digikam