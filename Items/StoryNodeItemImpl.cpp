#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include "StoryNodeItemImpl.hpp"
#include "Common/StoryCommon.hpp"
#include "Scene/StoryScene.hpp"

//================================== public =============================================

StoryNodeItem::StoryNodeItem(const StoryNode& nodeInfo, QGraphicsItem* parent) :
    QGraphicsObject(parent),
    m_boundingRect(StoryGUI::DEFAULT_NODE_RECT)
{
    const int id = nodeInfo.getId();
    if (id < 0)
    {
        StoryScene* myScene = dynamic_cast<StoryScene*>(this->scene());
        if (myScene)
        {
            m_nodeInfo = StoryNode(myScene->getFreeID(), nodeInfo.getType());
            return;
        }
    }
    m_nodeInfo = nodeInfo;
    initialization();
}

StoryNodeItem::StoryNodeItem(int nodeID, const QString &typeNode, QGraphicsItem* parent) :
    QGraphicsObject(parent),
    m_boundingRect(StoryGUI::DEFAULT_NODE_RECT),
    m_nodeInfo(nodeID, typeNode)
{
    initialization();
}

StoryNodeItem::~StoryNodeItem()
{

}

const QUuid& StoryNodeItem::getNodeGUID()
{
    return m_guid;
}

void StoryNodeItem::setDefaultPen()
{
    m_borderPen = isHeadNode() ? StoryGUI::HEAD_NODE_PEN : StoryGUI::NODE_PEN;
}

void StoryNodeItem::setDefaultBrush()
{
    m_brush = isHeadNode() ? StoryGUI::HEAD_NODE_BRUSH : StoryGUI::NODE_BRUSH;
}

void StoryNodeItem::setIcon(const QIcon& icon)
{
    m_icon = icon;
}

StoryNode& StoryNodeItem::getNodeInfo()
{
    return m_nodeInfo;
}

const StoryNode& StoryNodeItem::getNodeInfo() const
{
    return m_nodeInfo;
}

bool StoryNodeItem::isHeadNode() const
{
    return m_nodeInfo.getId() == StoryCommon::HEAD_NODE_ID;
}

void StoryNodeItem::setNodeSelection(bool state)
{
    if (state)
    {
        m_borderPen.setWidth(SELECTED_NODE_FRAME_WIDTH);
        m_selectedTimeline.start();
    }
    else
    {
        m_selectedTimeline.stop();
        setDefaultPen();
    }
    setSelected(state);
}

//=======================================================================================
//===================================== private =========================================

void StoryNodeItem::initialization()
{
    m_guid = QUuid::createUuid();

    setFlag(QGraphicsItem::ItemIsSelectable);
    setDefaultPen();
    setDefaultBrush();

    initSelectedPulse();
    initErrorPulse();
}

void StoryNodeItem::initSelectedPulse()
{
    m_selectedTimeline.setDuration(TIME_LINE);
    m_selectedTimeline.setEasingCurve(QEasingCurve::Linear);
    m_selectedTimeline.setFrameRange(0, FRAMES);

    connect(&m_selectedTimeline, &QTimeLine::valueChanged, [=](double value)
    {
        QColor color;
        if (isHeadNode())
        {
            const int green = ((GREEN_MAX_HEAD - GREEN_MIN_HEAD) * value) + GREEN_MIN_HEAD;
            color = QColor(160, green, 245);
        }
        else
        {
            const int green = ((GREEN_MAX - GREEN_MIN) * value) + GREEN_MIN;
            color = QColor(70, green, 70);
        }
        m_borderPen.setColor(color);
        update();
    });

    connect(&m_selectedTimeline, &QTimeLine::finished, [=]()
    {
        if (m_selectedTimeline.direction() == QTimeLine::Forward)
            m_selectedTimeline.setDirection(QTimeLine::Backward);
        else if (m_selectedTimeline.direction() == QTimeLine::Backward)
            m_selectedTimeline.setDirection(QTimeLine::Forward);

        m_selectedTimeline.start();
    });
}

void StoryNodeItem::initErrorPulse()
{
    m_errorTimeline.setDuration(TIME_LINE);
    m_errorTimeline.setEasingCurve(QEasingCurve::OutQuad);
    m_errorTimeline.setFrameRange(0, FRAMES);

    connect(&m_errorTimeline, &QTimeLine::valueChanged, [=](double value)
    {
        const int red = ((RED_MAX - RED_MIN) * value) + RED_MIN;
        m_borderPen.setColor(QColor(red, 0, 0));
        update();
    });

    connect(&m_errorTimeline, &QTimeLine::finished, [=]()
    {
        if (m_errorTimeline.direction() == QTimeLine::Forward)
            m_errorTimeline.setDirection(QTimeLine::Backward);
        else if (m_errorTimeline.direction() == QTimeLine::Backward)
            m_errorTimeline.setDirection(QTimeLine::Forward);

        m_errorTimeline.start();
    });
}

QRectF StoryNodeItem::boundingRect() const
{
    return m_boundingRect;
}

void StoryNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // тут будет немного магии чисел, но уж ничего не поделать

    const qreal w = m_boundingRect.width();
    const qreal h = m_boundingRect.height();

    painter->setBrush(m_brush);
    painter->setPen(m_borderPen);
    painter->drawRect(QRectF(5, 5, w - 10, h - 10));

    // рисуем title и Node ID
    const QString srcTitle = m_nodeInfo.getTitle();
    // TODO сделать нормальное расположение текста на iteme
    const QString formatTitle = (srcTitle.length() > 17) ? srcTitle.left(15) + "..." : srcTitle;
    painter->drawText(QPointF(20, 20), tr("Title: ") + formatTitle);
    painter->drawText(QPointF(20, h - 15), tr("Node ID: ") + QString::number(m_nodeInfo.getId()));

    const qreal pixW = w/3;
    const qreal pixH = h/3;
    painter->drawPixmap(QPointF(w/2 - pixW/2, h/2 - pixH/4), m_icon.pixmap(pixW, pixH));
}
//=======================================================================================
