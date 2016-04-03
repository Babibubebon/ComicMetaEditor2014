/*!
 * \file
 */

#include "ComicMetadata.h"
#include <iostream>
QString UnDefinedCharacterName = "Undefined Character";


using namespace std;
FrameData::FrameData(){
    GIData.setColorPreset(GraphicsItemDataColor_Blue);
    this->sceneBoundary = false;
}

CharacterData::CharacterData()
{
    GIData.setColorPreset(GraphicsItemDataColor_Red);
    characterName = "--";
    characterID = 0;
    targetFrame = 0;
}

DialogData::DialogData()
{
    text = "";
    narration = false;
    fontSize = 3;
    targetCharacterID = 0;
    targetFrame = 0;
    GIData.setColorPreset(GraphicsItemDataColor_Green);
    characterName = "--";
}

OnomatopoeiaData::OnomatopoeiaData()
{
    GIData.setColorPreset(GraphicsItemDataColor_Purple);
    text = "";
    fontSize = 3;
    targetFrame = 0;
}

ItemData::ItemData()
{
    GIData.setColorPreset(GraphicsItemDataColor_LightBlue);
    description = "";
    itemClass = "Item";
    targetFrame = 0;
}

ComicMetadata::ComicMetadata()
{
    frame = QSharedPointer<QVector<FrameData> >(new QVector<FrameData>);
    character = QSharedPointer<QVector<CharacterData> >(new QVector<CharacterData>);
    dialog = QSharedPointer<QVector<DialogData> >(new QVector<DialogData>);
    onomatopoeia = QSharedPointer<QVector<OnomatopoeiaData> >(new QVector<OnomatopoeiaData>);
    item = QSharedPointer<QVector<ItemData> >(new QVector<ItemData>);
    clear();
    indent = 4;
}


QVector<GraphicsItemData> ComicMetadata::getGraphicsItemList(ComicMetadataType type)
{
    QVector<GraphicsItemData> GraphicsItemList;
    switch(type){
    case ComicMetadata_Frame:
        for(int i=0; i<frame.data()->size();i++){
            GraphicsItemList.push_back(frame.data()->at(i).GIData);
        }
        break;
    case ComicMetadata_Character:
        for(int i=0; i<character.data()->size();i++){
            GraphicsItemList.push_back(character.data()->at(i).GIData);
        }
        break;
    case ComicMetadata_Dialog:
        for(int i=0; i<dialog.data()->size();i++){
            GraphicsItemList.push_back(dialog.data()->at(i).GIData);
        }
        break;
    case ComicMetadata_Onomatopoeia:
        for(int i=0; i<onomatopoeia.data()->size();i++){
            GraphicsItemList.push_back(onomatopoeia.data()->at(i).GIData);
        }
        break;
    default:
        break;
    }
    return GraphicsItemList;
}

void ComicMetadata::clear()
{
    clearCommon();
    clearPageMetadata();
}
void ComicMetadata::clearCommon()
{
    workTitle.clear();
    episodeNumber = -1;
    pageNumber = 0;
    clearCharacterName();
    imageFileName = "";
}
void ComicMetadata::clearPageMetadata()
{
    frame.data()->clear();
    character.data()->clear();
    dialog.data()->clear();
    onomatopoeia.data()->clear();
    item.data()->clear();
}

void ComicMetadata::deleteItem(ComicMetadataType target, int number)
{
    switch(target){
    case ComicMetadata_Frame:
        if(number >= frame.data()->size()) return;
        std::cout << "deleteItem:Frame"<< number << std::endl;
        frame.data()->removeAt(number);
        break;
    case ComicMetadata_Character:
        if(number >= character.data()->size()) return;
        character.data()->removeAt(number);
        break;
    case ComicMetadata_Dialog:
        if(number >= dialog.data()->size()) return;
        dialog.data()->removeAt(number);
        break;
    case ComicMetadata_Onomatopoeia:
        if(number >= onomatopoeia.data()->size()) return;
        onomatopoeia.data()->removeAt(number);
        break;
    case ComicMetadata_Item:
        if(number >= item.data()->size()) return;
        item.data()->removeAt(number);
    case ComicMetadata_All:
        break;
    default:
        break;
    }

}


void OnomatopoeiaData::setText(QString str){
    text = str;
}

void ComicMetadata::renewAllMangaPath()
{
    for(int i=0; i<frame.data()->size(); i++){
       renewMangaPath_Frame(i);
    }
    for(int i=0; i<character.data()->size(); i++){
        renewMangaPath_Character(i);
    }
    for(int i=0; i<dialog.data()->size(); i++){
        renewMangaPath_Dialog(i);
    }
    for(int i=0; i<onomatopoeia.data()->size(); i++){
        renewMangaPath_Onomatopoeia(i);
    }
    for(int i=0; i<item.data()->size(); i++){
        renewMangaPath_Item(i);
    }
}

