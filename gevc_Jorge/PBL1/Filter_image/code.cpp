#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;

string convert_int_string(int number); //Function to convert int to string
void ordenar(vector<Vec3f>& circles); //Function to order the circles
void ordenar_opcional(vector<Vec3f>& circles);//Function to make a new order-OPTIONAL
void on_off(vector<Vec3f>& circles, Mat &img_gray); //Ver quais botãos estão ligados

int main(int argc, char *argv[])
{

    int H_Low =4 , H_High = 157;
    int S_Low = 0, S_High = 239;
    int V_Low = 247, V_High = 255;
    
    // reading the original image and storaging in mat object
    Mat img_original = imread("/home/jorge/Documents/Grupo_Visão/DataBase/painel.png",IMREAD_COLOR);
    Mat img_hsv, img_thresholded, Blurred_image;
    
    //filter blur
    blur(img_original,Blurred_image,Size(15,15),Point(-1,-1),BORDER_DEFAULT);
    

    namedWindow("Original image",WINDOW_NORMAL);
    namedWindow("test color",WINDOW_NORMAL);
    namedWindow("HSV image",WINDOW_NORMAL);
    namedWindow("Thresholded image",WINDOW_NORMAL);
    namedWindow("Gray image",WINDOW_NORMAL);
    

    if(img_original.empty()){
        cout<<"Error to open the image"<<endl;
        return -1;
    }
    
    //Creating the trackbars
    createTrackbar("H_Low","test color",&H_Low,255);
    createTrackbar("H_High","test color",&H_High,255);
    createTrackbar("S_Low","test color",&S_Low,255);
    createTrackbar("S_High","test color",&S_High,255);
    createTrackbar("V_Low","test color",&V_Low,255);
    createTrackbar("V_High","test color",&V_High,255);
    
    
    //for(int i=0;i<img_original.rows;i++){
        //for(int j =0;j<img_original.cols;j++){
            //img_original.at<uint8_t>(i,j);
        //}
    //}
    
    
    //converting normal image to hsv image
    cvtColor(Blurred_image,img_hsv, COLOR_BGR2HSV);
    
    
    
        //function in range to select the range os colors to show
        inRange(img_hsv,Scalar(H_Low,S_Low,V_Low),Scalar(H_High,S_High,V_High),img_thresholded);
    
        //Aplication of morphologic filters(erode and dilate)
        erode(img_thresholded,img_thresholded,getStructuringElement(MORPH_ELLIPSE,Size(5,5)));
        dilate(img_thresholded,img_thresholded,getStructuringElement(MORPH_ELLIPSE,Size(5,5)));
        erode(img_thresholded,img_thresholded,getStructuringElement(MORPH_ELLIPSE,Size(5,5)));
        dilate(img_thresholded,img_thresholded,getStructuringElement(MORPH_ELLIPSE,Size(5,5))); 
        
        //swap black and white
        //bitwise_not(img_thresholded,img_thresholded);
        
        vector<Vec3f>circles; //armazena x,y,raio

        Mat img_gray;
        cvtColor(img_original,img_gray,CV_BGR2GRAY);
        
        //detect circles
        HoughCircles(img_gray, circles, CV_HOUGH_GRADIENT, 1, img_gray.rows/8, 50, 40, 0, 0 );
        cout<<"Numero de circulos detectados "<<circles.size()<<endl;        
        
        ordenar(circles); //call the ordernar function
        //ordenar_opcional(circles);//nova ordem OPCIONAL

        int count = 0;
        
        //draw circles
         for( int i = 0; i < circles.size(); i++ )
        {
            count = count + 1;
            string ordem = convert_int_string(count);
            
            Vec3i c = circles[i];
            cout<<circles[i]<<endl;
            Point center = Point(c[0],c[1]);
            int radius = c[2];

            //circle center
            circle(img_original, center, 3, Scalar(0,255,0), -1, 8, 0 );//centro na imagem original
            // circle outline
            circle(img_original, center, radius, Scalar(255,0,0), 3, 8, 0 );//circulo na imagem original
    
        
            putText(img_original,ordem,Point(c[0]-50,c[1]-40),FONT_HERSHEY_COMPLEX,1,Scalar(255,255,255));    
            }

        cout<<endl;    
        on_off(circles,img_gray);
        

        imshow("Original image",img_original);
        imshow("Thresholded image",img_thresholded);
        imshow("HSV image",img_hsv);
        imshow("Gray image",img_gray);

        waitKey(0);

        
        
        destroyAllWindows();


    
    
    return 0;

}

//Function to convert int to string
string convert_int_string(int number){
        string ordem;
        stringstream ss;
        ss << number;
        ordem = ss.str();
        return ordem;
    }

//Function to order the circles
void ordenar(vector<Vec3f>& circles){
    
    for(int i = 0;i<circles.size();i++){
        for(int j = 0; j<circles.size();j++){
            if(sqrt(pow(circles[i][0],2)+pow(circles[i][1],2))<sqrt(pow(circles[j][0],2)+pow(circles[j][1],2)))
            {
                    Vec3i aux = circles[i];
                    circles[i] = circles[j];
                    circles[j] = aux;

                    }
                }
            }
        }

//Função para deixae em uma outra ordem para o respectivo problema (OPCIONAL)
void ordenar_opcional(vector<Vec3f>& circles){
    //trocar posição 4 por 5
    Vec3i aux_1 = circles[3];
    circles[3] = circles[4];
    circles[4] = aux_1;
    //trocar posição 4 por 3
    Vec3i aux_2 = circles[2];
    circles[2] = circles[3];
    circles[3] = aux_2;
    // trocar posição 4 por 2
    Vec3i aux_3 = circles[1];
    circles[1] = circles[3];
    circles[3] = aux_3;
    
}

//Função para ver quais botãos estão ligados
void on_off(vector<Vec3f>& circles, Mat &img_gray){
    int contador_verde = 0;
    int contador_vermelho = 0;
    //Para as luzes vermelhas (vetores pares)
    for(int i = 0;i<circles.size();i = i+2){
        float lum_vermelho = img_gray.at<uint8_t>(circles[i][1],circles[i][0]);
        contador_vermelho = contador_vermelho + 1;
        if (lum_vermelho>=127.5){
            cout<<"BOTAO VERMELHO "<<contador_vermelho<<" ligado !"<<endl;
        }
        else{
            cout<<"BOTAO VERMELHO "<<contador_vermelho<<" desligado !"<<endl;
        }

        }
    //Para as luzes verdes (vetores impares)    
    cout<<endl;
    for(int j = 1;j<circles.size();j+=2){
        float lum_verde = img_gray.at<uint8_t>(circles[j][1],circles[j][0]);
        contador_verde = contador_verde + 1;
        if(lum_verde >= 127.5){
            cout<<"BOTAO VERDE "<<contador_verde<<" ligado !"<<endl;
        }
        else{
            cout<<"BOTAO VERDE "<<contador_verde<<" desligado !"<<endl;
        }


    }
}

