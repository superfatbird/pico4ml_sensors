#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jan 30 14:03:53 2023

@author: dapeng
"""

import serial
imagesize = [96, 96]
ser = serial.Serial('/dev/ttyACM0', 115200)
ser.write(b'a')
imagedata = ser.read(imagesize[0]*imagesize[1])

ser.close()
print("Serial line closed")

from PIL import Image
# from PIL import ImageDraw
from PIL import ImageShow

img = Image.frombytes('L', (imagesize[0],imagesize[1]), imagedata)
ImageShow.show(img)
# draw = ImageDraw.Draw(img)
img.save("newimage11.png")
