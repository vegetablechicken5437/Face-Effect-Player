/*人臉區域特效處理 (提示)*/
#include "opencv2/opencv.hpp"
#include <iostream>
#include <time.h>

using namespace cv; //宣告 opencv 函式庫的命名空間
using namespace std; //宣告 C++函式庫的命名空間

/* 自訂函式 */
void detectAndDisplay(void); //偵測人臉的函式宣告

/* 全域變數 */
//正面人臉哈爾階層式分類器的參數
String face_cascade_name = "data/haarcascade_frontalface_alt.xml";
//人眼哈爾階層式分類器的參數
String eyes_cascade_name = "data/haarcascade_eye_tree_eyeglasses.xml";

CascadeClassifier face_cascade; //建立正面人臉哈爾階層式分類器物件
CascadeClassifier eyes_cascade; //建立人眼哈爾階層式分類器物件

Mat im; //輸入影像
int option = 5; //特效選項
VideoCapture cap1(0); //建立人臉視訊物件 "data/sleepy.mp4"
// VideoCapture cap1("data/sleepy.mp4"); //建立人臉視訊物件 "data/sleepy.mp4"
VideoCapture cap2("data/explosion.mp4"); //建立爆破視訊物件
Point eye_centers[2]; //雙眼中心的(x,y)位置
int mouseX, mouseY;
int frameNumber; //爆破視訊影格編號
bool activate = false; //爆破視訊是否啟動，初始值為"否"

//定義滑鼠反應函式
static void mouse_callback(int event, int x, int y, int flags, void *)
{
	switch (event) {
		case EVENT_LBUTTONDOWN:
			mouseX = x;
			mouseY = y;
			if (x > 50 && x < 150 && y > 375 && y < 405) { option = 1; }
			else if (x > 250 && x < 340 && y > 375 && y < 405) { option = 2; }
			else if (x > 450 && x < 570 && y > 375 && y < 405) { option = 3; }
			else { option = 5; activate = true; }
			break;
		}
	cout << option;
	// 當滑鼠按下左鍵，根據點選位置，得到選項 (option) 數值 
	// 可參考 cv_27 mouse events.cpp
	// 注意：在else選項，要將爆破視訊啟動
}

/** 主程式 */
int main( void )
{    
	if (!cap1.isOpened()) { printf("--(!)Error loading viedo/camera\n"); waitKey(0); return -1; };//讀不到人臉視訊的處理

	//讀取兩個視訊物件的高寬
	int w1 = static_cast<int>(cap1.get(cv::CAP_PROP_FRAME_WIDTH));
	int h1 = static_cast<int>(cap1.get(cv::CAP_PROP_FRAME_HEIGHT));
	int w2 = static_cast<int>(cap2.get(cv::CAP_PROP_FRAME_WIDTH));
	int h2 = static_cast<int>(cap2.get(cv::CAP_PROP_FRAME_HEIGHT));

    ///載入人臉與人眼分類器的參數
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); waitKey(0); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); waitKey(0); return -1; };

	while (char(waitKey(1)) != 27 && cap1.isOpened()) //當鍵盤沒按 Esc，以及視訊物件成功開啟時，持續執行 while 迴圈
    {
		cap1 >> im; //抓取視訊的畫面
        if( im.empty() ) //如果沒抓到畫面
        {
            printf(" --(!) No captured im -- Break!");  //顯示錯誤訊息
            break;
        }
		flip(im, im, 1);
		//定義視窗名稱 namedWindow
		namedWindow("window");

		//設定滑鼠反應函式 setMouseCallback
		setMouseCallback("window", mouse_callback);
        
		/*偵測人臉，並顯示特效影像*/
        detectAndDisplay();
    }
    return 0;
}

