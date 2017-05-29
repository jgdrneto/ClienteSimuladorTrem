#include <cstdio>       //printf
#include <cstring>      //memset
#include <cstdlib>      //exit
#include <netinet/in.h> //htons
#include <arpa/inet.h>  //inet_addr
#include <sys/socket.h> //socket
#include <unistd.h>     //close
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>

#include "Joystick.cpp"

#define MAXMSG 1024
#define PORTNUM 4325

void menuEscolherTrem(int op, string acao){
     for(int i=1;i<=7;i++){

        switch(i){
            case 1:
                if(op==1){
                    cout << "*";
                }else{
                    cout << "1";
                }
                cout << " - "<< acao <<" 1º Trem" << endl;
            break;
            case 2:
                if(op==2){
                    cout << "*";
                }else{
                    cout << "2";
                }
                cout << " - "<< acao <<" 2º Trem" << endl;
            break;
            case 3:
                if(op==3){
                    cout << "*";
                }else{
                    cout << "3";
                }
                cout << " - "<< acao << " 3º Trem" << endl;
            break;
            case 4:
                if(op==4){
                    cout << "*";
                }else{
                    cout << "4";
                }
                cout << " - "<< acao <<" 4º Trem" << endl;
            break;
            case 5:
                if(op==5){
                    cout << "*";
                }else{
                    cout << "5";
                }
                cout << " - "<< acao <<" 5º Trem" << endl;
            break;
            case 6:
                if(op==6){
                    cout << "*";
                }else{
                    cout << "6";
                }
                cout << " - "<< acao <<" 6º Trem" << endl;
            break;
            case 7:
                if(op==7){
                    cout << "*";
                }else{
                    cout << "7";
                }
                cout << " - "<< acao <<" 7º Trem" << endl;
            break;
        }
    }
}

void menu(int op){

    for(int i=1;i<=4;i++){

        switch(i){
            case 1:
                if(op==1){
                    cout << "*";
                }else{
                    cout << "1";
                }
                cout << " - Conectar/Desconectar servidor" << endl;
            break;
            case 2:
                if(op==2){
                    cout << "*";
                }else{
                    cout << "2";
                }
                cout << " - Ligar/Desligar todos os trens" << endl;
            break;
            case 3:
                if(op==3){
                    cout << "*";
                }else{
                    cout << "3";
                }
                cout << " - Ligar/Desligar um trem específico" << endl;
            break;
            case 4:
                if(op==4){
                    cout << "*";
                }else{
                    cout << "4";
                }
                cout << " - Aumentar a velocidade de um trem específico" << endl;
            break;
        }
    }

}

int velocidadeTrens(Joystick& j1, int trem){
    int velocidade = 0;

    while(!j1.valorBotaoAction()){

        system("clear");

        cout << "Escolha a velocidade do "<< trem << "º trem: ";
        velocidade = j1.escolherVelocidade();
        cout << velocidade << endl;
    }

    return velocidade;
}

int telaTrens(Joystick& j1,string acao){

    int valorAtual=1;

    while(!j1.valorBotaoAction()){

        system("clear");

        menuEscolherTrem(valorAtual,acao);

        valorAtual+= j1.valorBotaoDown()-j1.valorBotaoUp();

        if(valorAtual<1){
            valorAtual=1;
        }else{
            if(valorAtual>7){
                valorAtual=7;
            }
        }

    }

    return valorAtual;
}

int telaPrincipal(Joystick& j1){

    int valorAtual=1;

    while(!j1.valorBotaoAction()){

        system("clear");

        menu(valorAtual);

        valorAtual+= j1.valorBotaoDown()-j1.valorBotaoUp();

        if(valorAtual<1){
            valorAtual=1;
        }else{
            if(valorAtual>4){
                valorAtual=4;
            }
        }

    }

    return valorAtual;
}

int main(int argc, char *argv[])
{
    //Criando socket

    //==========================================================

    int     sockfd;
    struct sockaddr_in servaddr;

    memset(&servaddr, 0x00, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORTNUM);
    inet_pton(AF_INET, "192.168.7.1", &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1){
        std::cout << "Falha ao criar o socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    //==========================================================

    Joystick j1;
    
    bool sair=false;
    bool conectado=false;
    bool trensParados=false;
    std::stringstream buffer;

    while(!sair){

        int valorEscolhido = telaPrincipal(j1);

        switch(valorEscolhido){
            case 1:

                if(conectado==false){
                
                    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) ==-1){
                        std::cout << "Jogo finalizado" << std::endl;
                        exit(EXIT_SUCCESS);
                    }else{
                        conectado=true;
                    }
                }else{
                    close(sockfd);
                    conectado=false;
                    sair=true;
                }    

            break;
            case 2:
                if(trensParados==true){
                    buffer << "T: 1 2 3 4 5 6 7\nVL: 40 40 40 40 40 40 40";
                    trensParados=false;
                }else{
                     buffer << "T: 1 2 3 4 5 6 7\nVL: 0 0 0 0 0 0 0";
                     trensParados =true;
                }
            break;
            case 3:
                buffer << "T: " << telaTrens(j1,"Parar") << "\nVL: 0";
            break;
            case 4:
                int trem = telaTrens(j1,"Mudar velocidade");

                buffer << "T: " << trem << "\nVL: " << velocidadeTrens(j1,trem);
            break;    
        }

        //Se conectado manda as informações para o buffer
        if(conectado==true){
            //Escrevendo no canal de comunicação do socket
            if (send(sockfd, buffer.str().c_str(), buffer.str().length(),0)<0){
                std::cout << "Jogo finalizado" << std::endl;
                exit(EXIT_SUCCESS);
            }else{
                std::cout << "Enviado para o socket: " << buffer.str() << std::endl;
            }

            usleep(200000);
        }
    }

    std::cout << buffer.str() << std::endl;

    usleep(3000000);
    
    return 0;
}