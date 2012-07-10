#include <stdio.h>
#include <iostream>
#include <stdlib.h>
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


ImageType::Pointer LabelsToBinary(char* s1, char* s2)
{
  ReaderType::Pointer reader = ReaderType::New(); 
  reader->SetFileName(s1);
  
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
   
  char *p = (char*)malloc((strlen(s2)+13)*sizeof(char));
  strcpy(p,s2);
  strcat(p,"_binary.nrrd");
  
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( p );
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
  
  free(p);
  return filter->GetOutput();
}




int main (int argc, char *argv[])
{
 
  if (argc<4) std::cout<<"Not enough arguments"<<std::endl;
  else
  { 
    /* --------------------------------------------------------	*/
    /* 			Lecture des deux images 		*/
    /* --------------------------------------------------------	*/
    
    ReaderType::Pointer reader = ReaderType::New(); 
    reader->SetFileName(argv[1]);

    try 
    {
      reader->Update();
    }
    catch ( itk::ExceptionObject& err )
    {
      std::cout << "ExecptionObject caught : " << err << std::endl;
      return EXIT_FAILURE;
    }
       
    ImageType::Pointer T1 = reader->GetOutput();
    ImageType::Pointer labels = LabelsToBinary(argv[2],argv[3]);
    
    
    /* --------------------------------------------------------	*/
    /* 		     Creation d une nouvelle image 		*/
    /* --------------------------------------------------------	*/
    ImageType::Pointer image = ImageType::New();
    
    const ImageType::SizeType& size = T1->GetLargestPossibleRegion().GetSize();   
    const ImageType::SpacingType& spacing = T1->GetSpacing();
    const ImageType::PointType& origin = T1->GetOrigin();  
    
    
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
   
   ConstIteratorType labels_It( labels , region );
   ConstIteratorType T1_It( T1 , region );   
   IteratorType im_It( image , region );

   
   for ( labels_It.GoToBegin() ; !labels_It.IsAtEnd() ; ++labels_It, ++T1_It, ++im_It )
   {
     ImageType::PixelType pixVal = labels_It.Get();
      
     if ( pixVal != 0 ) im_It.Set( T1_It.Get() ); 
     else im_It.Set( 0 );

   }
   

    /* --------------------------------------------------------	*/
    /* 	    Ecriture de la nouvelle image sur le disque		*/
    /* --------------------------------------------------------	*/
    
    char *p = (char*)malloc((strlen(argv[3])+6)*sizeof(char));
    strcpy(p,argv[3]);
    strcat(p,".nrrd"); 
    
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( p );
    writer->SetInput( image );
    writer->SetUseCompression( true );
    
    
    try 
    {
      writer->Update();
    }
    catch ( itk::ExceptionObject& err )
    {
      std::cout << "ExecptionObject caught : " << err << std::endl;
      return EXIT_FAILURE;
    }  


    free(p);
   }
  
  return 0;
}