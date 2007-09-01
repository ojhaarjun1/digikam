/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-12-21
 * Description : abstract camera interface class
 * 
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com> 
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

// Local includes.

#include "albumsettings.h"
#include "dkcamera.h"

namespace Digikam
{

DKCamera::DKCamera(const QString& title, const QString& model, const QString& port, const QString& path)
{
    m_title = title;
    m_model = model;
    m_port  = port;
    m_path  = path;

    AlbumSettings* settings = AlbumSettings::instance();
    m_imageFilter = settings->getImageFileFilter();
    m_movieFilter = settings->getMovieFileFilter();
    m_audioFilter = settings->getAudioFileFilter();
    m_rawFilter   = settings->getRawFileFilter();

    m_imageFilter = m_imageFilter.toLower();
    m_movieFilter = m_movieFilter.toLower();
    m_audioFilter = m_audioFilter.toLower();
    m_rawFilter   = m_rawFilter.toLower();
}

DKCamera::~DKCamera()
{
}

QString DKCamera::title() const
{
    return m_title;    
}

QString DKCamera::model() const
{
    return m_model;    
}

QString DKCamera::port() const
{
    return m_port;
}

QString DKCamera::path() const
{
    return m_path;
}

QString DKCamera::mimeType(const QString& fileext) const
{
    QString ext = fileext;
    
    // Massage known variations of known mimetypes into kde specific ones
    if (ext == "jpg" || ext == "jpe")
        ext = "jpeg";
    else if (ext == "tif")
        ext = "tiff";
    
    if (m_rawFilter.contains(ext))
    {
        return QString("image/x-raw");
    }
    else if (m_imageFilter.contains(ext))
    {
        return QString("image/") + ext;
    }
    else if (m_movieFilter.contains(ext))
    {
        return QString("video/") + ext;
    }
    else if (m_audioFilter.contains(ext))
    {
        return QString("audio/") + ext;
    }
    else
    {
        return QString();
    }
}

}  // namespace Digikam
