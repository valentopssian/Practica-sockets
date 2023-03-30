#include <winsock2.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <windns.h> //es utilizado por el Sistema de nombres de dominio (DNS)
#include <math.h>
#include <ctype.h> //isDigit
#include <fstream> //Lib. para trabajar con archivos
#include <ctime>   //Lib. para trabajar con fechas / tiempos
#include <string>

using namespace std;
bool desconectado=false;


void log(string msg)
{
    // Declaramos las variables
    ofstream log;
    string log_file;
    string nombre = "server.log";
    log_file.assign(nombre);
    log.open(log_file.c_str(),ios::app);

    //Declaramos la fecha/hora del dia.
    time_t now = time(0);
    tm* time = localtime(&now);
    int dia = time-> tm_mday;
    int mes = time-> tm_mon + 1;
    int anio = 1900 + time-> tm_year;
    int hora = time-> tm_hour;
    int minuto = time-> tm_min;

    log << anio <<" -"<< mes <<"-"<< dia <<"_"<< hora <<":"<< minuto << ": " << msg << std::endl;
    log.close();
}
class Servidor
{
public:
    WSADATA wsaData; //variable para inicializar la libreria winsock2
    //socket de conexion y socket de comunicacion con el cliente
    SOCKET conexion_socket,comunicacion_socket;
    struct sockaddr_in servidor;//direccion del socket servidor
    struct sockaddr_in cliente;//direccion del socket cliente
    struct hostent *hp;
    int resp,stsize;
    char buffer[102400]=" ";


    Servidor(int puerto)
    {

        log("INICIA SERVIDOR");

        cout<<"Inicializando Winsock..."<<endl;
        resp=WSAStartup(MAKEWORD(1,0),&wsaData);
        if(resp)
        {
            cout<<"Error al inicializar socket"<<WSAGetLastError()<<endl;
            getchar();
        }
        cout<<"Winsock Inicializado"<<endl;


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
            cout<<"Error al crear socket"<<WSAGetLastError()<<endl;
            getchar();
            WSACleanup();
        }
        cout<<"Socket creado."<<endl;

        //asociamos la address al socket
        memset(&servidor, 0, sizeof(servidor)) ;
        memcpy(&servidor.sin_addr, hp->h_addr, hp->h_length);
        servidor.sin_family = hp->h_addrtype;
        servidor.sin_port = htons(puerto);

        log("Server Escucha puerto :"+to_string(puerto));
        // Asociamos ip y puerto al socket
        resp=bind(conexion_socket, (struct sockaddr *)&servidor, sizeof(servidor));
        if(resp==SOCKET_ERROR)
        {
            cout<<"Error al asociar puerto e ip al socket"<<WSAGetLastError()<<endl;
            closesocket(conexion_socket);
            WSACleanup();
            getchar();
        }
        cout<<"Bind Success"<<endl;

        //ponemos a escuchar al servidor
        if(listen(conexion_socket, 1)==SOCKET_ERROR)
        {
            cout<<"Error al habilitar conexiones entrantes"<<WSAGetLastError()<<endl;
            closesocket(conexion_socket);
            WSACleanup();
            getchar();
        }

