#include "InspectorContrastWidget.h"

#include "Global.h"

#include "DatabaseManager.h"
#include "EventManager.h"
#include "Events/PreviewEvent.h"
#include "Models/TweenModel.h"

#include <QtGui/QApplication>

InspectorContrastWidget::InspectorContrastWidget(QWidget* parent)
    : QWidget(parent),
      model(NULL), command(NULL)
{
    setupUi(this);

    QObject::connect(&EventManager::getInstance(), SIGNAL(rundownItemSelected(const RundownItemSelectedEvent&)), this, SLOT(rundownItemSelected(const RundownItemSelectedEvent&)));

    loadTween();
}

void InspectorContrastWidget::rundownItemSelected(const RundownItemSelectedEvent& event)
{
    this->model = event.getLibraryModel();

    blockAllSignals(true);

    if (dynamic_cast<ContrastCommand*>(event.getCommand()))
    {
        this->command = dynamic_cast<ContrastCommand*>(event.getCommand());

        this->sliderContrast->setValue(QString("%1").arg(this->command->getContrast() * 100).toFloat());
        this->spinBoxContrast->setValue(QString("%1").arg(this->command->getContrast() * 100).toFloat());

        this->spinBoxDuration->setValue(this->command->getDuration());
        this->comboBoxTween->setCurrentIndex(this->comboBoxTween->findText(this->command->getTween()));
        this->checkBoxDefer->setChecked(this->command->getDefer());
    }

    blockAllSignals(false);
}

void InspectorContrastWidget::blockAllSignals(bool block)
{
    this->sliderContrast->blockSignals(block);
    this->spinBoxContrast->blockSignals(block);
    this->spinBoxDuration->blockSignals(block);
    this->comboBoxTween->blockSignals(block);
    this->checkBoxDefer->blockSignals(block);
}

void InspectorContrastWidget::loadTween()
{
    // We do not have a command object, block the signals.
    // Events will not be triggered while we update the values.
    this->comboBoxTween->blockSignals(true);

    QList<TweenModel> models = DatabaseManager::getInstance().getTween();
    foreach (TweenModel model, models)
        this->comboBoxTween->addItem(model.getValue());

    this->comboBoxTween->blockSignals(false);
}

void InspectorContrastWidget::durationChanged(int duration)
{
    this->command->setDuration(duration);
}

void InspectorContrastWidget::tweenChanged(QString tween)
{
    this->command->setTween(tween);
}

void InspectorContrastWidget::sliderContrastChanged(int contrast)
{
    this->command->setContrast(static_cast<float>(contrast) / 100);

    this->spinBoxContrast->setValue(contrast);

    EventManager::getInstance().firePreviewEvent(PreviewEvent());
}

void InspectorContrastWidget::spinBoxContrastChanged(int contrast)
{
    this->sliderContrast->setValue(contrast);
}

void InspectorContrastWidget::resetContrast(QString contrast)
{
    this->sliderContrast->setValue(Mixer::DEFAULT_CONTRAST * 100);
    this->command->setContrast(static_cast<float>(this->sliderContrast->value()) / 100);

    EventManager::getInstance().firePreviewEvent(PreviewEvent());
}

void InspectorContrastWidget::resetDuration(QString duration)
{
    this->spinBoxDuration->setValue(Mixer::DEFAULT_DURATION);
    this->command->setDuration(this->spinBoxDuration->value());
}

void InspectorContrastWidget::resetTween(QString tween)
{
    this->comboBoxTween->setCurrentIndex(this->comboBoxTween->findText(Mixer::DEFAULT_TWEEN));
    this->command->setTween(this->comboBoxTween->currentText());
}

void InspectorContrastWidget::deferChanged(int state)
{
    this->command->setDefer((state == Qt::Checked) ? true : false);
}

void InspectorContrastWidget::resetDefer(QString defer)
{
    this->checkBoxDefer->setChecked(Mixer::DEFAULT_DEFER);
    this->command->setDefer(this->checkBoxDefer->isChecked());
}
