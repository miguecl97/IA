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
	int profundidad = 4;
	int alpha = -99999;
	int beta = 99999;
	int bestMove = 1;
	Player jugadorBot = state.getCurrentPlayer();

		GameState hijo = state.simulateMove((Move) 1);
		max = alpha_beta( hijo, profundidad-1, alpha , beta, jugadorBot);
		for(int i = 2 ; i <= 6 ; ++i){
			GameState hijo = state.simulateMove((Move) i);
			alpha = alpha_beta( hijo, profundidad-1, alpha , beta, jugadorBot);
			cerr << "\nValor de alpha en nextmove iteracion " << i << " es " << alpha << "\n\n";
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
		int score= 0;		
		int p1score = 0; 
		int p2score =	0; 
		p1score = count_seeds(status,1);
		p2score = count_seeds(status,2);
		int ply,oponent;
		cerr << "\n Semillas del turno " << p1score << "\n\n";
		cerr << "\n Semillas del oponente " << p2score << "\n\n";
		
		if(p1 == J1){
			ply = 0;
			oponent = 1;
		}
		else{
			ply = 1;
			oponent = 0; 
		}

		if(status.isFinalState()){
			Player winner = status.getWinner();
			if(winner == p1)
				p1score += 1000;
			if(winner == p2)
				p2score += 1000;
		}

		int i, countEmpty=0, mostOpponent = 0, aiMin = 9999, aiMost = -9999;
    for(i = 1; i < 7; i++) {
        if(status.getSeedsAt((Player) ply,(Position) i) < aiMin) {
            aiMin = status.getSeedsAt((Player) ply,(Position) i);
        }
        if(status.getSeedsAt((Player) ply,(Position) i) > aiMost) {
            aiMost = status.getSeedsAt((Player) ply,(Position) i);
        }
        if(status.getSeedsAt((Player) ply,(Position) i) == 0) {
            if(status.getSeedsAt((Player) oponent,(Position) i) > 0) {
                
                //do small cost checks
                int canSteal = 0;
                for(int j = 0; j < 6; j++) {
                    if((status.getSeedsAt((Player) ply,(Position) i) - (i-j)) % 13 == 0) {
                        canSteal = 1;
                    }
                }
                if(canSteal) {
                    score += status.getSeedsAt((Player) oponent,(Position) i);
                }
            }
            countEmpty++;
        }
				//cerr << "MItadr de get score .--- " << score << "\n\n";
        if(status.getSeedsAt((Player) oponent,(Position) i)> mostOpponent)
            mostOpponent = status.getSeedsAt((Player) oponent,(Position) i);
        if(status.getSeedsAt((Player) oponent,(Position) i) == 0) {
            int canOpponentSteal = 0;
            if(status.getSeedsAt((Player) oponent,(Position) i) > 5) {
                for(int j = 1; j < 7; j++) {
                    if((status.getSeedsAt((Player) oponent,(Position) j) - (i-j)) % 13 == 0) {
                        canOpponentSteal = 1;
                    }
                }
                if(canOpponentSteal) {
                    score -= status.getSeedsAt((Player) ply, (Position) i);
                }
            }
        }
    }
   //	cerr << "casi final de get score .--- " << score << "\n\n";
    if((aiMost - aiMin) > 10) {
        score--;
    }
    
    
    float ratio = (float)p1score/p2score;
    if(ratio > .72) {
        score-=mostOpponent*2;
    }
    
    float inverseRatio = (float)p1score/p2score;
    
    if(inverseRatio > .85) {
        score-=mostOpponent/2;
    }
    
    score-=get_free_turns_for_playerX(status, 2);
    score+=get_free_turns_for_playerX(status, 1);
    
		if(p1 == J2) score *= -1;
		//cerr << "Antes de salir de get score .--- " << score << "\n\n";
		return score;

}

int vivalavida::count_seeds(GameState &status, int pNum){
    int sum = 0;
		int current;

		if(pNum == 1)
			current = 0;
		else
			current = 1; 

	  for(int i = 1; i < 7; i++) {
            sum = sum + status.getSeedsAt((Player) current,(Position) i);
        }
    
}

int vivalavida::get_free_turns_for_playerX(GameState &status, int pNum) {
    int count = 0;
		int current;

		if(pNum == 1)
			current = 0;
		else
			current = 1; 

    for(int i = 1; i < 7; i++) {
      if(status.getSeedsAt((Player) current,(Position) i) == (6-i)) {
        count++;
            }
        }
 
    return count;
}

