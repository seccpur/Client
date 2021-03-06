#pragma once

#include "Shared.h"
#include "ui_PreviewWidget.h"

#include "Events/Inspector/TargetChangedEvent.h"
#include "Events/Library/LibraryItemSelectedEvent.h"
#include "Events/Rundown/RundownItemSelectedEvent.h"
#include "Models/LibraryModel.h"

#include <QtGui/QImage>
#include <QtGui/QWidget>

class WIDGETS_EXPORT PreviewWidget : public QWidget, Ui::PreviewWidget
{
    Q_OBJECT

    public:
        explicit PreviewWidget(QWidget* parent = 0);

    private:
        QImage image;
        bool previewAlpha;
        LibraryModel* model;

        void setThumbnail();

        Q_SLOT void switchPreview();
        Q_SLOT void targetChanged(const TargetChangedEvent&);
        Q_SLOT void libraryItemSelected(const LibraryItemSelectedEvent&);
        Q_SLOT void rundownItemSelected(const RundownItemSelectedEvent&);
};
