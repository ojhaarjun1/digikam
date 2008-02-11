/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-04-09
 * Description : Collection location management
 *
 * Copyright (C) 2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef COLLECTIONMANAGER_H
#define COLLECTIONMANAGER_H

// Qt includes

#include <QString>
#include <QStringList>

// KDE includes

#include <kurl.h>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class CollectionLocation;
class CollectionManagerPrivate;

class DIGIKAM_EXPORT CollectionManager : public QObject
{

    Q_OBJECT

public:

    static CollectionManager *instance();
    static void cleanUp();

    /**
     * Clears all locations and re-reads the lists of collection locations
     */
    void refresh();

    /** CollectionLocation objects returned are simple data containers.
     *  If the corresponding location is returned, the data is still safe to access,
     *  but does not represent anything.
     *  Therefore, do not store returned objects, but prefer to retrieve them freshly.
     */

    /**
     * Add the given file system location as new collection location.
     * Type and availability will be detected.
     * On failure returns null. This would be the case if the given
     * url is already contained in another collection location.
     */
    CollectionLocation addLocation(const KUrl &fileUrl);

    enum LocationCheckResult
    {
        /// The check did not succeed, status unknown
        LocationInvalidCheck,
        /// All right. The accompanying message may be empty.
        LocationAllRight,
        /// Location can be added, but the user should be aware of a problem
        LocationHasProblems,
        /// Adding the location will fail (e.g. there is already a location for the path)
        LocationNotAllowed
    };

    /**
     * Analyzes the given file path. Creates an info message
     * describing the result of identification or possible problems.
     * The text is i18n'ed and can be presented to the user.
     * The returned result enum describes the test result.
     */
    LocationCheckResult checkLocation(const KUrl &fileUrl, QString *message = 0);

    /**
     * Removes the given location. This means that all images contained on the
     * location will be removed from the database, all tags will be lost.
     */
    void removeLocation(const CollectionLocation &location);

    /**
     * Returns a list of all CollectionLocations stored in the database
     */
    QList<CollectionLocation> allLocations();
    /**
     * Returns a list of all currently available CollectionLocations
     */
    QList<CollectionLocation> allAvailableLocations();
    /**
     * Returns a list of the paths of all currently available CollectionLocations
     */
    QStringList allAvailableAlbumRootPaths();

    /**
     * Returns the location for the given album root id
     */
    CollectionLocation locationForAlbumRootId(int id);

    /**
     * Returns the CollectionLocation that contains the given album root.
     * The path must be an album root with isAlbumRoot() == true.
     * Returns 0 if no collection location matches.
     * Only available (or hidden, but available) locations are guaranteed to be found.
     */
    CollectionLocation locationForAlbumRoot(const KUrl &fileUrl);
    CollectionLocation locationForAlbumRootPath(const QString &albumRootPath);

    /**
     * Returns the CollectionLocation that contains the given path.
     * Equivalent to calling locationForAlbumRoot(albumRoot(fileUrl)).
     * Only available (or hidden, but available) locations are guaranteed to be found.
     */
    CollectionLocation locationForUrl(const KUrl &fileUrl);
    CollectionLocation locationForPath(const QString &filePath);

    /**
     * Returns the album root path for the location with the given id.
     * Returns a null QString if the location does not exist or is not available.
     */
    QString albumRootPath(int id);

    /**
     * For a given path, the part of the path that forms the album root is returned,
     * ending without a slash. Example: "/media/fotos/Paris 2007" gives "/media/fotos".
     * Only available (or hidden, but available) album roots are guaranteed to be found.
     */
    KUrl    albumRoot(const KUrl &fileUrl);
    QString albumRootPath(const KUrl &fileUrl);
    QString albumRootPath(const QString &filePath);
    /**
     * Returns true if the given path forms an album root.
     * It will return false if the path is a path below an album root,
     * or if the the path does not belong to an album root.
     * Example: "/media/fotos/Paris 2007" is an album with album root "/media/fotos".
     *          "/media/fotos" returns true, "/media/fotos/Paris 2007" and "/media" return false.
     * Only available (or hidden, but available) album roots are guaranteed to be found.
     */
    bool    isAlbumRoot(const KUrl &fileUrl);
    /// the file path should not end with the directory slash. Using DatabaseUrl's method is fine.
    bool    isAlbumRoot(const QString &filePath);

    /**
     * Returns the album part of of the given file path, i.e. the album root path
     * at the beginning is removed and the second part, starting with "/", ending without a slash,
     * is returned. Example: "/media/fotos/Paris 2007" gives "/Paris 2007"
     * Returns a null QString if the file path is not located in an album root.
     * Returns "/" if the file path is an album root.
     * Note that trailing slashes are removed in the return value, regardless if there was
     * one or not.
     * Note that you have to feed a path/url pointing to a directory. File names cannot
     * be recognized as such by this method, and will be treated as a directory.
     */
    QString album(const KUrl &fileUrl);
    QString album(const QString &filePath);
    QString album(const CollectionLocation &location, const KUrl &fileUrl);
    QString album(const CollectionLocation &location, const QString &filePath);

    /**
     * Returns just one album root, out of the list of available location,
     * the one that is most suitable to serve as a default, e.g.
     * to suggest as default place when the user wants to add files.
     */
    KUrl oneAlbumRoot();
    QString oneAlbumRootPath();

signals:

    /** Emitted when the status of a collection location changed.
     *  This means that the location became available, hidden or unavailable.
     *
     *  An added location will change its status after addition,
     *  from Null to Available, Hidden or Unavailable.
     *
     *  A removed location will change its status to Deleted
     *  during the removal; in this case, you shall not use the object
     *  passed with this signal with any method of CollectionManager.
     *
     *  The second signal argument is of type CollectionLocation::Status
     *  and describes the status before the state change occurred
     */
    void locationStatusChanged(const CollectionLocation &location, int oldStatus);

private slots:

    void deviceChange(const QString &);

private:

    CollectionManager();
    ~CollectionManager();
    static CollectionManager *m_instance;
    void updateLocations();

    CollectionManagerPrivate *d;
    friend class CollectionManagerPrivate;

    Q_PRIVATE_SLOT(d, void slotTriggerUpdateVolumesList());

signals: // private

    void triggerUpdateVolumesList();
};

}  // namespace Digikam

#endif // COLLECTIONMANAGER_H
