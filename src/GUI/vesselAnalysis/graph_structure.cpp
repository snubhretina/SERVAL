#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <vector>

#include "graph_structure.hpp"
#include "opencv2/opencv.hpp"

using namespace std;

/*구조체*/

//-----------------------------------------------------------------------
//--------------------------   vnode   ----------------------------------
//-----------------------------------------------------------------------

/*getter, setter*/
vector<vnode*>& vnode::get_head() {
	return head;
}

vector<vnode*>& vnode::get_tail() {
	return tail;
}

int vnode::get_ID() {
	return ID_num;
}

void vnode::set_ID(int num) {
	ID_num = num;
}


/*method*/
void vnode::change_head_tail() {
	swap(head, tail);
}
void vnode::push_head(vnode* target) {
	head.push_back(target);
};

void vnode::push_tail(vnode* target) {
	tail.push_back(target);
};

void vnode::disconnect(vnode* parent, vnode* child) {
	//head.erase(remove(head.begin(), head.end(), target), head.end());
	//tail.erase(remove(tail.begin(), tail.end(), target), tail.end());
};


//-----------------------------------------------------------------------
//--------------------------   vbranch   --------------------------------
//-----------------------------------------------------------------------

/*getter, setter*/
vector<vector<double>> vbranch::get_poly_x() {
	return branch_poly_x;
}

vector<vector<double>> vbranch::get_poly_y() {
	return branch_poly_y;
}

vector<vector<double>> vbranch::get_poly_r() {
	return branch_poly_r;
}

void vbranch::set_poly_x(const std::vector<std::vector<double>>& poly_x) {
	branch_poly_x = poly_x;
}

void vbranch::set_poly_y(const std::vector<std::vector<double>>& poly_y) {
	branch_poly_y = poly_y;
}

void vbranch::set_poly_r(const std::vector<std::vector<double>>& poly_r) {
	branch_poly_r = poly_r;
}

void vbranch::set_end_points(cv::Point pt1, cv::Point pt2) {
	end_points[0] = pt1;
	end_points[1] = pt2;
}


//-----------------------------------------------------------------------
//-------------------------    vbifur    --------------------------------
//-----------------------------------------------------------------------

/*getter, setter*/
cv::Point& vbifur::get_center_coor() {
	return center_coor;
}

cv::Mat& vbifur::get_vbifur_mask() {
	return vbifur_mask;
}

vector<cv::Point>& vbifur::get_bifur_edge() {
	return bifur_edge;
}
void vbifur::set_center_coor(cv::Point& center) {
	center_coor = center;
}

void vbifur::set_vbifur_mask(cv::Mat& mask) {
	vbifur_mask = mask;
}

void vbifur::set_bifur_edge(vector<cv::Point> edge) {
	bifur_edge = edge;
}

//-----------------------------------------------------------------------
//-------------------------     graph     -------------------------------
//-----------------------------------------------------------------------

void vgraph::add_bifur(vbifur new_bifur) {
	vbifurs.push_back(new_bifur);
}

void vgraph::add_branch(vbranch new_branch) {
	vbranches.push_back(new_branch);
}

vector <vbranch> vgraph::get_branch() {
	return vbranches;
}

vector <vbifur> vgraph::get_bifur() {
	return vbifurs;
}


vbifur& vgraph::find_bifur(cv::Point center) {
	for (int i = 0; i < vbifurs.size(); ++i) {
		if(vbifurs[i].get_center_coor() == center){
			vbifur&  dst = vbifurs[i];
			return dst;
		}
	}
}

void vgraph::connect() {
	//1. bifur을 이미지를 나타내는데, 1번 bifur에 해당하는 마스크는 픽셀값을 1로 둔다.
	//2. 다이레이트를 한다.
	//3. branch의 end point의 픽셀값이랑 연결해준다.
}