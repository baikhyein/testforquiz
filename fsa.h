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
	//�ʱ� ����
	set<int> InitialState;

	//��ǥ����
	set<int> acceptState;

	//(����,�Է°�) ���� key�� ������
	//�ش� ���¿��� �Է°��� �޾� ������ ���¸� value�� ������ map ����
	map < pair< set<int>, char >, set<int> > graph;

	//NFA �׷����� ����ϴ� MAP
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

