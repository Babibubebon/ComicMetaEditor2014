/*!
 * \file
 */

#include "ComicMetaEditorSetting.h"

ComicMetaEditorSetting::ComicMetaEditorSetting()
{
#ifdef Q_OS_MAC
    _fileDirectory = "../../../../";
#else
    _fileDirectory = "../../";
#endif
    _filterForImage = "*.bmp *.BMP *.gif *.tif *.tiff *.png *.jpg *.jpeg *.pgm *.pbm";
    loadSetting(DEFAULT_SETTING_FILE);
}


bool ComicMetaEditorSetting::loadSetting(QString fileName)
{
    return true;
}

QString ComicMetaEditorSetting::getFileDirectory() const
{
    return _fileDirectory;
}

void ComicMetaEditorSetting::setFileDirectory(QString directoryPath)
{
    _fileDirectory = directoryPath;
}

bool ComicMetaEditorSetting::saveSetting(QString fileName)
{
    return true;
}

QString ComicMetaEditorSetting::getFilterForImage() const
{
    return _filterForImage;
}

