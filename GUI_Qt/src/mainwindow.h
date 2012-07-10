#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkBinaryThresholdImageFilter.h"


typedef short PixelType;
const unsigned int Dimension = 3;

typedef itk::Image< PixelType,Dimension > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;
typedef itk::ImageFileWriter< ImageType > WriterType;

typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< ImageType > IteratorType;

typedef itk::BinaryThresholdImageFilter< ImageType,ImageType > FilterType;


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QString output_name;
    QString output_bin;
    QString output_dir;
    QString T1;
    QString Labels;
    bool T1_clicked;
    bool Labels_clicked;
    bool Output_clicked;
      

private slots:
    void Close_Button();
    void Browse_T1();
    void Browse_Labels();
    void Browse_Output();
    void Execute_Button();
    void Reset_Button();
    ImageType::Pointer LabelsToBinary();
};

#endif // MAINWINDOW_H
