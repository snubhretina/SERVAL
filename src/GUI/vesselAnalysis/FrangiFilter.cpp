#include "stdafx.h"
#include "FrangiFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


cFrangiFilter::cFrangiFilter()
{
}


cFrangiFilter::~cFrangiFilter()
{
}

cv::Mat cFrangiFilter::MakeMultiScaleHessianEigenvalues(cv::Mat img, std::vector<double> &sigmas, std::vector<cv::Mat> &hevfmat_vec)
{
	/*std::vector<double> sigmasStock = sigmas;*/
	int n_scl = (int)sigmas.size();
	cv::Mat img_g, img_g_64;
	if (img.channels() == 3)
		cv::cvtColor(img, img_g, CV_BGR2GRAY);
	else img_g = img;
	img_g.convertTo(img_g_64, CV_64F);

	cv::Mat Vesselness(img_g_64.rows, img_g_64.cols, CV_64F);


	for (int i = 0; i < n_scl; i++) {
		cv::Mat gimg;
		cv::GaussianBlur(img_g_64, gimg, cv::Size(0, 0), sigmas[i], sigmas[i]);

		cv::Mat gix = GetCentralPartialDerivative2DcvMat_X<double>(gimg);
		cv::Mat giy = GetCentralPartialDerivative2DcvMat_Y<double>(gimg);
		cv::Mat gixx = GetCentralPartialDerivative2DcvMat_X<double>(gix);
		cv::Mat giyy = GetCentralPartialDerivative2DcvMat_Y<double>(giy);
		cv::Mat gixy = GetCentralPartialDerivative2DcvMat_Y<double>(gix);
		float fSmxx, fSmxy, fSmyy, fSqRtD, fL, fL1, fL2;
		float fSigmaSq = sigmas[i] * sigmas[i];
		cv::Mat l1 = cv::Mat::zeros(img.rows, img.cols, CV_64F);
		cv::Mat l2 = cv::Mat::zeros(img.rows, img.cols, CV_64F);
		for (int y = 1; y < img.rows - 1; y++) {
			for (int x = 1; x < img.cols - 1; x++) {
				fSmxx = fSigmaSq*gixx.at<double>(y, x); /// Correct for scale (fSigmaSq*)
				fSmxy = fSigmaSq*gixy.at<double>(y, x);
				fSmyy = fSigmaSq*giyy.at<double>(y, x);
				fSqRtD = sqrt((fSmxx - fSmyy)*(fSmxx - fSmyy) + 4.0f*fSmxy*fSmxy);
				fL1 = (fSmxx + fSmyy - fSqRtD) * 0.5f;
				fL2 = (fSmxx + fSmyy + fSqRtD) * 0.5f;
				if (fabs(fL1) > fabs(fL2)) {
					l1.at<double>(y, x) = fL2;
					l2.at<double>(y, x) = fL1;
				}
				else {
					l1.at<double>(y, x) = fL1;
					l2.at<double>(y, x) = fL2;
				}
			}
		}



		hevfmat_vec.push_back(l1);
		hevfmat_vec.push_back(l2);
	}



	return Vesselness;


}

template <typename T>
cv::Mat cFrangiFilter::GetCentralPartialDerivative2DcvMat_X(cv::Mat in)
{
	//VERIFY(in.channels() == 1);
	int x, y;
	cv::Mat pdmat = cv::Mat::zeros(in.rows, in.cols, CV_MAKETYPE(cv::DataType<T>::type, 1));
	for (y = 1; y < in.rows - 1; y++) {
		for (x = 1; x < in.cols - 1; x++) {
			T v1 = in.at<T>(y, x + 1);
			T v2 = in.at<T>(y, x - 1);
			T v = (v1 - v2)*0.5;
			pdmat.at<T>(y, x) = v;
		}
	}
	return pdmat;
}

template <typename T>
cv::Mat cFrangiFilter::GetCentralPartialDerivative2DcvMat_Y(cv::Mat in)
{
	//VERIFY(in.channels() == 1);
	int x, y;
	cv::Mat pdmat = cv::Mat::zeros(in.rows, in.cols, CV_MAKETYPE(cv::DataType<T>::type, 1));
	for (y = 1; y < in.rows - 1; y++) {
		for (x = 1; x < in.cols - 1; x++) {
			pdmat.at<T>(y, x) = (in.at<T>(y + 1, x) - in.at<T>(y - 1, x))*0.5;
		}
	}
	return pdmat;
}

