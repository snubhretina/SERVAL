#ifndef _graph_structure
#define _graph_structure

#include "opencv2/opencv.hpp"
#include <vector>

//-----------------------------------------------------------------------
//--------------------------   구조체   ---------------------------------
//-----------------------------------------------------------------------

class vnode {
protected:
	int ID_num;
	std::vector<vnode*> head, tail;

public:
	//getter
	int get_ID();
	std::vector<vnode*>& get_head();
	std::vector<vnode*>& get_tail();

	void set_ID(int num);

	//method
	void change_head_tail();		//트리 구조에서 노드의 방향성을 바꾸는 함수 head쪽이 optical_disk와 가까운 쪽
	void push_head(vnode* target);	//head쪽에 대상 노드를 연결
	void push_tail(vnode* target);	//tail쪽에 대상 노드를 연결 -> graph 클래스에서 두개를 합치는 메소드 생성 해야함
	void disconnect(vnode* parent, vnode* child); //해당 노드가 특정 노드와 연결되어 있을 때 대상 노드와의 연결을 해제하는 메소드
};

class vbranch : public vnode {
private:
	std::vector<std::vector<double>> branch_poly_x;
	std::vector<std::vector<double>> branch_poly_y;
	std::vector<std::vector<double>> branch_poly_r;
	cv::Point end_points[2]; //그래프에서 바이퍼랑 연결 할 때 필요한 코드

public:
	//getter
	std::vector<std::vector<double>> get_poly_x();
	std::vector<std::vector<double>> get_poly_y();
	std::vector<std::vector<double>> get_poly_r();

	//setter
	void set_poly_x(const std::vector<std::vector<double>>& poly_x);
	void set_poly_y(const std::vector<std::vector<double>>& poly_y);
	void set_poly_r(const std::vector<std::vector<double>>& poly_r);
	void set_end_points(cv::Point pt1, cv::Point pt2);
};

class vbifur : public vnode {
private:
	cv::Point center_coor;
	cv::Mat vbifur_mask;
	std::vector<cv::Point> bifur_edge;

public:
	//getter
	cv::Point& get_center_coor();
	cv::Mat& get_vbifur_mask();
	std::vector<cv::Point>& get_bifur_edge();

	//setter
	void set_center_coor(cv::Point& center);
	void set_vbifur_mask(cv::Mat& mask);
	void set_bifur_edge(std::vector<cv::Point> edge);
};

//-----------------------------------------------------------------------
//--------------------------    구조    ---------------------------------
//-----------------------------------------------------------------------
class vgraph {
private:
	std::vector <vbranch> vbranches;
	std::vector <vbifur> vbifurs;
public:
	std::vector<vbranch> get_branch();
	std::vector<vbifur> get_bifur();
	void add_bifur(vbifur new_bifur);
	void add_branch(vbranch new_branch);
	vbifur& find_bifur(cv::Point center); //중심 좌표를 입력하면, 해당하는 bifur을 가져오는 함수
	void connect();
};

// 나중에 그래프 완성 후 수정 작업
class vtree {
private:
	vnode* root;
	std::vector <vbranch> vbranches;
	std::vector <vbifur> vbifurs;
public:
};

#endif