        // Aceptamos conexiones entrantes
        cout<<"Esperando conexiones entrantes..."<<endl;
        stsize=sizeof(struct sockaddr);
        comunicacion_socket=accept(conexion_socket,(struct sockaddr *)&cliente,&stsize);
        if(comunicacion_socket==INVALID_SOCKET)
        {
            cout<<"Error al aceptar conexión entrante "<<WSAGetLastError()<<endl;
            closesocket(conexion_socket);
            WSACleanup();
            getchar();
        }
        cout<<"Conexion entrante desde: "<<inet_ntoa(cliente.sin_addr)<<endl;




    }   //----------------------------------------------------------------------FIN SERVIDOR

    //recibir mensajes
    void recibir()
    {
        recv (comunicacion_socket, buffer, sizeof(buffer), 0);
        cout<<"El cliente dice: "<<buffer<<endl;
        // memset(RecvBuff,0,sizeof(RecvBuff));

    }


    string NewRecibir()

    {
        recv (comunicacion_socket, buffer, sizeof(buffer), 0);
        string msg = buffer;
        cout<<"El cliente dice: "<<buffer<<endl;
        memset(buffer, 0, sizeof(buffer));
        return msg;

    }




    //metodo comun para enviar mensajes


    void enviar(char msg[102400])
    {
        //memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, msg);
        cout<<buffer<<endl;
        send(comunicacion_socket, buffer, sizeof(buffer), 0);
        cout << "Mensaje enviado!" <<endl;

    }



    void cerrarConexion()
    {
        // Cerramos el socket de la conexion
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;
    }

    void cerrarComunicacion()
    {
        // Cerramos el socket de la comunicacion
        closesocket(comunicacion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;
    }
    void enviarString(string msg)
    {
        for (int i = 0; i < (int)msg.length(); i++)
        {
            this->buffer[i] = msg[i];
        }
        send(comunicacion_socket, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    void verRegistroActividades()
    {

        ifstream archivo;
        string linea;
        string mensaje ="";
        string nombre = "server.log";
        archivo.open(nombre.c_str(),ios::in);//

        while(getline(archivo,linea))
        {
            mensaje = mensaje + "\n" + linea ;
        }
        archivo.close();
        enviarString(mensaje);
    }
};
void menu(Servidor * server)
{
    string opcion="";
    char opcionChar[30]=" ";
    string operacion="";
    string errorCaracter=" ";
    char errorDigito;
    char delimitadores[]= {'+', '-', '*', '/', '^', '!'};
    char a[1024]= {' '};
    char b[1024]= {' '};
    int x;
    int y;
    char signo=' ';
    int posSigno=-1;
    int resultado=0;
    int contadorDigitos=0;
    int contadorDelimitadores=0;
    opcion=server->NewRecibir();
    switch(opcion[0])
    {
    case '1':
    {
        for (int i = 1; i < opcion.length(); i++)
        {
            opcionChar[i - 1] = opcion[i];
        }
    string guardar="El usuario ingresa la operacio : "+string(opcionChar);
    log(guardar);

        for(int c=0; c< strlen(opcionChar); c++)
        {
            if (opcionChar[c]=='+' ||opcionChar[c]=='-' ||opcionChar[c]=='*' ||opcionChar[c]=='/' ||opcionChar[c]=='^' ||opcionChar[c]=='!' || isdigit(opcionChar[c])==true)
            {
                contadorDigitos++;
            }
            else
            {
                errorDigito=opcionChar[c];
                break;
            }


        }
        for (int j=0; j<strlen(delimitadores); j++)
        {
            for (int i=0; i<strlen(opcionChar); i++)
            {
                if(opcionChar[i]==delimitadores[j])
                {
                    contadorDelimitadores=contadorDelimitadores+1;
                    signo=opcionChar[i];
                    posSigno=i;
                }
            }
        }
        if(contadorDelimitadores>1)
        {
            /*char error[]="ERROR!! La operacion esta mal formada solo se puede utilizar un unico signo por operacion ";
            char posicionerror[]=strcat(opcionChar[posSigno-1],signo,opcionChar[posSigno+1]);
            strcat(error,posicionerror);
            server->enviar(error);*/
            string error="ERROR!! La operacion esta mal formada solo se puede utilizar un unico signo por operacion ";
            server->enviarString(error+opcionChar[posSigno-1]+signo+opcionChar[posSigno+1]);
            menu(server);
        }
        else if(signo=='!' && opcionChar[strlen(opcionChar)-1]!='!')
        {
             string error="ERROR!! El simbolo ! en la factorizacion debe ser el ultimo digito ";
            server->enviarString(error+opcionChar[posSigno-1]+signo+opcionChar[posSigno+1]);
            //server->enviarString("ERROR!! El simbolo ! en la factorizacion debe ser el ultimo digito");
            menu(server);
        }
        else if(signo!='!' && posSigno==0 || signo!='!' && posSigno==strlen(opcionChar)-1)
        {
            string error="ERROR!! La operacion esta mal formada ";
            server->enviarString(error+opcionChar[posSigno-1]+signo+opcionChar[posSigno+1]);
            //server->enviarString("ERROR!! La operacion esta mal formada");
            menu(server);
        }
        else if(opcion.length()>20)
        {
            server->enviar("ERROR!! Solo se admiten operaciones de entre 1 y 20 caracteres");
            menu(server);
        }
        else if(strlen(opcionChar)!=contadorDigitos)
        {
            errorCaracter="Caracter erroneo: ";
            server->enviarString(errorCaracter+errorDigito);
            menu(server);
        }
        else if(isdigit(opcionChar[0])==false)
        {
            errorCaracter="ERROR!! Operacion mal hecha: ";
            server->enviarString(errorCaracter+opcionChar[0]+opcionChar[1]);
            menu(server);


        }
        else
        {

            for(int i=0; i<posSigno; i++)
            {
                if(isdigit(opcionChar[i]))
                {
                    a[i]=opcionChar[i];
                }
            }
            for (int i=posSigno+1; i<opcion.length()-1; i++)
            {
                if(isdigit(opcionChar[i]))
                {
                    b[i-posSigno-1]=opcionChar[i];
                }

            }
            x=atoi(a);
            y=atoi(b);
            cout<<"signo= "<<signo<<endl;
            cout<<"x= "<<x<<endl;
            cout<<"y= "<<y<<endl;
            switch(signo)
            {
            case '+':
                resultado=x + y;
                break;
            case '-':
                resultado=x - y;
                break;
            case '*':
                resultado=x * y;
                break;
            case '/':
                resultado=x/y;
                break;
            case '^':
                resultado=pow(x,y);
                break;
            case '!':
            {
                int factorial=1;
                for(int i=1; i<=x; i++)
                {
                    factorial=factorial*i;
                }
                resultado=factorial;

            }

            break;
            case ' ':
                resultado=y;
                break;
            default:
                break;
            }

            char result[30];
            itoa(resultado,result,10);
            char suRespuesta[100]="Su respuesta es= ";
            strcat(suRespuesta,result);
            server->enviarString(suRespuesta);
            log(string(suRespuesta));

        }

        break;
    }

    case '2':
    {
        log("USUARIO SOLICITA VER REGISTRO ACTIVIDADES");
        server->verRegistroActividades();
        break;
    }
    case '3':
    {
        desconectado=true;
    }



    }

}


int main()
{
    system("cls");
    Servidor *server = new Servidor(5000);
    while(desconectado==false)
    {
        menu(server);
    }
    server->cerrarComunicacion();
    desconectado=false;
    main();
}
