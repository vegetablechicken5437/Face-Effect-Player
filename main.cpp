/*�H�y�ϰ�S�ĳB�z (����)*/
#include "opencv2/opencv.hpp"
#include <iostream>
#include <time.h>

using namespace cv; //�ŧi opencv �禡�w���R�W�Ŷ�
using namespace std; //�ŧi C++�禡�w���R�W�Ŷ�

/* �ۭq�禡 */
void detectAndDisplay(void); //�����H�y���禡�ŧi

/* �����ܼ� */
//�����H�y�������h�����������Ѽ�
String face_cascade_name = "data/haarcascade_frontalface_alt.xml";
//�H���������h�����������Ѽ�
String eyes_cascade_name = "data/haarcascade_eye_tree_eyeglasses.xml";

CascadeClassifier face_cascade; //�إߥ����H�y�������h������������
CascadeClassifier eyes_cascade; //�إߤH���������h������������

Mat im; //��J�v��
int option = 5; //�S�Ŀﶵ
VideoCapture cap1(0); //�إߤH�y���T���� "data/sleepy.mp4"
// VideoCapture cap1("data/sleepy.mp4"); //�إߤH�y���T���� "data/sleepy.mp4"
VideoCapture cap2("data/explosion.mp4"); //�إ��z�}���T����
Point eye_centers[2]; //�������ߪ�(x,y)��m
int mouseX, mouseY;
int frameNumber; //�z�}���T�v��s��
bool activate = false; //�z�}���T�O�_�ҰʡA��l�Ȭ�"�_"

//�w�q�ƹ������禡
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
	// ��ƹ����U����A�ھ��I���m�A�o��ﶵ (option) �ƭ� 
	// �i�Ѧ� cv_27 mouse events.cpp
	// �`�N�G�belse�ﶵ�A�n�N�z�}���T�Ұ�
}

/** �D�{�� */
int main( void )
{    
	if (!cap1.isOpened()) { printf("--(!)Error loading viedo/camera\n"); waitKey(0); return -1; };//Ū����H�y���T���B�z

	//Ū����ӵ��T���󪺰��e
	int w1 = static_cast<int>(cap1.get(cv::CAP_PROP_FRAME_WIDTH));
	int h1 = static_cast<int>(cap1.get(cv::CAP_PROP_FRAME_HEIGHT));
	int w2 = static_cast<int>(cap2.get(cv::CAP_PROP_FRAME_WIDTH));
	int h2 = static_cast<int>(cap2.get(cv::CAP_PROP_FRAME_HEIGHT));

    ///���J�H�y�P�H�����������Ѽ�
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); waitKey(0); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); waitKey(0); return -1; };

	while (char(waitKey(1)) != 27 && cap1.isOpened()) //����L�S�� Esc�A�H�ε��T���󦨥\�}�ҮɡA������� while �j��
    {
		cap1 >> im; //������T���e��
        if( im.empty() ) //�p�G�S���e��
        {
            printf(" --(!) No captured im -- Break!");  //��ܿ��~�T��
            break;
        }
		flip(im, im, 1);
		//�w�q�����W�� namedWindow
		namedWindow("window");

		//�]�w�ƹ������禡 setMouseCallback
		setMouseCallback("window", mouse_callback);
        
		/*�����H�y�A����ܯS�ļv��*/
        detectAndDisplay();
    }
    return 0;
}

