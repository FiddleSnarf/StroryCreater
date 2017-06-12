#pragma once

#include <QMainWindow>
#include <QAction>
#include <QLabel>
#include <QPointer>

#include "Core/ICore.hpp"

namespace Ui
{
    class StoryCreator;
}
class StoryManager;

class StoryCreator : public QMainWindow
{
    Q_OBJECT

public:
    explicit StoryCreator(ICorePtr core, QWidget* parent = Q_NULLPTR);
    ~StoryCreator();

private:
    void initialize();
    void initToolBar();
    void initConnects();
    void initStoryView();
    void initSelectTemplateNodesView();

private slots:
    void slotStorySceneChanged();

    void slotStoryOpened();
    void slotStoryClosed();


private:
    QScopedPointer<Ui::StoryCreator> m_ui;
    ICorePtr m_core;
    StoryManagerPtr m_storyManager;

    // Tool bar
    QPointer<QAction> m_actCreateNewStory;
    QPointer<QAction> m_actLoadStory;
    QPointer<QAction> m_actCloseStory;

    // Status bar
    QPointer<QLabel> m_nodeCounterView; // TODO всю эту шляпу заменить на отдельный статус бар
};

