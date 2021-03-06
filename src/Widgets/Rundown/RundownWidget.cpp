#include "RundownWidget.h"
#include "RundownTreeWidget.h"

#include "EventManager.h"
#include "Events/Rundown/CompactViewEvent.h"
#include "Events/Rundown/RemoteRundownTriggeringEvent.h"

#include <QtCore/QUuid>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QFileDialog>
#include <QtGui/QToolButton>

RundownWidget::RundownWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);

    setupMenus();

    QToolButton* toolButtonRundownDropdown = new QToolButton(this);
    toolButtonRundownDropdown->setObjectName("toolButtonRundownDropdown");
    toolButtonRundownDropdown->setMenu(this->contextMenuRundownDropdown);
    toolButtonRundownDropdown->setPopupMode(QToolButton::InstantPopup);
    this->tabWidgetRundown->setCornerWidget(toolButtonRundownDropdown);
    //this->tabWidgetRundown->setTabIcon(0, QIcon(":/Graphics/Images/TabSplitter.png"));

    RundownTreeWidget* widget = new RundownTreeWidget(this);
    int index = this->tabWidgetRundown->addTab(widget/*, QIcon(":/Graphics/Images/TabSplitter.png")*/, Rundown::DEFAULT_NAME);
    this->tabWidgetRundown->setCurrentIndex(index);

    QObject::connect(&EventManager::getInstance(), SIGNAL(newRundownMenu(const NewRundownMenuEvent&)), this, SLOT(newRundownMenu(const NewRundownMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(openRundownMenu(const OpenRundownMenuEvent&)), this, SLOT(openRundownMenu(const OpenRundownMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(newRundown(const NewRundownEvent&)), this, SLOT(newRundown(const NewRundownEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(allowRemoteTriggeringMenu(const AllowRemoteTriggeringMenuEvent&)), this, SLOT(allowRemoteTriggeringMenu(const AllowRemoteTriggeringMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(closeRundown(const CloseRundownEvent&)), this, SLOT(closeRundown(const CloseRundownEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(deleteRundown(const DeleteRundownEvent&)), this, SLOT(deleteRundown(const DeleteRundownEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(openRundown(const OpenRundownEvent&)), this, SLOT(openRundown(const OpenRundownEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(saveRundown(const SaveRundownEvent&)), this, SLOT(saveRundown(const SaveRundownEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(activeRundownChanged(const ActiveRundownChangedEvent&)), this, SLOT(activeRundownChanged(const ActiveRundownChangedEvent&)));
}

void RundownWidget::setupMenus()
{
    this->contextMenuRundownDropdown = new QMenu(this);
    this->contextMenuRundownDropdown->setTitle("Rundown Dropdown");
    this->newRundownAction = this->contextMenuRundownDropdown->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "New Rundown", this, SLOT(createNewRundown()));
    this->openRundownAction = this->contextMenuRundownDropdown->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "Open Rundown...", this, SLOT(openRundownFromDisk()));
    this->contextMenuRundownDropdown->addSeparator();
    this->contextMenuRundownDropdown->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "Save", this, SLOT(saveRundownToDisk()));
    this->contextMenuRundownDropdown->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "Save As...", this, SLOT(saveAsRundownToDisk()));
    this->contextMenuRundownDropdown->addSeparator();
    this->contextMenuRundownDropdown->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "Toggle Compact View", this, SLOT(toggleCompactView()));
    this->allowRemoteTriggeringAction = this->contextMenuRundownDropdown->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "Allow Remote Triggering");
    this->contextMenuRundownDropdown->addSeparator();
    this->contextMenuRundownDropdown->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "Close Rundown", this, SLOT(closeCurrentRundown()));
    this->allowRemoteTriggeringAction->setCheckable(true);
    QObject::connect(this->allowRemoteTriggeringAction, SIGNAL(toggled(bool)), this, SLOT(allowRemoteTriggering(bool)));
}


void RundownWidget::newRundownMenu(const NewRundownMenuEvent& event)
{
    this->newRundownAction->setEnabled(event.getEnabled());
}

void RundownWidget::openRundownMenu(const OpenRundownMenuEvent& event)
{
    this->openRundownAction->setEnabled(event.getEnabled());
}

void RundownWidget::newRundown(const NewRundownEvent& event)
{
    RundownTreeWidget* widget = new RundownTreeWidget(this);
    int index = this->tabWidgetRundown->addTab(widget/*, QIcon(":/Graphics/Images/TabSplitter.png")*/, Rundown::DEFAULT_NAME);
    this->tabWidgetRundown->setCurrentIndex(index);

    if (this->tabWidgetRundown->count() == Rundown::MAX_NUMBER_OF_RUNDONWS)
    {
        EventManager::getInstance().fireNewRundownMenuEvent(NewRundownMenuEvent(false));
        EventManager::getInstance().fireOpenRundownMenuEvent(OpenRundownMenuEvent(false));
    }
}

void RundownWidget::allowRemoteTriggeringMenu(const AllowRemoteTriggeringMenuEvent& event)
{
    // We do not want to trigger check changed event.
    this->allowRemoteTriggeringAction->blockSignals(true);
    this->allowRemoteTriggeringAction->setChecked(event.getEnabled());
    this->allowRemoteTriggeringAction->blockSignals(false);

    if (!event.getEnabled())
        this->tabWidgetRundown->setTabIcon(this->tabWidgetRundown->currentIndex(), QIcon());
    else
        this->tabWidgetRundown->setTabIcon(this->tabWidgetRundown->currentIndex(), QIcon(":/Graphics/Images/OscTriggerSmall.png"));
}

void RundownWidget::closeRundown(const CloseRundownEvent& event)
{
    if (this->tabWidgetRundown->count() > 1)
        EventManager::getInstance().fireDeleteRundownEvent(DeleteRundownEvent(this->tabWidgetRundown->currentIndex()));
}

void RundownWidget::deleteRundown(const DeleteRundownEvent& event)
{
    if (this->tabWidgetRundown->count() <= Rundown::MAX_NUMBER_OF_RUNDONWS)
    {
        EventManager::getInstance().fireNewRundownMenuEvent(NewRundownMenuEvent(true));
        EventManager::getInstance().fireOpenRundownMenuEvent(OpenRundownMenuEvent(true));
    }

    // Delete the page widget, which automatically removes the tab as well.
    delete this->tabWidgetRundown->widget(event.getIndex());
}

void RundownWidget::openRundown(const OpenRundownEvent& event)
{
    QString path = "";

    if (event.getPath().isEmpty())
        path = QFileDialog::getOpenFileName(this, "Open Rundown", "", "Rundown (*.xml)");
    else
        path =  event.getPath();

    if (!path.isEmpty())
    {
        EventManager::getInstance().fireStatusbarEvent(StatusbarEvent("Opening rundown..."));

        QFileInfo info(path);
        RundownTreeWidget* widget = new RundownTreeWidget(this);       

        int index = this->tabWidgetRundown->addTab(widget/*, QIcon(":/Graphics/Images/TabSplitter.png")*/, info.baseName());
        this->tabWidgetRundown->setCurrentIndex(index);

        widget->openRundown(path);

        if (this->tabWidgetRundown->count() == Rundown::MAX_NUMBER_OF_RUNDONWS)
        {
            EventManager::getInstance().fireNewRundownMenuEvent(NewRundownMenuEvent(false));
            EventManager::getInstance().fireOpenRundownMenuEvent(OpenRundownMenuEvent(false));
        }

        EventManager::getInstance().fireActiveRundownChangedEvent(ActiveRundownChangedEvent(path));
        EventManager::getInstance().fireStatusbarEvent(StatusbarEvent(""));
    }
}

void RundownWidget::saveRundown(const SaveRundownEvent& event)
{
    dynamic_cast<RundownTreeWidget*>(this->tabWidgetRundown->currentWidget())->saveRundown(event.getSaveAs());
}

void RundownWidget::activeRundownChanged(const ActiveRundownChangedEvent& event)
{
    QFileInfo info(event.getPath());
    this->tabWidgetRundown->setTabText(this->tabWidgetRundown->currentIndex(), info.baseName());
}









void RundownWidget::createNewRundown()
{
    EventManager::getInstance().fireNewRundownEvent(NewRundownEvent());
}

void RundownWidget::openRundownFromDisk()
{
    EventManager::getInstance().fireOpenRundownEvent(OpenRundownEvent());
}

void RundownWidget::saveRundownToDisk()
{
    EventManager::getInstance().fireSaveRundownEvent(SaveRundownEvent(false));
}

void RundownWidget::saveAsRundownToDisk()
{
    EventManager::getInstance().fireSaveRundownEvent(SaveRundownEvent(true));
}

void RundownWidget::toggleCompactView()
{
    EventManager::getInstance().fireToggleCompactViewEvent(CompactViewEvent());
}

void RundownWidget::closeCurrentRundown()
{
    EventManager::getInstance().fireCloseRundownEvent(CloseRundownEvent());
}

void RundownWidget::allowRemoteTriggering(bool enabled)
{
    EventManager::getInstance().fireRemoteRundownTriggeringEvent(RemoteRundownTriggeringEvent(enabled));
    EventManager::getInstance().fireAllowRemoteTriggeringMenuEvent(AllowRemoteTriggeringMenuEvent(enabled));
}

bool RundownWidget::selectTab(int index)
{
    if (index <= this->tabWidgetRundown->count())
        this->tabWidgetRundown->setCurrentIndex(index - 1);

    return true;
}

void RundownWidget::tabCloseRequested(int index)
{
    if (this->tabWidgetRundown->count() > 1)
        EventManager::getInstance().fireDeleteRundownEvent(DeleteRundownEvent(index));
}

void RundownWidget::currentChanged(int index)
{
    for (int i = 0; i < this->tabWidgetRundown->count(); i++)
        dynamic_cast<RundownTreeWidget*>(this->tabWidgetRundown->widget(i))->setActive(false);

    dynamic_cast<RundownTreeWidget*>(this->tabWidgetRundown->widget(index))->setActive(true);

    bool allowRemoteTriggering = dynamic_cast<RundownTreeWidget*>(this->tabWidgetRundown->widget(index))->getAllowRemoteTriggering();

    // We do not want to trigger check changed event.
    this->allowRemoteTriggeringAction->blockSignals(true);
    this->allowRemoteTriggeringAction->setChecked(allowRemoteTriggering);
    this->allowRemoteTriggeringAction->blockSignals(false);
}

void RundownWidget::gpiBindingChanged(int gpiPort, Playout::PlayoutType::Type binding)
{
    for (int i = 0; i < this->tabWidgetRundown->count(); i++)
        dynamic_cast<RundownTreeWidget*>(this->tabWidgetRundown->widget(i))->gpiBindingChanged(gpiPort, binding);
}
