CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Configuration du projet
PROJECT( skull_qt )


FIND_PACKAGE(ITK)
IF(ITK_FOUND) 
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND) 
  MESSAGE(FATAL_ERROR
	  "ITK not found")
ENDIF(ITK_FOUND)


FIND_PACKAGE(Qt4 REQUIRED)
IF( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
ELSE(QT4_FOUND) 
  MESSAGE(FATAL_ERROR
	  "QT4 not found")
ENDIF(QT4_FOUND)

ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

SET( skull_qt_sources main.cpp mainwindow.cpp )
SET( skull_qt_headers mainwindow.h )
SET( skull_qt_forms mainwindow.ui )

QT4_WRAP_UI(skull_qt_forms_headers ${skull_qt_forms})	
QT4_WRAP_CPP(skull_qt_headers_moc ${skull_qt_headers})


ADD_EXECUTABLE( skull_qt ${skull_qt_sources} ${skull_qt_headers_moc} ${skull_qt_forms_headers} )

TARGET_LINK_LIBRARIES(skull_qt ITKCommon ITKIO ${QT_LIBRARIES})

