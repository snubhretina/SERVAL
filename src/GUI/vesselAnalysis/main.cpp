#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

#include "vascularity.hpp"
#include "skeletonize.hpp"
#include "json.h"

using namespace std;
using namespace cv;

int main() {
    // ��� �ܾ����
    for (int i = 200; i < 449; i++) {
        cout << i << endl;

        string num = to_string(i + 1000);
        num = num.substr(1);
        
        //string mask_path = "C:/Users/uicha/Desktop/vascularity_analysis_cpp/data/mask/"+ num +"_mask.png";
        //string img_path = "C:/Users/uicha/Desktop/vascularity_analysis_cpp/data/img/"+ num +"_img.png";

        string mask_path = "D:/SERVAL_zone/1_mask.png";
        string img_path = "D:/SERVAL_zone/1_mask.png";

        // �̹��� ���� �о����
        Mat mask = imread(mask_path);
        Mat image = imread(img_path);

        // ���� ����
        vascularity example(image, mask);

        example.visualize();
        example.write("D:/SERVAL_zone/1.json");
    }

    return 0;
}

