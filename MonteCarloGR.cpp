#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cmath>
#include<ctime>
using namespace std;

/******************************************************************/
/******************************************************************/
//Definimos una clase Esfera para facilitar el codigo.
class cEsfera
{	
	public:
		double x, y, z, radio;
		
		void ColocaEsfera(double& pos_x, double& pos_y, double& pos_z, double& r);		
};
void cEsfera::ColocaEsfera(double& pos_x, double& pos_y, double& pos_z, double& r)
{
	x=pos_x;
	y=pos_y;
	z=pos_z;
	radio=r;
	return;
}
/******************************************************************/
/******************************************************************/
//Constantes globales:
const unsigned short int NGR=400;			//divisiones en g(r)
double radio;								//radio particulas
const double DeltaMueve=0.1;				//vibracion particulas (equivalente a energia termica)
const unsigned short int N=125;				//numero de particulas
double Distancia[N][N];						//matriz donde guardamos distancia entre particulas
cEsfera esfera [N];
#define archivo "Valores_GR.dat"
#define archivo_posiciones "posiciones.dat"

void IniciaSistema();
void MueveEsfera();
void CondicionesDeFrontera(unsigned short int& i);
void CalculaGR(double GR[NGR], double& deltaGR);


/**********************//////////////////////**********************/
/**********************//////////////////////**********************/
int main()
{
	unsigned short int i,j,k;
	unsigned long int t,PMC=10000;
	const double pi=4.*atan(1.);
	double t1,t2,GR[NGR],fracvol,r,deltaGR;
	ofstream fich, fich2;
		
	srand(time(NULL));
	fich.open(archivo);
	fich2.open(archivo_posiciones);
	for(i=1;i<=4;i++)
	{
		t1=clock();		
		if(i==1){fracvol=0.05; radio=pow(3.*fracvol/(4.*pi*N),1./3.); deltaGR=6.*radio/NGR; cout<<"fracvol = "<<fracvol<<" ----- radio particulas = "<<radio<<endl;}
		if(i==2){fracvol=0.10; radio=pow(3.*fracvol/(4.*pi*N),1./3.); deltaGR=6.*radio/NGR; cout<<"fracvol = "<<fracvol<<" ----- radio particulas = "<<radio<<endl;}
		if(i==3){fracvol=0.20; radio=pow(3.*fracvol/(4.*pi*N),1./3.); deltaGR=6.*radio/NGR; cout<<"fracvol = "<<fracvol<<" ----- radio particulas = "<<radio<<endl;}
		if(i==4){fracvol=0.30; radio=pow(3.*fracvol/(4.*pi*N),1./3.); deltaGR=6.*radio/NGR; cout<<"fracvol = "<<fracvol<<" ----- radio particulas = "<<radio<<endl;}
		
		//Iniciamos el sistema colocando las particulas en lugares aleatorios (sin superposicion)
		cout<<"Iniciando sistema..."<<endl<<endl;
		IniciaSistema();
		for(j=0;j<NGR;j++) GR[j]=0;
		cout<<"||Sistema Iniciado||"<<endl<<endl;

		//Evolucionamos sistema moviendo aleatoriamente las particulas.
		//Cada 4*N iteraciones, calculamos la funcion g(r).
		cout<<"Evolucionando sistema..."<<endl;
		for(t=1;t<=PMC;t++)
		{
			for(k=1;k<=4.*N;k++)
			{
				MueveEsfera();				
			}
			CalculaGR(GR,deltaGR);
			// Escribimos en fichero las posiciones de las esferas despues de 4N iteraciones
			for(k=1;k<N;k++){fich2<<esfera[k].x<<" "<<esfera[k].y<<" "<<esfera[k].z<<endl;}
			fich2<<endl<<endl;
			if (10*t%PMC) {cout<<<<endl;}
		}
		cout<<"||Sistema Finalizado||"<<endl;
		
		//calculamos promedio de g(r) y escribimos en fichero	
		for(j=1;j<NGR;j++)
		{
			r=j*deltaGR;
			fich<<r/radio<<" "<<GR[j]/(4.*pi*r*r*deltaGR*N*N*PMC)<<endl;
		}
		fich<<endl<<endl;
		//Mostramos por pantalla el tiempo de ejecucion de cada sistema
		//y emitimos un sonido para percatarnos de ello.
		t2=clock();
		cout<<"\a";
		cout<<"\n\nTiempo de ejecucion: "<<(double)(t2-t1)/(CLOCKS_PER_SEC)<<endl;
		cout<<"-----------------------------------------------------------"<<endl;
	}
	fich.close();
	fich2.close();
	return 0;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void IniciaSistema()
{
	unsigned short int i,j;
	double x,y,z,dist_x,dist_y,dist_z,distancia_con_imagen;
	bool solapan;
	
	for(i=0;i<N;i++)
	{
		do
		{
			solapan=false;
			//Colocamos una esfera en la posicion (x,y,z) aleatoriamente
			x=rand()/double(RAND_MAX);
			y=rand()/double(RAND_MAX);
			z=rand()/double(RAND_MAX);
			esfera[i].ColocaEsfera(x,y,z,radio);
			
			//Comprobamos si la esfera nueva ha solapado con alguna de las anteriores. En ese caso, la colocamos en otro punto.
			//Debemos tener en cuenta las imagenes de las esferas situadas fuera del sistema. Es por ello que restamos o sumamos
			// 1 a la distancia entre esferas. (Estamos buscando la distancia minima entre esferas i,j).
			for(j=0;j<i;j++)
			{									
				dist_x=esfera[j].x - esfera[i].x;
				if(dist_x > 0.5) dist_x= dist_x - 1.0;				//queda una distancia negativa
				if(dist_x < -0.5) dist_x= dist_x + 1.0;				//queda una distancia positiva
									
				dist_y=esfera[j].y - esfera[i].y;
				if(dist_y > 0.5) dist_y= dist_y - 1.0;				//queda una distancia negativa
				if(dist_y < -0.5) dist_y= dist_y + 1.0;				//queda una distancia positiva
					
				dist_z=esfera[j].z - esfera[i].z;
				if(dist_z > 0.5) dist_z= dist_z - 1.0;				//queda una distancia negativa
				if(dist_z < -0.5) dist_z= dist_z + 1.0;				//queda una distancia positiva
						
				distancia_con_imagen = dist_x*dist_x + dist_y*dist_y + dist_z*dist_z;
				//Si solapan volvera a intentarlo.
				if(distancia_con_imagen <= 4.*radio*radio) solapan=true;
				//Si no lo hace, guardo la distancia entre la esfera i y la j.
				else Distancia[i][j]=distancia_con_imagen;
			}
		}while(solapan==true);
	}
	return;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void MueveEsfera()
{
	unsigned short int i,k;
	bool solapan;
	double auxX,auxY,auxZ,dist_x,dist_y,dist_z,aux[N];
	
	
	//Escogemos aleatoriamente la esfera que vamos a mover aleatoriamente
	i=rand()%N;
	
	//Partimos de la suposicion de que no solapan y guardamos
	//la posicion por si acaso solapan despues del movimiento.	
	solapan=false;
	auxX=esfera[i].x;
	auxY=esfera[i].y;
	auxZ=esfera[i].z;
		
	esfera[i].x+=DeltaMueve*(1.-rand()/(RAND_MAX*0.5));
	esfera[i].y+=DeltaMueve*(1.-rand()/(RAND_MAX*0.5));
	esfera[i].z+=DeltaMueve*(1.-rand()/(RAND_MAX*0.5));
		
	CondicionesDeFrontera(i);
	
	//Comprobamos si la esfera, despues del movimiento, ha solapado con otra. En ese caso, volvemos al estado inicial y movemos de nuevo
	k=0;
	do
	{
		//No se compara la esfera consigo misma
		if(i!=k)
		{
			//Calculamos las nuevas distancias respecto a la esfera que hemos movido
			dist_x=esfera[k].x - esfera[i].x;
			if(dist_x > 0.5) dist_x= dist_x - 1.0;
			if(dist_x < -0.5) dist_x= dist_x + 1.0;
							
			dist_y=esfera[k].y - esfera[i].y;
			if(dist_y > 0.5) dist_y= dist_y - 1.0;
			if(dist_y < -0.5) dist_y= dist_y + 1.0;
			
			dist_z=esfera[k].z - esfera[i].z;
			if(dist_z > 0.5) dist_z= dist_z - 1.0;
			if(dist_z < -0.5) dist_z= dist_z + 1.0;
				
			aux[k] = dist_x*dist_x + dist_y*dist_y + dist_z*dist_z;
			//En caso de que haya solapado, volvemos a situar la esfera
			//movida en su posicion inicial.
			if(aux[k] <= 4.*radio*radio)
			{
				solapan=true;
				esfera[i].x=auxX;
				esfera[i].y=auxY;
				esfera[i].z=auxZ;
			}				
		}			
		k++;
	}while((solapan==false)&&(k<N));
			
	//Si la particula desplazada no ha solapado con las demas,
	//guardamos las distancias nuevas en la matriz.
	if(solapan==false)
	{
		for(k=0;k<N;k++)
		{
			//Cambiamos el orden, pues la matriz es simetrica,
			//por tanto solo estamos usando la mitad de abajo
			//de la matriz.
			if(k<i) Distancia[i][k]=aux[k];
			if(k>i) Distancia[k][i]=aux[k];
		}
	}
			
	return;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void CondicionesDeFrontera(unsigned short int& i)
{
	//Condiciones para que si la esfera sobrepasa el sistema, vuelva por el otro lado
	
	//Borde de atras/delante
	if(esfera[i].x < 0)  esfera[i].x+=1.;
	if(esfera[i].x > 1.) esfera[i].x-=1.;
	
	//Borde de derecha/izquierda
	if(esfera[i].y > 1.) esfera[i].y-=1.;
	if(esfera[i].y < 0)  esfera[i].y+=1.;
	
	//Borde de abajo/arriba
	if(esfera[i].z < 0)  esfera[i].z+=1.;
	if(esfera[i].z > 1.) esfera[i].z-=1.;
	
	return;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void CalculaGR(double GR[NGR], double& deltaGR)
{	
	unsigned short int i,j,k;
	unsigned int num;
	double r, rdr;

	//Bucle para movernos en g(r)
	for(j=1;j<NGR;j++)
	{
		r=j*deltaGR;
		rdr=r+deltaGR;
		num=0;	
		
		//En vez de centrarnos en todas las particulas, lo hacemos solo
		//en la mitad de ellas, pues centrarnos en 1 y decir que 2 esta
		//dentro es lo mismo que centrarnos en 2 y decir que 1 esta dentro.
		//Por ello contamos de dos en dos.	
		for(i=0;i<N;i++)
			for(k=0;k<i;k++)
			{
				if((Distancia[i][k] >= r*r)&&(Distancia[i][k] < rdr*rdr)) {num=num+2;}				
			}			
		//Sumo las g(r) de cada particula (luego haremos el promedio)
		GR[j]+=num;
	}
	return;
}
