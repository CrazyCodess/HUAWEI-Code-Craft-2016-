/********************************************************************************************************
*                                             头文件
********************************************************************************************************/
#include "route.h"
#include "lib_record.h"
#include "lib_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/********************************************************************************************************
*                                             宏定义
********************************************************************************************************/
#define maxsize 1000  										//表示两点间不可达，距离为无穷远
#define N		600  										//结点的数目;
/********************************************************************************************************
*                                             全 局 变量
********************************************************************************************************/

struct Graph 	G_graph[N][N] = {0};			//整个拓扑图
struct Graph 	G_graph_Temp[N][N] = {0};		//整个拓扑图
int	   			G_P[N] = {0};

struct NODE  	G_Node = {0};					//中间节点集合
struct NODE  	G_Node_Temp = {0};				//中间节点集合
int    			G_Vout = -1;
struct DEGREE 	G_Degree[50] = {0};				//中间节点的出入度
int 			*Mid_Set = NULL;				//中间节点集
//int 			N = 0;
int Path = 65535;
int  G_MAX = 0;
/********************************************************************************************************
* 功 能 : 初始化函数
* 入 口 : 无
* 返 回 : 无
* 说 明 : 
********************************************************************************************************/
void init(void)
{
	int i = 0, j = 0;
	for (i=0; i<N; i++)
	{
		G_P[i] = -1;
	}

	for(i=0; i<N; i++)
	{
		for(j =0; j <N; j++)
		{
			G_graph[i][j].LinkID = -1;
			G_graph[i][j].Cost = maxsize;
			G_graph_Temp[i][j].LinkID = -1;
			G_graph_Temp[i][j].Cost = maxsize;
		}	
	}
}
/********************************************************************************************************
* 功 能 : 读Topo函数
* 入 口 : 无
* 返 回 : 无
* 说 明 : 
********************************************************************************************************/
void Read_Topo_Data(char *topo[5000], int edge_num)
{
	char buffer[5000] = {0};
	memset(buffer, 0, sizeof(buffer));
	
	
	//解析数据,填入二维数组中
	char *delim = ",|\n";
	char *p = NULL;	
	int i = 0, j = 0;
	int value = 0;
	int LinkID,SourceID,DestinationID;
	

	for(i=0; i<edge_num; i++)
	{
		memcpy(buffer, topo[i], strlen(topo[i]));
		j = 0;
		p = strtok(buffer, delim);
		//while(NULL != p)
		while(4 != j)
		{
			value =atoi(p);
			
			if(0 == j%4)
			{
				LinkID = value;
			}
			else if(1 == j%4)
			{
				SourceID = value;
			}
			else if(2 == j%4)
			{
				DestinationID = value;
			}	
			else
			{
				if( maxsize == G_graph[SourceID][DestinationID].Cost  || G_graph[SourceID][DestinationID].Cost > value)
				{
					G_graph[SourceID][DestinationID].Cost = value;
					G_graph[SourceID][DestinationID].LinkID = LinkID;
					G_graph_Temp[SourceID][DestinationID].Cost = value;
					G_graph_Temp[SourceID][DestinationID].LinkID = LinkID;
				}	
			}
			
			if((1 == j%4) || (2 == j%4))
			{
				if(value > G_MAX)
					G_MAX = value;
			}
			j++;
			p = strtok(NULL, delim);
		}
		memset(buffer, 0, sizeof(buffer));
	}
}
/********************************************************************************************************
* 功 能 : 读Demand函数
* 入 口 : 无
* 返 回 : 无
* 说 明 : 
********************************************************************************************************/
void Read_Demand_Data(char *demand)
{
	char buffer[5000] = {0};
	memset(buffer, 0, sizeof(buffer));
	
	
	//解析数据,填入二维数组中
	char *delim = ",|\n";
	char *p = NULL;	
	int j = 0;
	int value = 0;
	

	memcpy(buffer, demand, strlen(demand));
	p = strtok(buffer, delim);
	
	while(NULL != p)
	{
		value =atoi(p);
		if(0 == j)
		{
			G_Node.BeginID = value;
			G_Node_Temp.BeginID = value;
		}
		else if(1 == j)
		{
			G_Node.EndID = value;
			G_Node_Temp.EndID = value;
		}
		else
		{
			G_Node.IncludingSet[value] = 1;
			G_Node.Num_Mid++;

			G_Node_Temp.IncludingSet[value] = 1;
			G_Node_Temp.Num_Mid++;
		}
		j++;
		p = strtok(NULL, delim);
	}

}
/********************************************************************************************************
* 功 能 : 求中间节点最短路径及其长度
* 入 口 : 1>G_graph[N][N]表示全网拓扑图
* 入 口 : 2>int Vbegin, 表示指定的中间节点
* 返 回 : 1>0--成功找到路径；-1--没有找到路径
* 返 回 : 2>G_P保存着路径
* 返 回 : 3>Vout保存着中间节点的最后一个
* 说 明 : 
********************************************************************************************************/
int D_Mid(int Vbegin)
{
	int D[N];				//Distance:原节点到各个节点的距离。D[k] = x表示节点k距离原节点的距离
	int P[N];				//Path:中间经过的节点编号。P[k] = x表示k节点的前一节点为x。
	int S[N];				//Source:红点集合，S[k] = 1表示节点k加入到了红点集合
	int i,j,k,pre;
	int min,max = maxsize,inf = 1200;
	int nextbegin = -1;

	//1--初始化D，P，S集合
	for(i=0; i<N; i++)
	{
		D[i] = G_graph[Vbegin][i].Cost;
		if(max != D[i])
			P[i] = Vbegin;
		else
			P[i] = -1;
	}
	for(i=0; i<N; i++)
	{
		S[i] = 0;		//红点集S开始为空
	}
	D[Vbegin] = 0;

	//2--计算
	S[Vbegin] = 1;		//开始点Vbegin送入S

	for(i=0; i<N-1; i++)
	{
		//扩充红点集
		min = inf;		//令inf>max，保证距离值为max的蓝点能扩充到S中
		for(j=0; j<N; j++)
		{
			if(j == G_Node.EndID)
				continue;
			if((!S[j]) && (D[j] < min))	
			{
				min = D[j];
				k = j;
			}
		}
		//如果该点不可达
		if(maxsize == min)
		{
			//printf("dddddddddddd\n");
			return -1;
		}
		//加入
		S[k] = 1;    		//将k加入红点集

		//判断节点是否是中间节点
		if(G_Node.IncludingSet[k])
		{
			pre = k;
			nextbegin = k;
			
			//保存从源节点到该中间节点k所经过的路径
			while(Vbegin != pre)
			{
				//G_P[pre] = P[pre];
				G_P[P[pre]] = pre;	
				pre = P[pre];
			}
			//修改k节点的入度为0
			pre = P[k];
			for(j=0; j<N; j++)
			{
				G_graph[j][k].Cost = maxsize;
			}
			//修改路径经过的中间节点的入度和出度为0
			while(pre != Vbegin)
			{
				k = pre;
				pre = P[pre];
				for(j=0; j<N; j++)
				{
					G_graph[j][k].Cost = maxsize;
				}
				for(j=0; j<N; j++)
				{
					G_graph[k][j].Cost = maxsize;
				}
			}
			//修改源节点的入度出度为0
			k = pre;
			for(j=0; j<N; j++)
			{
				G_graph[j][k].Cost = maxsize;
			}
			for(j=0; j<N; j++)
			{
				G_graph[k][j].Cost = maxsize;
			}
			//中间节点是否为空
			G_Node.Num_Mid--;
			if(G_Node.Num_Mid > 1)
			{
				if(-1 == D_Mid(nextbegin))
				{
					return -1;
				}
			}
			else 
			{
				G_Vout = nextbegin;
			}
			break;
		}
		else
		{		
			//更新蓝点集合中距离S集合的D
			for(j=0; j <N; j++)
			{
				if((!S[j]) && (D[j] > D[k] + G_graph[k][j].Cost))	//调整各蓝点的距离值
				{
					D[j] = D[k] + G_graph[k][j].Cost;				//修改蓝点j的距离
					P[j] = k;									//k是j的前趋
				}
			}
		}
	}
	return 0;
}
/********************************************************************************************************
* 功 能 : 求原节点BeginID到指定的第一个中间节点Vend最短路径及其长度
* 入 口 : 1>G_graph[N][N]表示全网拓扑图
* 入 口 : 2>int Vend, 表示终点
* 返 回 : 1>0--成功找到路径；-1--没有找到路径
* 返 回 : 2>G_P保存着路径
* 说 明 : 
********************************************************************************************************/
int D_Begin(int Vend)
{
	int D[N];				//Distance:原节点到各个节点的距离。D[k] = x表示节点k距离原节点的距离
	int P[N];				//Path:中间经过的节点编号。P[k] = x表示k节点的前一节点为x。
	int S[N];				//Source:红点集合，S[k] = 1表示节点k加入到了红点集合
	int i,j,k,pre;
	int min,max = maxsize,inf = 1200;

	//1--初始化D，P，S集合
	for(i=0; i<N; i++)
	{
		D[i] = G_graph[G_Node.BeginID][i].Cost;
		if(max != D[i])
			P[i] = G_Node.BeginID;
		else
			P[i] = -1;
	}
	for(i=0; i<N; i++)
	{
		S[i] = 0;		//红点集S开始为空
	}
	D[G_Node.BeginID] = 0;

	//2--计算
	S[G_Node.BeginID] = 1;		//开始点Vbegin送入S

	for(i=0; i<N-1; i++)
	{
		//扩充红点集
		min = inf;						//令inf>max，保证距离值为max的蓝点能扩充到S中
		for(j=0; j<N; j++)
		{
			
			if((j == G_Node.EndID))
				continue;
			if((!S[j]) && (D[j] < min))	
			{			
				min = D[j];
				k = j;
			}
		}
		//如果该点不可达
		if(maxsize == min)
		{
			//printf("dddddddddddd\n");
			return -1;
		}
		//加入
		S[k] = 1;    		//将k加入红点集

		//是否找到中间节点
		if(k == Vend)
		{
			pre = k;
			
			//保存从源节点到该中间节点k所经过的路径
			while(G_Node.BeginID != pre)
			{
				//G_P[pre] = P[pre];	
				G_P[P[pre]] = pre;
				pre = P[pre];
				
				if(G_Node.IncludingSet[pre] == 1)
				{
					G_Node.Num_Mid--;
				}
			}
			//修改k节点的入度为0
			pre = P[k];
			for(j=0; j<N; j++)
			{
				G_graph[j][k].Cost = maxsize;
			}
			//修改路径经过的中间节点的入度和出度为0
			while(G_Node.BeginID != pre)
			{
				k = pre;
				pre = P[pre];
				for(j=0; j<N; j++)
				{
					G_graph[j][k].Cost = maxsize;
				}
				for(j=0; j<N; j++)
				{
					G_graph[k][j].Cost = maxsize;
				}
			}
			//修改源节点的出度出度为0
			k = pre;
			for(j=0; j<N; j++)
			{
				G_graph[k][j].Cost = maxsize;
			}
			for(j=0; j<N; j++)
			{
				G_graph[j][k].Cost = maxsize;
			}
			break;
		}
		//更新蓝点集合中距离S集合的D
		for(j=0; j <N; j++)
		{
			if((!S[j]) && (D[j] > D[k] + G_graph[k][j].Cost))	//调整各蓝点的距离值
			{
				D[j] = D[k] + G_graph[k][j].Cost;				//修改蓝点j的距离
				P[j] = k;									//k是j的前趋
			}
		}
	}
	return 0;
}
/********************************************************************************************************
* 功 能 : 求指定中间节点Vbegin到终点EndID最短路径及其长度
* 入 口 : 1>G_graph[N][N]表示全网拓扑图
* 入 口 : 2>int Vbegin,指定中间节点Vbegin
* 返 回 : 1>0--成功找到路径；-1--没有找到路径
* 返 回 : 2>G_P保存着路径
* 说 明 : 
********************************************************************************************************/
int D_End(int Vbegin)
{
	int D[N];				//Distance:原节点到各个节点的距离。D[k] = x表示节点k距离原节点的距离
	int P[N];				//Path:中间经过的节点编号。P[k] = x表示k节点的前一节点为x。
	int S[N];				//Source:红点集合，S[k] = 1表示节点k加入到了红点集合
	int i,j,k,pre;
	int min,max = maxsize,inf = 1200;

	//1--初始化D，P，S集合
	for(i=0; i<N; i++)
	{
		D[i] = G_graph[Vbegin][i].Cost;
		if(max != D[i])
			P[i] = Vbegin;
		else
			P[i] = -1;
	}
	for(i=0; i<N; i++)
	{
		S[i] = 0;		//红点集S开始为空
	}
	D[Vbegin] = 0;

	//2--计算
	S[Vbegin] = 1;		//开始点Vbegin送入S

	for(i=0; i<N-1; i++)
	{
		//扩充红点集
		min = inf;						//令inf>max，保证距离值为max的蓝点能扩充到S中
		for(j=0; j<N; j++)
		{
			if((!S[j]) && (D[j] < min))	
			{
				min = D[j];
				k = j;
			}
		}
		//如果该点不可达
		if(maxsize == min)
		{
			//printf("dddddddddddd\n");
			return -1;
		}
		//加入
		S[k] = 1;    		//将k加入红点集

		//是否找到终点
		if(k == G_Node.EndID)
		{
			pre = k;
			
			//保存从源节点到该中间节点k所经过的路径
			while(Vbegin != pre)
			{
				//G_P[pre] = P[pre];
				G_P[P[pre]] = pre;	
				pre = P[pre];
			}
			break;
		}

		//更新蓝点集合中距离S集合的D
		for(j=0; j <N; j++)
		{
			if((!S[j]) && (D[j] > D[k] + G_graph[k][j].Cost))	//调整各蓝点的距离值
			{
				D[j] = D[k] + G_graph[k][j].Cost;				//修改蓝点j的距离
				P[j] = k;									//k是j的前趋
			}
		}
	}
	return 0;
}

