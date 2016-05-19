/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QtPlugin>

// PorousScaffolds Logic includes
#include <vtkSlicerPorousScaffoldsLogic.h>

// PorousScaffolds includes
#include "qSlicerPorousScaffoldsModule.h"
#include "qSlicerPorousScaffoldsModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerPorousScaffoldsModule, qSlicerPorousScaffoldsModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerPorousScaffoldsModulePrivate
{
public:
  qSlicerPorousScaffoldsModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerPorousScaffoldsModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerPorousScaffoldsModulePrivate::qSlicerPorousScaffoldsModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerPorousScaffoldsModule methods

//-----------------------------------------------------------------------------
qSlicerPorousScaffoldsModule::qSlicerPorousScaffoldsModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerPorousScaffoldsModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerPorousScaffoldsModule::~qSlicerPorousScaffoldsModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerPorousScaffoldsModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerPorousScaffoldsModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerPorousScaffoldsModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerPorousScaffoldsModule::icon() const
{
  return QIcon(":/Icons/PorousScaffolds.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerPorousScaffoldsModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerPorousScaffoldsModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerPorousScaffoldsModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerPorousScaffoldsModule
::createWidgetRepresentation()
{
  return new qSlicerPorousScaffoldsModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerPorousScaffoldsModule::createLogic()
{
  return vtkSlicerPorousScaffoldsLogic::New();
}
