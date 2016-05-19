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

//	MRML includes
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelStorageNode.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLViewNode.h>

// Qt includes
#include <QDebug>

// SlicerQt includes
#include <qMRMLThreeDView.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDViewControllerWidget.h>
#include <qSlicerApplication.h>
#include <qSlicerIOManager.h>
#include <qSlicerLayoutManager.h>
#include "qSlicerPorousScaffoldsModuleWidget.h"
#include "ui_qSlicerPorousScaffoldsModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerPorousScaffoldsModuleWidgetPrivate : public Ui_qSlicerPorousScaffoldsModuleWidget
{
public:
	qSlicerPorousScaffoldsModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerPorousScaffoldsModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerPorousScaffoldsModuleWidgetPrivate::qSlicerPorousScaffoldsModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerPorousScaffoldsModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerPorousScaffoldsModuleWidget::qSlicerPorousScaffoldsModuleWidget(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSlicerPorousScaffoldsModuleWidgetPrivate)
{
	//  return the reference to the renderWindow of the Slicer application singleton 
	//  and those to its renderer and renderWindowInteractor
	this->app = qSlicerApplication::application();
	qSlicerLayoutManager *layoutManager = this->app->layoutManager();
	qMRMLThreeDWidget *threeDWidget = layoutManager->threeDWidget(0);
	qMRMLThreeDView *threeDView = threeDWidget->threeDView();
	this->renderWindow = threeDView->renderWindow();
	this->renderer = this->renderWindow->GetRenderers()->GetFirstRenderer();
	this->iren = this->renderWindow->GetInteractor();
}

//-----------------------------------------------------------------------------
qSlicerPorousScaffoldsModuleWidget::~qSlicerPorousScaffoldsModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerPorousScaffoldsModuleWidget::setup()
{
	Q_D(qSlicerPorousScaffoldsModuleWidget);
	d->setupUi(this);
	this->Superclass::setup();

	//Initiatilize Variance
	this->polydataResult = vtkSmartPointer<vtkPolyData>::New();
	this->polydataMain = vtkSmartPointer<vtkPolyData>::New();
	this->polydataCell = vtkSmartPointer<vtkPolyData>::New();
	this->cellType = 0;
	this->cellScale = 1.0;

	this->m_vtkConnection = vtkSmartPointer<vtkEventQtSlotConnect>::New();

	//Set combo box
	d->comboBoxCell->addItem("None");
	d->comboBoxCell->addItem("Cube");
	d->comboBoxCell->addItem("Tetrahedron");

	d->comboBoxCell->setItemText(0, "None");
	d->comboBoxCell->setItemText(1, "Cube");
	d->comboBoxCell->setItemText(2, "Tetrahedron");

	//Connect slots
	connect(d->buttonScaffoldOK, SIGNAL(clicked()), this, SLOT(buttonScaffoldOKClicked()));
	connect(d->buttonScaffoldCancel, SIGNAL(clicked()), this, SLOT(buttonScaffoldCancelClicked()));
	connect(d->sliderCellScale, SIGNAL(valueChanged(int)), this, SLOT(sliderCellScaleChanged()));
	connect(d->comboBoxCell, SIGNAL(activated(int)), this, SLOT(comboBoxCellChanged()));
}

//Button OK clicked event
void qSlicerPorousScaffoldsModuleWidget::buttonScaffoldOKClicked()
{
	Q_D(qSlicerPorousScaffoldsModuleWidget);

	//	obtain the source model
	vtkMRMLScene *mrmlScene = this->app->mrmlScene();

	QString nodeID;
	char *charNodeID;
	nodeID = d->MRMLNodeComboBox->currentNodeId();
	charNodeID = nodeID.toLatin1().data();
	if (nodeID == "")
	{
		QMessageBox::information(this,
			tr("Warning"),
			tr("Please select a model!"));
		return;
	}

	vtkMRMLNode *sourceNode = mrmlScene->GetNodeByID(charNodeID);
	vtkSmartPointer<vtkMRMLModelNode> sourceModel =
		vtkSmartPointer<vtkMRMLModelNode>::New();
	sourceModel->Copy(sourceNode);

	//Compute normals of the model
	vtkSmartPointer<vtkPolyDataNormals>normFilter =
		vtkSmartPointer<vtkPolyDataNormals>::New();
	normFilter->SetInputData(sourceModel->GetPolyData());
	normFilter->SetComputeCellNormals(1);
	normFilter->SetComputePointNormals(1);
	normFilter->Update();

	this->polydataResult->DeepCopy(normFilter->GetOutput());
	this->polydataMain->DeepCopy(normFilter->GetOutput());


	switch (this->cellType)
	{
	case 1:
		this->SetCellsCube();
		break;
	case 2:
		this->SetCellsTetrahedron();
		break;

	default:
		break;
	}

	//	display and store the result model
	vtkSmartPointer<vtkMRMLModelNode> resultModel =
		vtkSmartPointer<vtkMRMLModelNode>::New();
	QString resultName = d->MRMLNodeComboBox->currentNodeID();
	resultName.append("_pores");
	resultModel->SetName(resultName.toLatin1().data());
	resultModel->SetAndObservePolyData(polydataResult);
	resultModel->SetScene(mrmlScene);

	vtkSmartPointer<vtkMRMLModelDisplayNode> resultDisplay =
		vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	vtkSmartPointer<vtkMRMLModelStorageNode> resultStorage =
		vtkSmartPointer<vtkMRMLModelStorageNode>::New();
	resultDisplay->SetScene(mrmlScene);
	resultStorage->SetScene(mrmlScene);
	resultDisplay->SetInputPolyDataConnection(resultModel->GetPolyDataConnection());
	//	resultDisplay->SetColor(0.0, 1.0, 0.0);
	resultStorage->SetFileName(resultName.toLatin1().data());
	mrmlScene->AddNode(resultDisplay);
	mrmlScene->AddNode(resultStorage);
	resultModel->SetAndObserveDisplayNodeID(resultDisplay->GetID());
	resultModel->SetAndObserveStorageNodeID(resultStorage->GetID());
	mrmlScene->AddNode(resultModel);

	d->buttonScaffoldOK->setDisabled(true);
	d->buttonScaffoldCancel->setDisabled(true);
}

//Button Cancel clicked event
void qSlicerPorousScaffoldsModuleWidget::buttonScaffoldCancelClicked()
{
	Q_D(qSlicerPorousScaffoldsModuleWidget);

	d->buttonScaffoldOK->setDisabled(true);
	d->buttonScaffoldCancel->setDisabled(true);
}

//Slider dragged event
void qSlicerPorousScaffoldsModuleWidget::sliderCellScaleChanged()
{
	Q_D(qSlicerPorousScaffoldsModuleWidget);

	this->cellScale = double(d->sliderCellScale->value() / 20.0);

	QString textCellScale;
	textCellScale.sprintf("%.1f", this->cellScale*2.0);
	d->labelCellDiameter->setText(textCellScale);
}

//ComboxBox Select Cell event
void qSlicerPorousScaffoldsModuleWidget::comboBoxCellChanged()
{
	Q_D(qSlicerPorousScaffoldsModuleWidget);

	int curIndex = d->comboBoxCell->currentIndex();
	this->cellType = curIndex;
	QString filePathCell;

	switch (curIndex)
	{
	case 1:
		filePathCell = "./pores/Cell01.stl";
		d->buttonScaffoldOK->setEnabled(1);
		d->buttonScaffoldCancel->setEnabled(1);
		break;

	case 2:
		filePathCell = "./pores/Cell02.stl";
		d->buttonScaffoldOK->setEnabled(1);
		d->buttonScaffoldCancel->setEnabled(1);
		break;

	default:
		d->buttonScaffoldOK->setEnabled(0);
		d->buttonScaffoldCancel->setEnabled(0);
		QMessageBox::information(this,
			tr("Warning"),
			tr("None pore selected!"));
		return;
		break;
	}

	//Support path in Chinese 
	QByteArray tempLoadPath = filePathCell.toLocal8Bit();
	const char *fileLoad_cn = tempLoadPath.data();

	vtkSmartPointer<vtkSTLReader>readerPolySTL =
		vtkSmartPointer<vtkSTLReader>::New();
	readerPolySTL->SetFileName(fileLoad_cn);

	//Compute normals of the model
	vtkSmartPointer<vtkPolyDataNormals>normFilter =
		vtkSmartPointer<vtkPolyDataNormals>::New();
	normFilter->SetInputConnection(readerPolySTL->GetOutputPort());
	normFilter->SetComputeCellNormals(1);
	normFilter->SetComputePointNormals(1);
	normFilter->ConsistencyOn();
	normFilter->SplittingOff();
	normFilter->Update();

	//copy polydata
	this->polydataCell->DeepCopy(normFilter->GetOutput());

	d->buttonScaffoldOK->setEnabled(true);
	d->buttonScaffoldCancel->setEnabled(true);

}

//Compute bound points coordinates
double** qSlicerPorousScaffoldsModuleWidget::PointsCoord(double bounds[6])
{
	double** res = new double*[8];
	for (int i = 0; i < 8; i++)
	{
		res[i] = new double[3];
	}

	res[0][0] = bounds[0];
	res[0][1] = bounds[2];
	res[0][2] = bounds[4];

	res[1][0] = bounds[1];
	res[1][1] = bounds[2];
	res[1][2] = bounds[4];

	res[2][0] = bounds[0];
	res[2][1] = bounds[3];
	res[2][2] = bounds[4];

	res[3][0] = bounds[1];
	res[3][1] = bounds[3];
	res[3][2] = bounds[4];

	res[4][0] = bounds[0];
	res[4][1] = bounds[2];
	res[4][2] = bounds[5];

	res[5][0] = bounds[1];
	res[5][1] = bounds[2];
	res[5][2] = bounds[5];

	res[6][0] = bounds[0];
	res[6][1] = bounds[3];
	res[6][2] = bounds[5];

	res[7][0] = bounds[1];
	res[7][1] = bounds[3];
	res[7][2] = bounds[5];

	return res;

}

//Compute distance of two points
double qSlicerPorousScaffoldsModuleWidget::PointsDistance(double point1[3], double point2[3])
{
	double difference[3];
	double res;

	for (int i = 0; i < 3; i++)
	{
		difference[i] = point2[i] - point1[i];
	}
	res = sqrt(difference[0] * difference[0] + difference[1] * difference[1] + difference[2] * difference[2]);

	return res;
}

//Compute the vector from point1 to point2
double* qSlicerPorousScaffoldsModuleWidget::ComputeVector(double point1[3], double point2[3])
{
	double* res = new double[3];
	for (int i = 0; i < 3; i++)
	{
		res[i] = point2[i] - point1[i];
	}

	double absRes;
	absRes = sqrt(res[0] * res[0] + res[1] * res[1] + res[2] * res[2]);
	for (int i = 0; i < 3; i++)
	{
		res[i] = res[i] / absRes;
	}

	return res;
}

// Combine pore cells
vtkSmartPointer<vtkPolyData>qSlicerPorousScaffoldsModuleWidget::
CombinePolyData(vtkPolyData* input1, vtkPolyData* input2)
{
	vtkSmartPointer<vtkAppendPolyData> appendFilter =
		vtkSmartPointer<vtkAppendPolyData>::New();
#if VTK_MAJOR_VERSION <= 5
	appendFilter->AddInputConnection(input1->GetProducerPort());
	appendFilter->AddInputConnection(input2->GetProducerPort());
#else
	appendFilter->AddInputData(input1);
	appendFilter->AddInputData(input2);
#endif
	appendFilter->Update();

	return appendFilter->GetOutput();
}

//Bool intersection
vtkSmartPointer<vtkPolyData>qSlicerPorousScaffoldsModuleWidget::
BoolIntersection(vtkPolyData* input1, vtkPolyData*input2)
{
	vtkSmartPointer<vtkBooleanOperationPolyDataFilter>BoolOperation =
		vtkSmartPointer<vtkBooleanOperationPolyDataFilter>::New();
	BoolOperation->SetOperationToIntersection();
	BoolOperation->SetInputData(0, input1);
	BoolOperation->SetInputData(1, input2);
	BoolOperation->Update();

	return BoolOperation->GetOutput();
}

//Bool Union
vtkSmartPointer<vtkPolyData>qSlicerPorousScaffoldsModuleWidget::BoolUnion(vtkPolyData* input1, vtkPolyData* input2)
{
	vtkSmartPointer<vtkBooleanOperationPolyDataFilter>BoolOperation =
		vtkSmartPointer<vtkBooleanOperationPolyDataFilter>::New();
	BoolOperation->SetOperationToUnion();
	BoolOperation->SetInputData(0, input1);
	BoolOperation->SetInputData(1, input2);
	BoolOperation->Update();

	return BoolOperation->GetOutput();
}

//Cell transform
vtkSmartPointer<vtkPolyData>qSlicerPorousScaffoldsModuleWidget::
TransformCell(double Coordx, double Coordy, double Coordz, double CellScale)
{
	vtkSmartPointer<vtkTransform> trans = vtkSmartPointer<vtkTransform>::New();
	trans->PostMultiply();
	trans->Scale(CellScale, CellScale, CellScale);
	trans->Translate(Coordx, Coordy, Coordz);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetTransform(trans);
	transformFilter->SetInputData(this->polydataCell);
	transformFilter->Update();

	return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData>qSlicerPorousScaffoldsModuleWidget::RemoveOddCell(vtkPolyData* polyInput)
{
	//Compute normals
	vtkSmartPointer<vtkPolyDataNormals>norm =
		vtkSmartPointer<vtkPolyDataNormals>::New();
	norm->SetInputData(polyInput);
	norm->ComputePointNormalsOn();
	norm->ComputeCellNormalsOn();
	norm->ConsistencyOn();
	norm->SplittingOff();
	norm->Update();
	polyInput = norm->GetOutput();

	vtkSmartPointer<vtkIdList> cellPointIds =
		vtkSmartPointer<vtkIdList>::New();
	vtkSmartPointer<vtkIdList> neighborCellIds =
		vtkSmartPointer<vtkIdList>::New();

	for (vtkIdType cellId = 0; cellId < polyInput->GetNumberOfCells(); cellId++)
	{
		polyInput->GetCellPoints(cellId, cellPointIds);

		std::list<vtkIdType> neighbors;
		vtkIdType neighborsCount = 0;
		int noddedgeCount = 0;

		for (vtkIdType i = 0; i < cellPointIds->GetNumberOfIds(); i++)
		{
			vtkSmartPointer<vtkIdList> idList =
				vtkSmartPointer<vtkIdList>::New();

			//add one of the edge points
			idList->InsertNextId(cellPointIds->GetId(i));

			//add the other edge point
			if (i + 1 == cellPointIds->GetNumberOfIds())
			{
				idList->InsertNextId(cellPointIds->GetId(0));
			}
			else
			{
				idList->InsertNextId(cellPointIds->GetId(i + 1));
			}

			//get the neighbors of the cell
			polyInput->GetCellNeighbors(cellId, idList, neighborCellIds);

			neighborsCount += neighborCellIds->GetNumberOfIds();
			if (neighborCellIds->GetNumberOfIds() > 0)
			{
				noddedgeCount++;
			}
		}

		if (noddedgeCount < 1)
		{
			polyInput->DeleteCell(cellId);
		}

	}
	polyInput->RemoveDeletedCells();

	return polyInput;
}

//Simplify polydata
vtkSmartPointer<vtkPolyData>qSlicerPorousScaffoldsModuleWidget::SimplifyPolydata(vtkPolyData* polyInput)
{
	vtkIdType cellNum = polyInput->GetNumberOfCells();
	if (cellNum > 5000)
	{
		double reduceRate;
		reduceRate = 1.0 - 8000.0 / double(cellNum);
		vtkSmartPointer<vtkDecimatePro>decimate =
			vtkSmartPointer<vtkDecimatePro>::New();
		decimate->SetInputData(polyInput);
		decimate->SetTargetReduction(0.6);
		decimate->Update();

		return decimate->GetOutput();
		//		return polyInput;
	}
	else
	{
		return polyInput;
	}

}

//Set cells of cube shape
void qSlicerPorousScaffoldsModuleWidget::SetCellsCube()
{
	Q_D(qSlicerPorousScaffoldsModuleWidget);

	double bounds[6];
	this->polydataMain->GetBounds(bounds);

	double** BoundCoord;
	BoundCoord = this->PointsCoord(bounds);
	double BoundCoord0[3];
	for (int i = 0; i < 3; i++)
	{
		BoundCoord0[i] = BoundCoord[0][i];
	}

	double BoundDistX, BoundDistY, BoundDistZ;
	BoundDistX = this->PointsDistance(BoundCoord[0], BoundCoord[1]);
	BoundDistY = this->PointsDistance(BoundCoord[0], BoundCoord[2]);
	BoundDistZ = this->PointsDistance(BoundCoord[0], BoundCoord[4]);

	double CellDist = 3.0 * this->cellScale;

	int CellNumberX, CellNumberY, CellNumberZ;
	CellNumberX = ceil(BoundDistX / CellDist + 1);
	CellNumberY = ceil(BoundDistY / CellDist + 1);
	CellNumberZ = ceil(BoundDistZ / CellDist + 1);
	int CellNumber = CellNumberX*CellNumberY*CellNumberZ;
	int CurNumber = 0;

	this->polydataResult->Initialize();

	vtkSmartPointer<vtkPolyData>polydataTemp;

	vtkSmartPointer<vtkPolyDataMapper>mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	for (int k = 0; k < CellNumberZ; k++)
	{
		for (int j = 0; j < CellNumberY; j++)
		{
			for (int i = 0; i < CellNumberX; i++)
			{
				polydataTemp = this->TransformCell(BoundCoord0[0] + CellDist*double(i),
					BoundCoord0[1] + CellDist*double(j), BoundCoord0[2] + CellDist*double(k), this->cellScale);
				polydataTemp = this->BoolIntersection(polydataTemp, this->polydataMain);
				polydataTemp = this->RemoveOddCell(polydataTemp);
				this->polydataResult = this->CombinePolyData(this->polydataResult, polydataTemp);

				CurNumber++;
				int per = double(CurNumber) / double(CellNumber) *100.0;
				d->progressBar->setValue(per);
			}
		}
	}

	//Delete pointers
	for (int i = 0; i < 8; i++)
	{
		delete *(BoundCoord + i);
	}
	delete BoundCoord;
}

//Set cells of tetrahedron shape
void qSlicerPorousScaffoldsModuleWidget::SetCellsTetrahedron()
{
	Q_D(qSlicerPorousScaffoldsModuleWidget);

	double bounds[6];
	this->polydataMain->GetBounds(bounds);

	double** BoundCoord;
	BoundCoord = this->PointsCoord(bounds);
	double BoundCoord0[3];
	for (int i = 0; i < 3; i++)
	{
		BoundCoord0[i] = BoundCoord[0][i];
	}

	double BoundDistX, BoundDistY, BoundDistZ;
	BoundDistX = this->PointsDistance(BoundCoord[0], BoundCoord[1]);
	BoundDistY = this->PointsDistance(BoundCoord[0], BoundCoord[2]);
	BoundDistZ = this->PointsDistance(BoundCoord[0], BoundCoord[4]);

	double CellDistx = 4.4 * this->cellScale;
	double CellDisty = 8.0 * this->cellScale;
	double CellDistz = 4.5 * this->cellScale;

	int CellNumberX, CellNumberY, CellNumberZ;
	CellNumberX = ceil(BoundDistX / CellDistx + 1);
	CellNumberY = ceil(BoundDistY / CellDisty + 1);
	CellNumberZ = ceil(BoundDistZ / CellDistz + 1);
	int CellNumber = (2 * CellNumberX + 1)*CellNumberY*CellNumberZ;
	int CurNumber = 0;

	vtkSmartPointer<vtkPolyData>polydataTemp;

	this->polydataResult->Initialize();

	vtkSmartPointer<vtkPolyDataMapper>mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	double startX = BoundCoord0[0];
	double startY = BoundCoord0[1];
	double startZ = BoundCoord0[2];
	for (int i = 0; i < CellNumberZ; i++)
	{
		for (int j = 0; j < CellNumberY; j++)
		{

			for (int k = 0; k < CellNumberX; k++)
			{
				polydataTemp = this->TransformCell(startX + CellDistx*double(k),
					startY + CellDisty*double(j), startZ + CellDistz*double(i), this->cellScale);

				polydataTemp = this->BoolIntersection(polydataTemp, this->polydataMain);

				polydataTemp = this->RemoveOddCell(polydataTemp);
				this->polydataResult = this->CombinePolyData(this->polydataResult, polydataTemp);

				CurNumber++;
				int per = double(CurNumber) / double(CellNumber) *100.0;
				d->progressBar->setValue(per);
			}

			for (int k = 0; k < CellNumberX + 1; k++)
			{
				polydataTemp = this->TransformCell(startX + CellDistx*double(k - 0.5),
					startY + CellDisty*double(j + 0.5), startZ + CellDistz*double(i), this->cellScale);

				polydataTemp = this->BoolIntersection(polydataTemp, this->polydataMain);

				polydataTemp = this->RemoveOddCell(polydataTemp);
				this->polydataResult = this->CombinePolyData(this->polydataResult, polydataTemp);

				CurNumber++;
				int per = double(CurNumber) / double(CellNumber) *100.0;
				d->progressBar->setValue(per);
			}

		}

		switch (i % 8)
		{
		case 0:
			startY -= 3.0 * this->cellScale;
			break;
		case 1:
			startX -= 2.2 * this->cellScale;
			startY += 1.0 * this->cellScale;
			break;
		case 2:
			startX += 2.2 * this->cellScale;
			startY += 1.0 * this->cellScale;
			break;
		case 3:
			startX -= 2.2 * this->cellScale;
			startY += 1.0 * this->cellScale;
			break;
		case 4:
			startY -= 3.0 * this->cellScale;
			break;
		case 5:
			startX += 2.2 * this->cellScale;
			startY += 1.0 * this->cellScale;
			break;
		case 6:
			startX -= 2.2 * this->cellScale;
			startY += 1.0 * this->cellScale;
			break;
		case 7:
			startX += 2.2 * this->cellScale;
			startY += 1.0 * this->cellScale;
			break;
		}

	}

	//Delete pointers
	for (int i = 0; i < 8; i++)
	{
		delete *(BoundCoord + i);
	}
	delete BoundCoord;
}