void ComicMetadata::renewMangaPath_Frame(int number)
{
    if(number >= frame.data()->size() || number < 0) return;

    FrameData buff = frame.data()->at(number);
    QString path("");
    path += MangaPath_title_episode();
    path += MangaPath_page();
    path += MangaPath_frame(number+1);
    buff.mangaPath = path;
    frame.data()->replace(number, buff);
}
void ComicMetadata::renewMangaPath_Character(int number)
{
    if(number >= character.data()->size() || number < 0) return;
    CharacterData buff = character.data()->at(number);
    QString path = MangaPath_title_episode_page_frame(buff.targetFrame);
    path += QString("c%1/").arg(number+1, 3, 10, QChar('0'));
    buff.mangaPath = path;
    character.data()->replace(number, buff);
}
void ComicMetadata::renewMangaPath_Dialog(int number)
{
    if(number >= dialog.data()->size() || number < 0) return;
    DialogData buff = dialog.data()->at(number);
    QString path = MangaPath_title_episode_page_frame(buff.targetFrame);
    path += QString("d%1/").arg(number+1, 3, 10, QChar('0'));
    buff.mangaPath = path;
    dialog.data()->replace(number, buff);
}
void ComicMetadata::renewMangaPath_Onomatopoeia(int number)
{
    if(number >= onomatopoeia.data()->size() || number < 0) return;
    OnomatopoeiaData buff = onomatopoeia.data()->at(number);
    QString path = MangaPath_title_episode_page_frame(buff.targetFrame);
    path += QString("o%1").arg(number+1, 3, 10, QChar('0'));
    buff.mangaPath = path;
    onomatopoeia.data()->replace(number, buff);
}
void ComicMetadata::renewMangaPath_Item(int number)
{
    if(number >= item.data()->size() || number < 0) return;
    ItemData buff = item.data()->at(number);
    QString path = MangaPath_title_episode_page_frame(buff.targetFrame);
    path += QString("i%1").arg(number+1, 3, 10, QChar('0'));
    buff.mangaPath = path;
    item.data()->replace(number, buff);
}

QString ComicMetadata::MangaPath_title_episode()
{
    QString str;
    if(!workTitle.isEmpty()){
        str += workTitle;
        str += "/";
    }
    if(episodeNumber > -1){
        str += QString("e%1/").arg(episodeNumber, 4, 10, QChar('0'));
    }
    return str;
}

QString ComicMetadata::MangaPath_page()
{
    QString str;
    if(pageNumber > -1){
        str = QString("p%1/").arg(pageNumber, 4, 10, QChar('0'));
    }
    return str;
}

QString ComicMetadata::MangaPath_frame(int number)
{
    QString str;
    if(number > -1){
        str = QString("f%1/").arg(number, 3, 10, QChar('0'));
    }
    return str;
}

QString ComicMetadata::MangaPath_title_episode_page_frame(int frameNumber)
{
    QString str;
    str += MangaPath_title_episode();
    str += MangaPath_page();
    str += MangaPath_frame(frameNumber);
    return str;
}


bool ComicMetadata::loadMetadata_Common(QString fileName)
{
    //!処理概要
    clearCommon();
    //!- ファイルを開く
    QFile file(fileName);

    //!- ファイルが開けない場合は何もせず終了
    if(!file.open(QFile::ReadOnly)) return false;

    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument doc;

    //!- 開いたファイルの内部を受け取る
    if (!doc.setContent(&file, true, &errorStr, &errorLine,
                        &errorColumn))
    {
        return false;
    }

    QDomElement root = doc.documentElement();

    //!- ComicMetadata以外のXMLファイルの場合終了
    if (root.tagName() != "ComicMetadata")
        return false;

    QDomNode node = root.firstChild();

    //!- 実際にメタデータ本体を読み込む
    while (!node.isNull())
    {
        if (node.toElement().tagName() == "BookData"){
            QDomNode childNode = node.firstChild();
            if(childNode.nodeType() == QDomNode::TextNode){
                workTitle = childNode.toText().data();
            }
        }
        else if(node.toElement().tagName() == "CharacterData"){
            QDomElement element = node.toElement();
            XMLPurse_CharacterList(element);
        }
        node = node.nextSibling();
    }

    //!終了
    return true;
}



