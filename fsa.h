// PL homework: hw1
// fsa.h

#ifndef _PL_HOMEWORK_FSA_H_
#define _PL_HOMEWORK_FSA_H_

#include <map>
#include <set>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <algorithm>
using namespace std;

typedef struct TableElement
{
	int state;
	char input_char;
	int next_state;
};

typedef struct FiniteStateAutomaton
{
	//초기 상태
	set<int> InitialState;

	//목표상태
	set<int> acceptState;

	//(상태,입력값) 쌍을 key로 가지고
	//해당 상태에서 입력값을 받아 전이한 상태를 value로 가지는 map 선언
	map < pair< set<int>, char >, set<int> > graph;

	//NFA 그래프를 등록하는 MAP
	map< pair<int, char>, set<int> >NFA_graph;
};

// Homework 1.1
bool RunFSA(const FiniteStateAutomaton* fsa, const char* str);

// Homework 1.1 and 1.2
bool BuildFSA(const TableElement* elements, int num_elements,
	const int* accept_states, int num_accept_states,
	FiniteStateAutomaton* fsa);

set<int> GetEpsilonClosure(map< pair<int, char>, set<int> > graph,
	set<int> state, map< set<int>, bool > check);

set<int> GetConnectedClosure(map< pair<int, char>, set<int> > graph,
	set<int> closure, char used_character);

void NFAtoDFA(set<int> CurrentState, set<char> UsedCharacter, 
	FiniteStateAutomaton* fsa, map< set<int>, bool > Check);
// Homework 1.3
bool BuildFSA(const char* regex, FiniteStateAutomaton* fsa);

#endif //_PL_HOMEWORK_FSA_H_

