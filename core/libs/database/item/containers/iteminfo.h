/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_ITEM_INFO_H
#define DIGIKAM_ITEM_INFO_H

// Qt includes

#include <QString>
#include <QDateTime>
#include <QList>
#include <QSize>
#include <QUrl>

// Local includes

#include "coredbalbuminfo.h"
#include "digikam_export.h"
#include "dshareddata.h"
#include "coredburl.h"
#include "iteminfolist.h"
#include "coredbfields.h"

namespace Digikam
{

class DImageHistory;
class HistoryImageId;
class ItemComments;
class ImageCommonContainer;
class ItemCopyright;
class ItemExtendedProperties;
class ItemInfoData;
class ItemListerRecord;
class ImageMetadataContainer;
class VideoMetadataContainer;
class ItemPosition;
class ItemTagPair;
class PhotoInfoContainer;
class VideoInfoContainer;
class Template;
class ThumbnailIdentifier;
class ThumbnailInfo;

/**
 * The ItemInfo class contains provides access to the database for a single image.
 * The properties can be read and written. Information will be cached.
 */
class DIGIKAM_DATABASE_EXPORT ItemInfo
{
public:

    /**
     * Constructor
     * Creates a null image info
     */
    ItemInfo();

    /**
     * Constructor. Creates an ItemInfo object without any cached data initially.
     * @param    ID       unique ID for this image
     */
    explicit ItemInfo(qlonglong ID);

    /**
     * Constructor. Creates an ItemInfo object where the provided information
     * will initially be available cached, without database access.
     */
    explicit ItemInfo(const ItemListerRecord& record);

    ItemInfo(const ItemInfo& info);

    /**
     * Creates an ItemInfo object from a file url.
     */
    static ItemInfo fromLocalFile(const QString& path);
    static ItemInfo fromUrl(const QUrl& url);

    /**
     * Create an ItemInfo object from the given combination, which
     * must be cleaned and corresponding to the values in the database
     */
    static ItemInfo fromLocationAlbumAndName(int locationId, const QString& album, const QString& name);

    /**
     * Destructor
     */
    ~ItemInfo();

    ItemInfo& operator=(const ItemInfo& info);

    bool operator==(const ItemInfo& info) const;
    bool operator!=(const ItemInfo& info) const
    {
        return !operator==(info);
    }
    bool operator<(const ItemInfo& info)  const;
    uint hash()                            const;

    /**
     * Returns if this objects contains valid data
     */
    bool isNull() const;

    /**
     * @return the name of the image
     */
    QString name() const;

    /**
     * @return the datetime of the image
     */
    QDateTime dateTime() const;

    /**
     * @return the modification datetime of the image
     */
    QDateTime modDateTime() const;

    /**
     * @return the filesize of the image
     */
    qlonglong fileSize() const;

    /**
     * @return the unique hash of the image.
     */
    QString uniqueHash() const;

    /**
     * @return the dimensions of the image (valid only if dimensions
     * have been requested)
     */
    QSize dimensions() const;

    /**
     * Returns the file:// url.
     * This is equivalent to QUrl::fromLocalFile(filePath())
     */
    QUrl fileUrl() const;

    /**
     * Returns the file path to the image
     */
    QString filePath() const;

    /**
     * @return the unique image id for this item
     */
    qlonglong id() const;

    /**
     * @return the id of the PAlbum to which this item belongs
     */
    int albumId() const;

    /**
     * The album root id
     */
    int albumRootId() const;

    /**
     * @return the default title for this item
     */
    QString title() const;

    /**
     * @return the default comment for this item
     */
    QString comment() const;

    /**
     * @return the id of the Aspect Ratio for this item
     */
    double aspectRatio() const;

    /**
     * Returns the Pick Label Id (see PickLabel values in globals.h)
     */
    int pickLabel() const;

    /**
     * Returns the Color Label Id (see ColorLabel values in globals.h)
     */
    int colorLabel() const;

    /**
     * Returns the rating
     */
    int rating() const;

    /**
     * Returns the manual sort order
     */
    qlonglong manualOrder() const;

    /**
     * Returns the category of the item: Image, Audio, Video
     */
    DatabaseItem::Category category() const;

    /** Returns the image format / mimetype as a standardized
     *  string (see project/documents/DBSCHEMA.ODS).
     */
    QString format() const;

