/*!
 * \file
 */
#include "FileUtility.h"
const int QTSTRCOMPARE_TRUE = 0;
// QString::compareでは比較結果が同じとき0が帰ってくる

using namespace std;
namespace IL{

FileUtility::FileUtility()
{
#ifdef P_CONSTRUCT
    cout << "P_CONSTRUCT > FileUtility::FileUtility()" << endl;
#endif
}

FileUtility::~FileUtility()
{
#ifdef P_DESTRUCT
    cout << "P_DESTRUCT  > FileUtility::~FileUtility()" << endl;
#endif
}

void FileUtility::setSuffixFilter(QStringList fileSuffixFilter)
{
    _fileSuffixFilter = fileSuffixFilter;
}

void FileUtility::setFile(QString fileName)
{
    QFileInfo fileInfo(fileName);
    QDir directory;
    directory = fileInfo.absoluteDir();

    if(_fileSuffixFilter.isEmpty()){
        QString suffix = "*.";
        suffix += fileInfo.suffix();
        _fileSuffixFilter.push_back(suffix);
    }

    directory.setNameFilters(_fileSuffixFilter);

    _fileInfoList = directory.entryInfoList(QDir::Files);
    _currentFileNumber = 0;
#ifdef P_FILEUTILITY_DEBUG
    cout << fileInfo.absoluteDir().absolutePath().toStdString() << endl;
    cout << fileInfo.absolutePath().toStdString() << endl;
#endif
    for(int i=0; i<_fileInfoList.size(); i++){
#ifdef P_FILEUTILITY_DEBUG
        cout << _fileInfoList.at(i).absoluteFilePath().toStdString() << endl;
#endif
        if(QTSTRCOMPARE_TRUE == QString::compare(_fileInfoList.at(i).absoluteFilePath(),
                                                 fileInfo.absoluteFilePath())) {
            _currentFileNumber = i;
            break;
        }
    }
#ifdef P_FILEUTILITY
    std::cout << "P_FILEUTILITY >> FileUtility::setFile FileName = "
              << fileName << endl;
    std::cout << "P_FILEUTILITY >> FileUtility::setFile CurrentFileNumber = "
              <<_currentFileNumber << endl;
#endif
    return;
}

int FileUtility::size() const
{
    return _fileInfoList.size();
}

int FileUtility::getCurrentNumber() const
{
    if(_fileInfoList.isEmpty()) return -1;
    return _currentFileNumber;
}

QString FileUtility::getCurrentFileName() const
{
    if(_fileInfoList.isEmpty()) return "";

    return _fileInfoList.at(_currentFileNumber).absoluteFilePath();
}

QString FileUtility::getCurrentFileNameCore() const
{
    QFileInfo info(getCurrentFileName());
    return info.fileName();
}

QString FileUtility::getCurrentFileNameCore_WOExt() const
{
    QFileInfo info(getCurrentFileName());
    return info.baseName();
}

QString FileUtility::getNextFileName() const
{
    if(_fileInfoList.isEmpty()) return "";
    int next = _currentFileNumber + 1;
    if(next >= _fileInfoList.size()){
        next = 0;
    }
    return _fileInfoList.at(next).absoluteFilePath();
}

QString FileUtility::getPreviousFileName() const
{
    int previous;
    if(_fileInfoList.isEmpty()){
        return "";
    }

    if(0 < _currentFileNumber){
        previous = _currentFileNumber - 1;
    }
    else{
        previous = _fileInfoList.size() - 1;
    }
    return _fileInfoList.at(previous).absoluteFilePath();
}

QString FileUtility::getFileName(int number) const
{
    if(_fileInfoList.isEmpty() || number >= _fileInfoList.size() || number < 0){
        return "";
    }
    return _fileInfoList.at(number).absoluteFilePath();
}

} // namespace IL
