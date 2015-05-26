// PL homework: hw1
// fsa.cc

#include <iostream>

#include "fsa.h"

#define DISABLE_LOG true
#define LOG \
    if (DISABLE_LOG) {} else std::cerr

const char kEps = '#';

#define ITR map< pair< set<int>, char > ,set<int> >::iterator
#define NFA_ITR map< pair<int,char>, set<int> >::iterator
using namespace std;

bool CheckIfNFA(const TableElement* elements, int num_elements)
{
	//NFA여부를 확인하기 위한 map
	map< pair<int, char>, int> checkMap;

	for (int i = 0; i < num_elements; i++)
	{
		//전이 조건으로 # 등장시 NFA
		if (elements->input_char == kEps)
			return true;

		//KEY 값 생성
		int current_state = elements->state;
		char input_char = elements->input_char;
		pair<int, char> key(current_state, input_char);

		//VALUE값 생성
		int value = elements->next_state;

		map< pair<int, char>, int>::iterator itr = checkMap.find(key);
		//중복된 key값 발견
		if (itr != checkMap.end())
		{
			//같은 key값에 다른 value값을 가진다면 NFA
			if (itr->second != value)
				return true;
		}

		//등록된 key값이 없다면 등록한다.
		else
			checkMap[key] = value;

		elements++;
	}

	return false;
}

bool BuildDFA(const TableElement* elements, int num_elements,
	const int* accept_states, int num_accept_states,
	FiniteStateAutomaton* fsa)
{
	// Implement this function.
	LOG << "num_elements: " << num_elements << endl;
	if (num_elements <= 0) return false;

	//LOGIC 그래프 등록
	for (int i = 0; i < num_elements; i++)
	{
		if (i == 0)
			fsa->InitialState.insert(elements->state);

		set<int>prev_state;
		prev_state.insert(elements->state);

		set<int>next_state;
		next_state.insert(elements->next_state);
		//set<int>prev_state(1, elements->state);
		//set<int>next_state(1, elements->next_state);
		pair< set<int>, char > push(prev_state, elements->input_char);

		fsa->graph[push] = next_state;
		elements++;
	}

	//ACCEPT STATE에 등록
	for (int i = 0; i < num_accept_states; i++)
	{
		fsa->acceptState.insert(*accept_states);
		accept_states++;
	}

	return true;
}


bool BuildNFA(const TableElement* elements, int num_elements,
	const int* accept_states_array, int num_accept_states,
	FiniteStateAutomaton* fsa)
{
	set<int> InitialState;
	set<char> used_character;
	//NFA 그래프에 등록
	for (int i = 0; i < num_elements; i++)
	{
		pair<int, char> key(elements->state, elements->input_char);
		set<int> value;
		value.insert(elements->next_state);

		if (i == 0)
			InitialState.insert(elements->state);
		//사용된 모든 엣지값 등록
		if (elements->input_char != '#')
			used_character.insert(elements->input_char);

		//이미 등록된 key값이 있다면
		if (fsa->NFA_graph.find(key) != fsa->NFA_graph.end())
		{
			//VALUE값에 추가 삽입
			fsa->NFA_graph[key].insert(elements->next_state);
		}

		//이미 등록된 key값이 없다면
		else
		{
			//새로운 엣지와 정점 추가
			fsa->NFA_graph[key] = value;
		}
		elements++;
	}

	//ACCEPT STATE 등록
	for (int i = 0; i < num_accept_states; i++)
	{
		fsa->acceptState.insert(*accept_states_array);
		accept_states_array++;
	}


	/* NFA -> DFA 변환*/


	//입실론 클로저용 체크맵 선언
	map< set<int>, bool > EpsilonChecker;

	//NFA를 DFA로 변환한 그래프에서
	//초기 클로저를 가져온다
	//initial state에서 입실론으로 연결된 클로저를 찾는 것.
	
	set<int> InitialClosure = GetEpsilonClosure(fsa->NFA_graph, InitialState,EpsilonChecker);
	set<int> CurrentClosure = InitialClosure;

	//InitialState 갱신
	fsa->InitialState = InitialClosure;


	//DFA 그래프 생성용 체크맵
	map< set<int>, bool >Checker;

	//DFA 그래프 생성
	NFAtoDFA(CurrentClosure, used_character, fsa, Checker);

	// 그래프 출력
	/*
	for (ITR itr = fsa->graph.begin(); itr != fsa->graph.end(); itr++)
	{
		printf("state : { ");
		for (set<int>::iterator myitr = itr->first.first.begin(); myitr != itr->first.first.end(); myitr++)
			printf("%d ,", *myitr);
		printf("}  to %c state : {", itr->first.second);

		for (set<int>::iterator myitr = itr->second.begin(); myitr != itr->second.end(); myitr++)
			printf("%d ,", *myitr);
		printf("}\n");
		
	}
	*/
	// Implement this function.
	return true;
}

