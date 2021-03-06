#pragma once

#include "../Shared.h"

#include "Global.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>

class WIDGETS_EXPORT TemplateTreeBaseWidget : public QTreeWidget
{
    Q_OBJECT

    public:
        explicit TemplateTreeBaseWidget(QWidget* parent = 0);

    protected:
        void mouseMoveEvent(QMouseEvent* event);
        void mousePressEvent(QMouseEvent* event);

    private:
        QPoint dragStartPosition;
};
