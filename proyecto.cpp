#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <xlsxwriter.h>

using namespace std;


struct Juegos{
	int contrincante;
};
struct Equipo{
	string nombre_equipo;
	int num_equipo;
	string nombre_estadio;
	float latitud;
	float longitud;
	Juegos historial[15];
	float distancia_r;
};


struct Distancia{
	float distancia;
	int puntoA;
	int puntoB;
};

struct Fixture{
	int local;
	int visita;
};

void CalcularDistancias(Equipo equipos[],Distancia dist[]){
   int cont=0;
   float max=0;
   for(int i=0;i<16;i++){
   	for(int j=i;j<16;j++){
   		if(i != j){
   		 dist[cont].distancia = acos((sin(M_PI*equipos[i].latitud/180)*sin(M_PI*equipos[j].latitud/180)+cos(M_PI*equipos[i].latitud/180)*cos(M_PI*equipos[j].latitud/180)*cos((equipos[i].longitud-equipos[j].longitud)*M_PI/180)))*180/M_PI*1.852*60;
   		 dist[cont].puntoA = equipos[i].num_equipo;
   		 dist[cont].puntoB = equipos[j].num_equipo; 
   		 if(max < dist[cont].distancia){
   		 	max = dist[cont].distancia;
   		 }
   		 cont++;
   		}
   	}
   }
}

void OrdenarEquipos(Equipo equipos[]){
	Equipo auxiliar;
	bool swapagain = true;
	while(swapagain){
		swapagain = false;
		for(int i=0;i<15;i++){
		if(equipos[i].longitud < equipos[i+1].longitud){
			swapagain = true;
			auxiliar = equipos[i];
			equipos[i] = equipos[i + 1];
			equipos[i + 1] = auxiliar;
			}
		}

	}
	for(int i=0;i<16;i++){
		equipos[i].num_equipo = i;
	}
}
bool ComprobarFila(Fixture matriz[][8],int num_random,int fila, int indice){
	for(int i=0;i<indice;i++){
		if(num_random == matriz[fila][i].visita){	
			return false;
		}
	}
	return true;
}
bool ComprobarColumna(Fixture matriz[][8],int num_random,int columna,int indice){
	for(int i=0;i<indice;i++){				
		if(num_random == matriz[i][columna].visita){
			return false;
		}
	}

	return true;
}

void setEquipo(Equipo equipos[],char asd[]){
	ifstream infile(asd);
	string linea;
	int i =0;
	while(getline(infile, linea)){		
		stringstream strings(linea);

		string auxiliar;
		getline(strings, auxiliar, ';');
		equipos[i].nombre_equipo=auxiliar;
		getline(strings, auxiliar, ';');
		equipos[i].nombre_estadio=auxiliar;
		float latitud;
		strings>>latitud;
		equipos[i].latitud=latitud;
		char pc;
		strings>>pc;
		float longitud;
		strings>>longitud;
		equipos[i].longitud=longitud;
		i++;
		}
}
void Inicializar(Fixture matriz[][8],int indice){
	for(int i=0;i<indice;i++){
		for(int j=0;j<indice;j++){
			matriz[i][j].local = -1;
			matriz[i][j].visita = -1;
		}
	}
}
void MostrarMatriz(Fixture matriz[][8]){
	cout<<"  0 1 2 3 4 5 6 7"<<endl;
	for(int i=0;i<8;i++){
		cout<<i<<" ";
		for(int j=0;j<8;j++){
			cout<<matriz[i][j].visita<<" ";
		}
		cout<<endl;
	}
	cout<<" "<<endl;
}
void LlenarMatriz(Fixture matriz[][8],Equipo equipos[],int lim_i_i,int lim_s_i,int lim_i_j,int lim_s_j,int rand_i, int rand_f, int matriztipo){
	int random,cont=0;
	bool var = true;
here:
	for(int i=lim_i_i;i<lim_s_i;i++){ //equipos locales
		for(int j=lim_i_j;j<lim_s_j;j++){ //fechas
			while(var){
			  random = rand()%rand_f + rand_i;
				if(ComprobarFila(matriz, random,i,rand_f) && ComprobarColumna(matriz, random, j, rand_f)){
				 
				 matriz[i][j].visita = random;
				 switch(matriztipo){
				 	case 0: equipos[random].historial[j].contrincante = i;
				 			matriz[i][j].local = i;				 		
				 			break;
				 	case 1: equipos[random].historial[j].contrincante = i+8;
				 			matriz[i][j].local = i+8;
				 			break;
				 	case 2:	equipos[random].historial[j+8].contrincante = i;
				 			matriz[i][j].local = i;
				 			break;
				 	case 3: equipos[random].historial[j+8].contrincante = i+4;
				 			matriz[i][j].local = i+4;
				 			break;		
				 	case 4: equipos[random].historial[j+8].contrincante = i+8;
				 			matriz[i][j].local = i+8;
				 			break;
				 	case 5: equipos[random].historial[j+8].contrincante = i+12;
				 			matriz[i][j].local = i+12;
				 			break;		
				 }
				 var=false;
				 cont=0;
				 
				}
			  	cont++;
			  	if(cont>100){

			  		Inicializar(matriz,rand_f);
			  		goto here;
			  	}
			}
			var=true;

		}
	}
}