    /**
     * @return a list of IDs of tags assigned to this item
     * @see tagNames
     * @see tagPaths
     * @see Album::id()
     */
    QList<int> tagIds() const;

    /**
     * Returns true if the image is marked as visible in the database.
     */
    bool isVisible() const;

    /**
     * Returns true if the corresponding file was not deleted.
     */
    bool isRemoved() const;

    /**
     * Returns the orientation of the image,
     * (MetaEngine::ImageOrientation, EXIF standard)
     */
    int orientation() const;

    /**
     * Retrieve the ItemComments object for this item.
     * This object allows full read and write access to all comments
     * and their properties.
     * You need to hold CoreDbAccess to ensure the validity.
     * For simple, cached read access see comment().
     */
    ItemComments imageComments(CoreDbAccess& access) const;

    /**
     * Retrieve the ItemCopyright object for this item.
     * This object allows full read and write access to all copyright
     * values.
     */
    ItemCopyright imageCopyright() const;

    /**
     * Retrieve the ItemExtendedProperties object for this item.
     * This object allows full read and write access to all extended properties
     * values.
     */
    ItemExtendedProperties imageExtendedProperties() const;

    /**
     * Retrieve the ItemPosition object for this item.
     */
    ItemPosition imagePosition() const;

    /**
     * Retrieves the coordinates and the altitude.
     * Returns 0 if hasCoordinates(), or hasAltitude resp, is false.
     */
    double longitudeNumber() const;
    double latitudeNumber()  const;
    double altitudeNumber()  const;
    bool   hasCoordinates()  const;
    bool   hasAltitude()     const;

    /**
     * Retrieve an ItemTagPair object for a single tag, or for all
     * image/tag pairs for which properties are available
     * (not necessarily the assigned tags)
     */
    ItemTagPair imageTagPair(int tagId)         const;
    QList<ItemTagPair> availableItemTagPairs() const;

    /**
     * Retrieves and sets the image history from the database.
     * Note: The image history retrieved here does typically include all
     * steps from the original to this image, but does not reference this image
     * itself.
     *
     */
    DImageHistory imageHistory() const;
    void setItemHistory(const DImageHistory& history);
    bool hasImageHistory()       const;

    /**
     * Retrieves and sets this' images UUID
     */
    QString uuid() const;
    void setUuid(const QString& uuid);

    /**
     * Constructs a HistoryImageId with all available information for this image.
     */
    HistoryImageId historyImageId() const;

    /**
     * Retrieve information about images from which this image
     * is derived (ancestorImages) and images that have been derived
     * from this images (derivedImages).
     */
    bool hasDerivedImages()           const;
    bool hasAncestorImages()          const;

    QList<ItemInfo> derivedImages()  const;
    QList<ItemInfo> ancestorImages() const;

    /**
     * Returns the cloud of all directly or indirectly related images,
     * derived images or ancestors, in from of "a derived from b" pairs.
     */
    QList<QPair<qlonglong, qlonglong> > relationCloud() const;

    /**
     * Add a relation to the database:
     * This image is derived from the ancestorImage.
     */
    void markDerivedFrom(const ItemInfo& ancestorImage);

    /**
     * The image is grouped in the group of another (leading) image.
     */
    bool isGrouped() const;
    /**
     * The image is the leading image of a group,
     * there are other images grouped behind this one.
     */
    bool hasGroupedImages()      const;
    int  numberOfGroupedImages() const;

    /**
     * Returns the leading image of the group.
     * Returns a null image if this image is not grouped (isGrouped())
     */
    ItemInfo groupImage()   const;
    qlonglong groupImageId() const;

    /**
     * Returns the list of images grouped behind this image (not including this
     * image itself) and an empty list if there is none.
     */
    QList<ItemInfo> groupedImages() const;

    /**
     * Group this image behind the given image
     */
    void addToGroup(const ItemInfo& info);

    /**
     * This image is grouped behind another image:
     * Remove this image from its group
     */
    void removeFromGroup();

    /**
     * This image hasGroupedImages(): Split up the group,
     * remove all groupedImages() from this image's group.
     */
    void clearGroup();

