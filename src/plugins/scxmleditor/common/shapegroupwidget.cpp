/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "shapegroupwidget.h"
#include "dragshapebutton.h"
#include "shapeprovider.h"

#include <utils/flowlayout.h>
#include <utils/utilsicons.h>

using namespace ScxmlEditor::PluginInterface;
using namespace ScxmlEditor::Common;

ShapeGroupWidget::ShapeGroupWidget(ShapeProvider *shapeProvider, int groupIndex, QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    m_ui.m_closeButton->setIcon(Utils::Icons::COLLAPSE_TOOLBAR.icon());
    auto layout = new Utils::FlowLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    m_ui.m_title->setText(shapeProvider->groupTitle(groupIndex));

    for (int i = 0; i < shapeProvider->shapeCount(groupIndex); ++i) {
        auto button = new DragShapeButton(this);
        button->setText(shapeProvider->shapeTitle(groupIndex, i));
        button->setIcon(shapeProvider->shapeIcon(groupIndex, i));
        button->setShapeInfo(groupIndex, i);

        layout->addWidget(button);
    }

    connect(m_ui.m_closeButton, &QToolButton::clicked, this, [this]() {
        m_ui.m_content->setVisible(!m_ui.m_content->isVisible());
        m_ui.m_closeButton->setIcon(m_ui.m_content->isVisible()
                                                        ? Utils::Icons::COLLAPSE_TOOLBAR.icon()
                                                        : Utils::Icons::EXPAND_TOOLBAR.icon());
    });

    m_ui.m_content->setLayout(layout);
}