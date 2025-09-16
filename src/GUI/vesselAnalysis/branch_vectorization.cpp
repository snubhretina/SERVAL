#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include "branch_vectorization.hpp"
#include "graph_structure.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>
#include <stdexcept>
#include <numeric>

using namespace std;

struct Edge {
	std::vector<double> edge_x1;
	std::vector<double> edge_y1;
	std::vector<double> edge_x2;
	std::vector<double> edge_y2;
};

//----------------------------------------------------------------------------------
//---------------------------   vectorization   ------------------------------------
//----------------------------------------------------------------------------------
vbranch get_branch_vector(int id, std::vector<cv::Point2d>& center_points, cv::Mat& mask, cv::Mat& fundus) {
	//처음점 끝점 추가 및 번호 부여
	vbranch result;
	result.set_end_points(center_points[0], center_points.back());
	result.set_ID(id);

	//예외처리 <- 이부분을 바꿔야 고소정 선생님이 말 하신 부분 반영 가능
	if (center_points.size() < 2)
		return result;

	if (fundus.channels() != 1)
		throw std::runtime_error("The channel of fundus to vectorize the branch must be 1");
	
	// 마스크 기반 width 계산
	Edge edge = mask_witdth_detection(mask, center_points);

	vector<double> edge_x = edge.edge_x1;
	vector<double> edge_x2 = edge.edge_x2;
	vector<double> edge_y = edge.edge_y1;
	vector<double> edge_y2 = edge.edge_y2;

	//sampling
	int sampling_dense = 4;
	edge_x = simple_sampling(edge_x, sampling_dense);
	edge_y = simple_sampling(edge_y, sampling_dense);
	edge_x2 = simple_sampling(edge_x2, sampling_dense);
	edge_y2 = simple_sampling(edge_y2, sampling_dense);

	vector<double> x_cen(edge_x.size());
	vector<double> y_cen(edge_x.size());
	vector<double> center_tan(edge_x.size());
	vector<double> vessel_w(edge_x.size());
	vector<double> r;

	for (size_t i = 0; i < edge_x.size(); i++) {
		x_cen[i] = (edge_x[i] + edge_x2[i]) / 2.0;
		y_cen[i] = (edge_y[i] + edge_y2[i]) / 2.0;
		center_tan[i] = (edge_y[i] - edge_y2[i]) / (edge_x[i] - edge_x2[i] + 1e-12);
		vessel_w[i] = sqrt(pow((edge_y[i] - edge_y2[i]), 2) + pow((edge_x[i] - edge_x2[i]), 2))/2.0;
	}

	// subpixel localization
	vector<cv::Point2d> sub;
	for (size_t i = 0; i < x_cen.size(); i++) {
		vector<cv::Point2d> edge_coor;
		edge_coor = get_edge(fundus, cv::Point2d(x_cen[i], y_cen[i]), center_tan[i], vessel_w[i]);

		sub.push_back(edge_coor[0]);
		sub.push_back(edge_coor[1]);

		x_cen[i] = (edge_coor[0].x + edge_coor[1].x) / 2;
		y_cen[i] = (edge_coor[0].y + edge_coor[1].y) / 2;
		r.push_back(sqrt(pow((edge_coor[0].x - edge_coor[1].x), 2) + pow((edge_coor[0].y - edge_coor[1].y), 2)) / 2.0);
	}

	int sampling_num = 1;
	vector<vector<double>> spline_diff_x, spline_diff_y, spline_diff_poly, spline_x, spline_y, spline_r_len, spline_r;
	vector<double> spline_diff, r_len, angle;

	fit(x_cen, y_cen, spline_x, spline_y, 1.5);

	// supixel_localization point 기반 혈관 중심과 edge_point 개선
	spline_diff_x = differentiate(spline_x);
	spline_diff_y = differentiate(spline_y);

	// centerline 미분
	for (size_t i = 0; i < spline_diff_x.size(); i++) {
		vector<double> spline_diff_poly_row;
		for (size_t j = 0; j < spline_diff_x[i].size(); j++) {
			spline_diff_poly_row.push_back(spline_diff_y[i][j] / (spline_diff_x[i][j] + 1e-9));
		}//오류
		spline_diff_poly.push_back(spline_diff_poly_row);
	}

	// spline_diff_poly centerline 기울기
	spline_diff = get_lines(spline_diff_poly, sampling_num);

	for (int i = 0; i < r.size(); i++) {
		r_len.push_back(static_cast<double>(i));
	}

	fit(r_len, r, spline_r_len, spline_r, 1.5);

	result.set_poly_x(spline_x);
	result.set_poly_y(spline_y);
	result.set_poly_r(spline_r);

	return result;
}

