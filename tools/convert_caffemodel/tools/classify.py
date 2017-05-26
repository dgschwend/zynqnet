#!/usr/bin/env python2
# Copyright (c) 2015-2016, NVIDIA CORPORATION.  All rights reserved.

"""
Classify an image using individual model files

Use this script as an example to build your own tool
"""

import argparse
import os
import time
import struct

from google.protobuf import text_format
import numpy as np
import PIL.Image
import scipy.misc, scipy.ndimage

os.environ['GLOG_minloglevel'] = '2' # Suppress most caffe output
import caffe
from caffe.proto import caffe_pb2

def get_net(caffemodel, deploy_file, use_gpu=True):
    """
    Returns an instance of caffe.Net

    Arguments:
    caffemodel -- path to a .caffemodel file
    deploy_file -- path to a .prototxt file

    Keyword arguments:
    use_gpu -- if True, use the GPU for inference
    """
    if use_gpu:
        caffe.set_mode_gpu()

    # load a new model
    return caffe.Net(deploy_file, caffemodel, caffe.TEST)

# def get_transformer(deploy_file, mean_file=None):
#     """
#     Returns an instance of caffe.io.Transformer
#
#     Arguments:
#     deploy_file -- path to a .prototxt file
#
#     Keyword arguments:
#     mean_file -- path to a .binaryproto file (optional)
#     """
#     network = caffe_pb2.NetParameter()
#     with open(deploy_file) as infile:
#         text_format.Merge(infile.read(), network)
#
#     if network.input_shape:
#         dims = network.input_shape[0].dim
#     else:
#         dims = network.input_dim[:4]
#
#     t = caffe.io.Transformer(
#             inputs = {'data': dims}
#             )
#     #t.set_transpose('data', (2,0,1)) # transpose to (channels, height, width)
#
#     # color images
#     if dims[1] == 3:
#         # channel swap
#         t.set_channel_swap('data', (2,1,0))
#         pass
#
#     # MEAN SUBTRACT
#     #t.set_mean('data', np.array([104, 117, 123]))
#
#     return t

def load_image(path, height, width, mode='RGB'):
    """
    Load an image from disk

    Returns an np.ndarray (channels x width x height)

    Arguments:
    path -- path to an image on disk
    width -- resize dimension
    height -- resize dimension

    Keyword arguments:
    mode -- the PIL mode that the image should be converted to
        (RGB for color or L for grayscale)
    """
    image = PIL.Image.open(path)
    image = image.convert(mode)
    image = np.array(image)
    # half-crop, half-fill
    height_ratio = float(image.shape[0])/height
    width_ratio = float(image.shape[1])/width
    new_ratio = (width_ratio + height_ratio) / 2.0
    resize_width = int(round(image.shape[1] / new_ratio))
    resize_height = int(round(image.shape[0] / new_ratio))
    if width_ratio > height_ratio and (height - resize_height) % 2 == 1:
        resize_height += 1
    elif width_ratio < height_ratio and (width - resize_width) % 2 == 1:
        resize_width += 1
    image = scipy.misc.imresize(image, (resize_height, resize_width), interp='bicubic')
    if width_ratio > height_ratio:
        start = int(round((resize_width-width)/2.0))
        image = image[:,start:start+width]
    else:
        start = int(round((resize_height-height)/2.0))
        image = image[start:start+height,:]
    
    # fill ends of dimension that is too short with random noise
    if width_ratio > height_ratio:
        padding = (height - resize_height)/2
        noise_size = (padding, width, 3)
        noise = np.random.randint(0, 255, noise_size).astype('uint8')
        image = np.concatenate((noise, image, noise), axis=0)
    else:
        padding = (width - resize_width)/2
        noise_size = (height, padding, 3)
        noise = np.random.randint(0, 255, noise_size).astype('uint8')
        image = np.concatenate((noise, image, noise), axis=1)
    
    processed = np.zeros((3, width, height), np.float32)
    
    # Transpose from (height, width, channels) to (channels, height, width)
    #processed = processed.transpose((2,0,1))
    
    # Channel Swap: RGB -> BGR
    #image = image[(2,1,0),:,:]
    
    # Subtract Mean, Swap Channels RGB -> BGR, Transpose (H,W,CH) to (CH,H,W)
    #mean_rgb = [104,117,123]
    processed[0,:,:] = (image[:,:,2]-104.0)
    processed[1,:,:] = (image[:,:,1]-117.0)
    processed[2,:,:] = (image[:,:,0]-123.0)
        
    return processed

def forward_pass(image, net, batch_size=None):
    """
    Returns scores for each image as an np.ndarray (nImages x nClasses)

    Arguments:
    images -- a list of np.ndarrays
    net -- a caffe.Net
    transformer -- a caffe.io.Transformer

    Keyword arguments:
    batch_size -- how many images can be processed at once
        (a high value may result in out-of-memory errors)
    """

    net.blobs['data'].data[0] = image
    
    print "net.outputs[-1] = ", net.outputs[-1]
    start = time.time()
    net.forward()
    output = net.blobs[net.outputs[-1]].data
    #pool10avg = (net.blobs['pool10'].data).flatten()
    #pool10 = pool10avg
    #print "conv10 output:\n", net.blobs['conv10'].data
    #print "pool10 output:", pool10avg
    scores = np.copy(output)
    end = time.time()
    print 'Inference took %f seconds ...' % (end - start)

    return scores

