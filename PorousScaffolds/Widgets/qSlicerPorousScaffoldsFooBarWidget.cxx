/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerPorousScaffoldsFooBarWidget.h"
#include "ui_qSlicerPorousScaffoldsFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_PorousScaffolds
class qSlicerPorousScaffoldsFooBarWidgetPrivate
  : public Ui_qSlicerPorousScaffoldsFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerPorousScaffoldsFooBarWidget);
protected:
  qSlicerPorousScaffoldsFooBarWidget* const q_ptr;

public:
  qSlicerPorousScaffoldsFooBarWidgetPrivate(
    qSlicerPorousScaffoldsFooBarWidget& object);
  virtual void setupUi(qSlicerPorousScaffoldsFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerPorousScaffoldsFooBarWidgetPrivate
::qSlicerPorousScaffoldsFooBarWidgetPrivate(
  qSlicerPorousScaffoldsFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerPorousScaffoldsFooBarWidgetPrivate
::setupUi(qSlicerPorousScaffoldsFooBarWidget* widget)
{
  this->Ui_qSlicerPorousScaffoldsFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerPorousScaffoldsFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerPorousScaffoldsFooBarWidget
::qSlicerPorousScaffoldsFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerPorousScaffoldsFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerPorousScaffoldsFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerPorousScaffoldsFooBarWidget
::~qSlicerPorousScaffoldsFooBarWidget()
{
}
