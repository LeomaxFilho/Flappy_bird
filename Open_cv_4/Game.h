#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "facedetect_extra.cpp"
//#include <Flappy.h>

using namespace cv;
using namespace std;

//Variavel temporaria para manipular a saida da janela de menu
bool temp, temp2;

class Game 
{
private:
    //Variavel da classe responsavel pela criacao da janela    
    Mat background;

public:
    //METODO PARA TESTAR ACIONAMENTO DO BOTAO
    //void CallbuttonBack(int event, int x, int y, int flags, void* userdata);
    void run();
    Game();
};

Game::Game(){
    background = imread("background.png");
    temp = false;
    temp2 = false;
}

//FUNCAO PARA DETECTAR OS LOCAIS DOS CLIQUES
void CallbuttonBack(int event, int x, int y, int flags, void* userdata) {
    
    //Verifica se o botao esquerdo do mouse foi acionado e depois realiza o mapeamento de qual local foi realizado o aperto do botao
    if (event == EVENT_LBUTTONDOWN) {
        if(x >= 400 && x <= 600 && y >= 250 && y <= 300){ 
            cout << "Botão START clicado!" << endl;
            temp2 = true;
        }
            // Implemente a ação para iniciar o jogo aqui (por exemplo, abra uma nova janela de jogo)
        if(x >= 400 && x <= 600 && y >= 350 && y <= 400)
            cout << "Botão RECORD clicado!" << endl;
        if(x >= 400 && x <= 600 && y >= 450 && y <= 500){ 
            cout << "Botão EXIT clicado!" << endl;
            temp = true;
        }
    }
}

void Game::run(){ 

    //Testando se a imagem foi aberta corretamente
    if(background.empty()){
        cout << "Erro ao abrir a imagem\n";
        return; //-> Encerra o programa caso nao ocorra a abertura da imagem de forma correta
    }


    cv::putText(background, "FLAPPY BIRD", cv::Point(275, 155), cv::FONT_HERSHEY_TRIPLEX, 2, cv::Scalar(0, 0, 0), 2);

    cv::rectangle(background, cv::Rect(400, 250, 200, 50), cv::Scalar(255, 255, 255), -1);
    cv::putText(background, "START", cv::Point(450, 285), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0, 0, 0), 2);

    cv::rectangle(background, cv::Rect(400, 350, 200, 50), cv::Scalar(255, 255, 255), -1);
    cv::putText(background, "RECORD", cv::Point(440, 385), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0, 0, 0), 2);

    cv::rectangle(background, cv::Rect(400, 450, 200, 50), cv::Scalar(255, 255, 255), -1);
    cv::putText(background, "EXIT", cv::Point(465, 485), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0, 0, 0), 2);

    // Display the menu and manage events
    while (true) {

        //Desenha a imagem do menu na tela
        imshow("Menu", background);

        setMouseCallback("Menu", CallbuttonBack, NULL);

        int key = cv::waitKey(20);

        //Condicao para fechamento da janela do menu
        if (temp == true || temp2 == true) break; 
    }
    if(temp2 == true){
        b();
    }
}