//----------------------------------------------------------------------------------
//---------------------------  width detection  ------------------------------------
//----------------------------------------------------------------------------------
Edge mask_witdth_detection(cv::Mat& seg_mask, vector<cv::Point2d> pts_arr) {
	vector<double> x1_edge, x2_edge, y1_edge, y2_edge;
	double x, y, diff_x, diff_y, normal_x, normal_y;

	for (int i = 0; i < pts_arr.size(); i += 2) {
		if (i == pts_arr.size() - 1) {
			x = pts_arr[i].x;
			y = pts_arr[i].y;
			diff_x = pts_arr[i].x - pts_arr[i - 1].x;
			diff_y = pts_arr[i].y - pts_arr[i - 1].y;
		}
		else {

			x = pts_arr[i].x;
			y = pts_arr[i].y;
			diff_x = pts_arr[i + 1].x - pts_arr[i].x;
			diff_y = pts_arr[i + 1].y - pts_arr[i].y;
			if (diff_x == 0 && diff_y == 0) {
				diff_x = pts_arr[i + 2].x - pts_arr[i].x;
				diff_y = pts_arr[i + 2].y - pts_arr[i].y;
			}
			normal_x = diff_y;
			normal_y = -diff_x;
		}


		if (normal_x == 0 && normal_y == 0) {
			continue;
		}

		double x1 = x;
		double x2 = x;
		double y1 = y;
		double y2 = y;

		while (x1 < seg_mask.cols && y1 < seg_mask.rows) {
			double x_prime = x1 + normal_x;
			double x_pprime = x2 - normal_x;
			double y_prime = y1 + normal_y;
			double y_pprime = y2 - normal_y;


			if (static_cast<int>(seg_mask.at<uchar>(static_cast<int>(y_prime), static_cast<int>(x_prime))) == 0 && static_cast<int>(seg_mask.at<uchar>(static_cast<int>(y_pprime), static_cast<int>(x_pprime))) == 0) {
				x1_edge.push_back(x_prime - 0.5);
				x2_edge.push_back(x_pprime + 0.5);
				y1_edge.push_back(y_prime - 0.5);
				y2_edge.push_back(y_pprime + 0.5);
				break;
			}

			if (static_cast<int>(seg_mask.at<uchar>(static_cast<int>(y_prime), static_cast<int>(x_prime))) == 0) {
				x_prime = x1;
				y_prime = y1;
			}
			if (static_cast<int>(seg_mask.at<uchar>(static_cast<int>(y_pprime), static_cast<int>(x_pprime))) == 0) {
				x_pprime = x2;
				y_pprime = y2;
			}
			x1 = x_prime;
			x2 = x_pprime;
			y1 = y_prime;
			y2 = y_pprime;
		}
	}
	Edge edge;
	edge.edge_x1 = x1_edge;
	edge.edge_x2 = x2_edge;
	edge.edge_y1 = y1_edge;
	edge.edge_y2 = y2_edge;

	return edge;
}

//----------------------------------------------------------------------------------
//----------------------------  subpixel edge  -------------------------------------
//----------------------------------------------------------------------------------

double calculate_pixel(cv::Mat& img, cv::Point2d coor) {
	/**
	Calculates the pixel value at a given coordinate of an image.

	Args:
		img: A numpy array representing the image.
		coor: A tuple or list representing the (x, y) coordinate of the pixel.

	Returns:
		A float value representing the pixel value at the given coordinate.
	*/
	int min_x = coor.x;
	int min_y = coor.y;
	int max_x = min_x + 1;
	int max_y = min_y + 1;

	if (min_x < 0 || max_x >= img.cols - 1 || min_y < 0 || max_y >= img.rows - 1)
		throw out_of_range("The given point is out of image range.");


	double a = coor.y - min_y;
	double b = 1 - a;
	double p = coor.x - min_x;
	double q = 1 - p;

	uchar A = img.at<uchar>(min_y, min_x);
	uchar B = img.at<uchar>(max_y, min_x);
	uchar C = img.at<uchar>(min_y, max_x);
	uchar D = img.at<uchar>(max_y, max_x);

	double pixel_val = q * (b * A + a * B) + p * (b * D + a * C);

	return pixel_val;
}