bool ComicMetadata::loadMetadata_Page(QString fileName, int targetPageNumber)
{
    clearPageMetadata();
    loadFrame.clear();
    loadFrameCoordinate.clear();
    loadCharacter.clear();
    loadCharacterCoordinate.clear();
    loadDialog.clear();
    loadDialogCoordinate.clear();
    loadOnomatopoeia.clear();
    loadOnomatopoeiaCoordinate.clear();
    loadItem.clear();
    loadItemCoordinate.clear();
    loadEpisodeNumber = -1;
    loadPageNumber = 0;

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)) return false;

    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument doc;

    //読み込めなかった場合終了
    if (!doc.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) return false;

    QDomElement root = doc.documentElement();

    //ComicMetadata以外のXMLファイルの場合終了
    if (root.tagName() != "ComicMetadata") return false;

    QDomNode node = root.firstChild();

    QString tag;
    while (!node.isNull())
    {
        tag = node.toElement().tagName();
        //読み込んだものがページデータなら展開する
        if(0 == QString::compare(tag,"PageData",Qt::CaseInsensitive)){
            bool pageStatus = true;
            QDomNode currentNode = node.firstChild();
            while(!currentNode.isNull()){
                tag = currentNode.toElement().tagName();
                if(0 == QString::compare(tag, "EpisodeNumber",Qt::CaseInsensitive)){
                    QString str = currentNode.firstChild().toText().data();
                    if(0 == QString::compare(str, "NoData", Qt::CaseInsensitive)){
                        loadEpisodeNumber = -1;
                    }
                    else{
                        int num = currentNode.firstChild().toText().data().toInt();
                        if(num >= 0) loadEpisodeNumber = num;
                        else loadEpisodeNumber = 0;
                    }
                    loadEpisodeNumber;
                }
                else if(0 == QString::compare(tag, "PageNumber",Qt::CaseInsensitive)){
                    loadPageNumber = currentNode.firstChild().toText().data().toInt();
                    //targetPageNumberを指定してXMLデータを読み込む部分は後日作成したい
                    if(targetPageNumber >=0 && loadPageNumber != targetPageNumber){
                        pageStatus = false;
                    }
                    loadPageNumber;
                }
                else if(0 == QString::compare(tag, "FileName", Qt::CaseInsensitive)){
                    loadImageFileName = currentNode.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "FrameData",Qt::CaseInsensitive)){
                    QDomElement element = currentNode.toElement();
                    XMLPurse_Frame(element);
                }
                else if(0 == QString::compare(tag, "CharacterData",Qt::CaseInsensitive)){
                    QDomElement element = currentNode.toElement();
                    XMLPurse_Character(element);
                }
                else if(0 == QString::compare(tag, "DialogData",Qt::CaseInsensitive)){
                    QDomElement element = currentNode.toElement();
                    XMLPurse_Dialog(element);
                }
                else if(0 == QString::compare(tag, "OnomatopoeiaData", Qt::CaseInsensitive)){
                    QDomElement element = currentNode.toElement();
                    XMLPurse_Onomatopoeia(element);
                }
                else if(0 == QString::compare(tag, "ItemData", Qt::CaseInsensitive)){
                    QDomElement element = currentNode.toElement();
                    XMLPurse_Item(element);
                }
                if(!pageStatus) break;
                currentNode = currentNode.nextSibling();
            }
        }
        node = node.nextSibling();
    }
    return true;
}

bool ComicMetadata::writeMetadata_Common(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly| QFile::Truncate)){
        return false;
    }

    QDomDocument doc;

    //comic metadata root
    QDomElement comicMetadata = doc.createElement("ComicMetadata");
    doc.appendChild(comicMetadata);

    //book data
    QDomElement bookData = doc.createElement("BookData");
    comicMetadata.appendChild(bookData);

    QDomText title = doc.createTextNode(workTitle);
    bookData.appendChild(title);

    //character data
    QDomElement EL_CharacterData = doc.createElement("CharacterData");
    comicMetadata.appendChild(EL_CharacterData);

    for(int i=0; i<characterName.size(); i++){
        //Entry
        QDomElement EL_CharacterEntry = doc.createElement("Character");
        EL_CharacterData.appendChild(EL_CharacterEntry);

        QDomText EL_CharacterNameText = doc.createTextNode(characterName.at(i));
        EL_CharacterEntry.appendChild(EL_CharacterNameText);
    }

    QTextStream out(&file);
    doc.save(out, indent);
    std::cout << "Output XML(Common) : " << fileName.toStdString() << std::endl;
    file.close();
    return true;
}