void MostrarEquipo(Equipo equipos[]){
	
	for(int i=0;i<16;i++){
		for(int j=0;j<15;j++){
		  cout<<"equipo: "<<i<<" local "<<equipos[i].historial[j].contrincante<<endl;
		  
		}
		cout<<endl;
	}
}

float SumaDistancias(Fixture matriz[][8],Distancia dist[], Equipo equipos[],int lim_i_i,int lim_s_i,int lim_i_j,int lim_s_j){
	float distancia = 0;
	bool var= true;
	for(int i=lim_i_i;i<lim_s_i;i++){
		for(int j=lim_i_j;j<lim_s_j;j++){
			for(int k=0;k<120;k++){
				if(var && (dist[k].puntoA == i && dist[k].puntoB == equipos[i].historial[j].contrincante || dist[k].puntoA == equipos[i].historial[j].contrincante && dist[k].puntoB == i)){
					equipos[i].distancia_r = dist[k].distancia;
					var= false;
				}
				if((dist[k].puntoA == equipos[i].historial[j].contrincante && dist[k].puntoB == equipos[i].historial[j+1].contrincante)|| (dist[k].puntoB == equipos[i].historial[j].contrincante && dist[k].puntoA == equipos[i].historial[j+1].contrincante)){
					equipos[i].distancia_r = equipos[i].distancia_r + dist[k].distancia;
				}
				if(j== 6 && (dist[k].puntoA == i && dist[k].puntoB == equipos[i].historial[j+1].contrincante || dist[k].puntoA == equipos[i].historial[j+1].contrincante && dist[k].puntoB == i)){
					 equipos[i].distancia_r = equipos[i].distancia_r + dist[k].distancia;
				}
			}
		}
		distancia = distancia + equipos[i].distancia_r;
		var=true;
	}
	return distancia;

}
void CopiarMatriz(Fixture final[][8],Fixture matriz[][8], int a,int b,int c,int d){
	for(int i=a;i<b;i++){
		for(int j=c;j<d;j++){
			final[i][j] = matriz[i][j];
			/*final[i][j].local = i;
			final[i][j].visita = matriz[i][j].visita;*/
		}
	}
}

