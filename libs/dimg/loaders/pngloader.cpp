/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at free.fr> 
 * Date  : 2005-11-01
 * Description : A PNG files loader for DImg framework.
 * 
 * Copyright 2005 by Gilles Caulier
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

#define PNG_BYTES_TO_CHECK 4

extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <png.h>
}

// QT includes.

#include <qfile.h>
#include <qcstring.h>

// KDE includes.

#include <kdebug.h>

// Local includes.

#include "dimg.h"
#include "pngloader.h"

namespace Digikam
{

PNGLoader::PNGLoader(DImg* image)
         : DImgLoader(image)
{
    m_hasAlpha   = false;
    m_sixteenBit = false;
}

bool PNGLoader::load(const QString& filePath)
{
    png_uint_32  w32, h32;
    int          width, height;
    FILE        *f;
    int          bit_depth, color_type, interlace_type;
    png_structp  png_ptr  = NULL;
    png_infop    info_ptr = NULL;
    
    // -------------------------------------------------------------------
    // Open the file
    
    f = fopen(QFile::encodeName(filePath), "rb");
    if ( !f )
    {
        kdWarning() << k_funcinfo << "Cannot open image file." << endl;
        return false;
    }

    unsigned char buf[PNG_BYTES_TO_CHECK];

    fread(buf, 1, PNG_BYTES_TO_CHECK, f);
    if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
    {
        kdWarning() << k_funcinfo << "Not a PNG image file." << endl;
        fclose(f);
        return false;
    }
    rewind(f);

    // -------------------------------------------------------------------
    // Initialize the internal structures
    
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        kdWarning() << k_funcinfo << "Invalid PNG image file structure." << endl;
        fclose(f);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        kdWarning() << k_funcinfo << "Cannot reading PNG image file structure." << endl;
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(f);
        return false;
    }

    // -------------------------------------------------------------------
    // PNG error handling. If an error occurs during reading, libpng
    // will jump here
    
    if (setjmp(png_ptr->jmpbuf))
    {
        kdWarning() << k_funcinfo << "Cannot acces to PNG image data." << endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(f);
        return false;
    }

    png_init_io(png_ptr, f);
    
    // -------------------------------------------------------------------
    // Read all PNG info up to image data
    
    png_read_info(png_ptr, info_ptr);
    
