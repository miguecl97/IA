/*
 * vivalavida.cpp
 *
 *  Created on: 15 ene. 2018
 *      Author: miguel
 */

#include "vivalavida.h"

#include <string>
#include <cstdlib>
#include <iostream>
using namespace std;

vivalavida::vivalavida() {
	// Inicializar las variables necesarias para ejecutar la partida

}

vivalavida::~vivalavida() {
	// Liberar los recursos reservados (memoria, ficheros, etc.)
}

void vivalavida::initialize() {
	// Inicializar el bot antes de jugar una partida
}

string vivalavida::getName() {
	return "vivalavida"; // Sustituir por el nombre del bot
}

Move vivalavida::nextMove(const vector<Move> &adversary, const GameState &state){

	Move movimiento,move;
	int max;
	int profundidad = 8;
	int alpha = -99999;
	int beta = 99999;
	int bestMove = 1;
	Player jugadorBot = state.getCurrentPlayer();

		GameState hijo = state.simulateMove((Move) 1);
		max = alpha_beta( hijo, profundidad-1, alpha , beta, jugadorBot);
		for(int i = 2 ; i <= 6 ; ++i){
			GameState hijo = state.simulateMove((Move) i);
			alpha = alpha_beta( hijo, profundidad-1, alpha , beta, jugadorBot);
			//cerr << " Valor de alpha en nextmove iteracion " << i << " es " << alpha << "\n\n";
			if( alpha > max){
				bestMove = i;
				max = alpha;
			}
	 }
	movimiento = (Move) bestMove;
	return movimiento;
}

int vivalavida::alpha_beta(GameState &status, int profundidad, int alpha , int beta, Player current){
	
		
	if(profundidad == 0 || status.isFinalState()){
		if(current==J1)
			return getScore(J1,J2,status);
		else
			return getScore(J2,J1,status);
	}

	if(status.getCurrentPlayer() == current ){
		for(int i=1; i <= 6; ++i){
			GameState hijo = status.simulateMove((Move) i);
			alpha = max(alpha, alpha_beta(hijo, profundidad-1, alpha, beta, current));
			if(beta <= alpha)
				break;
		}
		return alpha;
	}
	
	else{
		for(int i=1; i <= 6; ++i){
			GameState hijo = status.simulateMove((Move) i);
			beta = min(beta, alpha_beta(hijo, profundidad-1, alpha, beta, current));
			if(beta <= alpha)
				break;
		}		
		return beta;
	}

}

int vivalavida::getScore(Player p1, Player p2, GameState &status){
		int p1score = 0;
		int p2score = 0;

		if(status.isFinalState()){
			Player winner = status.getWinner();
			if(winner == p1)
				p1score = 500;
			if(winner == p2)
				p2score = 500;
		}

		p1score = p1score + status.getScore(p1) - status.getScore(p2);
		p2score = p2score + status.getScore(p2) - status.getScore(p1);

		return p1score - p2score;

}


