////////////////////////////////////////////////////////////////////////
//Copyright (c) 2007-2015, John Chiverton 
//
//Permission to use, copy, modify, and/or distribute this software for any
//purpose with or without fee is hereby granted, provided that the above
//copyright notice and this permission notice appear in all copies.
//
//THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
//WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
//MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
//ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
//WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
//ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
//OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
////////////////////////////////////////////////////////////////////////
#include "opencv/cv.h"
#include "opencv/cxmisc.h"
//#include "opencv/cxeigen.hpp"
#include "opencv/cvaux.h"
#include "opencv/cxcore.hpp"
#include "opencv/cxcore.h"
#include "opencv/cvwimage.h"
#include "opencv/cv.hpp"
#include "opencv/ml.h"
#include "opencv/highgui.h"
#include "opencv/cvaux.hpp"


#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/contrib/retina.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ts/ts_gtest.h"
#include "opencv2/ts/ts.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#ifdef GPU_INCLUDE
#include "opencv2/gpu/matrix_operations.hpp"
#include "opencv2/gpu/gpumat.hpp"
#include "opencv2/gpu/stream_accessor.hpp"
#include "opencv2/gpu/NCV.hpp"
#include "opencv2/gpu/NPP_staging.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/gpu/devmem2d.hpp"
#endif //GPU_INCLUDE

#include "opencv2/core/operations.hpp"
//#include "opencv2/core/eigen.hpp"
#include "opencv2/core/types_c.h"
#include "opencv2/core/internal.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/core/wimage.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/version.hpp"
#include "opencv2/core/core_c.h"

#ifdef LEGACY_INCLUDE
#include "opencv2/legacy/compat.hpp"
#include "opencv2/legacy/streams.hpp"
#include "opencv2/legacy/blobtrack.hpp"
#include "opencv2/legacy/legacy.hpp"
#endif //LEGACY_INCLUDE

#ifdef FLANN_INCLUDE
#include "opencv2/flann/autotuned_index.h"
#include "opencv2/flann/lsh_index.h"
#include "opencv2/flann/simplex_downhill.h"
#include "opencv2/flann/all_indices.h"
#include "opencv2/flann/index_testing.h"
#include "opencv2/flann/config.h"
#include "opencv2/flann/lsh_table.h"
#include "opencv2/flann/result_set.h"
#include "opencv2/flann/hdf5.h"
#include "opencv2/flann/dummy.h"
#include "opencv2/flann/flann.hpp"
#include "opencv2/flann/any.h"
#include "opencv2/flann/general.h"
#include "opencv2/flann/kdtree_index.h"
#include "opencv2/flann/hierarchical_clustering_index.h"
#include "opencv2/flann/dist.h"
#include "opencv2/flann/matrix.h"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/flann/nn_index.h"
#include "opencv2/flann/flann_base.hpp"
#include "opencv2/flann/allocator.h"
#include "opencv2/flann/object_factory.h"
#include "opencv2/flann/sampling.h"
#include "opencv2/flann/params.h"
#include "opencv2/flann/timer.h"
#include "opencv2/flann/linear_index.h"
#include "opencv2/flann/kdtree_single_index.h"
#include "opencv2/flann/composite_index.h"
#include "opencv2/flann/logger.h"
#include "opencv2/flann/defines.h"
#include "opencv2/flann/dynamic_bitset.h"
#include "opencv2/flann/saving.h"
#include "opencv2/flann/ground_truth.h"
#include "opencv2/flann/heap.h"
#include "opencv2/flann/kmeans_index.h"
#include "opencv2/flann/random.h"
#endif //FLANN_INCLUDE
