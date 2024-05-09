#pragma once
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <fstream>
#include <unistd.h>

using namespace std;
using namespace cv;
int counter;

class Flappy
{
private:
    //VARIAVEIS DO PROGRAMA
    bool chave = true;
    bool key = false;    
    string cascadeName;
    clock_t tempo_inicio;
    double duracao_tempo;


public:
    void salvarScore(int pontuacao);
    int resgataScore();
    void record();

    int FlappyGame();

    void drawTransparency(Mat frame, Mat transp, int xPos, int yPos);
    void drawTransRect(Mat frame, Scalar color, double alpha, Rect region);
    void detectAndDraw( Mat& img, CascadeClassifier& cascade, double scale, bool tryflip);
};

void Flappy::detectAndDraw( Mat& img, CascadeClassifier& cascade, double scale, bool tryflip){
    double t = 0;
    vector<Rect> faces;
    Mat gray, smallImg;
    Scalar color = Scalar(255,0,200);

    double fx = 1 / scale;
    resize( img, smallImg, Size(848,480), 1, 1, INTER_LINEAR_EXACT );
    if( tryflip )
        flip(smallImg, smallImg, 1);
    cvtColor( smallImg, gray, COLOR_BGR2GRAY );
    equalizeHist( gray, gray );

    t = (double)getTickCount();

    cascade.detectMultiScale( gray, faces, 1.3, 2, 0 |CASCADE_SCALE_IMAGE, Size(40, 40) );
    t = (double)getTickCount() - t;
    printf( "detection time = %g ms\n", t*1000/getTickFrequency());
    
   
    //desenha o flappy
    Mat flappy = cv::imread("flappy.png", IMREAD_UNCHANGED);
    Rect flappyRect = cv::Rect(1,1,flappy.cols, flappy.rows);

    //PRIMEIRO PADRAO DE COLUNAS - VARIAVEIS PARA O PRIMEIRO PADRAO DE COLUNAS
    Mat coluna1 = cv::imread("pipe1.png", IMREAD_UNCHANGED);
    Rect coluna1Rect = cv::Rect(1,1,coluna1.cols, coluna1.rows);
    Size colun = coluna1.size();

    Mat coluna_invertido1 = cv::imread("pipe_invertido1.png", IMREAD_UNCHANGED);
    Rect coluna_invertido1Rect = cv::Rect(1,1,coluna_invertido1.cols, coluna_invertido1.rows);
    Size colun2 = coluna_invertido1.size();

    Mat coluna2 = cv::imread("pipe2.png", IMREAD_UNCHANGED);
    Rect coluna2Rect = cv::Rect(1,1,coluna2.cols, coluna2.rows);
    Size colun3 = coluna2.size();

    Mat coluna_invertido2 = cv::imread("pipe_invertido2.png", IMREAD_UNCHANGED);
    Rect coluna_invertido2Rect = cv::Rect(1,1,coluna_invertido2.cols, coluna_invertido2.rows);
    Size colun4 = coluna_invertido2.size();
    
    //*ponto 0,0 da imagem 1.1
    static int ix1 = smallImg.cols-colun.width, iy1= smallImg.rows-smallImg.rows;
    //*ponto 0,0 da imagem 1.2
    static int x1 = smallImg.cols-colun.width, y1 = smallImg.rows-colun.height;
    //*ponto 0,0 da imagem 1.1
    static int ix2 = smallImg.cols-colun3.width, iy2= smallImg.rows-smallImg.rows;
    //*ponto 0,0 da imagem 1.2
    static int x2 = smallImg.cols-colun3.width, y2 = smallImg.rows-colun3.height;

    //CONDICAO PARA O DESLOCAMENTO DAS COLUNAS
    if(x1 <= 15)
    {
        x1 = smallImg.cols-colun.width;
        ix1 = smallImg.cols-colun.width;
    }else{
        x1 -=4*log(10+duracao_tempo);
        ix1 -=4*log(10+duracao_tempo);
    }
    
    //DETERMINAÇÃO DE UM RETANGULO PARA CADA TUBO
    coluna1Rect = cv::Rect(x1,y1,coluna1.cols, coluna1.rows);
    coluna_invertido1Rect = cv::Rect(ix1,iy1,coluna_invertido1.cols, coluna_invertido1.rows);

    coluna2Rect = cv::Rect(x2,y2,coluna2.cols, coluna2.rows);
    coluna_invertido2Rect = cv::Rect(ix2,iy2,coluna_invertido2.cols, coluna_invertido2.rows);

    drawTransparency(smallImg, coluna1, x1, y1);
    drawTransparency(smallImg, coluna_invertido1, ix1, iy1);

    if (x1 < (smallImg.cols/2) && ix1 < (smallImg.cols/2) )
    {
        key = true;
    }
    printf("colun.width:%d", smallImg.cols-colun.width);

    if (key)
    {
        drawTransparency(smallImg, coluna2, x2, y2);
        drawTransparency(smallImg, coluna_invertido2, ix2, iy2);

        if(x2 <= 15)
        {
            x2 = smallImg.cols-colun.width;
            ix2 = smallImg.cols-colun.width;
        }else{
            x2 -=4*log(10+duracao_tempo);
            ix2 -=4*log(10+duracao_tempo); 
        }

    }

    // PERCORRE AS FACES ENCONTRADAS
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        duracao_tempo = ( std::clock() - tempo_inicio ) / (double) CLOCKS_PER_SEC;
        Rect r = faces[i];

        //colisao
        if(((r.br() + r.tl())*0.46).x + flappy.cols < (smallImg.cols) && ((r.br() + r.tl())*0.46).y + flappy.rows < (smallImg.rows) &&  ((r.br() + r.tl())*0.46).x > 0 && ((r.br() + r.tl())*0.46).y > 0){
            drawTransparency(smallImg, flappy, cvRound(((r.br() + r.tl())*0.46).x),cvRound(((r.br() + r.tl())*0.46).y));

            //*((r.br() + r.tl())*0.46).y) = ponto medio R.y
            //*((r.br() + r.tl())*0.46).x) = ponto medio R.x
            flappyRect = cv::Rect(((r.br() + r.tl())*0.46).x,((r.br() + r.tl())*0.46).y,flappy.cols, flappy.rows);

            //TESTE SE ESTA OCORRENDO A COLISAO ENTRE O PASSARO E O TUBO
            if(((flappyRect & coluna2Rect).area() > 1 || (flappyRect & coluna_invertido2Rect).area() > 1)|| ((flappyRect & coluna1Rect).area() > 1 || (flappyRect & coluna_invertido1Rect).area() > 1)){
                system("cvlc --play-and-exit explosao.mp3 &");
                //sleep(2);
                //system("cvlc --loop explosao.mp3 &");
                chave = false;
                key = false;
                x1 = smallImg.cols-colun.width;
                ix1 = smallImg.cols-colun.width;
                x2 = smallImg.cols-colun.width;
                ix2 = smallImg.cols-colun.width;
                //system("cvlc --play-and-exit explosao.mp3 &");
            }
            else{
            counter++;
            if (counter > 50)
            {
                //chave = false;
                cerr << "Rosto nao identificado";
                cv::putText(smallImg, "Rosto Fora da Tela", cv::Point(320, 360), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0 , 215, 255), 2);
            }else
                counter = 0;
            }
        }
    }
    // Desenha um texto
    color = Scalar(255,255,255);

    char a[100];
    sprintf(a ,"SCORE: %.1f",duracao_tempo*100);
    putText	(smallImg, a, Point(smallImg.cols-(smallImg.cols), 25), FONT_HERSHEY_PLAIN, 2, color); // fonte

    // Desenha o frame na tela
    imshow("result", smallImg );
    printf("image::width: %d, height=%d\n", smallImg.cols, smallImg.rows );

}

