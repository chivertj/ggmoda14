cmake_minimum_required(VERSION 2.6)
PROJECT( boost-proj )
FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIR} )
#SET(CMAKE_CXX_FLAGS "-DDEBUG -g -std=c++0x -O0")
SET(CMAKE_CXX_FLAGS "-DDEBUG -g -std=c++11 -O0")
SET(boost_SRCS main.cpp ../src/ACDefinitions.cpp ../src/motion/ggmatlearn.cpp ../src/motion/BootStrapProjector.cpp ../src/motion/MotClustNS.cpp ../src/region.cpp  ../src/shape/ShapeContour.cpp ../src/shape/SignedDistT.cpp ../src/shape/WJImgNS.cpp ../src/shape/PntMinDists.cpp ../src/shape/DistTSkel.cpp ../src/shape/Thinning.cpp ../src/bwlabel.cpp ../src/gguuid.cpp
)

SET(EXECNAME "moda14_gg-mdboost050")
ADD_EXECUTABLE( ${EXECNAME} ${boost_SRCS} )
TARGET_LINK_LIBRARIES( ${EXECNAME} ${OpenCV_LIBS})