vector<cv::Point2d> get_edge(cv::Mat& img, cv::Point2d center_coordinate, double center_tan, double branch_radius) {
	/**
	Get the endpoints of a branch segment by calculating the branch's edge profile and its mass center.

	Args:
		img (ndarray): Image data.
		center_coordinate (tuple): The center coordinates of the branch.
		center_tan (float): The tangent of the branch at its center.
		bran ch_radius (float): The radius of the branch.
		sampling_num (int, optional): The number of samples used to calculate the edge profile. Defaults to 10.
		power_factor (int, optional): The exponent used to determine the intensity of the edge profile. Defaults to 2.
		profile (bool, optional): If True, returns the edge profile; otherwise, returns the endpoint coordinates. Defaults to False.

	Returns:
		tuple or ndarray: The coordinates of the endpoints of the branch segment or the edge profile, depending on the value of the "profile" argument.
	*/

	const double edge_width = max(1.42,branch_radius*0.8);
	const int sampling_num = 100;
	const int P = 2; //power_factor

	//1. edge_profile 가져오기
	double edge_start_point = branch_radius - edge_width / 2;
	double sample[sampling_num];
	for (int i = 0; i < sampling_num; i++)
		sample[i] = edge_start_point + edge_width / (sampling_num - 1) * i;

	double x1[sampling_num], y1[sampling_num], x2[sampling_num], y2[sampling_num];
	double angle = atan(center_tan);
	for (int i = 0; i < sampling_num; i++) {
		x1[i] = center_coordinate.x + (sample[i] * cos(angle));
		y1[i] = center_coordinate.y + (sample[i] * sin(angle));
		x2[i] = center_coordinate.x - (sample[i] * cos(angle));
		y2[i] = center_coordinate.y - (sample[i] * sin(angle));
	}


	double edge_profile_1[sampling_num], edge_profile_2[sampling_num];
	for (int i = 0; i < sampling_num; i++) {
		edge_profile_1[i] = calculate_pixel(img, cv::Point2d(x1[i], y1[i]));
		edge_profile_2[i] = calculate_pixel(img, cv::Point2d(x2[i], y2[i]));
	}

	//2. gradient 및 weight계산
	long double w1, w2, w1_s = 0, w2_s = 0;
	long double l1 = 0, l2 = 0;

	//3. 질량 중심 구하기
	for (int i = 0; i < sampling_num - 1; i++) {
		w1 = pow((edge_profile_1[i + 1] - edge_profile_1[i]), P);
		w2 = pow((edge_profile_2[i + 1] - edge_profile_2[i]), P);
		w1_s += w1;    w2_s += w2;
		l1 += (i + 0.5) * edge_width / sampling_num * w1;
		l2 += (i + 0.5) * edge_width / sampling_num * w2;
	}

	if (w1_s == 0 || w2_s == 0) {
		cv::Point2d coor1(center_coordinate.x + branch_radius * cos(angle), center_coordinate.y + branch_radius * sin(angle));
		cv::Point2d coor2(center_coordinate.x - branch_radius * cos(angle), center_coordinate.y - branch_radius * sin(angle));
		vector<cv::Point2d> edge_coor = { coor1, coor2 };
		return edge_coor;
	}
		

	l1 /= w1_s;   l2 /= w2_s;

	//원래 좌표로 환산
	double edge1 = edge_start_point + l1 / (sampling_num - 1) * edge_width;
	double edge2 = edge_start_point + l2 / (sampling_num - 1) * edge_width;

	cv::Point2d coor1(center_coordinate.x + edge1 * cos(angle), center_coordinate.y + edge1 * sin(angle));
	cv::Point2d coor2(center_coordinate.x - edge2 * cos(angle), center_coordinate.y - edge2 * sin(angle));


	vector<cv::Point2d> edge_coor = { coor1, coor2 };
	return edge_coor;
}