bool ComicMetadata::writeMetadata_Page(QString fileName)
{
//    std::cout << "writeMetadata_Page" << std::endl;
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly| QFile::Truncate)){
        return false;
    }
    QDomDocument doc;

    //Comic metadata root
    QDomElement EL_ComicMetadata = doc.createElement("ComicMetadata");
    doc.appendChild(EL_ComicMetadata);

    //Page data
    QDomElement EL_Page = doc.createElement("PageData");
    EL_ComicMetadata.appendChild(EL_Page);

    //Episode Number
    QDomElement EL_Episode = doc.createElement("EpisodeNumber");
    EL_Page.appendChild(EL_Episode);
    QString episode_str;
    if(episodeNumber < 0){
        episode_str = ("NoData");
    }
    else{
        episode_str = QString("%1").arg(episodeNumber,4,10,QChar('0'));
    }
    QDomText DomText_Episode = doc.createTextNode(episode_str);
    EL_Episode.appendChild(DomText_Episode);

    //File Name
    QDomElement EL_FileName = doc.createElement("FileName");
    EL_Page.appendChild(EL_FileName);
    QDomText DomText_FileName = doc.createTextNode(imageFileName);
    EL_FileName.appendChild(DomText_FileName);

    //Page Number
    QDomElement EL_PageNumber = doc.createElement("PageNumber");
    EL_Page.appendChild(EL_PageNumber);
    QDomText DT_PageNumber = doc.createTextNode(QString("%1").arg(pageNumber,4,10,QChar('0')));
    EL_PageNumber.appendChild(DT_PageNumber);

    //Image Size
    QDomElement EL_ImageSize = doc.createElement("ImageSize");
    EL_Page.appendChild(EL_ImageSize);
    QDomElement EL_ImageWidth = doc.createElement("Width");
    EL_ImageSize.appendChild(EL_ImageWidth);
    QDomText DT_ImageWidth = doc.createTextNode(QString("%1").arg(imageWidth));
    EL_ImageWidth.appendChild(DT_ImageWidth);
    QDomElement EL_ImageHeight = doc.createElement("Height");
    EL_ImageSize.appendChild(EL_ImageHeight);
    QDomText DT_ImageHeight = doc.createTextNode(QString("%1").arg(imageHeight));
    EL_ImageHeight.appendChild(DT_ImageHeight);


    XMLCreate_Frame(EL_Page);
    XMLCreate_Character(EL_Page);
    XMLCreate_Dialog(EL_Page);
    XMLCreate_Onomatopoeia(EL_Page);
    XMLCreate_Item(EL_Page);

    QTextStream out(&file);
    doc.save(out, indent);//第2引数はインデントのスペース数
    std::cout << "Output XML(Page) : " << fileName.toStdString() << std::endl;
    return true;
}

void ComicMetadata::XMLCreate_Frame(QDomElement &element)
{
    QDomDocument doc;
    //frame
    QDomElement EL_FrameList = doc.createElement("FrameData");
    element.appendChild(EL_FrameList);

    for(int i=0; i<frame.data()->size(); i++){
        QDomElement EL_Frame = doc.createElement("Frame");
        EL_FrameList.appendChild(EL_Frame);

        //mangaPath
        QDomElement EL_mpath_frame = doc.createElement("MangaPath");
        EL_Frame.appendChild(EL_mpath_frame);
        QDomText DT_mpath_frame = doc.createTextNode(frame.data()->at(i).mangaPath);
        EL_mpath_frame.appendChild(DT_mpath_frame);

        //scene change
        QDomElement EL_SceneChange = doc.createElement("SceneChange");
        EL_Frame.appendChild(EL_SceneChange);
        if(frame.data()->at(i).sceneBoundary){
            EL_SceneChange.appendChild(doc.createTextNode("1"));
        }
        else{
            EL_SceneChange.appendChild(doc.createTextNode("0"));
        }

        //coordinate
        XMLCreate_Coordinage(EL_Frame, frame.data()->at(i).GIData);
    }
}

void ComicMetadata::XMLCreate_Character(QDomElement &element)
{
    QDomDocument doc;
    //character
    QDomElement EL_CharacterList = doc.createElement("CharacterData");
    element.appendChild(EL_CharacterList);

    for(int i=0; i<character.data()->size(); i++){
        QDomElement EL_Character = doc.createElement("Character");
        EL_CharacterList.appendChild(EL_Character);

        //mangaPath
        QDomElement EL_mpath_Character = doc.createElement("MangaPath");
        EL_Character.appendChild(EL_mpath_Character);
        QDomText DT_mpath_Character = doc.createTextNode(character.data()->at(i).mangaPath);
        EL_mpath_Character.appendChild(DT_mpath_Character);

        //character id
        QDomElement EL_CharacterID = doc.createElement("CharacterID");
        EL_Character.appendChild(EL_CharacterID);
        QDomText DT_CharacterID = doc.createTextNode
                (QString("%1").arg(character.data()->at(i).characterID, 3, 10, QChar('0')));
        EL_CharacterID.appendChild(DT_CharacterID);

        //character name
        QDomElement EL_CharacterName = doc.createElement("CharacterName");
        EL_Character.appendChild(EL_CharacterName);
        QDomText DT_CharacterName = doc.createTextNode(character.data()->at(i).characterName);
        EL_CharacterName.appendChild(DT_CharacterName);

        //target frame
        QDomElement EL_TargetFrame = doc.createElement("Frame");
        EL_Character.appendChild(EL_TargetFrame);
        QDomText DT_TargetFrame = doc.createTextNode
                (QString("%1").arg(character.data()->at(i).targetFrame, 3, 10, QChar('0')));
        EL_TargetFrame.appendChild(DT_TargetFrame);

        //coordinate
        XMLCreate_Coordinage(EL_Character, character.data()->at(i).GIData);
    }
}

