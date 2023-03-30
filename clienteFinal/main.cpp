#include <winsock2.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <windns.h>
#include <math.h>
#include <ctype.h>
#include <fstream>
#include <ctime>
#include <string>
using namespace std;
unsigned t1;
class Cliente
{
public:
    WSADATA wsaData;
    SOCKET conexion_socket;
    struct sockaddr_in servidor;
    struct hostent *hp;
    int resp;
    char buffer[102400]=" ";
    Cliente(int puerto)
    {
        //Inicializamos la libreria winsock2
        cout<<"Inicializando Winsock..."<<endl;
        resp=WSAStartup(MAKEWORD(1,0),&wsaData);
        if(resp)
        {
            cout<<"Error al inicializar socket"<<WSAGetLastError()<<endl;
            getchar();
        }
        cout<<"Winsock Inicializado"<<endl;

        //Obtenemos la IP del servidor.En este caso
        // localhost indica nuestra propia máquina...
        hp=(struct hostent *)gethostbyname("localhost");
        if(!hp)
        {
            cout<<"No se ha encontrado servidor..."<<endl;
            getchar();
            WSACleanup();
        }

        // Creamos el socket...
        conexion_socket=socket(AF_INET,SOCK_STREAM, 0);
        if(conexion_socket==INVALID_SOCKET)
        {
            cout<<"Error al crear socket "<<WSAGetLastError()<<endl;
            getchar();
            WSACleanup();
        }
        cout<<"Socket creado."<<endl;

        //asociamos la address al socket
        memset(&servidor, 0, sizeof(servidor)) ;
        memcpy(&servidor.sin_addr, hp->h_addr, hp->h_length);
        servidor.sin_family = hp->h_addrtype;
        servidor.sin_port = htons(puerto);

        // Nos conectamos con el servidor...
        if(connect(conexion_socket,(struct sockaddr *)&servidor,sizeof(servidor))==SOCKET_ERROR)
        {
            cout<<"Fallo al conectarse con el servidor "<<WSAGetLastError()<<endl;

            closesocket(conexion_socket);
            WSACleanup();
            getchar();
            system("pause");
            exit(0);

        }
        cout<<"Conexion establecida con: "<<inet_ntoa(servidor.sin_addr)<<endl;

    }
    //metodo comun para enviar mensajes
    void enviar()
    {
        memset(buffer, 0, sizeof(buffer));
        cin>>this->buffer;
        send(conexion_socket, buffer, sizeof(buffer), 0);

    }


    void newEnviar(char msg[102400])
    {
        strcpy(buffer,msg);
        send(conexion_socket, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    void enviarString(string msg)
    {

        for (int i = 0; i < msg.length(); i++)
        {
            this->buffer[i] = msg[i];
        }
        send(conexion_socket, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }


    //metodo comun para recibir mensajes
    void recibir()
    {

        recv(conexion_socket, buffer, sizeof(buffer), 0);
        cout<<buffer<<endl;
        memset(buffer, 0, sizeof(buffer));


    }

    string recibirString()
    {
        recv(conexion_socket, buffer, sizeof(buffer), 0);
        string linea= buffer;
        memset(buffer, 0, sizeof(buffer));
        return linea;



    }

    void cerrar()
    {
        // Cerramos el socket y liberamos la DLL de sockets
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;

    }
};
void menu(Cliente * cliente)
{
    int times=0;
    int opcion=0;
    string linea="";
    cout<<"BIENVENIDO; 1- Realizar calculo  2- Ver registros de actividades 3- Cerrar sesion"<<endl;
    t1=clock();
    cin>>opcion;
    t1=clock()-t1;
    times=(t1/CLOCKS_PER_SEC);
    //cout<<"Tiempo de respuesta: "<<times<<" segundos"<<endl;


    switch (opcion)
    {
    case 1:
    {


        cout << "Para volver al menu anterior ingrese volver" << endl;
        cout << "Ingrese la operacion a resolver: " << endl;
        cin.ignore();
         getline(cin, linea);

        if(times>=120){
            cout<<"CLIENTE DESCONECTADO POR INACTIVIDAD"<<endl;
            cliente->newEnviar("3");
            cliente->cerrar();
            break;
        }

        if (linea == "volver" || linea=="VOLVER" || linea=="Volver")
        {
            menu(cliente);
        }
        cliente->enviarString("1" + linea);
        cout<<cliente->recibirString()<<endl;
        system("PAUSE");
        system("cls");
        menu(cliente);
        break;
    }
    case 2:
    {

        cliente->newEnviar("2");
        cliente->recibir();
        menu(cliente);
        break;
    }

    case 3:
    {
        cliente->newEnviar("3");
        cliente->cerrar();
        break;
    }
    default:
    {
        cout <<"Opcion incorrecta" << endl;
        menu(cliente);
        break;
    }
    }


}
int main()
{
    cout<<"Ingrese puerto"<<endl;
    int puerto = 0;
    cin >> puerto;
    Cliente *cliente = new Cliente(puerto);
    menu(cliente);

return 0;

}


