from cv2 import *
import cv2
import serial
import time
import random
from fastai.vision.all import *
from fastai.vision.data import *
from fastai.metrics import error_rate
from pathlib import Path
from glob2 import glob
from sklearn.metrics import confusion_matrix

import random
import pandas as pd
import numpy as np
import os
import zipfile as zf
import shutil
import re
import seaborn as sns

def predictTrash():
    data = ['cardboard', 'glass', 'metal', 'paper', 'plastic', 'trash']
    test = load_learner("C:\\Users\\cwihe\\Documents\\Waste Sorter Project\\data\\waste_sorter.pkl")
    test_dl1 = test.dls.test_dl(test_items="C:/Users/cwihe/Documents/Waste Sorter Project/Trash.jpg", after_item = [ToTensor(), Resize((200, 150), method='squish')])
    preds1 = test.get_preds(dl = test_dl1)
    print(preds1[0])
    max_idxs = np.asarray(np.argmax(preds1[0],axis=1))
    x = data[max_idxs[0]]
    print(x)
    if x == "trash":
        return "1"
    elif x == "plastic":
        return "2"
    elif x == "paper" or x == "cardboard":
        return "3"
    else:
        return "4"

def takeImage():
    cam_port = 0
    cam = cv2.VideoCapture(cam_port, cv2.CAP_DSHOW)

    # reading the input using the camera
    result, image = cam.read()

    # If image will detected without any error, 
    # show result
    if result:
        # showing result, it take frame name and image 
        # output
        #cv2.imshow("Trash", image)

        # saving image in local storage
        cv2.imwrite("Trash.jpg", image)

        # If keyboard interrupt occurs, destroy image 
        # window
        #cv2.waitKey(0)
        #cv2.destroyWindow("Trash")

    # If captured image is corrupted, moving to else part
    else:
        print("No image detected. Please! try again")

def write_read(x):
    a.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    data = y
    return data

if __name__ == "__main__":
    a = serial.Serial(port="COM3", baudrate = 115200, timeout=.1)
    
    while True:
        y = a.readline()
        #print(y)
        #num = input("Enter a number: ")
        #y = ''
        #if(num == 'q'):
        #    a.close()
        #    break
        j = y.decode("utf-8")
        if(j == "test"):
            print()
            print("Taking Picture...")
            takeImage()
            s = predictTrash()
            value = write_read(s)
            print("sending num:", s)
    

    