    png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32),
                 (png_uint_32 *) (&h32), &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);

    width  = (int)w32;
    height = (int)h32;

    if (bit_depth == 16)                       
    {
        kdWarning() << "PNG in 16 bits/color/pixel." << endl;
        m_sixteenBit = true;
        
        switch (color_type)
        {
            case PNG_COLOR_TYPE_RGB :		// RGB 
                kdWarning() << "PNG in PNG_COLOR_TYPE_RGB" << endl;
                m_hasAlpha = false;
                
                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_BEFORE);

                break;

            case PNG_COLOR_TYPE_RGB_ALPHA :	// RGBA 
                kdWarning() << "PNG in PNG_COLOR_TYPE_RGB_ALPHA" << endl;
                m_hasAlpha = true;
                break;

            case PNG_COLOR_TYPE_GRAY :		// Grayscale 
                kdWarning() << "PNG in PNG_COLOR_TYPE_GRAY" << endl;
                png_set_gray_to_rgb(png_ptr);

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_BEFORE);

                m_hasAlpha = false;
                break;

            case PNG_COLOR_TYPE_GRAY_ALPHA :	// Grayscale + Alpha 
                kdWarning() << "PNG in PNG_COLOR_TYPE_GRAY_ALPHA" << endl;
                png_set_gray_to_rgb(png_ptr);
                m_hasAlpha = true;
                break;

            case PNG_COLOR_TYPE_PALETTE :	// Indexed 
                kdWarning() << "PNG in PNG_COLOR_TYPE_PALETTE" << endl;
                png_set_palette_to_rgb(png_ptr);

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_BEFORE);

                m_hasAlpha = false;
                break;

            default:		
                kdWarning() << k_funcinfo << "PNG color type unknown." << endl;
                return false;
        };
    }
    else
    {
        kdWarning() << k_funcinfo << "PNG in >=8 bits/color/pixel." << endl;
        m_sixteenBit = false;
        png_set_packing(png_ptr);
            
        switch (color_type)
        {            
            case PNG_COLOR_TYPE_RGB :		// RGB 
                kdWarning() << "PNG in PNG_COLOR_TYPE_RGB" << endl;

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_BEFORE);

                m_hasAlpha = false;
                break;

            case PNG_COLOR_TYPE_RGB_ALPHA :	// RGBA 
                kdWarning() << "PNG in PNG_COLOR_TYPE_RGB_ALPHA" << endl;
                m_hasAlpha = true;
                break;

            case PNG_COLOR_TYPE_GRAY :		// Grayscale 
                kdWarning() << "PNG in PNG_COLOR_TYPE_GRAY" << endl;
                png_set_gray_1_2_4_to_8(png_ptr);
                png_set_gray_to_rgb(png_ptr);

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_BEFORE);

                m_hasAlpha = false;
                break;

            case PNG_COLOR_TYPE_GRAY_ALPHA :	// Grayscale + alpha 
                kdWarning() << "PNG in PNG_COLOR_TYPE_GRAY_ALPHA" << endl;
                png_set_gray_to_rgb(png_ptr);
                m_hasAlpha = true;
                break;

            case PNG_COLOR_TYPE_PALETTE :	// Indexed 
                kdWarning() << "PNG in PNG_COLOR_TYPE_PALETTE" << endl;
                png_set_packing(png_ptr);
                png_set_palette_to_rgb(png_ptr);

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_BEFORE);

                m_hasAlpha = true;
                break;

            default:		
                kdWarning() << k_funcinfo << "PNG color type unknown." << endl;
                return false;
        };
    }

    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	png_set_tRNS_to_alpha(png_ptr);

    if (QImage::systemByteOrder() == QImage::LittleEndian)  // Intel 
        png_set_bgr(png_ptr);
    else                                                    // PPC
        png_set_swap_alpha(png_ptr);
    
    // -------------------------------------------------------------------
    // Allocate DImg image data container depending pixels depth.
    
    png_read_update_info(png_ptr, info_ptr);

    uchar *data  = 0;

    if (m_sixteenBit)                        
        data = new uchar[width*height*8];  // 16 bits/color/pixel
    else 
        data = new uchar[width*height*4];  // 8 bits/color/pixel

    // -------------------------------------------------------------------
    // Get image lines data.

    uchar **lines = 0;
    lines = (uchar **)malloc(height * sizeof(uchar *));
    if (!lines)
    {
        kdWarning() << k_funcinfo << "Cannot allocate memory to load PNG image data." << endl;
        png_read_end(png_ptr, info_ptr);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(f);
        delete [] data;
        return false;
    }

    for (int i = 0; i < height; i++)
    {
        if (m_sixteenBit)
            lines[i] = data + (i * width * 8);
        else
            lines[i] = data + (i * width * 4);
    }

    png_read_image(png_ptr, lines);
    free(lines);

    // -------------------------------------------------------------------
    // Swap bytes in 16 bits/color/pixel for DImg
    
    if (m_sixteenBit)   
    {
        uchar ptr[8];   // One pixel to swap
        
        for (int p = 0; p < width*height*8; p+=8)
        {
            memcpy (&ptr[0], &data[p], 8);  // Current pixel 
                
            data[ p ] = ptr[1];  // Blue
            data[p+1] = ptr[0];
            data[p+2] = ptr[3];  // Green
            data[p+3] = ptr[2];  
            data[p+4] = ptr[5];  // Red
            data[p+5] = ptr[4];
            data[p+6] = ptr[7];  // Alpha
            data[p+7] = ptr[6];            
        }
    }
    
    // -------------------------------------------------------------------
    // Read image ICC profile
    
    png_charp   profile_name, profile_data=NULL;
    png_uint_32 profile_size;
    int         compression_type;
    
    png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &profile_size);
    
    if (profile_data != NULL) 
    {
        kdDebug() << "Reading PNG ICC Profil (" << profile_name << ")" << endl;
        QByteArray profile_rawdata = imageICCProfil();
        profile_rawdata.resize(profile_size);
        memcpy(profile_rawdata.data(), profile_data, profile_size);
    }
    
    // -------------------------------------------------------------------
    // Get embbeded text data.
    
    png_text* text_ptr;
    int num_comments = png_get_text(png_ptr, info_ptr, &text_ptr, NULL);
    
    /*
    Standard Embedded text includes in PNG :
    
    Title            Short (one line) title or caption for image
    Author           Name of image's creator
    Description      Description of image (possibly long)
    Copyright        Copyright notice
    Creation Time    Time of original image creation
    Software         Software used to create the image
    Disclaimer       Legal disclaimer
    Warning          Warning of nature of content
    Source           Device used to create the image
    Comment          Miscellaneous comment; conversion from GIF comment
    */    
        
    for (int i = 0; i < num_comments; i++)
    {
        imageSetEmbbededText(text_ptr[i].key, text_ptr[i].text);
        kdDebug() << "Reading PNG Embedded text: key=" << text_ptr[i].key << " text=" << text_ptr[i].text << endl;

        QString key(text_ptr[i].key);

        if (key == "Source")
           imageSetCameraModel(key);

        if (key == "Software")
           imageSetCameraConstructor(key);
    }
    
    // -------------------------------------------------------------------
    
    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    fclose(f);

    imageWidth()  = width;
    imageHeight() = height;
    imageData()   = data;
    imageSetAttribute("format", "PNG");
    
    return true;
}