//----------------------------------------------------------------------------------
//----------------------------------  center  --------------------------------------
//----------------------------------------------------------------------------------
vector<cv::Point2d> get_neighbors(cv::Point point) {
	vector<cv::Point2d> neighbors = {
				cv::Point2d(point.x - 1, point.y),
				cv::Point2d(point.x + 1, point.y),
				cv::Point2d(point.x, point.y - 1),
				cv::Point2d(point.x, point.y + 1),
				cv::Point2d(point.x - 1, point.y - 1),
				cv::Point2d(point.x - 1, point.y + 1),
				cv::Point2d(point.x + 1, point.y - 1),
				cv::Point2d(point.x + 1, point.y + 1),
	};

	return neighbors;
}

int count_boundary_point(cv::Mat target_line_mask, cv::Point2d point) {
	int num_count = 0;

	num_count += (target_line_mask.at<uchar>(point.y - 1, point.x - 1) != 0) ? 1 : 0;
	num_count += (target_line_mask.at<uchar>(point.y - 1, point.x) != 0) ? 1 : 0;
	num_count += (target_line_mask.at<uchar>(point.y - 1, point.x + 1) != 0) ? 1 : 0;
	num_count += (target_line_mask.at<uchar>(point.y, point.x - 1) != 0) ? 1 : 0;
	num_count += (target_line_mask.at<uchar>(point.y, point.x + 1) != 0) ? 1 : 0;
	num_count += (target_line_mask.at<uchar>(point.y, point.x) != 0) ? 1 : 0;
	num_count += (target_line_mask.at<uchar>(point.y + 1, point.x - 1) != 0) ? 1 : 0;
	num_count += (target_line_mask.at<uchar>(point.y + 1, point.x) != 0) ? 1 : 0;
	num_count += (target_line_mask.at<uchar>(point.y + 1, point.x + 1) != 0) ? 1 : 0;

	return num_count;
}

cv::Point2d find_end_point(cv::Mat target_line_mask) {
	cv::Point2d point = cv::Point2d(-1, -1);

	for (int i = 0; i < target_line_mask.rows; i++) {
		for (int j = 0; j < target_line_mask.cols; j++) {
			if (target_line_mask.at<uchar>(i, j)) {
				point = cv::Point2d(j, i);
			}
		}
	}


	while (count_boundary_point(target_line_mask, point) != 1) {
		target_line_mask.at<uchar>(point.y, point.x) = 0;

		vector<cv::Point2d> neighbors = get_neighbors(point);
		for (const auto& neighbor : neighbors) {
			if (target_line_mask.at<uchar>(neighbor.y, neighbor.x)) {
				point = neighbor;
			}
		}
	}
	return point;

}

vector<cv::Point2d> find_track_path(cv::Mat target_line_mask, cv::Point2d point) {
	vector<cv::Point2d> path;

	while (count_boundary_point(target_line_mask, point) != 1) {
		target_line_mask.at<uchar>(point.y, point.x) = 0;
		path.push_back(point);

		vector<cv::Point2d> neighbors = get_neighbors(point);
		for (const auto& neighbor : neighbors) {
			if (target_line_mask.at<uchar>(neighbor.y, neighbor.x)) {
				point = neighbor;
			}
		}
	}
	path.push_back(point);

	return path;
}

vector<cv::Point2d> sort_points(const cv::Mat& target_line_mask) {
	cv::Mat mask_copy;
	cv::Mat mask_copy2;
	target_line_mask.copyTo(mask_copy);
	target_line_mask.copyTo(mask_copy2);


	cv::Point2d end_point = find_end_point(mask_copy);

	vector<cv::Point2d> result = find_track_path(mask_copy2, end_point);

	return result;
}

vector<cv::Point2d> track_branch_centerline(cv::Point2d start_point, cv::Mat& skel, cv::Mat& bifur_center_map, cv::Point2d& end_branch) {
	std::vector<cv::Point2d> branch_line;

	cv::Point2d target_point = start_point;
	branch_line.push_back(target_point);

	end_branch = cv::Point2d(-1, -1);
	while (!bifur_center_map.at<uchar>(target_point.y, target_point.x)) {
		branch_line.push_back(target_point);
		skel.at<cv::Vec3b>(target_point.y, target_point.x) = 0;
		vector<cv::Point2d> neighbors = get_neighbors(target_point);

		for (int i = 0; i < 8; i++) {
			if (skel.at<uchar>(neighbors[i].y, neighbors[i].x)) {
				target_point = neighbors[i];
				break;
			}
			return branch_line; //주변에 아무 포인트가 없을때
		}

	}
	end_branch = target_point;
	return branch_line;
}

