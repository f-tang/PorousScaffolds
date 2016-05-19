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

#ifndef __qSlicerPorousScaffoldsModuleWidget_h
#define __qSlicerPorousScaffoldsModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include <qSlicerApplication.h>
#include "qSlicerPorousScaffoldsModuleExport.h"

//	Qt includes
#include <QMessageBox>

//	vtk includes
#include<vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkActor.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkReverseSense.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkWorldPointPicker.h>
#include <vtkCellPicker.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkThreshold.h>
#include <vtkDistancePolyDataFilter.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkIdList.h>
#include <vtkFillHolesFilter.h>
#include <vtkDecimatePro.h>
#include <vtkLinearSubdivisionFilter.h>



class qSlicerPorousScaffoldsModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_POROUSSCAFFOLDS_EXPORT qSlicerPorousScaffoldsModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerPorousScaffoldsModuleWidget(QWidget *parent=0);
  virtual ~qSlicerPorousScaffoldsModuleWidget();

public slots:


protected:
  QScopedPointer<qSlicerPorousScaffoldsModuleWidgetPrivate> d_ptr;

  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerPorousScaffoldsModuleWidget);
  Q_DISABLE_COPY(qSlicerPorousScaffoldsModuleWidget);

//Below is extension code
protected:
	vtkRenderWindowInteractor* iren;
	vtkRenderWindow* renderWindow;
	vtkRenderer* renderer;
	qSlicerApplication *app;

private slots:
	void buttonScaffoldOKClicked();
	void buttonScaffoldCancelClicked();
	void sliderCellScaleChanged();
	void comboBoxCellChanged();

private:
	double** PointsCoord(double bounds[6]);
	double PointsDistance(double point1[3], double point2[3]);
	double* ComputeVector(double point1[3], double point2[3]);
	vtkSmartPointer<vtkPolyData>CombinePolyData(vtkPolyData* input1, vtkPolyData* input2);
	vtkSmartPointer<vtkPolyData>BoolIntersection(vtkPolyData* input1, vtkPolyData* input2);
	vtkSmartPointer<vtkPolyData>BoolUnion(vtkPolyData* input1, vtkPolyData* input2);
	vtkSmartPointer<vtkPolyData>TransformCell(double Coordx, double Coordy, double Coordz, double CellScale);
	vtkSmartPointer<vtkPolyData>RemoveOddCell(vtkPolyData* polyInput);
	vtkSmartPointer<vtkPolyData>SimplifyPolydata(vtkPolyData* polyInput);
	void SetCellsCube();
	void SetCellsTetrahedron();

private:
	int cellType;
	double cellScale;
	vtkSmartPointer<vtkActor>mActor;
	vtkSmartPointer<vtkPolyData>polydataMain;
	vtkSmartPointer<vtkPolyData>polydataResult;
	vtkSmartPointer<vtkPolyData>polydataCell;
	vtkSmartPointer<vtkEventQtSlotConnect>m_vtkConnection;

};

#endif