void ComicMetadata::XMLCreate_Dialog(QDomElement &element)
{
    QDomDocument doc;
    //character
    QDomElement EL_List = doc.createElement("DialogData");
    element.appendChild(EL_List);

    for(int i=0; i<dialog.data()->size(); i++){
        QDomElement EL = doc.createElement("Dialog");
        EL_List.appendChild(EL);

        //mangaPath
        QDomElement EL_mpath = doc.createElement("MangaPath");
        EL.appendChild(EL_mpath);
        QDomText DT_mpath = doc.createTextNode(dialog.data()->at(i).mangaPath);
        EL_mpath.appendChild(DT_mpath);

        //speaker
        QDomElement EL_Speaker = doc.createElement("Speaker");
        EL.appendChild(EL_Speaker);

        QDomElement EL_SpeakerID = doc.createElement("SpeakerID");
        EL_Speaker.appendChild(EL_SpeakerID);
        QDomText DT_SpeakerID = doc.createTextNode
                (QString("%1").arg(dialog.data()->at(i).targetCharacterID, 3, 10, QChar('0')));
        EL_SpeakerID.appendChild(DT_SpeakerID);
        QDomElement EL_SpeakerName = doc.createElement("SpeakerName");
        EL_Speaker.appendChild(EL_SpeakerName);
        QDomText DT_SpeakerName = doc.createTextNode(dialog.data()->at(i).characterName);
        EL_Speaker.appendChild(DT_SpeakerName);

        //font size
        QDomElement EL_FontSize = doc.createElement("FontSize");
        EL.appendChild(EL_FontSize);
        QDomText DT_FontSize = doc.createTextNode(QString("%1").arg(dialog.data()->at(i).fontSize));
        EL_FontSize.appendChild(DT_FontSize);

        //DialogType dialog or narration
        QDomElement EL_DialogType = doc.createElement("DialogType");
        EL.appendChild(EL_DialogType);
        QDomText DT_DialogType;
        if(dialog.data()->at(i).narration){
            DT_DialogType = doc.createTextNode("Narration");
        }
        else{
            DT_DialogType = doc.createTextNode("Dialog");
        }
        EL_DialogType.appendChild(DT_DialogType);

        //Text
        QDomElement EL_Text = doc.createElement("Text");
        EL.appendChild(EL_Text);
        QDomText DT_Text = doc.createTextNode(dialog.data()->at(i).text);
        EL_Text.appendChild(DT_Text);

        //target frame
        QDomElement EL_TargetFrame = doc.createElement("Frame");
        EL.appendChild(EL_TargetFrame);
        QDomText DT_TargetFrame = doc.createTextNode
                (QString("%1").arg(dialog.data()->at(i).targetFrame, 3, 10, QChar('0')));
        EL_TargetFrame.appendChild(DT_TargetFrame);

        //coordinate
        XMLCreate_Coordinage(EL, dialog.data()->at(i).GIData);
    }
}

void ComicMetadata::XMLCreate_Onomatopoeia(QDomElement &element)
{
    QDomDocument doc;
    //character
    QDomElement EL_List = doc.createElement("OnomatopoeiaData");
    element.appendChild(EL_List);

    for(int i=0; i<onomatopoeia.data()->size(); i++){
        QDomElement EL = doc.createElement("Onomatopoeia");
        EL_List.appendChild(EL);

        //mangaPath
        QDomElement EL_mpath = doc.createElement("MangaPath");
        EL.appendChild(EL_mpath);
        QDomText DT_mpath = doc.createTextNode(onomatopoeia.data()->at(i).mangaPath);
        EL_mpath.appendChild(DT_mpath);

        //font size
        QDomElement EL_FontSize = doc.createElement("FontSize");
        EL.appendChild(EL_FontSize);
        QDomText DT_FontSize = doc.createTextNode(QString("%1").arg(onomatopoeia.data()->at(i).fontSize));
        EL_FontSize.appendChild(DT_FontSize);

        //Text
        QDomElement EL_Text = doc.createElement("Text");
        EL.appendChild(EL_Text);
        QDomText DT_Text = doc.createTextNode(onomatopoeia.data()->at(i).text);
        EL_Text.appendChild(DT_Text);

        //target frame
        QDomElement EL_TargetFrame = doc.createElement("Frame");
        EL.appendChild(EL_TargetFrame);
        QDomText DT_TargetFrame = doc.createTextNode
                (QString("%1").arg(onomatopoeia.data()->at(i).targetFrame, 3, 10, QChar('0')));
        EL_TargetFrame.appendChild(DT_TargetFrame);

        //coordinate
        XMLCreate_Coordinage(EL, onomatopoeia.data()->at(i).GIData);
    }
}

