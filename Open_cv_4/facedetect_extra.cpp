#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void detectAndDraw( Mat& img, CascadeClassifier& cascade, double scale, bool tryflip);

string cascadeName;

int main( int argc, const char** argv )
{
    VideoCapture capture;
    Mat frame;
    bool tryflip;
    CascadeClassifier cascade;
    double scale;

    cascadeName = "haarcascade_frontalface_default.xml";
    scale = 2; // usar 1, 2, 4.
    if (scale < 1)
        scale = 1;
    tryflip = true;

    if (!cascade.load(cascadeName)) {
        cerr << "ERROR: Could not load classifier cascade: " << cascadeName << endl;
        return -1;
    }

    if(!capture.open("video.mp4")) // para testar com um video
    //if(!capture.open(0)) // para testar com a webcam
    {
        cout << "Capture from camera #0 didn't work" << endl;
        return 1;
    }

    if( capture.isOpened() ) {
        cout << "Video capturing has been started ..." << endl;

        while (1)
        {
            capture >> frame;
            if( frame.empty() )
                break;

            detectAndDraw( frame, cascade, scale, tryflip );

            char c = (char)waitKey(10);
            if( c == 27 || c == 'q' || c == 'Q' )
                break;
        }
    }

    return 0;
}

/**
 * @brief Draws a transparent image over a frame Mat.
 * 
 * @param frame the frame where the transparent image will be drawn
 * @param transp the Mat image with transparency, read from a PNG image, with the IMREAD_UNCHANGED flag
 * @param xPos x position of the frame image where the image will start.
 * @param yPos y position of the frame image where the image will start.
 */
void drawTransparency(Mat frame, Mat transp, int xPos, int yPos) {
    Mat mask;
    vector<Mat> layers;

    split(transp, layers); // seperate channels
    Mat rgb[3] = { layers[0],layers[1],layers[2] };
    mask = layers[3]; // png's alpha channel used as mask
    merge(rgb, 3, transp);  // put together the RGB channels, now transp insn't transparent 
    transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}

/**
 * @brief Draws a transparent rect over a frame Mat.
 * 
 * @param frame the frame where the transparent image will be drawn
 * @param color the color of the rect
 * @param alpha transparence level. 0 is 100% transparent, 1 is opaque.
 * @param regin rect region where the should be positioned
 */
void drawTransRect(Mat frame, Scalar color, double alpha, Rect region) {
    Mat roi = frame(region);
    Mat rectImg(roi.size(), CV_8UC3, color); 
    addWeighted(rectImg, alpha, roi, 1.0 - alpha , 0, roi); 
}

void detectAndDraw( Mat& img, CascadeClassifier& cascade, double scale, bool tryflip)
{
    double t = 0;
    vector<Rect> faces;
    Mat gray, smallImg;
    Scalar color = Scalar(255,0,200);

    double fx = 1 / scale;
    resize( img, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT );
    if( tryflip )
        flip(smallImg, smallImg, 1);
    cvtColor( smallImg, gray, COLOR_BGR2GRAY );
    equalizeHist( gray, gray );

    t = (double)getTickCount();

    cascade.detectMultiScale( gray, faces,
        1.3, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(40, 40) );
    t = (double)getTickCount() - t;
    printf( "detection time = %g ms\n", t*1000/getTickFrequency());
    
    // Desenha uma imagem
    static int px = 1;
    Mat orange = cv::imread("orange.png", IMREAD_UNCHANGED);
    Rect orangeRect = cv::Rect(px,150,orange.cols, orange.rows);
    drawTransparency(smallImg, orange, px++, 150);
    printf("orang::width: %d, height=%d\n", orange.cols, orange.rows );
    
    //desenha o flappy
    Mat flappy = cv::imread("flappy.png", IMREAD_UNCHANGED);
    Rect flappyRect = cv::Rect(1,1,flappy.cols, flappy.rows);

    //PRIMEIRO PADRAO DE COLUNAS - VARIAVEIS PARA O PRIMEIRO PADRAO DE COLUNAS
    Mat coluna1 = cv::imread("pipe1.png", IMREAD_UNCHANGED);
    Mat coluna_invertido1 = cv::imread("pipe_invertido1.png", IMREAD_UNCHANGED);
    static int ix1 = 908, iy1= 0;
    static int x1 = 908, y1 = 220;

    //CONDICAO PARA O DESLOCAMENTO DAS COLUNAS
    if(x1 <= 15)
    {
        x1 = 908;
        ix1 = 908;
    }else{
        x1 -=4;
        ix1 -=4; 
    }
    drawTransparency(smallImg, coluna1, x1, y1);
    drawTransparency(smallImg, coluna_invertido1, ix1, iy1);

    // PERCORRE AS FACES ENCONTRADAS
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Rect r = faces[i];
        if((r & orangeRect).area() > 10)
            color = Scalar(255,0,200);
        else
            color = Scalar(255,0,0);

        if(r.x + flappy.cols < (smallImg.cols+50) && r.y + flappy.rows < (smallImg.rows+50)){
            drawTransparency(smallImg, flappy, cvRound(r.x - 50),cvRound(r.y - 50));


            rectangle( smallImg, Point(cvRound(r.x), cvRound(r.y)),
                    Point(cvRound((r.x + r.width-1)), cvRound((r.y + r.height-1))),
                    color, 3);
        }

    }


    // Desenha quadrados com transparencia
    double alpha = 0.3;
    drawTransRect(smallImg, Scalar(0,0,255), alpha, Rect(  0, 0, 200, 200));
    drawTransRect(smallImg, Scalar(255,0,0), alpha, Rect(200, 0, 200, 200));

    // Desenha um texto
    color = Scalar(0,0,255);
    putText	(smallImg, "Placar:", Point(300, 50), FONT_HERSHEY_PLAIN, 2, color); // fonte

    // Desenha o frame na tela
    imshow("result", smallImg );
    printf("image::width: %d, height=%d\n", smallImg.cols, smallImg.rows );
}