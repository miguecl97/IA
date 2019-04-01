#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>


bool estado::operator == (const estado& status){
	return (fila == status.fila && columna == status.columna);
}

Node::Node(){
		father = nullptr;
		position.fila = 99;
		position.columna = 99;
		objective.fila = 99;
		objective.columna = 99;
		g=h=0;
		movement = actIDLE;
		brujula=0;
		walkable = true;	
	}

Node::Node(estado start, estado end){
		father = nullptr;
		position.fila = start.fila;
		position.columna = start.columna;
		objective.fila = end.fila;
		objective.columna = end.columna;
		g=h=0;
		movement = actIDLE;
		brujula=start.orientacion;
		walkable = true;
}

Node::Node(Node* antecessor, Action accion){
		father = antecessor;
		position.fila = antecessor->position.fila;
		position.columna = antecessor->position.columna;
		objective.fila = antecessor->objective.fila;
		objective.columna = antecessor->objective.columna;
		h=0;
		g = father->g+1;
		brujula = antecessor->brujula;
		movement = accion;
		walkable = false;
		if(accion == actFORWARD){
			if(brujula == 0)
					position.fila = antecessor->position.fila-1;
			if(brujula == 1 )
					position.columna = antecessor->position.columna+1;
			if(brujula == 2 )
					position.fila = antecessor->position.fila+1;
			if(brujula == 3)
					position.columna = antecessor->position.columna-1;
		}
		if(accion == actTURN_R){
			brujula= (brujula+1) % 4;
		}
		if(accion == actTURN_L){
			brujula= (brujula+3) % 4;
		}
}

double Node::getScore(){
	return g+h;
}

void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}


void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}

void ComportamientoJugador::VisualizaPlan(const estado &st, list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst;
	cst.fila = st.fila;
	cst.columna = st.columna;
	cst.orientacion = 0;

	auto it = plan.begin();
	PintaPlan(plan);
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else if (*it == actTURN_L){
			cst.orientacion = (cst.orientacion+3)%4;
		}		
		it++;
	}
}


bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {
	plan.clear();
 	bool encontrado = false;
	Astar(plan,encontrado,origen,destino);
	//VisualizaPlan(origen, plan);
	return encontrado;  
}

Action ComportamientoJugador::think(Sensores sensores) {
  Action action = actIDLE;
	//descubrirMapa(sensores);
 	estado origen;
	if(nivel3){
		if(!foundPK){
				action = findPK(sensores,foundPK);
				if(sensores.mensajeF != -1){
					fil = sensores.mensajeF;
					col = sensores.mensajeC;
		
					origen.fila = fil;
					origen.columna = col;
					origen.orientacion = brujula;

					destino.fila = sensores.destinoF;
					destino.columna = sensores.destinoC;

			}
		}
		else{
				descubrirMapa(sensores);
				if(sensores.destinoF!=destino.fila && sensores.destinoC != destino.columna){
						hayPlan = false;
				}
				if (!hayPlan){
					origen.fila = fil;
					origen.columna = col;
					origen.orientacion = brujula;

					destino.fila = sensores.destinoF;
					destino.columna = sensores.destinoC;
	
    			hayPlan = pathFinding(origen,destino,plan);

				}
				if (hayPlan and plan.size()>0){
						if(sensores.superficie[2] == 'a'){
							aldeano(fil,col,brujula);
							hayPlan=false;
							action=actIDLE;
						}
						if(hayPlan){
							action = plan.front();
						if(action == actFORWARD && (sensores.terreno[2] == 'A' || sensores.superficie[2] == 'a' || sensores.terreno[2] == 'P' || sensores.terreno[2] == 'B' || sensores.terreno[2] == 'M')){	
							action= actIDLE;
							hayPlan = false;
						}
							plan.pop_front();
						}
					}

		}
				ActualizarEstado(action);
				return action;
	

	}else{

		if (sensores.mensajeF != -1){
			fil = sensores.mensajeF;
			col = sensores.mensajeC;
		
			origen.fila = fil;
			origen.columna = col;
			origen.orientacion = brujula;

			destino.fila = sensores.destinoF;
			destino.columna = sensores.destinoC;
	
		}

		if(sensores.reset == true || sensores.colision == true){
			hayPlan = false;
		}

		// Determinar si ha cambiado el destino desde la ultima planificacion
		if (hayPlan and (sensores.destinoF != destino.fila or sensores.destinoC != destino.columna)){
			hayPlan = false;
		}

		//Si no hay plan construyo uno nuevo
		if (!hayPlan){

			estado origen;
			origen.fila = fil;
			origen.columna = col;
			origen.orientacion = brujula;

			destino.fila = sensores.destinoF;
			destino.columna = sensores.destinoC;
		 
			hayPlan = pathFinding(origen,destino,plan);


		}

		if (hayPlan and plan.size()>0){

			if(sensores.superficie[2] == 'a'){
				aldeano(fil,col,brujula);
				hayPlan=false;
				action=actIDLE;
			}

			if(hayPlan){
			action = plan.front();
			ActualizarEstado(action);
			plan.pop_front();
			}

		}

		return action;
		}
}