float ComprobarFinal(Fixture matriz[][8],Distancia dist[],Equipo equipos[],int a,int b,int c,int d,int aux,Fixture caminofinal[][8],int w,int x,int y,int z){
	float distancia_total;
	distancia_total = SumaDistancias(matriz,dist,equipos,a,b,c,d);
	
	if(distancia_total < aux){
		aux = distancia_total;
		CopiarMatriz(caminofinal,matriz,w,x,y,z);
	}
	return aux;
}
void MostrarmatrizFinal(Fixture final[][32],Equipo equipos[]){

	lxw_workbook  *workbook  = workbook_new("Fixture.xlsx");
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
    lxw_format *format = workbook_add_format(workbook);


    format_set_bold(format);
    int indice=0;
	for(int i=0;i<30;i++){
		for(int j=0;j<8;j++){
			worksheet_set_column(worksheet, i, j, 20, NULL);
			indice = final[j][i].visita;
			worksheet_write_number(worksheet, 0, i,i,format);
		  	worksheet_write_string(worksheet, j+1, i, equipos[indice].nombre_equipo.c_str(), format);					
		}
	}
	workbook_close(workbook);
}
void JuntarMatrices(Equipo equipos[],Fixture final[][32],Fixture matriz[][8],Fixture matriz2[][8],Fixture matriz3[][8],Fixture matriz4[][8],Fixture matriz5[][8],Fixture matriz6[][8],Fixture matriz7[][8]){
	bool var=true;
	while(var){
		for(int j=0;j<8;j++){
			for(int k=0;k<30;k++){
				if(k<8){
				final[j][k] = matriz[j][k];
				}
				if(k>=8 && k<16){
				final[j][k] = matriz2[j][k-8];					
				}
				if(j<4 && k>=16 && k<20){
				final[j][k] = matriz3[j][k-16];					
				}
				if(j>3 && k>=16 && k<20){
				final[j][k] = matriz4[j-4][k-16];					
				}
				if(j<4 && k>=20 && k<24){
				final[j][k] = matriz5[j][k-20];					
				}
				if(j>3 && k>=20 && k<24){
				final[j][k] = matriz6[j-4][k-20];					
				}
				if(k>=24){
				final[j][k] = matriz7[j][k-24];
				}
				if(k==29){
					var=false;
				}
			}
		}
	}
	MostrarmatrizFinal(final,equipos);
}

void UltimaMatriz(Equipo equipos[],Fixture matriz7[][8]){
	int cont = 12,indf=0,indi=0,indi2=0;
	for(int i=0;i<16;i+=4){
		equipos[i].historial[cont].contrincante = i+2;
		matriz7[indi][indf].visita = i;
		matriz7[indi][indf].local = i+2;
		equipos[i+1].historial[cont].contrincante = i+3;
		indi++;
		matriz7[indi][indf].visita = i+1;
		matriz7[indi][indf].local = i+3;
		indi++;	
	}
	//AgregarDistancias();
	indf++;
	indi=0;
	for(int i=2;i<16;i=i+4){
		equipos[i].historial[cont].contrincante = i-2;
		matriz7[indi][indf].visita = i;
		matriz7[indi][indf].local = i-2;
		equipos[i+1].historial[cont].contrincante = i-1;
		indi++;
		matriz7[indi][indf].visita = i+1;
		matriz7[indi][indf].local = i-1;
		indi++;

	}
	cont++;
	indf++;
	indi=0;
	for(int i=0;i<16;i=i+4){
		equipos[i].historial[cont].contrincante = i+3;
		matriz7[indi][indf].visita = i;
		matriz7[indi][indf].local = i+3;		
		equipos[i+1].historial[cont].contrincante = i+2;
		indi++;
		matriz7[indi][indf].visita = i+1;
		matriz7[indi][indf].local = i+2;
		indi++;
		equipos[i+3].historial[cont].contrincante = i;
		matriz7[indi2][indf+1].visita = i+3;
		matriz7[indi2][indf+1].local = i;
		equipos[i+2].historial[cont].contrincante = i+1;
		indi2++;
		matriz7[indi2][indf+1].visita = i+2;
		matriz7[indi2][indf+1].local = i+1;	
		indi2++;	
	}
	cont++;
	indf=indf+2;
	indi2=0;
	indi=0;
	for(int i=0;i<8;i++){
		equipos[2*i].historial[cont].contrincante = 2*i+1;
		matriz7[indi][indf].visita = 2*i;
		matriz7[indi][indf].local = 2*i+1;
		indi++;		
		equipos[2*i+1].historial[cont].contrincante = 2*i;
		matriz7[indi2][indf+1].visita = 2*i+1;
		matriz7[indi2][indf+1].local = 2*i;
		indi2++;		
	}
}