//----------------------------------------------------------------------------------
//----------------------------------  spline  --------------------------------------
//----------------------------------------------------------------------------------

vector<double> hermite_spline(double x1, double y1, double g1, double x2, double y2, double g2) {
	vector<double> coefficients;

	cv::Mat A = (cv::Mat_<double>(4, 4) << pow(x1, 3), pow(x1, 2), x1, 1,
		pow(x2, 3), pow(x2, 2), x2, 1,
		3 * pow(x1, 2), 2 * x1, 1, 0,
		3 * pow(x2, 2), 2 * x2, 1, 0);

	cv::Mat B = (cv::Mat_<double>(4, 1) << y1, y2, g1, g2);

	cv::Mat solution;
	cv::solve(A, B, solution, cv::DECOMP_LU);

	cv::Mat coefficientsMat = solution.reshape(1);
	coefficients = vector<double>(coefficientsMat.begin<double>(), coefficientsMat.end<double>());

	return coefficients;
}

double substitute(vector<double> coefficients, double x) {
	return coefficients[0] * pow(x, 3) + coefficients[1] * pow(x, 2) + coefficients[2] * x + coefficients[3];
}

void fit(vector<double> x, vector<double> y, vector<vector<double>>& poly_x, vector<vector<double>>& poly_y, double k) {

	vector<double> xi = { x[0] };
	xi.insert(xi.end(), x.begin(), x.end());
	xi.push_back(x.back());

	vector<double> yi = { y[0] };
	yi.insert(yi.end(), y.begin(), y.end());
	yi.push_back(y.back());

	vector<double> gx, gy;

	for (int i = 1; i < xi.size() - 1; i++) {
		gx.push_back((xi[i + 1] - xi[i - 1]) / k);
		gy.push_back((yi[i + 1] - yi[i - 1]) / k);
	}

	for (int i = 0; i < x.size() - 1; i++) {
		vector<double> result_x = hermite_spline(0, x[i], gx[i], 1, x[i + 1], gx[i + 1]);
		vector<double> result_y = hermite_spline(0, y[i], gy[i], 1, y[i + 1], gy[i + 1]);

		poly_x.push_back(result_x);
		poly_y.push_back(result_y);
	}
	return;
}

vector<double> get_lines(vector<vector<double>> poly, int sample_num) {
	vector<double> result_x;
	double step = 1.0 / sample_num;

	for (int i = 0; i < poly.size(); i++) {
		for (int j = 0; j < sample_num; j++) {
			double x = substitute(poly[i], j * step);
			result_x.push_back(x);
		}
	}
	return result_x;
}

vector<vector<double>> differentiate(vector<vector<double>> poly_array) {
	vector<vector<double>> diff_array;

	for (auto poly : poly_array) {
		vector<double> diff_poly = { 0, 3 * poly[0], 2 * poly[1], poly[2] };
		diff_array.push_back(diff_poly);
	}
	return diff_array;
}

//----------------------------------------------------------------------------------
//------------------------------------  filtering  ---------------------------------
//----------------------------------------------------------------------------------
vector<double> simple_sampling(vector<double>& arr, int sparsity) {
	vector<double> sample_arr;
	for (int i = 0; i < arr.size(); i++) {
		if (i % sparsity == 0) {
			sample_arr.push_back(arr[i]);
		}
	}
	sample_arr.push_back(arr[arr.size()-1]);
	return sample_arr;
}

void delete_outliers(vector<double>& x_data, vector<double>& y_data, vector<double>& r_data, vector<double>& diff_data, double threshold) {
	double mean = accumulate(r_data.begin(), r_data.end(), 0.0) / r_data.size();
	double variance = accumulate(r_data.begin(), r_data.end(), 0.0, [mean](double acc, double x) { return acc + (x - mean) * (x - mean); }) / r_data.size();
	double std_dev = sqrt(variance);

	vector<double> r_copy = r_data;

	int count = 0;

	for (int i = 0; i < r_copy.size(); i++) {
		double score = (r_copy[i] - mean) / std_dev;
		if (abs(score) > threshold) {
			x_data.erase(x_data.begin() + (i - count));
			y_data.erase(y_data.begin() + (i - count));
			r_data.erase(r_data.begin() + (i - count));
			diff_data.erase(diff_data.begin() + (i - count));
			count++;
		}
	}
}