    /**
     * Retrieve information about the image,
     * in form of numbers and user presentable strings,
     * for certain defined fields of information (see databaseinfocontainers.h)
     */
    ImageCommonContainer   imageCommonContainer()   const;
    ImageMetadataContainer imageMetadataContainer() const;
    VideoMetadataContainer videoMetadataContainer() const;
    PhotoInfoContainer     photoInfoContainer()     const;
    VideoInfoContainer     videoInfoContainer()     const;

    typedef DatabaseFields::Hash<QVariant> DatabaseFieldsHashRaw;

    /**
     * @todo Supports only VideoMetadataField and ImageMetadataField values for now.
     */
    DatabaseFieldsHashRaw getDatabaseFieldsRaw(const DatabaseFields::Set& requestedSet) const;
    QVariant getDatabaseFieldRaw(const DatabaseFields::Set& requestedField) const;

    /**
     * Retrieve metadata template information about the image.
     */
    Template metadataTemplate() const;

    /**
     * Set metadata template information (write it to database)
     * @param t the new template data.
     */
    void setMetadataTemplate(const Template& t);

    /**
     * Remove all template info about the image from database.
     */
    void removeMetadataTemplate();

    /**
     * Set the name (write it to database)
     * @param newName the new name.
     */
    void setName(const QString& newName);

    /**
     * Set the date and time (write it to database)
     * @param dateTime the new date and time.
     */
    void setDateTime(const QDateTime& dateTime);

    /**
     * Set the modification date and time (write it to database)
     * @param dateTime the new modification date and time.
     */
    void setModDateTime(const QDateTime& dateTime);

    /**
     * Adds a tag to the item (writes it to database)
     * @param tagID the ID of the tag to add
     */
    void setTag(int tagID);

    /**
     * Adds tags in the list to the item.
     * Tags are created if they do not yet exist
     */
    void addTagPaths(const QStringList& tagPaths);

    /**
     * Remove a tag from the item (removes it from database)
     * @param tagID the ID of the tag to remove
     */
    void removeTag(int tagID);

    /**
     * Remove all tags from the item (removes it from database)
     */
    void removeAllTags();

    /** Set the pick Label Id for the item (see PickLabel values from globals.h)
     */
    void setPickLabel(int value);

    /**
     * Set the color Label Id for the item (see ColorLabel values from globals.h)
     */
    void setColorLabel(int value);

    /**
     * Set the rating for the item
     */
    void setRating(int value);

    /**
     * Set the manual sorting order for the item
     */
    void setManualOrder(qlonglong value);

    /**
     * Set the orientation for the item
     */
    void setOrientation(int value);

    /**
     * Set the visibility flag - triggers between Visible and Hidden
     */
    void setVisible(bool isVisible);

    /**
     * Copy database information of this item to a newly created item
     * @param  dstAlbumID  destination album id
     * @param  dstFileName new filename
     * @return an ItemInfo object of the new item
     */
    //TODO: Move to album?
    ItemInfo copyItem(int dstAlbumID, const QString& dstFileName);

    /**
     * Returns true if this is a valid ItemInfo,
     * and the location of the image is currently available
     * (information freshly obtained from CollectionManager)
     */
    bool isLocationAvailable() const;

    /**
     * Scans the database for items with the given signature.
     */
    QList<ItemInfo> fromUniqueHash(const QString& uniqueHash, qlonglong fileSize);

    /**
     * Fills a ThumbnailIdentifier / ThumbnailInfo from this ItemInfo
     */
    ThumbnailIdentifier thumbnailIdentifier() const;
    ThumbnailInfo thumbnailInfo() const;
    static ThumbnailIdentifier thumbnailIdentifier(qlonglong id);

    double similarityTo(const qlonglong imageId) const;
    double currentSimilarity() const;
    /**
     * Returns the id of the current fuzzy search reference image.
     */
    qlonglong currentReferenceImage() const;

private:

    friend class ItemInfoCache;
    friend class ItemInfoList;

    DSharedDataPointer<ItemInfoData> m_data;
};

inline uint qHash(const ItemInfo& info)
{
    return info.hash();
}

//! qDebug() stream operator. Writes property @a info to the debug output in a nicely formatted way.
DIGIKAM_DATABASE_EXPORT QDebug operator<<(QDebug stream, const ItemInfo& info);

} // namespace Digikam

Q_DECLARE_TYPEINFO(Digikam::ItemInfo, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(Digikam::ItemInfo)

#endif // DIGIKAM_ITEM_INFO_H