/** detectAndDisplay �禡���e*/
void detectAndDisplay(void)
{
	/**�H�y�����e�B�z*/
	vector<Rect> faces; //�H�yROI�ϰ��C
	Mat im_gray; //�Ƕ��v������

	cvtColor(im, im_gray, COLOR_BGR2GRAY); //�m��v����Ƕ�
	equalizeHist(im_gray, im_gray);	//�Ƕ��Ȥ�ϵ���(���۰ʼW�j)�C�Y���T�~��n�A�i����

	//�����H�y�������h������
	face_cascade.detectMultiScale(im_gray, faces, 1.1, 4, 0, Size(80, 80));

	/** �p�G��������H�y�A����H�U�ԭz*/
	if (faces.size() > 0) {
		//�q�H�yROI�ϰ��C���A��o���n�̤j���H�y ROI�s��
		Rect largestFace = faces[0];
		int maxArea = largestFace.width * largestFace.height;
		// �M���Ҧ��x�ΡA��X���n�̤j��
		for (size_t i = 1; i < faces.size(); ++i) {
			int area = faces[i].width * faces[i].height;
			if (area > maxArea) {
				maxArea = area;
				largestFace = faces[i];
			}
		}

		 //�N�̤j�H�y�� ROI�ƾڦs�J faceROI�AfaceROI �O Rect �榡���ܼ�
		Rect faceROI = largestFace;

		// �p���j�᪺�x��(��j1.5��)
		int new_width = faceROI.width * 1.5;
		int new_height = faceROI.height * 1.5;
		int new_x = faceROI.x - (new_width - faceROI.width) / 2;
		int new_y = faceROI.y - (new_height - faceROI.height) / 2;

		Rect bigROI(new_x, new_y, new_width, new_height);
		if (bigROI.x > 0 && bigROI.y > 0 && bigROI.x + bigROI.width < im.cols && bigROI.y + bigROI.height < im.rows)
		{
			faceROI = bigROI;
		}
		// �y����jfaceROI�A�Ϸs�y��������л\���T�����H�y(�i����)
		// �`�N�GfaceROI��j����A���i��W�Xim�e���A�ɭP����A�n���b

		//�i�Ѧ� faceEyeDetection.cpp�A���� im_gray(faceROI) �������H����m
		
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
		//�H����m�������b�C�p�G�S��체���A�Υu���@�������ɡA
		//�n��eye_centers[0]�Aeye_centers[1]�j�����ƭ�

		//�Y�����쪺�����p���ӡA�ۭq������m
		if (eyes.size() < 2)
		{
			eye_centers[0] = Point(faceROI.x + faceROI.width/3, faceROI.y + 60);
			eye_centers[1] = Point(faceROI.x + faceROI.width * 2 / 3, faceROI.y + 60);
		}

		//��putText() ø�swindow�U�誺�S�Ŀﶵ��r�A�i�Ѧ� cv_38 putText.cpp
		//�`�N1: ���ζ¦�ԩ��A�A�|�W(x,y)��V�첾 2 �����������r�A������
		//�`�N2: putText()�Ȥ䴩�^��
		putText(im, "Green", Point(50, 400), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(0, 0, 0), 1, LINE_AA);
		putText(im, "Green", Point(52, 402), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(255, 255, 0), 1, LINE_AA);
		putText(im, "Slim", Point(250, 400), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(0, 0, 0), 1, LINE_AA);
		putText(im, "Slim", Point(252, 402), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(255, 255, 0), 1, LINE_AA);
		putText(im, "Mosaic", Point(450, 400), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(0, 0, 0), 1, LINE_AA);
		putText(im, "Mosaic", Point(452, 402), FONT_HERSHEY_COMPLEX | FONT_ITALIC, 1, CV_RGB(255, 255, 0), 1, LINE_AA);

		
		//�ھ� option �ﶵ�A�B�zROI�v��
		//�ﶵ1�d�ҡG�v��J
		if (option == 1) {
			rectangle(im, Point(50, 375), Point(150, 405), CV_RGB(255, 0, 0), 2);
			//�إߦh�� Mat �v���P�B�n����
			Mat im_face = im(faceROI);
			Mat im_hsv;
			cvtColor(im_face, im_hsv, COLOR_BGR2HSV); //�Nim_face �ഫ�� HSV �Ŷ�(COLOR_BGR2HSV)
			Mat mask = Mat::zeros(im_face.size(), CV_8UC1); //�إ�im_face �P�ؤo���B�n����
			inRange(im_hsv, Scalar(0, 30, 40), Scalar(50, 180, 250), mask); //�� inRange �z�ｧ��ϰ�Ahsv���W�U�ɤj���O(0, 30, 40)�P(50, 180, 250)
			//�i�Ѧ� code5-7 camera RGB selection using inRange.cpp
			int dilation_size = 3; // ���Ȥ��֤j�p
			Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1), cv::Point(dilation_size, dilation_size));
			dilate(mask, mask, element); //��B�n���κA�ǿ��ȹB��
			Mat im_hsv_copy = im_hsv.clone(); //�ƻshsv�H�y�v��
			vector<Mat> hsv_plane;
			split(im_hsv, hsv_plane);
			add(hsv_plane[0], 50, hsv_plane[0], noArray(), CV_8U);
			merge(hsv_plane, im_hsv_copy);
			cvtColor(im_hsv_copy, im_hsv, COLOR_HSV2BGR); //�Ѧ�cv_24 cvtColor HSV.cpp�A�Nhue�q�D�ƾڥ���50����A��^BGR�Ŷ�
			im_hsv.copyTo(im_face, mask); //�� src.copyTo(dst, mask) ��k�A�N�վ�᪺ im_hsv �ƾڡA�ƻs�� im_face ������B�n�ϰ�	
		}

		/*�ﶵ2�d�ҡG�G�y(�ۭq�ﶵ�A�i�H���O���S��)*/
		else if (option == 2) {	
			rectangle(im, Point(250, 375), Point(340, 405), CV_RGB(255, 0, 0), 2);
			Mat im_face = im(faceROI);
			Mat map_x, map_y;
			map_x.create(im_face.size(), CV_32FC1);
			map_y.create(im_face.size(), CV_32FC1);
			int rows = im_face.rows;
			int cols = im_face.cols;
			for (int y = 0; y < rows; ++y) {
				float scale = pow(1.5, static_cast<float>(y) / rows); // �p���j�ث�
				for (int x = 0; x < cols; ++x) {
					// �p��s���y��
					float new_x = x * scale - (scale - 1) * cols / 2;
					// ��ɳB�z
					if (new_x < 3) new_x = 3;
					if (new_x >= cols-2) new_x = cols - 3;
					map_x.at<float>(y, x) = new_x;
					map_y.at<float>(y, x) = y;
				}
			}
			//�ѤW�ӤUmap_x�H����(pow)�覡�v���������X�i�A
			//�ɭPremap()����A�Y�v��m���ܡA���U���ܦy
			//�i�Ѧ� cv_43.cpp remap�禡���ϥ�
			remap(im_face, im_face, map_x, map_y, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
		}

		/*�ﶵ3�d�ҡG���ɧJ*/
		else if (option == 3) {
			rectangle(im, Point(450, 375), Point(570, 405), CV_RGB(255, 0, 0), 2);
			Mat im_face = im(faceROI);
			Mat im_face_copy = im(faceROI).clone(); //�ƻs�H�y�ϰ�
			Size ori_size = im_face.size();
			resize(im_face_copy, im_face_copy, Size(10, 10)); //�Y�p�H�y��10x10���k�A�Ьd�\ opencv doc�Aresize
			resize(im_face_copy, im_face, ori_size, 0, 0, INTER_NEAREST); //��j�H�y�^��ؤo�A���ϥ� nearest neighbor interpolation ���t�ﶵ
		}

		/*�i�ۭq�ﶵ*/
		else if (option == 4) {
		}

		/*��l�ﶵ�G�p�g��*/
		else {
			//�p�G�z�}���T�w�ҰʡA�B�v��s���p��30�A�h����U�C�j�ԭz
			//int frameNumber = static_cast<int>(cap2.get(CAP_PROP_POS_FRAMES));
			if (activate && frameNumber < 30) {
				line(im, eye_centers[0], Point(mouseX, mouseY), Scalar(0, 0, 255), 2); //�ھ� eye_centers �P�ƹ��� x,y ��m�A�� line() ø�s�p�g�u
				line(im, eye_centers[1], Point(mouseX, mouseY), Scalar(0, 0, 255), 2);
				Mat frame, mask; //�إ߼v������
				cap2 >> frame; //���J�z�}���T�v��
				frameNumber++; //�v��s���[1
				double scale = 0.2;
				resize(frame, frame, Size(), scale, scale); //�z�}�v���Y�p
				threshold(frame, mask, 50, 255, 0); //�ϥ� threshold(src, mask, 50, 255, 0); �إ߾B�n
				//���b�G�����z�}���T��b�H x,y �����ߪ���m�ɡA�W�L im �����
				int posX = mouseX - mask.cols / 2;
				int posY = mouseY - mask.rows / 2;
				if (posX < 0) { posX = 0; }
				if (posX + mask.cols > im.cols) { posX = im.cols - mask.cols; }
				if (posY < 0) { posY = 0; }
				if (posY + mask.rows > im.rows) { posY = im.rows - mask.rows; }
				Rect roi(posX, posY, mask.cols, mask.rows);
				frame.copyTo(im(roi), mask); //�ھھB�n mask�A�� src.copyTo(dst, mask) ��k�A�N�z�}�v���K�J im ���۹��m
			}
			//�_�h����(�ƹ����I���A���z�}���T�w���񧹲�)�A����U�C�ԭz
			else {
				activate = false; //�N�z�}���T�Ұʳ]��"�_"
				frameNumber = 0;//�N�v��s���]��0
				cap2.set(CAP_PROP_POS_FRAMES, 0); //�� cap2.set �N�v����Ы���}�l����m
			}
		}

		/* ø�s�H�y�ϰ�x�ήءA�H�ΤW�誺�Ǹ� */
		rectangle(im, Point(faceROI.x + 2, faceROI.y + 2), Point(faceROI.x + faceROI.width + 2, faceROI.y + faceROI.height + 2), CV_RGB(0, 0, 0), 3);
		rectangle(im, Point(faceROI.x, faceROI.y), Point(faceROI.x+faceROI.width, faceROI.y + faceROI.height), CV_RGB(255, 255, 0), 3); //�аѦ� cv_31 rectangle.cpp, cv_38 putText.cpp
}
	/*��ܼv��*/
	imshow("window", im);
}