void ComicMetadata::XMLCreate_Item(QDomElement &element)
{
    QDomDocument doc;
    //character
    QDomElement EL_List = doc.createElement("ItemData");
    element.appendChild(EL_List);

    for(int i=0; i<item.data()->size(); i++){
        QDomElement EL = doc.createElement("Item");
        EL_List.appendChild(EL);

        //mangaPath
        QDomElement EL_mpath = doc.createElement("MangaPath");
        EL.appendChild(EL_mpath);
        QDomText DT_mpath = doc.createTextNode(item.data()->at(i).mangaPath);
        EL_mpath.appendChild(DT_mpath);

        //itemClass
        QDomElement EL_Class = doc.createElement("Class");
        EL.appendChild(EL_Class);
        QDomText DT_Class = doc.createTextNode(item.data()->at(i).itemClass);
        EL_Class.appendChild(DT_Class);

        //Description
        QDomElement EL_Text = doc.createElement("Description");
        EL.appendChild(EL_Text);
        QDomText DT_Text = doc.createTextNode(item.data()->at(i).description);
        EL_Text.appendChild(DT_Text);

        //target frame
        QDomElement EL_TargetFrame = doc.createElement("Frame");
        EL.appendChild(EL_TargetFrame);
        QDomText DT_TargetFrame = doc.createTextNode
                (QString("%1").arg(item.data()->at(i).targetFrame, 3, 10, QChar('0')));
        EL_TargetFrame.appendChild(DT_TargetFrame);

        //coordinate
        XMLCreate_Coordinage(EL, item.data()->at(i).GIData);
    }
}

void ComicMetadata::XMLPurse_CharacterList(QDomElement &element)
{
    clearCharacterName();
    QDomNode node = element.firstChild();
    while(!node.isNull()){
        if(node.toElement().tagName() == "Character"){
            QDomNode childNode = node.firstChild();
            if(0 != QString::compare(childNode.toText().data(), UnDefinedCharacterName, Qt::CaseInsensitive)){
                characterName.push_back(childNode.toText().data());
            }
        }
        node = node.nextSibling();
    }
}

void ComicMetadata::XMLCreate_Coordinage(QDomElement &element, GraphicsItemData GIData)
{
    QDomDocument doc;
    QPolygonF polygon = GIData._relativePosition;
    QDomElement EL_Coordinate = doc.createElement("Coordinate");
    element.appendChild(EL_Coordinate);

    if(polygon.isEmpty()) return;
    for(int i=0; i<polygon.size(); i++){
        QPointF pt = polygon.at(i);
        QDomElement EL_Pt = doc.createElement("Point");
        EL_Coordinate.appendChild(EL_Pt);
        QDomElement EL_PtX = doc.createElement("X");
        EL_Pt.appendChild(EL_PtX);
        QDomText DT_PtX = doc.createTextNode(QString("%1").arg(pt.x()));
        EL_PtX.appendChild(DT_PtX);
        QDomElement EL_PtY = doc.createElement("Y");
        EL_Pt.appendChild(EL_PtY);
        QDomText DT_PtY = doc.createTextNode(QString("%1").arg(pt.y()));
        EL_PtY.appendChild(DT_PtY);
    }
}

QPolygonF ComicMetadata::XMLPurse_Coordinate(QDomElement &element){
    QPolygonF polygon;
    QString tag = element.tagName();
    if(0 != QString::compare(tag, "Coordinate", Qt::CaseInsensitive))
        return polygon;

    QDomNode node = element.firstChild();

    while(!node.isNull()){
        tag = node.toElement().tagName();
        if(0 == QString::compare(tag, "Point", Qt::CaseInsensitive)){
            QPointF pt(0.0, 0.0);
            QDomNode current = node.firstChild();
            while(!current.isNull()){
                if(0 == QString::compare
                        (current.toElement().tagName(), "X", Qt::CaseInsensitive)){
                    pt.setX(current.firstChild().toText().data().toDouble());
                }
                else if(0 == QString::compare
                        (current.toElement().tagName(), "Y", Qt::CaseInsensitive)){
                    pt.setY(current.firstChild().toText().data().toDouble());
                }
                current = current.nextSibling();
            }
            polygon.push_back(pt);
        }
        node = node.nextSibling();
    }
    return polygon;
}

