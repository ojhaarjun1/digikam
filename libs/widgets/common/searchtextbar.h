/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-11-25
 * Description : a bar used to search a string.
 *
 * Copyright (C) 2007-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SEARCH_TEXT_BAR_H
#define SEARCH_TEXT_BAR_H

// QT includes
#include <qabstractitemmodel.h>

// KDE includes

#include <klineedit.h>
#include <klocale.h>

// Local includes

#include "digikam_export.h"
#include "statesavingobject.h"

namespace Digikam
{

class AlbumFilterModel;

class SearchTextBarPriv;

class DIGIKAM_EXPORT SearchTextSettings
{

public:

    SearchTextSettings()
    {
        caseSensitive = Qt::CaseInsensitive;
    }

    Qt::CaseSensitivity caseSensitive;

    QString             text;

};

bool DIGIKAM_EXPORT operator==(const SearchTextSettings &a, const SearchTextSettings &b);

/**
 * A text input for searching entries with visual feedback. Can be used on
 * QAbstractItemModels.
 *
 * @todo the model code could also be placed in a subclass of KCompletion...
 *
 * @author Gilles Caulier
 * @author jwienke
 */
class DIGIKAM_EXPORT SearchTextBar : public KLineEdit, public StateSavingObject
{
    Q_OBJECT

public:

    /**
     * Possible highlighting states a SearchTextBar can have.
     */
    enum HighlightState
    {

        /**
         * No highlighting at all. Background is colored in a neutral way
         * according to the theme.
         */
        NEUTRAL,

        /**
         * The background color of the text input indicates that a result was
         * found.
         */
        HAS_RESULT,

        /**
         * The background color indicates that no result was found.
         */
        NO_RESULT

    };

    SearchTextBar(QWidget *parent, const char* name, const QString& msg=i18n("Search..."));
    ~SearchTextBar();

    void setTextQueryCompletion(bool b);
    bool hasTextQueryCompletion() const;

    /**
     * Tells whether highlighting for found search results shall be used or not
     * (green and red).
     *
     * Default behaviour has highlighting enabled.
     *
     * @param highlight <code>true</code> activates green and red highlighting,
     *                  with <code>false</code> the normal widget background
     *                  color will be displayed always
     */
    void setHighlightOnResult(bool highlight);

    /**
     * If the given model is != null, the model is used to populate the
     * completion for this text field.
     *
     * @param model to fill from or null for manual mode
     * @param uniqueIdRole a role for which the model will return a unique integer for each entry
     * @param displayRole the role to retrieve the text for completion, default is Qt::DisplayRole.
     */
    void setModel(QPointer<QAbstractItemModel> model, int uniqueIdRole, int displayRole = Qt::DisplayRole);

    /**
     * Sets the filter model this text bar shall use to invoke filtering on and
     * reading the result for highlighting from.
     *
     * @param filterModel filter model to use for filtering. <code>null</code>
     *                    means there is no model to use and external
     *                    connections need to be created with
     *                    signalSearchTextSettings and slotSearchResult
     */
    void setFilterModel(QPointer<AlbumFilterModel> filterModel);

    /**
     * Tells the current highlighting state of the text input indicated via the
     * background color.
     *
     * @return current highlight state
     */
    HighlightState getCurrentHighlightState() const;

    /**
     * Indicate whether this search text bar can be toggled to between case-
     * sensitive and -insensitiv or or if always case-insensitive shall be used.
     *
     * @param b if <code>true</code> the user can decide the toggle between
     *          case sensitivity, on <code>false</code> every search is case-
     *          insensiive
     */
    void setCaseSensitive(bool b);
    bool hasCaseSensitive() const;

    void setSearchTextSettings(const SearchTextSettings& settings);
    SearchTextSettings searchTextSettings() const;

Q_SIGNALS:

    void signalSearchTextSettings(const SearchTextSettings& settings);

public Q_SLOTS:
    void slotSearchResult(bool match);

private Q_SLOTS:

    void slotTextChanged(const QString&);
    void slotRowsInserted(const QModelIndex &parent, int start, int end);
    void slotRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void slotModelReset();

protected:

    virtual void doLoadState();
    virtual void doSaveState();

private:

    void contextMenuEvent(QContextMenuEvent* e);
    void connectToModel(QAbstractItemModel *model);
    void disconnectFromModel(QAbstractItemModel *model);
    void sync(QAbstractItemModel *model);
    void sync(QAbstractItemModel *model, const QModelIndex &index);

private:

    SearchTextBarPriv* const d;
};

}  // namespace Digikam

#endif /* SEARCH_TEXT_BAR_H */
