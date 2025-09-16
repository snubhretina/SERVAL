#ifndef _skeletonize
#define _skeletonize

#include "opencv2/opencv.hpp"
#include <vector>

//----------------------------------------------------------------------------------
//----------------------------------   skel   --------------------------------------
//----------------------------------------------------------------------------------
void skeletonize(const cv::Mat& mask, cv::Mat& skel);
void compute_thin_image(cv::Mat& img);
void find_simple_point_candidates(cv::Mat& img, int curr_border, std::vector<cv::Point3d>& simple_border_points, std::vector<int>& LUT);

void get_neighborhood(cv::Mat& img, int p, int r, int c, int* neighborhood);

bool is_endpoint(int* neighborhood);
bool is_Euler_invariant(int* neighborhood, std::vector<int>& LUT);
bool is_simple_point(int* neighborhood);

//LOOK UP TABLE
void fill_Euler_LUT(std::vector<int>& LUT);
void octree_labeling(int octant, int label, int *cube);

//----------------------------------------------------------------------------------
//-----------------------------   skel analysis  -----------------------------------
//----------------------------------------------------------------------------------
class points {
private:
	//bifur
	std::vector<cv::Mat> X;
	std::vector<cv::Mat> T;
	std::vector<cv::Mat> Y;

	//end
	std::vector<cv::Mat> E;

public:
	//method
	points();

	//getter
	std::vector<cv::Mat> get_X();
	std::vector<cv::Mat> get_T();
	std::vector<cv::Mat> get_Y();

	std::vector<cv::Mat> get_E();

};

void find_bifur_points(points P, const cv::Mat& skel_mask, cv::Mat& result_point_mask);
void find_end_points(points P, const cv::Mat& skel_mask, cv::Mat& result_point_mask);

#endif