void ComicMetadata::XMLPurse_Frame(QDomElement &element)
{
//    std::cout << "purse frame" << std::endl;
    QString tag = element.tagName();
    if(0 != QString::compare(tag, "FrameData", Qt::CaseInsensitive)){
        return;
    }
    QDomNode node = element.firstChild();
    while(!node.isNull()){
        tag = node.toElement().tagName();
        //frameであれば展開してデータを入れる
        if(0 == QString::compare(tag, "Frame", Qt::CaseInsensitive)){
            FrameData localFrame;
            QPolygonF localPolygon;
            QDomNode current = node.firstChild();
            while(!current.isNull()){
                tag = current.toElement().tagName();
                if(0 == QString::compare(tag, "MangaPath", Qt::CaseInsensitive)){
                    localFrame.mangaPath = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "SceneChange", Qt::CaseInsensitive)){
                    int val = current.firstChild().toText().data().toInt();
                    if(val == 1)localFrame.sceneBoundary = true;
                    else localFrame.sceneBoundary = false;
                }
                else if(0 == QString::compare(tag, "Coordinate", Qt::CaseInsensitive)){
                    QDomElement element = current.toElement();
                    localPolygon = XMLPurse_Coordinate(element);
                }
                current = current.nextSibling();
            }
            loadFrame.push_back(localFrame);
            loadFrameCoordinate.push_back(localPolygon);
        }
        node = node.nextSibling();
    }
}
/**
 * @brief ComicMetadata::XMLPurse_Character
 * @param element
 * CharacterIDが0未満である場合には強制的に0にリセット
 */
void ComicMetadata::XMLPurse_Character(QDomElement &element)
{
//    std::cout << "purse character" << std::endl;
    QString tag = element.tagName();
    if(0 != QString::compare(tag, "CharacterData", Qt::CaseInsensitive)){
        return;
    }
    QDomNode node = element.firstChild();
    while(!node.isNull()){
        tag = node.toElement().tagName();
        //Characterであれば展開してデータを入れる
        if(0 == QString::compare(tag, "Character", Qt::CaseInsensitive)){
            CharacterData localData;
            QPolygonF localPolygon;
            QDomNode current = node.firstChild();
            while(!current.isNull()){
                tag = current.toElement().tagName();
                if(0 == QString::compare(tag, "MangaPath", Qt::CaseInsensitive)){
                    localData.mangaPath = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "CharacterID", Qt::CaseInsensitive)){
                    int val = current.firstChild().toText().data().toInt();
                    if(val >= 0) localData.characterID = val;
                    else localData.characterID = 0;
                }
                else if(0 == QString::compare(tag, "CharacterName", Qt::CaseInsensitive)){
                    localData.characterName = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "Frame", Qt::CaseInsensitive)){
                    int val = current.firstChild().toText().data().toInt();
                    if(val >= 0) localData.targetFrame = val;
                    else localData.targetFrame = 0;
                }
                else if(0 == QString::compare(tag, "Coordinate", Qt::CaseInsensitive)){
                    QDomElement element = current.toElement();
                    localPolygon = XMLPurse_Coordinate(element);
                }
                current = current.nextSibling();
            }
            loadCharacter.push_back(localData);
            loadCharacterCoordinate.push_back(localPolygon);
        }
        node = node.nextSibling();
    }
}

void ComicMetadata::XMLPurse_Dialog(QDomElement &element)
{
//    std::cout << "purse dialog" << std::endl;
    QString tag = element.tagName();
    if(0 != QString::compare(tag, "DialogData", Qt::CaseInsensitive)){
        return;
    }
    QDomNode node = element.firstChild();
    while(!node.isNull()){
        tag = node.toElement().tagName();
        //Dialogであれば展開してデータを入れる
        if(0 == QString::compare(tag, "Dialog", Qt::CaseInsensitive)){
            DialogData localData;
            QPolygonF localPolygon;
            QDomNode current = node.firstChild();
            while(!current.isNull()){
                tag = current.toElement().tagName();
                if(0 == QString::compare(tag, "MangaPath", Qt::CaseInsensitive)){
                    localData.mangaPath = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "Speaker", Qt::CaseInsensitive)){
                    QDomNode node_speaker = current.firstChild();
                    while(!node_speaker.isNull()){
                        tag = node_speaker.toElement().tagName();
                        if(0 == QString::compare(tag, "SpeakerID", Qt::CaseInsensitive)){
                            int val = node_speaker.firstChild().toText().data().toInt();
                            if(val >= 0) localData.targetCharacterID = val;
                            else localData.targetCharacterID = 0;
                        }
                        if(0 == QString::compare(tag, "SpeakerName", Qt::CaseInsensitive)){
                            localData.characterName = node_speaker.firstChild().toText().data();
                        }
                        node_speaker = node_speaker.nextSibling();
                    }
                }
                else if(0 == QString::compare(tag, "FontSize", Qt::CaseInsensitive)){
                    int val = current.firstChild().toText().data().toInt();
                    if(val >= 0) localData.fontSize = val;
                    else localData.fontSize = 0;
                }
                else if(0 == QString::compare(tag, "DialogType", Qt::CaseInsensitive)){
                    QString dtype = current.firstChild().toText().data();
                    if(0 == QString::compare(dtype, "Narration", Qt::CaseInsensitive))
                        localData.narration = true;
                    else localData.narration = false;
                }
                else if(0 == QString::compare(tag, "Text", Qt::CaseInsensitive)){
                    localData.text = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "Frame", Qt::CaseInsensitive)){
                    int val = current.firstChild().toText().data().toInt();
                    if(val >= 0) localData.targetFrame = val;
                    else localData.targetFrame = 0;
                }
                else if(0 == QString::compare(tag, "Coordinate", Qt::CaseInsensitive)){
                    QDomElement element = current.toElement();
                    localPolygon = XMLPurse_Coordinate(element);
                }
                current = current.nextSibling();
            }
            loadDialog.push_back(localData);
            loadDialogCoordinate.push_back(localPolygon);
        }
        node = node.nextSibling();
    }
}