void Flappy::salvarScore(int pontuacao){

    fstream score;
    int record;

    //Abrindo primeiramento o arquivo para leitura
    score.open("Record.txt", ios::in);

    if(!score.is_open()){
        cout << "ERRO AO ABRIR O ARQUIVO\n";
        return;
    }

    //RECEBE O DADO ARMAZENA NO ARQUIVO PARA VERIFICAR COM A PONTUACAO ATUAL
    score >> record;

    if(record < pontuacao){
        score.close();
        score.open("Record.txt", ios::out);
        if(!score.is_open()){
        cout << "ERRO AO ABRIR O ARQUIVO\n";
            return;
        } 
        score << pontuacao << endl;  

        score.close();
    }
}

int Flappy::resgataScore(){
    ifstream score;
    int record;

    score.open("Record.txt");
    
    if(!score.is_open()){
        cout << "ERRO AO ABRIR O ARQUIVO SCORE\n";
        return 0;
    }

    score >> record; 
    return record;
}

void Flappy::record(){
    int score;
    Mat tela;

    //INICIALIZANDO OUTRA IMAGEM PARA O RECORDS
    tela = imread("background.png");

    score = resgataScore();
    
    cv::putText(tela, "FLAPPY BIRD", cv::Point(275, 155), cv::FONT_HERSHEY_TRIPLEX, 2, cv::Scalar(0, 0, 0), 2);

    //COLOCANDO NA TELA O RECORD
    putText(tela, "RECORD: " + to_string(score), cv::Point(400, 250), FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0, 0, 0), 2);

    putText(tela, "PRESS ESC TO LEAVE ", cv::Point(330, 300), FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0, 0, 0), 2);
    
    while(1){
        imshow("Record", tela);
        if(waitKey(1) == 27) break;
    }
}

int Flappy::FlappyGame(){ 
    
    tempo_inicio = clock();
    VideoCapture capture;
    Mat frame;
    bool tryflip;
    CascadeClassifier cascade;
    double scale;

    cascadeName = "haarcascade_frontalface_default.xml";
    scale = 1; // usar 1, 2, 4.
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
        while (chave)
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
    if(chave == false){
        destroyAllWindows();
        Mat GameOver =  imread("gameover.png");
        cv::putText(GameOver, "PRESS ESC TO LEAVE", cv::Point(320, 360), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0 , 215, 255), 2);
        
        cv::putText(GameOver, "SCORE: " + to_string((int)(duracao_tempo*100)), Point(400, 400), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0 , 215, 255), 2);

        cv::putText(GameOver, "BEST SCORE: " + to_string(resgataScore()), Point(360, 440), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0 , 215, 255), 2);

        //Mandando o score para comparar com o armazenado e fazer o salvamento
        salvarScore((int)(duracao_tempo*100));

        while (1){
            imshow("GameOver", GameOver);
            if(waitKey(1) == 27) break;
        }
    }

    chave = true;
    return 0;
}

void Flappy::drawTransparency(Mat frame, Mat transp, int xPos, int yPos) {
    Mat mask;
    vector<Mat> layers;

    split(transp, layers); // seperate channels
    Mat rgb[3] = { layers[0],layers[1],layers[2] };
    mask = layers[3]; // png's alpha channel used as mask
    merge(rgb, 3, transp);  // put together the RGB channels, now transp insn't transparent 
    transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}

void Flappy::drawTransRect(Mat frame, Scalar color, double alpha, Rect region) {
    Mat roi = frame(region);
    Mat rectImg(roi.size(), CV_8UC3, color); 
    addWeighted(rectImg, alpha, roi, 1.0 - alpha , 0, roi); 
}