/** detectAndDisplay 函式內容*/
void detectAndDisplay(void)
{
	/**人臉偵測前處理*/
	vector<Rect> faces; //人臉ROI區域表列
	Mat im_gray; //灰階影像物件

	cvtColor(im, im_gray, COLOR_BGR2GRAY); //彩色影像轉灰階
	equalizeHist(im_gray, im_gray);	//灰階值方圖等化(對比自動增強)。若視訊品質好，可不用

	//正面人臉哈爾階層式分類
	face_cascade.detectMultiScale(im_gray, faces, 1.1, 4, 0, Size(80, 80));

	/** 如果有偵測到人臉，執行以下敘述*/
	if (faces.size() > 0) {
		//從人臉ROI區域表列中，獲得面積最大的人臉 ROI編號
		Rect largestFace = faces[0];
		int maxArea = largestFace.width * largestFace.height;
		// 遍歷所有矩形，找出面積最大的
		for (size_t i = 1; i < faces.size(); ++i) {
			int area = faces[i].width * faces[i].height;
			if (area > maxArea) {
				maxArea = area;
				largestFace = faces[i];
			}
		}

		 //將最大人臉的 ROI數據存入 faceROI，faceROI 是 Rect 格式的變數
		Rect faceROI = largestFace;

		// 計算放大後的矩形(放大1.5倍)
		int new_width = faceROI.width * 1.5;
		int new_height = faceROI.height * 1.5;
		int new_x = faceROI.x - (new_width - faceROI.width) / 2;
		int new_y = faceROI.y - (new_height - faceROI.height) / 2;

		Rect bigROI(new_x, new_y, new_width, new_height);
		if (bigROI.x > 0 && bigROI.y > 0 && bigROI.x + bigROI.width < im.cols && bigROI.y + bigROI.height < im.rows)
		{
			faceROI = bigROI;
		}
		// 稍為放大faceROI，使新臉能夠完整覆蓋視訊中的人臉(可不做)
		// 注意：faceROI放大之後，有可能超出im畫面，導致當機，要防呆

		//可參考 faceEyeDetection.cpp，偵測 im_gray(faceROI) 之中的人眼位置
		
		std::vector<Rect> eyes;

		eyes_cascade.detectMultiScale(im_gray(faceROI), eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
		//cout << eyes.size();

		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point eye_center(faceROI.x + eyes[j].x + eyes[j].width / 2, faceROI.y + eyes[j].y + eyes[j].height / 2);
			eye_centers[j] = eye_center;
			int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
			//circle(im, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);
		}
		//人眼位置必須防呆。如果沒找到眼睛，或只找到一隻眼睛時，
		//要給eye_centers[0]，eye_centers[1]大概的數值

		//若偵測到的眼睛小於兩個，自訂眼睛位置
		if (eyes.size() < 2)
		{
			eye_centers[0] = Point(faceROI.x + faceROI.width/3, faceROI.y + 60);
			eye_centers[1] = Point(faceROI.x + faceROI.width * 2 / 3, faceROI.y + 60);
		}

		//用putText() 繪製window下方的特效選項文字，可參考 cv_38 putText.cpp
		//注意1: 先用黑色墊底，再疊上(x,y)方向位移 2 像素的黃色文字，較醒目
		//注意2: putText()僅支援英文
		putText(im, "Green", Point(50, 400), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(0, 0, 0), 1, LINE_AA);
		putText(im, "Green", Point(52, 402), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(255, 255, 0), 1, LINE_AA);
		putText(im, "Slim", Point(250, 400), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(0, 0, 0), 1, LINE_AA);
		putText(im, "Slim", Point(252, 402), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(255, 255, 0), 1, LINE_AA);
		putText(im, "Mosaic", Point(450, 400), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(0, 0, 0), 1, LINE_AA);
		putText(im, "Mosaic", Point(452, 402), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(255, 255, 0), 1, LINE_AA);

		
		//根據 option 選項，處理ROI影像
		//選項1範例：史瑞克
		if (option == 1) {
			rectangle(im, Point(50, 375), Point(150, 405), CV_RGB(255, 0, 0), 2);
			//建立多個 Mat 影像與遮罩物件
			Mat im_face = im(faceROI);
			Mat im_hsv;
			cvtColor(im_face, im_hsv, COLOR_BGR2HSV); //將im_face 轉換到 HSV 空間(COLOR_BGR2HSV)
			Mat mask = Mat::zeros(im_face.size(), CV_8UC1); //建立im_face 同尺寸的遮罩物件
			inRange(im_hsv, Scalar(0, 30, 40), Scalar(50, 180, 250), mask); //用 inRange 篩選膚色區域，hsv的上下界大約是(0, 30, 40)與(50, 180, 250)
			//可參考 code5-7 camera RGB selection using inRange.cpp
			int dilation_size = 3; // 膨脹內核大小
			Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1), cv::Point(dilation_size, dilation_size));
			dilate(mask, mask, element); //對遮罩做形態學膨脹運算
			Mat im_hsv_copy = im_hsv.clone(); //複製hsv人臉影像
			vector<Mat> hsv_plane;
			split(im_hsv, hsv_plane);
			add(hsv_plane[0], 50, hsv_plane[0], noArray(), CV_8U);
			merge(hsv_plane, im_hsv_copy);
			cvtColor(im_hsv_copy, im_hsv, COLOR_HSV2BGR); //參考cv_24 cvtColor HSV.cpp，將hue通道數據平移50之後，轉回BGR空間
			im_hsv.copyTo(im_face, mask); //用 src.copyTo(dst, mask) 方法，將調整後的 im_hsv 數據，複製到 im_face 的膚色遮罩區域	
		}

		/*選項2範例：瘦臉(自訂選項，可以做別的特效)*/
		else if (option == 2) {	
			rectangle(im, Point(250, 375), Point(340, 405), CV_RGB(255, 0, 0), 2);
			Mat im_face = im(faceROI);
			Mat map_x, map_y;
			map_x.create(im_face.size(), CV_32FC1);
			map_y.create(im_face.size(), CV_32FC1);
			int rows = im_face.rows;
			int cols = im_face.cols;
			for (int y = 0; y < rows; ++y) {
				float scale = pow(1.5, static_cast<float>(y) / rows); // 計算放大尺度
				for (int x = 0; x < cols; ++x) {
					// 計算新的座標
					float new_x = x * scale - (scale - 1) * cols / 2;
					// 邊界處理
					if (new_x < 3) new_x = 3;
					if (new_x >= cols-2) new_x = cols - 3;
					map_x.at<float>(y, x) = new_x;
					map_y.at<float>(y, x) = y;
				}
			}
			//由上而下map_x以指數(pow)方式逐漸往兩邊擴張，
			//導致remap()之後，頭髮位置不變，但下巴變尖
			//可參考 cv_43.cpp remap函式的使用
			remap(im_face, im_face, map_x, map_y, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
		}

		/*選項3範例：馬賽克*/
		else if (option == 3) {
			rectangle(im, Point(450, 375), Point(570, 405), CV_RGB(255, 0, 0), 2);
			Mat im_face = im(faceROI);
			Mat im_face_copy = im(faceROI).clone(); //複製人臉區域
			Size ori_size = im_face.size();
			resize(im_face_copy, im_face_copy, Size(10, 10)); //縮小人臉到10x10左右，請查閱 opencv doc，resize
			resize(im_face_copy, im_face, ori_size, 0, 0, INTER_NEAREST); //放大人臉回原尺寸，但使用 nearest neighbor interpolation 內差選項
		}

		/*可自訂選項*/
		else if (option == 4) {
		}

		/*其餘選項：雷射眼*/
		else {
			//如果爆破視訊已啟動，且影格編號小於30，則執行下列迴敘述
			//int frameNumber = static_cast<int>(cap2.get(CAP_PROP_POS_FRAMES));
			if (activate && frameNumber < 30) {
				line(im, eye_centers[0], Point(mouseX, mouseY), Scalar(0, 0, 255), 2); //根據 eye_centers 與滑鼠的 x,y 位置，用 line() 繪製雷射線
				line(im, eye_centers[1], Point(mouseX, mouseY), Scalar(0, 0, 255), 2);
				Mat frame, mask; //建立影像物件
				cap2 >> frame; //載入爆破視訊影像
				frameNumber++; //影格編號加1
				double scale = 0.2;
				resize(frame, frame, Size(), scale, scale); //爆破影像縮小
				threshold(frame, mask, 50, 255, 0); //使用 threshold(src, mask, 50, 255, 0); 建立遮罩
				//防呆：防止爆破視訊放在以 x,y 為中心的位置時，超過 im 的邊界
				int posX = mouseX - mask.cols / 2;
				int posY = mouseY - mask.rows / 2;
				if (posX < 0) { posX = 0; }
				if (posX + mask.cols > im.cols) { posX = im.cols - mask.cols; }
				if (posY < 0) { posY = 0; }
				if (posY + mask.rows > im.rows) { posY = im.rows - mask.rows; }
				Rect roi(posX, posY, mask.cols, mask.rows);
				frame.copyTo(im(roi), mask); //根據遮罩 mask，用 src.copyTo(dst, mask) 方法，將爆破影像貼入 im 的相對位置
			}
			//否則的話(滑鼠未點擊，或爆破視訊已撥放完畢)，執行下列敘述
			else {
				activate = false; //將爆破視訊啟動設為"否"
				frameNumber = 0;//將影格編號設為0
				cap2.set(CAP_PROP_POS_FRAMES, 0); //用 cap2.set 將影格指標指到開始的位置
			}
		}

		/* 繪製人臉區域矩形框，以及上方的學號 */
		rectangle(im, Point(faceROI.x + 2, faceROI.y + 2), Point(faceROI.x + faceROI.width + 2, faceROI.y + faceROI.height + 2), CV_RGB(0, 0, 0), 3);
		rectangle(im, Point(faceROI.x, faceROI.y), Point(faceROI.x+faceROI.width, faceROI.y + faceROI.height), CV_RGB(255, 255, 0), 3); //請參考 cv_31 rectangle.cpp, cv_38 putText.cpp
}
	/*顯示影像*/
	imshow("window", im);
}

