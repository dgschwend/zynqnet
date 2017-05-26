
#####
##
## Model Conversion Run Script
##
## To convert a model:
##   mkdir my_model
##   cp model_download_from_digits/deploy.prototxt my_model
##   cp model_download_from_digits/snap*.caffemodel my_model
##   cp run.sh my_model
##   cd my_model && ./run.sh
##   nano network.hpp -> adjust TEST_RESULT_EXPECTED
##
####


#### REQUIRES FULL CAFFE + PYTHON2.7 INSTALLATION