/********************************************************************************************************
* 功 能 : 查找中间节点集是否有出度或者入度为1的节点，并且将该节点保存起来
* 入 口 : 无
* 返 回 : 如果有出度或者入度为1的中间节点，则为1；否则为0；
* 说 明 : 1>在中间节点集合中找到出度或者入度为1的节点，然后将它们的下一跳去掉剩余的相反的度；
* 说 明 : 2>然后再递归查询是否有新的中间节点的出度或者入度为1，如果有，则继续1>；否则直接跳出
********************************************************************************************************/
void  Remove_Line(void)
{
	int i = 0, j = 0;
	//求出所有中间节点的出入度
	for(i=0; i<G_Node_Temp.Num_Mid; i++)
	{
		for(j=0; j<N; j++)
		{
			if(G_graph_Temp[Mid_Set[i]][j].Cost < maxsize)	//出度
			{
				G_Degree[i].Out_Degree++;	
				G_Degree[i].Next_Node = j;
			}
			if(G_graph_Temp[j][Mid_Set[i]].Cost < maxsize)	//入度
			{
				G_Degree[i].In_Degree++;
				G_Degree[i].Pre_Node = j;
			}
		}	
	}
	//处理出入度为1的中间节点
	//int Temp = 0;
	int Flag = 0;
	for(i=0; i<G_Node_Temp.Num_Mid; i++)
	{
		if(1 == G_Degree[i].In_Degree)	//对入度为1的中间节点进行处理
		{
			//该节点的上一跳节点的所有剩余出度为无穷
			for(j=0; j<N; j++)
			{
				if(j != Mid_Set[i])
					G_graph_Temp[G_Degree[i].Pre_Node][j].Cost = maxsize;
			}
			//Temp = G_graph_Temp[G_Degree[i].Pre_Node][Mid_Set[j]].Cost;
			//memset(G_graph_Temp[G_Degree[i].Pre_Node], maxsize, N*sizeof(struct Graph));
			//G_graph_Temp[G_Degree[i].Pre_Node][Mid_Set[i]].Cost = Temp;
			Flag = 1;
		}
		if(1 == G_Degree[i].Out_Degree) //对出度为1的中间节点进行处理
		{
			//该节点的下一跳节点的所有剩余入度为无穷
			for(j=0; j<N; j++)
			{
				if(j != Mid_Set[i])
					G_graph_Temp[j][G_Degree[i].Next_Node].Cost = maxsize;
			}
			Flag = 1;
		}
	}

	//查看G_graph_Temp
	if(Flag != 0)
		Remove_Line();

	return ;
}

