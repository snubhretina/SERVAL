#ifndef _vascularity
#define _vascularity

#include <vector>
#include <string>

#include "json.h"
#include "opencv2/opencv.hpp"
#include "graph_structure.hpp"

class vascularity {
private:
    //images
    cv::Mat fundus;    // 안구 이미지
    cv::Mat mask;      // 안구 마스크
    cv::Mat skel;      // 스켈레톤 이미지
    cv::Mat fa;        // 안구 조영 사진(optional)

    //optic disk
    cv::Point optic_disk_center; //optic disk 중심 좌표
    int optic_disk_r; // optic disk 반지름

	//vector 구조
	vgraph a_graph; //동맥 그래프 구조 r            
	vgraph v_graph; //정맥 그래프 구조 b

    std::vector<vtree*> a_tree; //동맥 트리 구조                  
    std::vector<vtree*> v_tree; //정맥 트리 구조   

    //기타 자료
    cv::Mat node_map;  // gui에서 어떤 대상으로 매핑해줄껀지 가이드 해주는 마스크 

public:
    //생성자
    vascularity(cv::Mat img, cv::Mat vmask);
	std::string jsonString;

    //vecterization
    void make_graph();
    void simple_vectorization(); //연결성이 없음 구 버전

    //기타 메소드
    void where(const cv::Mat& skel, std::vector<cv::Point> &result);
    bool write(std::string path = "result/vector_json/recent_result.json");
    void visualize(int sampling_dis = 1,bool save = false);
};

#endif