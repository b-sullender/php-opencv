/*
 +----------------------------------------------------------------------+
 | PHP-OpenCV                                                           |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | hihozhou@gmail.com so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
 | Author: HaiHao Zhou  <hihozhou@gmail.com>                            |
 +----------------------------------------------------------------------+
 */


#include "../../php_opencv.h"
#include "opencv_imgcodecs.h"
#include "core/opencv_mat.h"
#include "../../opencv_exception.h"



/**
 * CV\imread
 * @param execute_data
 * @param return_value
 */
PHP_FUNCTION(opencv_imread)
{
    long flags;
    char *filename;
    long filename_len;
    flags = IMREAD_COLOR;//flags default value

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &filename,&filename_len, &flags) == FAILURE) {
        RETURN_NULL();
    }
    zval instance;
    object_init_ex(&instance,opencv_mat_ce);
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(&instance);

    Mat im = imread(filename, (int)flags);
    if(im.empty() || !im.data){//check file exist
        char *error_message = (char*)malloc(strlen("Can not load image : ") + strlen(filename) + 1);
        strcpy(error_message,"Can not load image : ");
        strcat(error_message,filename);
        opencv_throw_exception(error_message);//throw exception
        free(error_message);
    }

    obj->mat = new Mat(im);

    //update php Mat object property
    opencv_mat_update_property_by_c_mat(&instance, obj->mat);

    //todo object_init_ex() memory leaks detected on RETURN_ZVAL(instance,1,0)
    RETURN_ZVAL(&instance,0,0); //return php Mat object
}

/**
 * CV\imwrite
 * @param execute_data
 * @param return_value
 */
PHP_FUNCTION(opencv_imwrite){
    char *filename;
    size_t filename_len;
    zval *object;
    zval *flags_array = nullptr;

    // Parse parameters: string, Mat object, optional array
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sO|a",
                              &filename, &filename_len,
                              &object, opencv_mat_ce,
                              &flags_array) == FAILURE) {
        RETURN_NULL();
    }

    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(object);

    // Convert PHP array to std::vector<int> in key/value pairs
    std::vector<int> params;
    if (flags_array) {
        HashTable *ht = Z_ARRVAL_P(flags_array);
        zend_string *key_str;
        zend_ulong key_index;
        zval *val;

        ZEND_HASH_FOREACH_KEY_VAL(ht, key_index, key_str, val) {
            int k = 0, v = 0;

            // Determine key
            if (key_str) {
                // If key is string, ignore or throw (optional)
                continue;
            } else {
                k = static_cast<int>(key_index); // numeric key (constant value)
            }

            // Convert value to long
            convert_to_long(val);
            v = Z_LVAL_P(val);

            // Push key and value as consecutive elements
            params.push_back(k);
            params.push_back(v);
        } ZEND_HASH_FOREACH_END();
    }

    // Call OpenCV imwrite
    try {
        imwrite(filename, *obj->mat, params);
    } catch (const cv::Exception &e) {
        opencv_throw_exception(e.what());
    }

    RETURN_TRUE;
}

/**
 * CV\imdecode
 * @param execute_data
 * @param return_value
 */
PHP_FUNCTION(opencv_imdecode)
{
    long flags;
    char *buf;
    long buf_len;
    flags = IMREAD_COLOR;//flags default value

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &buf,&buf_len, &flags) == FAILURE) {
        RETURN_NULL();
    }
    zval instance;
    object_init_ex(&instance,opencv_mat_ce);
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(&instance);

    Mat im = imdecode(Mat(1, buf_len, CV_8UC1, buf), (int)flags);
    if(im.empty() || !im.data){
        char *error_message = (char*)malloc(strlen("Can not load image") + 1);
        strcpy(error_message,"Can not load image");
        opencv_throw_exception(error_message);//throw exception
        free(error_message);
    }

    obj->mat = new Mat(im);

    //update php Mat object property
    opencv_mat_update_property_by_c_mat(&instance, obj->mat);

    RETURN_ZVAL(&instance,0,0); //return php Mat object
}

void opencv_imgcodecs_init(int module_number)
{
    /**
     * imread const flags
     */
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_UNCHANGED", IMREAD_UNCHANGED, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_GRAYSCALE", IMREAD_GRAYSCALE, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_COLOR", IMREAD_COLOR, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_ANYDEPTH", IMREAD_ANYDEPTH, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_ANYCOLOR", IMREAD_ANYCOLOR, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_LOAD_GDAL", IMREAD_LOAD_GDAL, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_REDUCED_GRAYSCALE_2", IMREAD_REDUCED_GRAYSCALE_2, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_REDUCED_COLOR_2", IMREAD_REDUCED_COLOR_2, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_REDUCED_GRAYSCALE_4", IMREAD_REDUCED_GRAYSCALE_4, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_REDUCED_COLOR_4", IMREAD_REDUCED_COLOR_4, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_REDUCED_GRAYSCALE_8", IMREAD_REDUCED_GRAYSCALE_8, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_REDUCED_COLOR_8", IMREAD_REDUCED_COLOR_8, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMREAD_IGNORE_ORIENTATION", IMREAD_IGNORE_ORIENTATION, CONST_CS | CONST_PERSISTENT);

    /**
     * imwrite const flags
     */
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_JPEG_QUALITY", IMWRITE_JPEG_QUALITY, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_JPEG_PROGRESSIVE", IMWRITE_JPEG_PROGRESSIVE, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_JPEG_OPTIMIZE", IMWRITE_JPEG_OPTIMIZE, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_JPEG_RST_INTERVAL", IMWRITE_JPEG_RST_INTERVAL, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_JPEG_LUMA_QUALITY", IMWRITE_JPEG_LUMA_QUALITY, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_JPEG_CHROMA_QUALITY", IMWRITE_JPEG_CHROMA_QUALITY, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_PNG_COMPRESSION", IMWRITE_PNG_COMPRESSION, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_PNG_STRATEGY", IMWRITE_PNG_STRATEGY, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_PNG_BILEVEL", IMWRITE_PNG_BILEVEL, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_PXM_BINARY", IMWRITE_PXM_BINARY, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_EXR_TYPE", IMWRITE_EXR_TYPE, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_WEBP_QUALITY", IMWRITE_WEBP_QUALITY, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_PAM_TUPLETYPE", IMWRITE_PAM_TUPLETYPE, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_TIFF_RESUNIT", IMWRITE_TIFF_RESUNIT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_TIFF_XDPI", IMWRITE_TIFF_XDPI, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_TIFF_YDPI", IMWRITE_TIFF_YDPI, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT(OPENCV_NS, "IMWRITE_TIFF_COMPRESSION", IMWRITE_TIFF_COMPRESSION, CONST_CS | CONST_PERSISTENT);
}