void ComicMetadata::XMLPurse_Onomatopoeia(QDomElement &element)
{
//    std::cout << "purse onomatopoeia" << std::endl;
    QString tag = element.tagName();
    if(0 != QString::compare(tag, "OnomatopoeiaData", Qt::CaseInsensitive)){
        return;
    }
    QDomNode node = element.firstChild();
    while(!node.isNull()){
        tag = node.toElement().tagName();
        //Onomatopoeiaであれば展開してデータを入れる
        if(0 == QString::compare(tag, "Onomatopoeia", Qt::CaseInsensitive)){
            OnomatopoeiaData localData;
            QPolygonF localPolygon;
            QDomNode current = node.firstChild();
            while(!current.isNull()){
                tag = current.toElement().tagName();
                if(0 == QString::compare(tag, "MangaPath", Qt::CaseInsensitive)){
                    localData.mangaPath = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "FontSize", Qt::CaseInsensitive)){
                    int val = current.firstChild().toText().data().toInt();
                    if(val >= 0) localData.fontSize = val;
                    else localData.fontSize = 0;
                }
                else if(0 == QString::compare(tag, "Text", Qt::CaseInsensitive)){
                    localData.text = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "Frame", Qt::CaseInsensitive)){
                    int val = current.firstChild().toText().data().toInt();
                    if(val >= 0) localData.targetFrame = val;
                    else localData.targetFrame = 0;
                }
                else if(0 == QString::compare(tag, "Coordinate", Qt::CaseInsensitive)){
                    QDomElement element = current.toElement();
                    localPolygon = XMLPurse_Coordinate(element);
                }
                current = current.nextSibling();
            }
            loadOnomatopoeia.push_back(localData);
            loadOnomatopoeiaCoordinate.push_back(localPolygon);
        }
        node = node.nextSibling();
    }
}

void ComicMetadata::XMLPurse_Item(QDomElement &element)
{
//    std::cout << "purse item" << std::endl;
    QString tag = element.tagName();
    if(0 != QString::compare(tag, "ItemData", Qt::CaseInsensitive)){
        return;
    }

    QDomNode node = element.firstChild();
    while(!node.isNull()){
        tag = node.toElement().tagName();
        if(0 == QString::compare(tag, "Item", Qt::CaseInsensitive)){
            ItemData localData;
            QPolygonF localPolygon;
            QDomNode current = node.firstChild();
            while(!current.isNull()){
                tag = current.toElement().tagName();
                if(0 == QString::compare(tag, "MangaPath", Qt::CaseInsensitive)){
                    localData.mangaPath = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "Class", Qt::CaseInsensitive)){
                    localData.itemClass = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "Description", Qt::CaseInsensitive)){
                    localData.description = current.firstChild().toText().data();
                }
                else if(0 == QString::compare(tag, "Frame", Qt::CaseInsensitive)){
                    int val = current.firstChild().toText().data().toInt();
                    if(val >= 0) localData.targetFrame = val;
                    else localData.targetFrame = 0;
                }
                else if(0 == QString::compare(tag, "Coordinate", Qt::CaseInsensitive)){
                    QDomElement element = current.toElement();
                    localPolygon = XMLPurse_Coordinate(element);
                }
                current = current.nextSibling();
            }
            loadItem.push_back(localData);
            loadItemCoordinate.push_back(localPolygon);
        }
        node = node.nextSibling();
    }
}



void ComicMetadata::clearCharacterName()
{
    characterName.clear();
    characterName.push_back(UnDefinedCharacterName);
}










