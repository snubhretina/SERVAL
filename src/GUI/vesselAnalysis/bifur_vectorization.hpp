#ifndef bifur_vectorization_hpp
#define bifur_vectorization_hpp

#include "opencv2/opencv.hpp"
#include "graph_structure.hpp"
#include <vector>

//------------------------------------------------------------------------------------------------
//-----------------------------------   vectorization   ------------------------------------------
//------------------------------------------------------------------------------------------------

vbifur get_bifur_vector(int id, cv::Point center, cv::Mat &Mask);

//------------------------------------------------------------------------------------------------
//-----------------------------------   class Circle   -------------------------------------------
//------------------------------------------------------------------------------------------------

class Circle {

private:
    int MAX_R;
    std::vector<cv::Mat> circle_edge_list;
    std::vector<cv::Mat> circle_mask_list;

public:
    Circle(int max_r);

    double angle(double x_center, double y_center, double x, double y);

    cv::Mat get_circle(int r);
    cv::Mat get_circle_mask(int radius = 19);
     
    std::vector<cv::Mat>& get_circle_edge_list();
    std::vector<cv::Mat>& get_circle_mask_list();

    int get_MAX_R();
};

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

std::vector<int> get_pixel_values(cv::Mat& mask, std::vector<cv::Point>& coordinates);
void find_bifur_mask(cv::Mat& mask, int x, int y, Circle C, cv::Mat& output);
void draw_line(cv::Mat& mask, cv::Point point1, cv::Point point2, char color = 'r', int thickness = 2);

#endif 