def read_labels(labels_file):
    """
    Returns a list of strings

    Arguments:
    labels_file -- path to a .txt file
    """
    if not labels_file:
        return None

    labels = []
    with open(labels_file) as infile:
        for line in infile:
            label = line.strip()
            if label:
                labels.append(label)
    assert len(labels), 'No labels found'
    return labels

def classify(caffemodel, deploy_file, image_file,
        mean_file=None, labels_file=None, batch_size=None, use_gpu=True):
    """
    Classify some images against a Caffe model and print the results

    Arguments:
    caffemodel -- path to a .caffemodel
    deploy_file -- path to a .prototxt
    image_files -- list of paths to images

    Keyword arguments:
    mean_file -- path to a .binaryproto
    labels_file path to a .txt file
    use_gpu -- if True, run inference on the GPU
    """
    # Load the model and images
    net = get_net(caffemodel, deploy_file, use_gpu)
    _, channels, height, width = np.array(net.blobs['data'].shape)
    mode = 'RGB'
    image = load_image(image_file, height, width, mode)
    labels = read_labels(labels_file)
    
    # Structured Input as Image
    
    #W = image.shape[2]
    #H = image.shape[1]
    #CH = image.shape[0]
    #for y in range(H):
    # for x in range(W):
    #  for c in range(CH):
    #      image[c,x,y] = 1000.0*y+x+c/1000.0#1;
    #            
    # Fixed Parameters for first Filter Bank
    
    # conv1param = np.array(net.params['conv1'][0].data)[:,:,:,:]
    # print "shape of conv1param: ", conv1param.shape
    # co = conv1param.shape[0]
    # ci = conv1param.shape[1]
    # kx = conv1param.shape[2]
    # ky = conv1param.shape[3]
    # pixels = []
    # for i in range(ci):
    #  for o in range(co):
    #   if i == 0:
    #       conv1param[o,i] = np.array([[0,0,0],[0,1,0],[0,0,0]])
    #   else:
    #       conv1param[o,i] = np.zeros((3,3)) #conv1param[o,i] = np.array([[1,0,0],[0,1,0],[0,1,0]])
    #net.params['conv1'][0].data[...] = conv1param
    #net.params['conv1'][1].data[...] = np.zeros(net.params['conv1'][1].shape)
    
    # Classify the image
    scores = forward_pass(image, net, batch_size=1)
    
    # Fish out some blobs...
    
    try: os.mkdir("blobs")
    except: pass;
            
    def save_blob(name):
        print "Saving result for layer", name
        data = np.array(net.blobs[name].data)[0,:,:,:]
        print "  shape:", data.shape
        CH = data.shape[0]
        W = data.shape[1]
        H = data.shape[2]
        pixels = []
        for y in range(H):
            for x in range(W):
                for c in range(CH):
                    pixel = data[c,x,y]
                    if pixel is None: pixel = 99999
                    pixels.append(pixel);
        # Write Pixels to binary file
        filename = "blobs/result_%s.bin" % (name.replace("/", "_"))
        print "  write to file", filename
        floatstruct = struct.pack('f'*len(pixels), *pixels)
        with open(filename, "wb") as f:
            f.write(floatstruct)

    def save_params(name):
        print "Saving params for layer", name
        param = np.array(net.params[name][0].data)[:,:,:,:]
        print "  shape : ", param.shape
        co = param.shape[0]
        ci = param.shape[1]
        kx = param.shape[2]
        ky = param.shape[3]
        weights = []
        for i in range(ci):
         for o in range(co):
             for y in range(ky):
                 for x in range(kx):
                     weights.append(param[o,i,x,y]);
                
        # Write Pixels to binary file
        filename = "blobs/weights_%s.bin" % (name.replace("/", "_"))
        print "  write to file", filename
        floatstruct = struct.pack('f'*len(weights), *weights)
        with open(filename, "wb") as f:
            f.write(floatstruct)
   
    for blob in net.blobs.keys():
        try: save_blob(blob)
        except: print "could not save blob", blob
        
    for params in net.params.keys():
        try: save_params(params)
        except: print "could not save params", params
   
    print "output shape: ", scores.shape
        
    ### Process the results

    indices = np.argsort(-scores.flatten())[:5] # take top 5 results
    result = []
    for i in indices:
        # 'i' is a category in labels and also an index into scores
        if labels is None:
            label = 'Class #%s' % i
        else:
            label = labels[i]
        result.append((label, round(100.0*scores[0, i],4)))

    print '{:-^80}'.format(' Prediction for %s ' % image_file)
    for label, confidence in result:
        print '{:9.4%} - "{}"'.format(confidence/100.0, label)
    print


if __name__ == '__main__':
    script_start_time = time.time()

    parser = argparse.ArgumentParser(description='Classification example - DIGITS')

    ### Positional arguments

    parser.add_argument('caffemodel',   help='Path to a .caffemodel')
    parser.add_argument('deploy_file',  help='Path to the deploy file')
    parser.add_argument('image_file',
                        help='Path to an image')

    ### Optional arguments

    parser.add_argument('-m', '--mean',
            help='Path to a mean file (*.npy)')
    parser.add_argument('-l', '--labels',
            help='Path to a labels file')
    parser.add_argument('--batch-size',
                        type=int)
    parser.add_argument('--nogpu',
            action='store_true',
            help="Don't use the GPU")

    args = vars(parser.parse_args())

    classify(args['caffemodel'], args['deploy_file'], args['image_file'],
            args['mean'], args['labels'], args['batch_size'], not args['nogpu'])

    print 'Script took %f seconds.' % (time.time() - script_start_time,)

