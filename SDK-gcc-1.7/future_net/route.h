#ifndef __ROUTE_H__
#define __ROUTE_H__

//顶点与有向边的索引均从0开始
//编号(不一定连续，但用例保证索引不重复)。 
struct Graph
{
	int LinkID;		//LinkID为该有向边的索引
	int Cost;		//Cost为该有向边的权重。
};

struct NODE{
	int BeginID;
	int EndID;
	int Num_Mid;
	int IncludingSet[600];	//中间节点最多不超过50个
};


struct DEGREE{
	int In_Degree;
	int Out_Degree;
	int Next_Node;
	int Pre_Node;
};

void search_route(char *graph[5000], int edge_num, char *condition, char *result_file);

#endif
