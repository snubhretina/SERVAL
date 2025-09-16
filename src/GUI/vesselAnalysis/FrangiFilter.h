#pragma once
#include "stdafx.h"
//#include <stdio.h>
//#include <vector>
//#include "opencv2/opencv.hpp"

#include <fstream>
#include "frangi/frangi.h"


class cFrangiFilter
{
public:
	cFrangiFilter();
	~cFrangiFilter();
public:
	cv::Mat MakeMultiScaleHessianEigenvalues(cv::Mat img, std::vector<double> &sigmas, std::vector<cv::Mat> &hevfmat_vec);
	template <typename T> cv::Mat GetCentralPartialDerivative2DcvMat_X(cv::Mat in);
	template <typename T> cv::Mat GetCentralPartialDerivative2DcvMat_Y(cv::Mat in);
	cv::Mat makeFrangiFilter(cv::Mat image, std::vector<double> sigmas, std::vector<cv::Mat> result);
	void ComputeMinMaxHevFeat(std::vector<cv::Mat> &hfeat, std::vector<double> &_min_fss, std::vector<double> &_max_fss);

	cv::Mat frangi(cv::Mat input, cv::Mat *o_scale);
	
};

