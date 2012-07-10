#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <stdlib.h>
#include <string.h>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->pushButton_4->setDisabled(true);
    
    T1_clicked = false;
    Labels_clicked = false;
    Output_clicked = false;

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(Close_Button()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(Browse_T1()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(Browse_Labels()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(Execute_Button()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(Reset_Button()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(Browse_Output()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::Close_Button()
{
    this->close();
}

void MainWindow::Browse_T1()
{
    T1 = QFileDialog::getOpenFileName(this, tr("Open image 'T1'"),"/home","Images (*.nrrd *.gipl *.nrrd.gz *.gipl.gz)");
    ui->lineEdit->setText(T1);
    ui->lineEdit->setDisabled(true);
    T1_clicked = true;

    if ( T1_clicked && Labels_clicked && Output_clicked ) ui->pushButton_4->setDisabled(false);
}

void MainWindow::Browse_Labels()
{
    Labels = QFileDialog::getOpenFileName(this, tr("Open image 'Labels'"),"/home","Images (*.nrrd *.gipl *.nrrd.gz *.gipl.gz)");
    ui->lineEdit_2->setText(Labels);
    ui->lineEdit_2->setDisabled(true);
    Labels_clicked = true;

    if ( T1_clicked && Labels_clicked && Output_clicked ) ui->pushButton_4->setDisabled(false);
}


void MainWindow::Browse_Output()
{
    output_dir = QFileDialog::getExistingDirectory(this, tr("Save output image"),"/home");  
    ui->lineEdit_3->setText(output_dir);
    ui->lineEdit_3->setDisabled(true);
    Output_clicked = true;
    output_dir = output_dir+"/";
    
    if ( T1_clicked && Labels_clicked && Output_clicked ) ui->pushButton_4->setDisabled(false);
}


void MainWindow::Reset_Button()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->textEdit->clear();

    ui->lineEdit->setDisabled(false);
    ui->lineEdit_2->setDisabled(false);
    ui->lineEdit_3->setDisabled(false);

    ui->progressBar->setValue(0);
    
    ui->pushButton_4->setDisabled(true);
    T1_clicked = false;
    Labels_clicked = false;
    Output_clicked = false;;
}

ImageType::Pointer MainWindow::LabelsToBinary()
{
  ReaderType::Pointer reader = ReaderType::New(); 
  reader->SetFileName( Labels.toAscii().data() );
  
  try 
  {
    reader->Update();
  }
  catch ( itk::ExceptionObject& err )
  {
    std::cout << "ExecptionObject caught : " << err << std::endl;
    exit(EXIT_FAILURE);
  }    
    
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  
  filter->SetOutsideValue( 0 );
  filter->SetInsideValue( 1 ); 
  
  filter->SetLowerThreshold( 1 );
  filter->SetUpperThreshold( 1000 );  
  
  filter->Update();
  
  output_name = ui->lineEdit_4->text();
  
  output_bin = output_dir + output_name + "_binary";
  if (ui->radioButton->isChecked()) output_bin = output_bin+".nrrd";
  else output_bin = output_bin+".gipl";
  
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( output_bin.toAscii().data() );
  writer->SetInput( filter->GetOutput() );
  writer->SetUseCompression( true );

  try 
  {
    writer->Update();
  }
  catch ( itk::ExceptionObject& err )
  {
    std::cout << "ExecptionObject caught : " << err << std::endl;
    exit(EXIT_FAILURE);
  }  
  
   
  return filter->GetOutput();
}


void MainWindow::Execute_Button()
{
    ui->textEdit->setText("Program is processing ... Please wait");
  
    /* --------------------------------------------------------	*/
    /* 			Lecture des deux images 		*/
    /* --------------------------------------------------------	*/
  
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( T1.toAscii().data());

    try 
    {
      reader->Update();
    }
    catch ( itk::ExceptionObject& err )
    {
      std::cout << "ExecptionObject caught : " << err << std::endl;
      exit(EXIT_FAILURE);
    } 
           
    ImageType::Pointer T1_pt = reader->GetOutput();
    
    ImageType::Pointer Binary_pt = LabelsToBinary();
    
    ui->progressBar->setValue(5.0);

    /* --------------------------------------------------------	*/
    /* 		     Creation d une nouvelle image 		*/
    /* --------------------------------------------------------	*/
    ImageType::Pointer image = ImageType::New();
    
    const ImageType::SizeType& size = T1_pt->GetLargestPossibleRegion().GetSize();   
    const ImageType::SpacingType& spacing = T1_pt->GetSpacing();
    const ImageType::PointType& origin = T1_pt->GetOrigin();  
   
    ImageType::IndexType start;
    start[0] = origin[0];
    start[1] = origin[1];    
    start[2] = origin[2];  
     
    ImageType::RegionType region;
    region.SetSize( size );
    region.SetIndex( start ); 
    
    image->SetRegions( region );
    image->SetSpacing( spacing );
    image->Allocate();
      
    ui->progressBar->setValue(10.0);
    
    /* --------------------------------------------------------	*/
    /* 			    Traitement 				*/
    /* --------------------------------------------------------	*/
    
   /* for (unsigned int x=0; x<size[0]; x += spacing[0])
    {
      for (unsigned int y=0; y<size[1]; y += spacing[1])
      {
	for (unsigned int z=0; z<size[2]; z += spacing[2])
	{
	  ImageType::IndexType pixInd;
	  pixInd[0] = x;
	  pixInd[1] = y;	  
	  pixInd[2] = z;	  
	  
	  ImageType::PixelType pixVal = labels->GetPixel(pixInd);
	    
	  if ( pixVal == 0 ) image->SetPixel( pixInd , T1->GetPixel(pixInd) ); 
	  else image->SetPixel( pixInd , 0 );
	  
	}
      }
    }*/
   
   ConstIteratorType Binary_It( Binary_pt , region );
   ConstIteratorType T1_It( T1_pt , region );   
   IteratorType im_It( image , region );

   long int nbr_it = size[0]*size[1]*size[2];
   long int cpt =(long int) nbr_it/90;
   long int inc = 1;

   
   
   for ( Binary_It.GoToBegin() ; !Binary_It.IsAtEnd() ; ++Binary_It, ++T1_It, ++im_It )
   {
     ImageType::PixelType pixVal = Binary_It.Get();
     
     if ( pixVal == 1 ) im_It.Set( T1_It.Get() ); 
     else im_It.Set( 0 );
     
     inc ++;
     if ( inc == cpt )
     {
       ui->progressBar->setValue( (double)(ui->progressBar->value()+1.0) ); 
       inc = 0;  
     }
     
    }

    /* --------------------------------------------------------	*/
    /* 	    Ecriture de la nouvelle image sur le disque		*/
    /* --------------------------------------------------------	*/
       
    output_name = output_dir + ui->lineEdit_4->text(); 
    if (ui->radioButton->isChecked()) output_name = output_name+".nrrd";
    else output_name = output_name+".gipl";

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( output_name.toAscii().data() );
    writer->SetInput( image );
    writer->SetUseCompression( true );

    try 
    {
      writer->Update();
    }
    catch ( itk::ExceptionObject& err )
    {
      std::cout << "ExecptionObject caught : " << err << std::endl;
      exit(EXIT_FAILURE);
    }  
    
    ui->progressBar->setValue( 100 );
    ui->textEdit->setText("Complete processing\n");
    QString text = "Images written on "+output_name+" and "+output_bin;
    ui->textEdit->append(text);
}
