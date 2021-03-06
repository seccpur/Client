#include "InspectorVolumeWidget.h"

#include "Global.h"

#include "DatabaseManager.h"
#include "EventManager.h"
#include "Events/PreviewEvent.h"
#include "Models/TweenModel.h"

#include <QtCore/QDebug>

#include <QtGui/QApplication>

InspectorVolumeWidget::InspectorVolumeWidget(QWidget* parent)
    : QWidget(parent),
      model(NULL), command(NULL)
{
    setupUi(this);

    QObject::connect(&EventManager::getInstance(), SIGNAL(rundownItemSelected(const RundownItemSelectedEvent&)), this, SLOT(rundownItemSelected(const RundownItemSelectedEvent&)));

    loadTween();
}

void InspectorVolumeWidget::rundownItemSelected(const RundownItemSelectedEvent& event)
{
    this->model = event.getLibraryModel();

    blockAllSignals(true);

    if (dynamic_cast<VolumeCommand*>(event.getCommand()))
    {
        this->command = dynamic_cast<VolumeCommand*>(event.getCommand());

        this->sliderVolume->setValue(QString("%1").arg(this->command->getVolume() * 100).toFloat());
        this->spinBoxVolume->setValue(QString("%1").arg(this->command->getVolume() * 100).toFloat());
        this->spinBoxDuration->setValue(this->command->getDuration());
        this->comboBoxTween->setCurrentIndex(this->comboBoxTween->findText(this->command->getTween()));
        this->checkBoxDefer->setChecked(this->command->getDefer());
    }

    blockAllSignals(false);
}

void InspectorVolumeWidget::blockAllSignals(bool block)
{
    this->sliderVolume->blockSignals(block);
    this->spinBoxVolume->blockSignals(block);
    this->spinBoxDuration->blockSignals(block);
    this->comboBoxTween->blockSignals(block);
    this->checkBoxDefer->blockSignals(block);
}

void InspectorVolumeWidget::loadTween()
{
    // We do not have a command object, block the signals.
    // Events will not be triggered while we update the values.
    this->comboBoxTween->blockSignals(true);

    QList<TweenModel> models = DatabaseManager::getInstance().getTween();
    foreach (TweenModel model, models)
        this->comboBoxTween->addItem(model.getValue());

    this->comboBoxTween->blockSignals(false);
}

void InspectorVolumeWidget::durationChanged(int duration)
{
    this->command->setDuration(duration);
}

void InspectorVolumeWidget::tweenChanged(QString tween)
{
    this->command->setTween(tween);
}

void InspectorVolumeWidget::sliderVolumeChanged(int volume)
{
    this->command->setVolume(static_cast<float>(volume) / 100);

    this->spinBoxVolume->setValue(volume);

    EventManager::getInstance().firePreviewEvent(PreviewEvent());
}

void InspectorVolumeWidget::spinBoxVolumeChanged(int volume)
{
    this->sliderVolume->setValue(volume);
}

void InspectorVolumeWidget::resetVolume(QString volume)
{
    this->sliderVolume->setValue(Mixer::DEFAULT_VOLUME * 100);
    this->command->setVolume(static_cast<float>(this->sliderVolume->value()) / 100);

    EventManager::getInstance().firePreviewEvent(PreviewEvent());
}

void InspectorVolumeWidget::resetDuration(QString duration)
{
    this->spinBoxDuration->setValue(Mixer::DEFAULT_DURATION);
    this->command->setDuration(this->spinBoxDuration->value());
}

void InspectorVolumeWidget::resetTween(QString tween)
{
    this->comboBoxTween->setCurrentIndex(this->comboBoxTween->findText(Mixer::DEFAULT_TWEEN));
    this->command->setTween(this->comboBoxTween->currentText());
}

void InspectorVolumeWidget::deferChanged(int state)
{
    this->command->setDefer((state == Qt::Checked) ? true : false);
}

void InspectorVolumeWidget::resetDefer(QString defer)
{
    this->checkBoxDefer->setChecked(Mixer::DEFAULT_DEFER);
    this->command->setDefer(this->checkBoxDefer->isChecked());
}