int main(int argc, char* argv[]){
	Equipo equipos[17];
	Distancia dist[130];
	Fixture matriz[8][8], matriz2[8][8],matriz3[8][8],matriz4[8][8],matriz5[8][8],matriz6[8][8], matriz7[8][8];
	Fixture camino1[8][8],camino2[8][8],camino3[8][8],camino4[8][8],camino5[8][8],camino6[8][8],camino7[8][8];
	Fixture final[16][32];
	float distancia[6], aux=100000000,distancia_total =0;

	//Inicializa la matriz de juegos en 0 para las comprobaciones
	Inicializar(matriz,8);
	Inicializar(matriz2,8);
	Inicializar(matriz3,4);	
	//MostrarMatriz(matriz);
	//Llena el array de equipos con los datos del fichero
	setEquipo(equipos,argv[1]);

	//Ordena los equipos en función a su longitud
	OrdenarEquipos(equipos);

	//Calcula todas las distancias por cada una de las 120 aristas para luego hacer la suma
	CalcularDistancias(equipos,dist);

	//Llena las matrices de 8x8
	for(int i=0;i<1000;i++){ 

	LlenarMatriz(matriz, equipos, 0,8,0,8,8,8,0);
	distancia[0] = ComprobarFinal(matriz,dist,equipos,0,8,0,7,aux,camino1,0,8,0,8);
	aux=distancia[0];
	//cout<<distancia[0]<<endl;
	}
	aux=100000000;
	for(int i=0;i<1000;i++){ 
	LlenarMatriz(matriz2, equipos, 0,8,0,8,0,8,1);
	distancia[1] = ComprobarFinal(matriz2,dist,equipos,8,16,7,15,aux,camino2,0,8,0,8);
	aux=distancia[1];
	//cout<<distancia[1]<<endl;

	}
	//MostrarMatriz(camino2);
	aux=100000000;

	//llena las matrices de 4x4
	for(int i=0;i<1000;i++){

	LlenarMatriz(matriz3, equipos, 0,4,0,4,4,4,2);
	distancia[2] = ComprobarFinal(matriz3,dist,equipos,0,4,0,3,aux,camino3,0,4,0,4);
	aux=distancia[2];
	//cout<<distancia[2]<<endl;

	}
	aux=100000000;
	for(int i=0;i<1000;i++){ 
	LlenarMatriz(matriz4, equipos, 0,4,0,4,0,4,3);
	distancia[3] = ComprobarFinal(matriz4,dist,equipos,4,8,3,7,aux,camino4,0,4,0,4);
	aux=distancia[3];
	//cout<<distancia[3]<<endl;

	}
	aux=100000000;
	for(int i=0;i<1000;i++){ 
	LlenarMatriz(matriz5, equipos, 0,4,0,4,12,4,4);
	distancia[4] = ComprobarFinal(matriz5,dist,equipos,8,12,0,3,aux,camino5,0,4,0,4);
	aux=distancia[4];
	//cout<<distancia[4]<<endl;

	}
	aux=100000000;
	for(int i=0;i<1000;i++){ 
	LlenarMatriz(matriz6, equipos, 0,4,0,4,8,4,5);
	distancia[5] = ComprobarFinal(matriz6,dist,equipos,12,16,3,7,aux,camino6,0,4,0,4);
	aux=distancia[5];
	}
	UltimaMatriz(equipos,matriz7);
	for(int i=0;i<1000;i++){
	distancia[6] = ComprobarFinal(matriz7,dist,equipos,0,8,12,15,aux,camino7,0,4,0,4);
	aux=distancia[6];	
	}
	//MostrarMatriz(camino5);
	JuntarMatrices(equipos,final,camino1,camino2,camino3,camino4,camino5,camino6,matriz7);
	for(int i=0;i<7;i++){
		distancia_total = distancia[i] + distancia_total;
	}
	cout<<distancia_total<<endl;
}