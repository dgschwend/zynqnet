#!/usr/bin/env python3

# need openBLAS
# need libraries: pip install pillow, numpy
import PIL.Image                    # pillow image library
import sys
import numpy as np
#import scipy.misc, scipy.ndimage

"""
Load an image from disk and write back as binary input data.

Arguments:
path -- path to an image on disk
width -- resize dimension
height -- resize dimension
"""

def convert_image(path, width, height, outfile):

    image = PIL.Image.open(path)
    image = image.convert('RGB')
    image = np.array(image)

    # Transform to desired size (half-crop, half-fill)
    # height_ratio = float(image.shape[0])/height
    # width_ratio = float(image.shape[1])/width
    # new_ratio = (width_ratio + height_ratio) / 2.0
    # resize_width = int(round(image.shape[1] / new_ratio))
    # resize_height = int(round(image.shape[0] / new_ratio))
    # if width_ratio > height_ratio and (height - resize_height) % 2 == 1:
    #     resize_height += 1
    # elif width_ratio < height_ratio and (width - resize_width) % 2 == 1:
    #     resize_width += 1
    # image = scipy.misc.imresize(image, (resize_height, resize_width), interp='bicubic')
    # if width_ratio > height_ratio:
    #     start = int(round((resize_width-width)/2.0))
    #     image = image[:,start:start+width]
    # else:
    #     start = int(round((resize_height-height)/2.0))
    #     image = image[start:start+height,:]
    #
    # # Fill ends of dimension that is too short with random noise
    # if width_ratio > height_ratio:
    #     padding = (height - resize_height)/2
    #     noise_size = (padding, width, 3)
    #     noise = np.random.randint(0, 255, noise_size).astype('uint8')
    #     image = np.concatenate((noise, image, noise), axis=0)
    # else:
    #     padding = (width - resize_width)/2
    #     noise_size = (height, padding, 3)
    #     noise = np.random.randint(0, 255, noise_size).astype('uint8')
    #     image = np.concatenate((noise, image, noise), axis=1)

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

    print("Saving input data in binary format")
    data = np.array(processed)

    print("  shape: %s" % data.shape)
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
    print("  write to file %s" % outfile)
    floatstruct = struct.pack('f'*len(pixels), *pixels)
    with open(outfile, "wb") as f:
        f.write(floatstruct)

if __name__ == "__main__":
    path = ""
    width = 256
    height = 256
    outfile = "image.bin"

    if len(sys.argv) == 2:
        path = sys.argv[1]
    elif len(sys.argv) == 4:
        path = sys.argv[1]
        width = int(sys.argv[2])
        height = int(sys.argv[3])
    elif len(sys.argv) == 5:
        path = sys.argv[1]
        width = int(sys.argv[2])
        height = int(sys.argv[3])
        outfile = sys.argv[4]
    else:
        print("usage: %s <inputfile> [width height [outputfile]]" % sys.argv[0])
        exit(-1)

    convert_image(path,width,height,outfile)
