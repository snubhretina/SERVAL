#ifndef branch_vectorization_hpp
#define branch_vectorization_hpp

#include "opencv2/opencv.hpp"
#include "graph_structure.hpp"
#include <vector>

struct Edge;
//----------------------------------------------------------------------------------
//---------------------------   vectorization   ------------------------------------
//----------------------------------------------------------------------------------
vbranch get_branch_vector(int id,std::vector<cv::Point2d>& center_points, cv::Mat& mask, cv::Mat& fundus);

//----------------------------------------------------------------------------------
//---------------------------  width detection  ------------------------------------
//----------------------------------------------------------------------------------
Edge mask_witdth_detection(cv::Mat& seg_mask, std::vector<cv::Point2d> pts_arr);

//----------------------------------------------------------------------------------
//----------------------------  subpixel edge  -------------------------------------
//----------------------------------------------------------------------------------
double calculate_pixel(cv::Mat& img, cv::Point2d coor);

std::vector<cv::Point2d> get_edge(cv::Mat& img, cv::Point2d center_coordinate, double center_tan, double vessel_radius);

//----------------------------------------------------------------------------------
//----------------------------------  center  --------------------------------------
//----------------------------------------------------------------------------------
std::vector<cv::Point2d> get_neighbors(cv::Point point);

int count_boundary_point(cv::Mat target_line_mask, cv::Point2d point);

cv::Point2d find_end_point(cv::Mat target_line_mask);

std::vector<cv::Point2d> find_track_path(cv::Mat target_line_mask, cv::Point2d point);

std::vector<cv::Point2d> sort_points(const cv::Mat& target_line_mask);

std::vector<cv::Point2d> track_branch_centerline(cv::Point2d start_point, cv::Mat& skel, cv::Mat& bifur_center_map, cv::Point2d& end_branch);

//----------------------------------------------------------------------------------
//------------------------------------  spline  ------------------------------------
//----------------------------------------------------------------------------------
std::vector<double> hermite_spline(double x1, double y1, double g1, double x2, double y2, double g2);

double substitute(std::vector<double> coefficients, double x);

void fit(std::vector<double> x, std::vector<double> y, std::vector<std::vector<double>>& poly_x, std::vector<std::vector<double>>& poly_y,  double k = 1);

std::vector<double> get_lines(std::vector<std::vector<double>> poly_x, int sample_num = 10);

std::vector<std::vector<double>> differentiate(std::vector<std::vector<double>> poly_array);

//----------------------------------------------------------------------------------
//------------------------------------  filtering  ---------------------------------
//----------------------------------------------------------------------------------
std::vector<double> simple_sampling(std::vector<double>& arr, int sparsity);

void delete_outliers(std::vector<double>& x_data, std::vector<double>& y_data, std::vector<double>& r_data, std::vector<double>& diff_data, double threshold = 3);

#endif 