cv::Mat cFrangiFilter::makeFrangiFilter(cv::Mat image,std::vector<double> sigmas,std::vector<cv::Mat> result)
{
	MakeMultiScaleHessianEigenvalues(image, sigmas, result);
	std::vector<double> min_fss, max_fss;
	ComputeMinMaxHevFeat(result, min_fss, max_fss);

	double alpha = 0.5; double beta = 3;

	int n_scl = sigmas.size();

	cv::Mat vsns(image.rows, image.cols, CV_64F);
	vsns = 0;
	for (int y = 1; y < image.rows - 1; y++) {
		for (int x = 1; x < image.cols - 1; x++) {
			for (int i = 0; i < n_scl; i++) {
				float l1 = (float)(result[2 * i].at<double>(y, x));
				float l2 = (float)(result[2 * i + 1].at<double>(y, x));
				if (l2 > 0) {
					double ra = fabs(l1) / fabs(l2);
					double ra_ = abs(l1) / abs(l2);
					double st = sqrt(l1*l1 + l2*l2);
					double v = exp(-(ra*ra) / (2 * alpha*alpha)) * (1 - exp(-(st*st) / (2 * beta*beta)));
					if (v > vsns.at<double>(y, x))
						vsns.at<double>(y, x) = v;
				}
			}
		}
	}

	double minv, maxv;
	cv::minMaxIdx(vsns, &minv, &maxv);
	//vsns = (vsns - minv) / (maxv - minv);
	

	/*cv::Mat test_8uc;
	m_curInputVessel.convertTo(test_8uc, CV_8U, 255);*/

	return vsns;
}
void cFrangiFilter::ComputeMinMaxHevFeat(std::vector<cv::Mat> &hfeat, std::vector<double> &_min_fss, std::vector<double> &_max_fss)
{
	_min_fss.clear();
	_max_fss.clear();
	//for ( int i = 0; i < n_scl; i++ ) {
	int nhev = (int)hfeat.size() / 2;
	/*int nhev = n_scl;*/
	for (int i = 0; i < nhev; i++) {
		cv::Mat fs = hfeat[2 * i].mul(hfeat[2 * i]) + hfeat[2 * i + 1].mul(hfeat[2 * i + 1]);
		double min_fs, max_fs;
		cv::minMaxIdx(fs, &min_fs, &max_fs);
		_min_fss.push_back(sqrt(min_fs));
		_max_fss.push_back(sqrt(max_fs));
	}
}

cv::Mat cFrangiFilter::frangi(cv::Mat input, cv::Mat *o_scale)
{
	//set default frangi opts
	frangi2d_opts_t opts;
	frangi2d_createopts(&opts);
	opts.sigma_start = 1;
	opts.sigma_end = 5;
	opts.sigma_step = 1;
	opts.BlackWhite = true;
	
	opts.BetaOne = 0.5;
	opts.BetaTwo = 15;
	//process command line options
	int index;
	char shortopts[] = "";
	//option *longopts;
	//createOptions(&longopts);

	//string outFilename;
	bool outFilenameSet = false;

	//while (true){
	//	int flag = getopt_long(argc, argv, shortopts, longopts, &index);
	//	switch (flag){
	//	case 0: //sigma_start 
	//		opts.sigma_start = strtod(optarg, NULL);
	//		break;

	//	case 1: //sigma_end
	//		opts.sigma_end = strtod(optarg, NULL);
	//		break;

	//	case 2: //sigma_step
	//		opts.sigma_step = strtod(optarg, NULL);
	//		break;

	//	case 3: //BetaOne (blob suppression)
	//		opts.BetaOne = strtod(optarg, NULL);
	//		break;

	//	case 4: //BetaTwo (background suppression)
	//		opts.BetaTwo = strtod(optarg, NULL);
	//		break;
	//	case 5:
	//		showHelp();
	//		exit(0);
	//		break;
	//	case 6:
	//		outFilename = string(optarg);
	//		outFilenameSet = true;
	//		break;
	//	case 7:
	//		opts.BlackWhite = false;
	//		break;
	//	}
	//	if (flag == -1){
	//		break;
	//	}
	//}

	////filenames
	//char *filename = argv[optind];
	//if (filename == NULL){
	//	cerr << "Missing filename!" << endl;
	//	exit(1);
	//}

	//if (!outFilenameSet){
	//	outFilename = string(filename) + "_frangi";
	//}
	//delete[] longopts;

	//read image file, run frangi, output to output file
	//cv::Mat input_img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat input_img;
	input.copyTo(input_img);
	cv::Mat input_img_fl;
	input_img.convertTo(input_img_fl, CV_32FC1);
	cv::Mat vesselness, scale, angles;
	frangi2d(input_img_fl, vesselness, scale, angles, opts);

	int exclude_boundary = 3;
	cv::Rect rc(exclude_boundary, exclude_boundary, input_img.cols - exclude_boundary * 2, input_img.rows - exclude_boundary * 2);

	cv::Mat boundary(input_img.size(), CV_8UC1);
	boundary = 255;
	boundary(rc) = 0;
	vesselness.setTo(0, boundary);
	//cv::Mat vesselness_8u;
	//vesselness.copyTo(vesselness_8u);


	//double minv;
	//double maxv;
	//cv::minMaxIdx(vesselness_8u, &minv, &maxv);
	//vesselness_8u = (vesselness_8u - minv) / (maxv - minv) * 250.f;
	////cv::GaussianBlur(vesselness, vesselness, cv::Size(11,11), 5,5);
	//vesselness_8u.convertTo(vesselness_8u, CV_8UC1);
	//cv::imshow("vesselness", vesselness_8u);
	//cv::imshow("input", input);
	//cv::waitKey();
	//cv::imwrite(outFilename + ".png", vesselness_8u * 255);

	*o_scale = scale;
	return vesselness;
}