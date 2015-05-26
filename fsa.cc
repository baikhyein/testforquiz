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
	//NFA���θ� Ȯ���ϱ� ���� map
	map< pair<int, char>, int> checkMap;

	for (int i = 0; i < num_elements; i++)
	{
		//���� �������� # ����� NFA
		if (elements->input_char == kEps)
			return true;

		//KEY �� ����
		int current_state = elements->state;
		char input_char = elements->input_char;
		pair<int, char> key(current_state, input_char);

		//VALUE�� ����
		int value = elements->next_state;

		map< pair<int, char>, int>::iterator itr = checkMap.find(key);
		//�ߺ��� key�� �߰�
		if (itr != checkMap.end())
		{
			//���� key���� �ٸ� value���� �����ٸ� NFA
			if (itr->second != value)
				return true;
		}

		//��ϵ� key���� ���ٸ� ����Ѵ�.
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

	//LOGIC �׷��� ���
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

	//ACCEPT STATE�� ���
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
	//NFA �׷����� ���
	for (int i = 0; i < num_elements; i++)
	{
		pair<int, char> key(elements->state, elements->input_char);
		set<int> value;
		value.insert(elements->next_state);

		if (i == 0)
			InitialState.insert(elements->state);
		//���� ��� ������ ���
		if (elements->input_char != '#')
			used_character.insert(elements->input_char);

		//�̹� ��ϵ� key���� �ִٸ�
		if (fsa->NFA_graph.find(key) != fsa->NFA_graph.end())
		{
			//VALUE���� �߰� ����
			fsa->NFA_graph[key].insert(elements->next_state);
		}

		//�̹� ��ϵ� key���� ���ٸ�
		else
		{
			//���ο� ������ ���� �߰�
			fsa->NFA_graph[key] = value;
		}
		elements++;
	}

	//ACCEPT STATE ���
	for (int i = 0; i < num_accept_states; i++)
	{
		fsa->acceptState.insert(*accept_states_array);
		accept_states_array++;
	}


	/* NFA -> DFA ��ȯ*/


	//�ԽǷ� Ŭ������ üũ�� ����
	map< set<int>, bool > EpsilonChecker;

	//NFA�� DFA�� ��ȯ�� �׷�������
	//�ʱ� Ŭ������ �����´�
	//initial state���� �ԽǷ����� ����� Ŭ������ ã�� ��.
	
	set<int> InitialClosure = GetEpsilonClosure(fsa->NFA_graph, InitialState,EpsilonChecker);
	set<int> CurrentClosure = InitialClosure;

	//InitialState ����
	fsa->InitialState = InitialClosure;


	//DFA �׷��� ������ üũ��
	map< set<int>, bool >Checker;

	//DFA �׷��� ����
	NFAtoDFA(CurrentClosure, used_character, fsa, Checker);

	// �׷��� ���
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
	//���� state
	set<int> current_state = fsa->InitialState;
	//set<int> current_state(1, fsa->initial_state);

	//���� state
	set<int> accept_state = fsa->acceptState;

	//graph �� �����´�
	map< pair< set<int>, char >, set<int> >graph = fsa->graph;

	//���� ���¿� �Է°��� �������� ������� ���� ����
	set<int>next_state;

	//�Է°��� ���� ���¸� �����Ѵ�
	for (int i = 0; i < strlen(str); i++)
	{
		//key������ value ���� Ȯ���Ͽ�
		//�ش� ���¿��� �־��� �Է����� ������ �� �ִ� ���°� �ִ��� Ȯ��
		pair< set<int>, char > key(current_state, str[i]);
		ITR itr = graph.find(key);


		//������ ���°� ���°��
		if (itr == graph.end())
			return false;

		//������ ���°� �ִ°��
		//���� ���¸� ������ ���·� �ٲ۴�
		current_state = itr->second;
	}

	//�������� ������ ������ ��ҵ��� ��ȸ�Ͽ�
	//accept state�� ��ġ�ϴ� ���� �ϳ��� �ִ°�쿡 
	//true ��ȯ
	for (set<int>::iterator i = current_state.begin(); i != current_state.end(); i++)
		for (set<int>::iterator j = accept_state.begin(); j != accept_state.end(); j++)
			if (*i == *j)
				return true;

	// ������ ���¿� accept state ��Ұ� ���°��
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

//�׷����� ���(state) �� �־����� ��
//�ش� state�� �ԽǷ����� ����� ��ҵ��� �������� ��ȯ�Ѵ�.
set<int> GetEpsilonClosure(map< pair<int, char>, set<int> > graph,
	set<int> state, map< set<int>, bool > check)
{

	set<int> closure;

	//state���� ��� ��ҵ鿡 ���ؼ�
	for (set<int>::iterator itr = state.begin(); itr != state.end(); itr++)
	{
		//�� ��ҿ��� ���ڷ� ����� ��尡 �ִ��� Ȯ���Ѵ�
		pair<int, char> key(*itr, '#');

		//Ȯ�� = #���� ����Ǵ� ��尡 �ְ� ���� �湮���� ���� �����
		if (graph.find(key) != graph.end() && check.find(graph[key]) == check.end())
		{
			//�ش� ��忡�� �Լ��� ���ȣ���Ѵ�.
			set<int> child_closure = GetEpsilonClosure(graph, graph[key],check);
			check[graph[key]] = true;
			//�ڽ� Ŭ�������� ������� �ԽǷ� ������ �����Ѵٸ�
			if (!child_closure.empty())
			{
				//�ش� ������ �߰��Ѵ�
				for (set<int>::iterator itr = child_closure.begin()
					; itr != child_closure.end(); itr++)
				{
					closure.insert(*itr);
				}
			}
		}
	}


	//�ڱ��ڽ��� �߰�
	for (set<int>::iterator itr = state.begin(); itr != state.end(); itr++)
		closure.insert(*itr);

	return closure;
}

//�׷����� state�� �־��� ��, state���� character�� ����� ������ �������� ���Ѵ�.
set<int> GetConnectedClosure(map< pair<int, char>, set<int> > graph,
	set<int> closure, char used_character)
{
	set<int> ConnectedClosure;

	for (set<int>::iterator itr = closure.begin()
		; itr != closure.end(); itr++)
	{
		//Ŭ������ �� ��� ����
		int ClosureElem = *itr;

		//���� ���ڷ� Ž��
		//�� ���,���ڷ� ����� ��尡 �ִ��� Ȯ��
		//����� ��尡 �ִٸ� 
		pair<int, char> key(ClosureElem, used_character);
		if (graph.find(key) != graph.end())
		{
			//�߰�
			for (set<int>::iterator itr = graph[key].begin(); itr != graph[key].end(); itr++)
				ConnectedClosure.insert(*itr);
		}
	}

	return ConnectedClosure;
}


//���� State�� ���� ���ڵ��� �־��� ��
//���� State�κ��� �����ϴ� DFA�� �����Ѵ�.
void NFAtoDFA(set<int> CurrentState, set<char> UsedCharacter, FiniteStateAutomaton* fsa,
	map< set<int>, bool > Check)
{
	
	for (set<char>::iterator itr = UsedCharacter.begin(); itr != UsedCharacter.end(); itr++)
	{
		//�ϳ��� ���ڷ� CONNECTED CLOSURE �˻�
		set<int> ConnectedCloser = GetConnectedClosure(fsa->NFA_graph, CurrentState, *itr);
		
		//�ԽǷ� CONNECTED CLOSURE �˻��� üũ��
		map< set<int>, bool > EpsilonChecker;

		//����� Ŭ������ ������ ����
		if (ConnectedCloser.empty())
			continue;

		//�ԽǷ� Ŭ������ �����´�
		ConnectedCloser = GetEpsilonClosure(fsa->NFA_graph, ConnectedCloser, EpsilonChecker);
		
		//KEY , VALUE�� ����
		pair< set<int>, char > key(CurrentState, *itr);
		set<int> value = ConnectedCloser;
		fsa->graph[key] = value;

		//������ Ŭ������ �̹� �湮�Ǿ����� üũ�Ѵ�
		if (Check.find(ConnectedCloser) == Check.end())
		{
			//üĿ�� ���
			Check[ConnectedCloser] = true;

			//���ȣ���Ͽ� �׷��� ����
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

