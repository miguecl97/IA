/*
 * vivalavida.h
 *
 *  Created on: 15 ene. 2018
 *      Author: miguel
 */

#include "Bot.h"
#include <list>

#ifndef MANUPCBOT_H_
#define MANUPCBOT_H_

class vivalavida:Bot {
public:
	vivalavida();
	~vivalavida();


	void initialize();
	string getName();
	Move nextMove(const vector<Move> &adversary, const GameState &state);
	int alpha_beta( GameState &status, int profundidad, int alpha , int beta, Player current);
	int getScore(Player p1, Player p2, GameState &status);
};

#endif /* MANUPCBOT_H_ */