bool PNGLoader::save(const QString& filePath)
{
    FILE          *f;
    png_structp    png_ptr;
    png_infop      info_ptr;
    uchar         *ptr, *data = 0;
    uint           x, y, j;
    png_bytep      row_ptr;
    png_color_8    sig_bit;
    int            quality = 75;
    int            compression = 3;
    
    // -------------------------------------------------------------------
    // Open the file
    
    f = fopen(QFile::encodeName(filePath), "wb");
    if ( !f )
    {
        kdWarning() << k_funcinfo << "Cannot open target image file." << endl;
        return false;
    }
    
    // -------------------------------------------------------------------
    // Initialize the internal structures
    
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        kdWarning() << k_funcinfo << "Invalid target PNG image file structure." << endl;
        fclose(f);
        return false;
    }
    
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        kdWarning() << k_funcinfo << "Cannot create PNG image file structure." << endl;
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        fclose(f);
        return false;
    }
    
    // -------------------------------------------------------------------
    // PNG error handling. If an error occurs during writing, libpng
    // will jump here
        
    if (setjmp(png_ptr->jmpbuf))
    {
        kdWarning() << k_funcinfo << "Internal PNG error during writing file." << endl;
        fclose(f);
        png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
        png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
        return false;
    }

    png_init_io(png_ptr, f);
    
    if (QImage::systemByteOrder() == QImage::LittleEndian)  // Intel 
        png_set_bgr(png_ptr);
    else                                                    // PPC
        png_set_swap_alpha(png_ptr);
    
    if (imageHasAlpha())
    {
        png_set_IHDR(png_ptr, info_ptr, imageWidth(), imageHeight(), imageBitsDepth(),
                     PNG_COLOR_TYPE_RGB_ALPHA,  PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        if (imageSixteenBit())
            data = new uchar[imageWidth() * 8 * sizeof(uchar)];
        else
            data = new uchar[imageWidth() * 4 * sizeof(uchar)];        
    }
    else
    {
        png_set_IHDR(png_ptr, info_ptr, imageWidth(), imageHeight(), imageBitsDepth(), 
                     PNG_COLOR_TYPE_RGB,        PNG_INTERLACE_NONE, 
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
        if (imageSixteenBit())
            data = new uchar[imageWidth() * 6 * sizeof(uchar)];
        else
            data = new uchar[imageWidth() * 3 * sizeof(uchar)];        
    }
    
    sig_bit.red   = imageBitsDepth();
    sig_bit.green = imageBitsDepth();
    sig_bit.blue  = imageBitsDepth();
    sig_bit.alpha = imageBitsDepth();
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);
        
    // -------------------------------------------------------------------
    // Quality to convert to compression 
    
    QVariant qualityAttr = imageGetAttribute("quality");
    quality = qualityAttr.isValid() ? qualityAttr.toInt() : 90;
    
    if (quality < 1)
        quality = 1;
    if (quality > 99)
        quality = 99;

    quality     = quality / 10;
    compression = 9 - quality;
    
    if (compression < 0)
        compression = 0;
    if (compression > 9)
        compression = 9;
    
    png_set_compression_level(png_ptr, compression);
    
    // -------------------------------------------------------------------
    // Write ICC profil.
    
    QByteArray profile_rawdata = imageICCProfil();
    
    if (profile_rawdata.data() != 0)
    {
        kdDebug() << "Writing PNG ICC Profil" << endl;
        png_set_iCCP(png_ptr, info_ptr, "icc", PNG_COMPRESSION_TYPE_BASE, profile_rawdata.data(), profile_rawdata.size());
    }    

    // -------------------------------------------------------------------
    // Write embbeded Text
    
    typedef QMap<QString, QString> EmbeddedTextMap;
    EmbeddedTextMap map = imageEmbeddedText();
    
    for (EmbeddedTextMap::iterator it = map.begin(); it != map.end(); ++it)
    {
        png_text text;

        text.key  = (char*)it.key().ascii();
        text.text = (char*)it.data().ascii();
        kdDebug() << "Writing PNG Embedded text: key=" << text.key << " text=" << text.text << endl;
        text.compression = PNG_TEXT_COMPRESSION_zTXt;
        png_set_text(png_ptr, info_ptr, &(text), 1);
    }

    // -------------------------------------------------------------------
    // Write image data

    png_write_info(png_ptr, info_ptr);
    png_set_shift(png_ptr, &sig_bit);
    png_set_packing(png_ptr);
    ptr = imageData();
    
    for (y = 0; y < imageHeight(); y++)
    {
        j = 0;
        
        for (x = 0; x < imageWidth()*imageBytesDepth(); x+=imageBytesDepth())
        {
            if (imageSixteenBit())
            {
                if (imageHasAlpha())
                {
                    data[j++] = ptr[x+1];  // Blue
                    data[j++] = ptr[ x ];
                    data[j++] = ptr[x+3];  // Green
                    data[j++] = ptr[x+2];  
                    data[j++] = ptr[x+5];  // Red
                    data[j++] = ptr[x+4];
                    data[j++] = ptr[x+7];  // Alpha
                    data[j++] = ptr[x+6];
                }
                else
                {    
                    data[j++] = ptr[x+1];  // Blue
                    data[j++] = ptr[ x ];
                    data[j++] = ptr[x+3];  // Green
                    data[j++] = ptr[x+2];  
                    data[j++] = ptr[x+5];  // Red
                    data[j++] = ptr[x+4];
                }
            }
            else
            {
                if (imageHasAlpha())
                {
                    data[j++] = ptr[ x ];  // Blue
                    data[j++] = ptr[x+1];  // Green
                    data[j++] = ptr[x+2];  // Red
                    data[j++] = ptr[x+3];  // Alpha
                }
                else
                {    
                    data[j++] = ptr[ x ];  // Blue
                    data[j++] = ptr[x+1];  // Green
                    data[j++] = ptr[x+2];  // Red
                }
            }
        }
        
        row_ptr = (png_bytep) data;
        
        png_write_rows(png_ptr, &row_ptr, 1);
        ptr += (imageWidth() * imageBytesDepth());
    }
    
    if (data) 
        delete [] data;
        
    // -------------------------------------------------------------------
            
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
    png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
    
    fclose(f);
    return true;
}

bool PNGLoader::hasAlpha() const
{
    return m_hasAlpha;    
}

bool PNGLoader::sixteenBit() const
{
    return m_sixteenBit;    
}

}  // NameSpace Digikam