/********************************************************************************************************
* 功 能 : 查找中间节点集是否有出度或者入度为1的节点，并且将该节点保存起来
* 入 口 : 无
* 返 回 : 如果有出度或者入度为1的中间节点，则为1；否则为0；
* 说 明 : 1>在中间节点集合中找到出度或者入度为1的节点，然后将它们的下一跳去掉剩余的相反的度；
* 说 明 : 2>然后再递归查询是否有新的中间节点的出度或者入度为1，如果有，则继续1>；否则直接跳出
********************************************************************************************************/
void Calculate(char *result_file)
{
	int pre = -1;				//表示路径中某个节点的上一邻居节点
	int ret = -1;
	int i = 0, j = 0, k = 0;

	int Sum = 0;				//该路径的总权重值
	//int Path = 65536;
	
	unsigned int result[600] = {0};					//示例中的一个解
	int Count = 0;
	
	//1--获取中间节点集
	Mid_Set = (int *)malloc(G_Node_Temp.Num_Mid * sizeof(int));
	if(NULL == Mid_Set)
	{
		printf("malloc is fail\n");
		return;
	}
	for(i=0,j=0; i<N; i++)
	{
		if(1 == G_Node_Temp.IncludingSet[i])
		{
			Mid_Set[j++] = i;
		}
	}

	//2--删除多余线段
	Remove_Line();
	
	//3--遍历
	for(i=0; i<G_Node_Temp.Num_Mid; i++)
	{
		//a-输入型全局变量初始化
		for (j=0; j<N; j++)
		{
			G_P[j] = -1;
		}

		for(j=0; j<N; j++)
		{
			for(k =0; k<N; k++)
			{
				G_graph[j][k].LinkID = G_graph_Temp[j][k].LinkID;
				G_graph[j][k].Cost = G_graph_Temp[j][k].Cost;
			}	
		}
		
		G_Node.BeginID = G_Node_Temp.BeginID; 
		G_Node.EndID = G_Node_Temp.EndID;
		G_Node.Num_Mid = G_Node_Temp.Num_Mid;
		for(j=0; j<N; j++)
		{				
			G_Node.IncludingSet[j] = G_Node_Temp.IncludingSet[j];
		}
		
		G_Vout = -1;
		
		//b-计算起始集路径
		ret = D_Begin(Mid_Set[i]);
		if(-1 == ret)
		{
			//printf("起始节点集没有找到路径\n");
			continue;
		}
		
		//c-计算中间集路径
		ret = D_Mid(Mid_Set[i]);
		if(-1 == ret)
		{
			//printf("中间节点集没有找到路径\n");
			continue;
		}
		
		//d-计算结束集路径
		ret = D_End(G_Vout);
		if(-1 == ret)
		{
			//printf("结束节点集没有找到路径\n");
			continue;
		}
		
		//e-计算本次的路径权重和
		pre = G_P[G_Node.BeginID];
		Sum = G_graph_Temp[G_Node.BeginID][pre].Cost;
		while(pre != G_Node.EndID)
		{
			Sum += G_graph_Temp[pre][G_P[pre]].Cost;
			pre = G_P[pre];
		}
		
		//f--如果权重较小则保存下来
		if(Sum < Path)
		{
			Count = 0;
			memset(result, 0, sizeof(result));
			

			//I-存储找到的路径
			pre = G_P[G_Node.BeginID];
			result[Count] = G_graph_Temp[G_Node.BeginID][pre].LinkID;
			Count++;
			
			while(pre != G_Node_Temp.EndID)
			{
				result[Count] = G_graph_Temp[pre][G_P[pre]].LinkID;
				Count++;
				pre = G_P[pre];
			}
			
			//III-写缓冲
			record_result(result[0], 0);
			for (i=1; i<Count; i++)
				record_result(result[i], 1);
			
			//IV-写文件
			write_result(result_file);
			
			//V-清缓冲区
			clear_buff();
		       
			Path = Sum;	
		}
	}
	return ;
}


//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand, char *result_file)
{

   //1--初始化全局变量
   init();
	
   //2--将读取的数据格式化
   Read_Topo_Data(topo, edge_num);
   Read_Demand_Data(demand);
   
   //3--计算
   Calculate(result_file);
  

   free(Mid_Set);

   if(65535 == Path)
   {
   	 write_result(result_file);
   } 

   return ;

}