int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
void ComportamientoJugador::orientaNorte(list<Action> &plan){
		if(brujula==0)
			plan.push_front(actIDLE);
		if(brujula==1)
			plan.push_front(actTURN_L);
		if(brujula==2)
			plan.push_front(actTURN_L);
			plan.push_front(actTURN_L);
		if(brujula==3)
			plan.push_front(actTURN_R);
	}
void ComportamientoJugador::ActualizarEstado(Action &action){
		if(action == actFORWARD)
			Avanzar();
		if(action == actTURN_R)
			Derecha();
		if(action == actTURN_L)
			Izquierda();
}
void ComportamientoJugador::Avanzar(){
	if (brujula == 0)
		fil= --fil;
	if (brujula == 1)
		col = ++col;
	if (brujula == 2)
		fil = ++fil;
	if (brujula == 3)
		col = --col;

}

void ComportamientoJugador::Derecha(){
	brujula= (brujula+1) % 4;

}

void ComportamientoJugador::Izquierda(){
	brujula= ((brujula-1)+4) % 4;
}

void ComportamientoJugador::Astar( list<Action> &plan,bool &encontrado, estado ini, estado end){
	plan.clear();

	Node *root = new Node(ini, end);
	Node *current = nullptr;

	
	std::list<Node*> abiertos;
	
	std::list<Node*> cerrados;

	abiertos.push_back(root);
	while(!abiertos.empty()){
		current = *abiertos.begin();
		for(auto node : abiertos){
			node->h = heuristica(node->position,node->objective);
			if(node->getScore() <= current->getScore() && isWalkable(node))
					current = node;
		}
	
		double totalCost = current->g;
		if(current->position == end){
			encontrado = true;
			break;
		}
		
		cerrados.push_back(current);
		abiertos.remove(current);
		std::list<Node*> successors;
		expandNode(current, successors);
		
		for(auto node : successors){
			if(findNodeOnList(node, abiertos)==false && findNodeOnList(node, cerrados)==false)
					abiertos.push_back(node);
			else if(totalCost < node->g){
						node->father = current;
						node->g = totalCost;
			}
		}
	}

		while(current != nullptr){
		
			plan.push_front(current->movement);
			current = current->father;
		
		}
		abiertos.clear();
		cerrados.clear();

		
}

void ComportamientoJugador::expandNode(Node* nodo, std::list<Node *> &successors){
		//limpiar succesors	
				successors.clear();	
		//comienzo a expandir los hijos
				Node *son1 = new Node(nodo, actFORWARD);
				if (isWalkable(son1) == true)
					successors.push_back(son1);
				
				Node *son2 = new Node(nodo, actTURN_R);
				if (isWalkable(son2) == true)
					successors.push_back(son2);
				
				Node *son3 = new Node(nodo, actTURN_L);
				if (isWalkable(son3) == true)
					successors.push_back(son3);

	}


