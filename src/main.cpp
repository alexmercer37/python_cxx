#include <python3.10/Python.h>
#include "numpy/arrayobject.h"
#include <stdlib.h>
#include <iostream>
#include "../inc/camera.h"
#include <pthread.h>
using namespace std;
cv::Mat *rgb_ptr, rgb_frame;

k4a::capture capture;
k4a::transformation k4aTransformation;
k4a::calibration k4aCalibration;

pthread_mutex_t protect_picture;
pthread_mutex_t buff_mutex;

std::shared_ptr<cv::Mat> matBuff;
std::shared_ptr<cv::Mat> Buff;

camera *camera_ptr;
void *init_kinect1(void *camera_ptr)
{
    camera *Camera = (camera *)camera_ptr;

    while (1)
    {
        pthread_mutex_lock(&protect_picture);

        Camera->picture_update(capture);

        rgb_ptr = Camera->getpicture(capture, rgb_frame, k4aTransformation);
        cv::cvtColor(*rgb_ptr, *rgb_ptr, cv::COLOR_RGB2BGR);

        pthread_mutex_lock(&buff_mutex);
        matBuff = std::make_shared<cv::Mat>(rgb_ptr->clone());
        pthread_mutex_unlock(&buff_mutex);

        pthread_mutex_unlock(&protect_picture);

        usleep(1000);

        if (matBuff->empty())
        {
            cout << "error" << endl;
        }

        rgb_ptr->release();
    }
    pthread_exit(NULL);
}

void *k4aUpdate(void *argc)
{
    cv::VideoCapture cap(0);
    cv::Mat frame;
    while (1)
    {
        pthread_mutex_lock(&buff_mutex);

        bool status = cap.read(frame);

        Buff = std::make_shared<cv::Mat>(frame.clone());

        pthread_mutex_unlock(&buff_mutex);
        // cv::imshow("test", *Buff);
        // cv::waitKey(1);

        frame.release();
    }
}
void *detect_python(void *argc)
{
    Py_Initialize();
    _import_array();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(\'/home/dxy/Downloads/yolov5-6.0_two_labels\')");
    PyRun_SimpleString("sys.path.append(\'.\')");
    PyRun_SimpleString("import cv2");
    PyRun_SimpleString("import numpy");
    PyRun_SimpleString("import torch");
    PyRun_SimpleString("import time");
    PyRun_SimpleString("import serial");

    PyObject *m_PyModule = PyImport_ImportModule("detect_ball");
    PyObject *m_PyDict = PyModule_GetDict(m_PyModule);
    PyObject *load_model = PyDict_GetItemString(m_PyDict, "load_model");
    PyObject *model = PyObject_CallObject(load_model, NULL);

    while (1)
    {
        if (load_model != NULL)
        {
            pthread_mutex_lock(&protect_picture);
            int r = (*Buff).rows;
            int c = (*Buff).cols;
            int chnl = (*Buff).channels();
            int nElem = r * c * chnl;

            uchar *m = new uchar[nElem];

            std::memcpy(m, (*Buff).data, nElem * sizeof(uchar));

            npy_intp mdim[] = {r, c, chnl};

            PyObject *mat = (PyObject *)PyArray_SimpleNewFromData(chnl, mdim, NPY_UINT8, (void *)m);
            pthread_mutex_unlock(&protect_picture);

            PyObject *detect_fun = PyDict_GetItemString(m_PyDict, "detect"); // 检测函数
            PyObject *detect_args = PyTuple_Pack(2, model, mat);             // 传入的参数
            PyObject *result = PyObject_CallObject(detect_fun, detect_args);

            // if (PyList_Check(result))
            // {
            //     float SizeOfList = PyList_Size(result);
            //     for (int i = 0; i < SizeOfList; i++)
            //     {
            //         PyObject *ListItem = PyList_GetItem(result, i);
            //         cout << ListItem << endl;
            //         Py_DECREF(ListItem);
            //     }
            // }

            Py_XDECREF(mat);
            Py_XDECREF(result);
            Py_XDECREF(detect_args);

            delete[] m;
        }
    }
}
void *detect_python2(void *argc)
{
    Py_Initialize();
    _import_array();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(\'/home/dxy/Downloads/yolov5-6.0_two_labels\')");
    PyRun_SimpleString("sys.path.append(\'.\')");
    PyRun_SimpleString("import cv2");
    PyRun_SimpleString("import numpy");
    PyRun_SimpleString("import torch");
    PyRun_SimpleString("import time");
    PyRun_SimpleString("import serial");

    PyObject *m_PyModule = PyImport_ImportModule("Detect");
    PyObject *m_PyDict = PyModule_GetDict(m_PyModule);
    PyObject *load_model = PyDict_GetItemString(m_PyDict, "load_model");
    PyObject *model = PyObject_CallObject(load_model, NULL);
}

int main()
{

    camera_ptr = new camera;

    pthread_mutex_init(&protect_picture, nullptr);
    pthread_mutex_init(&buff_mutex, nullptr);

    matBuff.reset(new cv::Mat);
    Buff.reset(new cv::Mat);
    camera_ptr->init_kinect(capture, k4aTransformation, k4aCalibration);
    pthread_t tid[4];
    // pthread_create(&tid[0], NULL, k4aUpdate, (void *)camera_ptr);
    pthread_create(&tid[0], NULL, init_kinect1, (void *)camera_ptr);
    pthread_create(&tid[1], NULL, detect_python, NULL);
    // sleep(10);
    // pthread_create(&tid[2], NULL, detect_python2, NULL);

    while (1)
        ;

    Py_Finalize();
    return 0;
}