// Homework 1.1
bool RunFSA(const FiniteStateAutomaton* fsa, const char* str)
{
	//현재 state
	set<int> current_state = fsa->InitialState;
	//set<int> current_state(1, fsa->initial_state);

	//최종 state
	set<int> accept_state = fsa->acceptState;

	//graph 를 가져온다
	map< pair< set<int>, char >, set<int> >graph = fsa->graph;

	//현재 상태와 입력값의 조합으로 얻어지는 다음 상태
	set<int>next_state;

	//입력값에 따라서 상태를 전이한다
	for (int i = 0; i < strlen(str); i++)
	{
		//key값으로 value 값을 확인하여
		//해당 상태에서 주어진 입력으로 전이할 수 있는 상태가 있는지 확인
		pair< set<int>, char > key(current_state, str[i]);
		ITR itr = graph.find(key);


		//전이할 상태가 없는경우
		if (itr == graph.end())
			return false;

		//전이할 상태가 있는경우
		//현재 상태를 전이한 상태로 바꾼다
		current_state = itr->second;
	}

	//마지막에 도달한 상태의 요소들을 순회하여
	//accept state와 일치하는 것이 하나라도 있는경우에 
	//true 반환
	for (set<int>::iterator i = current_state.begin(); i != current_state.end(); i++)
		for (set<int>::iterator j = accept_state.begin(); j != accept_state.end(); j++)
			if (*i == *j)
				return true;

	// 마지막 상태에 accept state 요소가 없는경우
	return false;
}

// Homework 1.1 and 1.2
bool BuildFSA(const TableElement* elements, int num_elements,
	const int* accept_states, int num_accepts,
	FiniteStateAutomaton* fsa)
{
	if (CheckIfNFA(elements, num_elements))
	{
		return BuildNFA(elements, num_elements, accept_states, num_accepts, fsa);
	}
	else
	{
		return BuildDFA(elements, num_elements, accept_states, num_accepts, fsa);
	}
}

//그래프와 노드(state) 가 주어졌을 때
//해당 state와 입실론으로 연결된 요소들의 합집합을 반환한다.
set<int> GetEpsilonClosure(map< pair<int, char>, set<int> > graph,
	set<int> state, map< set<int>, bool > check)
{

	set<int> closure;

	//state들의 모든 요소들에 대해서
	for (set<int>::iterator itr = state.begin(); itr != state.end(); itr++)
	{
		//각 요소에서 문자로 연결된 노드가 있는지 확인한다
		pair<int, char> key(*itr, '#');

		//확인 = #으로 연결되는 노드가 있고 아직 방문하지 않은 노드라면
		if (graph.find(key) != graph.end() && check.find(graph[key]) == check.end())
		{
			//해당 노드에서 함수를 재귀호출한다.
			set<int> child_closure = GetEpsilonClosure(graph, graph[key],check);
			check[graph[key]] = true;
			//자식 클로저에서 뻗어나가는 입실론 노드들이 존재한다면
			if (!child_closure.empty())
			{
				//해당 노드들을 추가한다
				for (set<int>::iterator itr = child_closure.begin()
					; itr != child_closure.end(); itr++)
				{
					closure.insert(*itr);
				}
			}
		}
	}


	//자기자신을 추가
	for (set<int>::iterator itr = state.begin(); itr != state.end(); itr++)
		closure.insert(*itr);

	return closure;
}

//그래프와 state가 주어질 때, state에서 character로 연결된 노드들의 합집합을 구한다.
set<int> GetConnectedClosure(map< pair<int, char>, set<int> > graph,
	set<int> closure, char used_character)
{
	set<int> ConnectedClosure;

	for (set<int>::iterator itr = closure.begin()
		; itr != closure.end(); itr++)
	{
		//클로저의 각 요소 생성
		int ClosureElem = *itr;

		//사용된 문자로 탐색
		//각 요소,문자로 연결된 노드가 있는지 확인
		//연결된 노드가 있다면 
		pair<int, char> key(ClosureElem, used_character);
		if (graph.find(key) != graph.end())
		{
			//추가
			for (set<int>::iterator itr = graph[key].begin(); itr != graph[key].end(); itr++)
				ConnectedClosure.insert(*itr);
		}
	}

	return ConnectedClosure;
}


//현재 State와 사용된 문자들이 주어질 때
//현재 State로부터 시작하는 DFA를 구성한다.
void NFAtoDFA(set<int> CurrentState, set<char> UsedCharacter, FiniteStateAutomaton* fsa,
	map< set<int>, bool > Check)
{
	
	for (set<char>::iterator itr = UsedCharacter.begin(); itr != UsedCharacter.end(); itr++)
	{
		//하나의 문자로 CONNECTED CLOSURE 검색
		set<int> ConnectedCloser = GetConnectedClosure(fsa->NFA_graph, CurrentState, *itr);
		
		//입실론 CONNECTED CLOSURE 검색용 체크맵
		map< set<int>, bool > EpsilonChecker;

		//연결된 클로저가 없으면 무시
		if (ConnectedCloser.empty())
			continue;

		//입실론 클로저를 가져온다
		ConnectedCloser = GetEpsilonClosure(fsa->NFA_graph, ConnectedCloser, EpsilonChecker);
		
		//KEY , VALUE값 생성
		pair< set<int>, char > key(CurrentState, *itr);
		set<int> value = ConnectedCloser;
		fsa->graph[key] = value;

		//구해진 클로저가 이미 방문되었는지 체크한다
		if (Check.find(ConnectedCloser) == Check.end())
		{
			//체커에 등록
			Check[ConnectedCloser] = true;

			//재귀호출하여 그래프 생성
			NFAtoDFA(ConnectedCloser, UsedCharacter, fsa, Check);
		}
	}
}
// Homework 1.3
bool BuildFSA(const char* regex, FiniteStateAutomaton* fsa)
{
	// Implement this function.
	return false;
}

