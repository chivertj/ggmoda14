cmake_minimum_required(VERSION 2.6)
PROJECT( lbptrack )
FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIR} )
SET(CMAKE_CXX_FLAGS "-DDEBUG -g -std=c++11 -O0")
SET(lbptrack_SRCS main.cpp 
  ../mainhelperfuncs.cpp ../src/bwlabel.cpp ../src/connected.cc ../src/region.cpp ../src/ACDefinitions.cpp ../src/gglowpass.cpp ../src/gglphisto.cpp ../src/gglpanalysis.cpp ../src/gghelpers.cpp ../src/csvfiling.cpp ../src/ggheadtracker.cpp ../src/ggtracker.cpp ../src/ggtimewindow.cpp ../src/ggregcompare.cpp ../src/ggsimcorr.cpp ../src/ggutils.cpp ../src/ggmath.cpp ../src/ggregionprops.cpp ../src/ggregiontrack.cpp ../src/ggregiontracker.cpp ../src/ggpropagating.cpp ../src/ggtracks.cpp ../src/ggframe.cpp ../src/ggclassify.cpp ../src/ggregionhierarchy.cpp ../src/ggclassification.cpp ../src/motion/BootStrapProjector.cpp ../src/motion/ClusterPnts.cpp ../src/motion/ggmatlearn.cpp ../src/motion/FeatPnts.cpp ../src/motion/mdBackProjector.cpp ../src/motion/mdMotionCluster.cpp ../src/motion/mdRasterVoronoi.cpp ../src/motion/mdShapeCluster.cpp ../src/motion/mdSpatialCluster.cpp ../src/motion/mkutilsopencv.cpp ../src/motion/MotClustNS.cpp ../src/motion/ggmotionsegment.cpp ../src/ggbgfgsegmenters.cpp ../src/ggexceptions.cpp ../src/gguuid.cpp ../src/ggexporters.cpp ../src/shape/ggshapeextractor.cpp ../src/shape/ggshapetrackproperties.cpp ../src/shape/ggshapetrackfilter.cpp ../src/pca/frbdp.cpp ../src/pca/ggpca.cpp ../src/pca/is2fsrob.cpp ../src/pca/utility.cpp ../src/pca/xjlinalg.cpp ../src/ggidxatt.cpp ../src/shape/ggregconsist.cpp
  ./src/lbphelpers.cpp 
)
SET(EXECNAME "moda14_gg-lbptrack60")
ADD_EXECUTABLE( ${EXECNAME} ${lbptrack_SRCS} )
TARGET_LINK_LIBRARIES( ${EXECNAME} ${OpenCV_LIBS})

#ADD_CUSTOM_COMMAND( TARGET $



