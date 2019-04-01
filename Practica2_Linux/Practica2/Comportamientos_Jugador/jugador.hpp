#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <set>
#include <list>
#include <algorithm>

struct estado {
  int fila;
  int columna;
  int orientacion;
	bool operator == (const estado& status);
};

struct Node {  
	Node *father;
  estado position;
  estado objective;
  double g;
	double h;
	int brujula;
	bool walkable;
	Action movement;
	Node();
  Node(estado start, estado end);
	Node(Node* antecessor, Action accion);
	double getScore();
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
			hayPlan = false;
			nivel3=true;
			foundPK=false;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;	
			hayPlan = false;
			nivel3=false;
			matriz=mapaR;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}
    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}
		list<Action> plan;
		std::vector< std::vector< unsigned char> > matriz;
  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado destino;
  //  list<Action> plan;
		vector<estado> coordenadas;
		bool hayPlan;
		bool nivel3;
		bool foundPK;

    bool pathFinding(const estado &origen, const estado &destino, list<Action> &plan);
    void PintaPlan(list<Action> plan);
		void orientaNorte(list<Action> &plan);
		void ActualizarEstado(Action &action);
	  void Izquierda();
	  void Derecha();
		void Avanzar();
		void Astar( list<Action> &plan,bool &encontrado,  estado ini, estado end);
		void expandNode(Node *nodo, std::list<Node *> &successors);
		double heuristica(estado start, estado end);
		void releaseNodes(std::list<Node *> &nodes_);
		bool isWalkable(Node *nodo);
		bool findNodeOnList(Node * nodo, std::list<Node *> &nodes_);
		Action findPK(Sensores sensores, bool &foundPK);
		void descubrirMapa(Sensores &sensores);
		void aldeano(int fila, int columna, int brujula);
};

#endif