double ComportamientoJugador::heuristica(estado start, estado end){
			double heuristica = 0;
			if(mapaResultado[start.fila][start.columna] == '?'){
				heuristica = 1000 *  abs(sqrt(pow(abs(start.fila - end.fila) + abs(start.columna - end.columna),2)));
			}else 
				heuristica = abs(sqrt(pow(abs(start.fila - end.fila) + abs(start.columna - end.columna),2)));
			

			return heuristica;
	}

void ComportamientoJugador::releaseNodes(std::list<Node *> &nodes_)
{
    for (auto it = nodes_.begin(); it != nodes_.end();) {
        delete *it;
        it = nodes_.erase(it);
    }
}
Action ComportamientoJugador::findPK(Sensores sensores, bool &foundPK){
			Action accion;
	
			if(sensores.terreno[0] == 'K'){
				foundPK=true;
				return actIDLE;
			}
			if(sensores.terreno[2] == 'K'){
				return actFORWARD;
			}
			if(sensores.terreno[6] == 'K' && (sensores.terreno[2] == 'S' || sensores.terreno[2] == 'T')){
				return actFORWARD;
			}
			if(sensores.terreno[12] == 'K' && (sensores.terreno[2] == 'S' || sensores.terreno[2] == 'T')){
				return actFORWARD;
			}

			int cual = aleatorio(8);
			switch (cual) {
			case 0: 
				if (sensores.terreno[2] != 'P' && sensores.terreno[2] != 'A' && sensores.terreno[2] != 'B' && sensores.terreno[2] != 'M' && sensores.terreno[2] != 'D') 
						accion = actFORWARD; break;
			case 1: accion = actTURN_L; break;
			case 2: accion = actTURN_R; break;
			case 3: case 4: case 5: case 6: case 7: 
				if (sensores.terreno[2] != 'P' && sensores.terreno[2] != 'A' && sensores.terreno[2] != 'B' && sensores.terreno[2] != 'M' && sensores.terreno[2] != 'D') accion = actFORWARD;

  }

			return accion;

}
bool ComportamientoJugador::isWalkable(Node *nodo){
			nodo -> walkable = false;
			char punto = mapaResultado[nodo->position.fila][nodo->position.columna];
			if(punto == 'S' || punto == 'T' || punto == 'K' || punto == '?' ){
				
				nodo->walkable= true;
			}
		
		return nodo->walkable;
}

bool ComportamientoJugador::findNodeOnList(Node *nodo, std::list<Node *> &nodes_){
 	bool found = false;
	for (auto node : nodes_){
     if (node->position == nodo->position && node->brujula == nodo->brujula ) {
         found = true;
        }
    }
	return found;
}
void ComportamientoJugador::aldeano(int fila, int columna, int brujula){
		if(brujula == 0)
				mapaResultado[fila-1][columna] = 'a';
		if(brujula == 1)
				mapaResultado[fila][columna+1] = 'a';
		if(brujula == 2)
				mapaResultado[fila+1][columna] = 'a';
		if(brujula == 3)
				mapaResultado[fila][columna-1] = 'a';
}


