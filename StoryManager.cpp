#include "StoryManager.hpp"
#include "JsonStoryHelper/JsonStoryHelper.h"
#include "Common/StoryTypesNodeCollector.hpp"

StoryManager::StoryManager(QObject* parent) :
    QObject(parent),
    m_storyScene(new StoryScene()),
    m_storyNodeSelectModel(new SelectNodeModel)
{
    m_storyScene->setTypesNodeCollector(m_typesCollector);
    initialization();
}

StoryManager::~StoryManager()
{

}

StoryScenePtr StoryManager::getStoryScene()
{
    return m_storyScene;
}

SelectNodeModelPtr StoryManager::getStoryNodeSelectModel()
{
    return m_storyNodeSelectModel;
}

int StoryManager::getCountStoryNodes() const
{
    return m_storyScene->nodeCount();
}

void StoryManager::initialization()
{
    StoryCommon::SelectTNodeList templateNodeList;
    QStringList nodesNameList = m_typesCollector.getNodesNameList();
    foreach(const QString& nodeName, nodesNameList)
    {
        StoryCommon::NodeSelectTemplate templateNode;
        const StoryTypesNodeCollector::TypeInfo typeInfo = m_typesCollector.getNodeTypeInfo(nodeName);

        templateNode.nodeType = nodeName;
        templateNode.toolTip = typeInfo.descriptionType;
        templateNode.icon = typeInfo.iconType;

        templateNodeList << templateNode;
    }
    m_storyNodeSelectModel->addTemplateNodesList(templateNodeList);
}

void StoryManager::createNewStory()
{
    const StoryCommon::StoryInfo newStoryInfo(StoryCommon::CURR_JSON_VERSION);
    // TODO Возможно тут прийдется добавлять пустой айтем и тут же удалять его, чтобы установить размер сцены и при этом дать ей возможность расширяться
    //m_storyScene->setSceneRect(StoryGUI::DEFAULT_SCENE_RECT);
    m_storyScene->initStoryInfo(newStoryInfo);
    emit signalStoryOpened();
}

void StoryManager::loadStory()
{
    const QString filePath = JsonStoryHelper::selectLoadStoryFilePath();
    if (!filePath.isEmpty())
    {
        StoryCommon::StoryInfo storyInfo;
        if (!JsonStoryHelper::loadJsonStory(filePath, storyInfo))
        {
            return; // TODO сделать что-нить
        }
        //if (!storyInfo.additionalViewParams.isValid()) // TODO с размерами сцены вообще сложный вопрос, надо подумать
        //    m_storyScene->setSceneRect(StoryGUI::DEFAULT_SCENE_RECT);

        m_storyScene->initStoryInfo(storyInfo);
        emit signalStoryOpened();
    }
}

void StoryManager::closeStory()
{

}