void ComportamientoJugador::descubrirMapa(Sensores &sensores){
	mapaResultado[fil][col] = sensores.terreno[0];

	if (brujula == 0){
		mapaResultado[fil-1][col-1] = sensores.terreno[1];
		mapaResultado[fil-1][col] = sensores.terreno[2];
		mapaResultado[fil-1][col+1] = sensores.terreno[3];

		mapaResultado[fil-2][col-2] = sensores.terreno[4];
		mapaResultado[fil-2][col-1] = sensores.terreno[5];
		mapaResultado[fil-2][col] = sensores.terreno[6];
		mapaResultado[fil-2][col+1] = sensores.terreno[7];
		mapaResultado[fil-2][col+2] = sensores.terreno[8];

		mapaResultado[fil-3][col-3] = sensores.terreno[9];
		mapaResultado[fil-3][col-2] = sensores.terreno[10];
		mapaResultado[fil-3][col-1] = sensores.terreno[11];
		mapaResultado[fil-3][col] = sensores.terreno[12];
		mapaResultado[fil-3][col+1] = sensores.terreno[13];
		mapaResultado[fil-3][col+2] = sensores.terreno[14];
		mapaResultado[fil-3][col+3] = sensores.terreno[15];
	}
	else
		if (brujula == 1){
			mapaResultado[fil-1][col+1] = sensores.terreno[1];
			mapaResultado[fil][col+1] = sensores.terreno[2];
			mapaResultado[fil+1][col+1] = sensores.terreno[3];

			mapaResultado[fil-2][col+2] = sensores.terreno[4];
			mapaResultado[fil-1][col+2] = sensores.terreno[5];
			mapaResultado[fil][col+2] = sensores.terreno[6];
			mapaResultado[fil+1][col+2] = sensores.terreno[7];
			mapaResultado[fil+2][col+2] = sensores.terreno[8];

			mapaResultado[fil-3][col+3] = sensores.terreno[9];
			mapaResultado[fil-2][col+3] = sensores.terreno[10];
			mapaResultado[fil-1][col+3] = sensores.terreno[11];
			mapaResultado[fil][col+3] = sensores.terreno[12];
			mapaResultado[fil+1][col+3] = sensores.terreno[13];
			mapaResultado[fil+2][col+3] = sensores.terreno[14];
			mapaResultado[fil+3][col+3] = sensores.terreno[15];
		}
		else
			if (brujula == 2){
				mapaResultado[fil+1][col+1] = sensores.terreno[1];
				mapaResultado[fil+1][col] = sensores.terreno[2];
				mapaResultado[fil+1][col-1] = sensores.terreno[3];

				mapaResultado[fil+2][col+2] = sensores.terreno[4];
				mapaResultado[fil+2][col+1] = sensores.terreno[5];
				mapaResultado[fil+2][col] = sensores.terreno[6];
				mapaResultado[fil+2][col-1] = sensores.terreno[7];
				mapaResultado[fil+2][col-2] = sensores.terreno[8];

				mapaResultado[fil+3][col+3] = sensores.terreno[9];
				mapaResultado[fil+3][col+2] = sensores.terreno[10];
				mapaResultado[fil+3][col+1] = sensores.terreno[11];
				mapaResultado[fil+3][col] = sensores.terreno[12];
				mapaResultado[fil+3][col-1] = sensores.terreno[13];
				mapaResultado[fil+3][col-2] = sensores.terreno[14];
				mapaResultado[fil+3][col-3] = sensores.terreno[15];
			}
			else
				if (brujula == 3){
					mapaResultado[fil+1][col-1] = sensores.terreno[1];
					mapaResultado[fil][col-1] = sensores.terreno[2];
					mapaResultado[fil-1][col-1] = sensores.terreno[3];

					mapaResultado[fil+2][col-2] = sensores.terreno[4];
					mapaResultado[fil+1][col-2] = sensores.terreno[5];
					mapaResultado[fil][col-2] = sensores.terreno[6];
					mapaResultado[fil-1][col-2] = sensores.terreno[7];
					mapaResultado[fil-2][col-2] = sensores.terreno[8];

					mapaResultado[fil+3][col-3] = sensores.terreno[9];
					mapaResultado[fil+2][col-3] = sensores.terreno[10];
					mapaResultado[fil+1][col-3] = sensores.terreno[11];
					mapaResultado[fil][col-3] = sensores.terreno[12];
					mapaResultado[fil-1][col-3] = sensores.terreno[13];
					mapaResultado[fil-2][col-3] = sensores.terreno[14];
					mapaResultado[fil-3][col-3] = sensores.terreno[15];
				}
}
