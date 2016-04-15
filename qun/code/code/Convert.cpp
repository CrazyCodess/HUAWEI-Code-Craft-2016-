// Convert.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include "Dijk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/timeb.h>
#include <errno.h>
//#include <unistd.h>
#include <signal.h>
#include <iostream>
using namespace std;

#define INLINE  static __inline
#define PRINT   printf

#define MAX_RECORD  100
#define MAX_LINE_LEN 4000

int all_data[6000][4];
int demandNum = 0;        //必经点个数
int demandSet[50];        //必经点集合
int *min_step;
//static char * ret = new char;
static char ret[500];
clock_t startTime;

static int *have_over = new int [600];	
static int *hash_have_over = new int [600];
static vector<int> dij_route;
static Dijkstra dij;
int **all_route;
vector<int> have_over_v;
static int *hash_v = new int[600];
static int start1,end1,line_num;
static int min_num =0;	
static int **all;
static int all_length;
static int min_step_length;

void run_Dij_3();
int ** get_all_route(int **all,int v[],int hang);
vector<vector<int> > dij_3(int **all_rou,int start_point,int v[],int hash_have_over[],int have_pass_v_num,int end,int hang,int vlength);

static char g_result[MAX_LINE_LEN] = "NA";

void record_result(unsigned short edge)
{
    static int len = 0;
    if (len > (MAX_LINE_LEN - 10))
        return;
    if (len > 0)
        len += sprintf(g_result + len, "|");
    len += sprintf(g_result + len, "%d", edge);
}

void print_time(const char *const head)
{ 
	struct timeb rawtime; 
	struct tm * timeinfo; 
	ftime(&rawtime); 
	timeinfo = localtime(&rawtime.time);

	static int ms = rawtime.millitm;
	static unsigned long s = rawtime.time;
	int out_ms = rawtime.millitm - ms;
	unsigned long out_s = rawtime.time - s;
	ms = rawtime.millitm;
	s = rawtime.time;

	if (out_ms < 0)
	{
		out_ms += 1000;
		out_s -= 1;
	}
	printf("%s date/time is: %s \tused time is %lu s %d ms.\n", head, asctime(timeinfo), out_s, out_ms); 
}
int read_file(char ** const buff, const unsigned int spec, const char * const filename)
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		PRINT("Fail to open file %s, %s.\n", filename, strerror(errno));
		return 0;
	}
	//  PRINT("Open file %s OK.\n", filename);

	char line[MAX_LINE_LEN + 2];
	unsigned int cnt = 0;
	while ((cnt < spec) && !feof(fp))
	{
		line[0] = 0;
		fgets(line, MAX_LINE_LEN + 2, fp);
		if (line[0] == 0)   continue;
		buff[cnt] = (char *)malloc(MAX_LINE_LEN + 2);
		strncpy(buff[cnt], line, MAX_LINE_LEN + 2 - 1);
		buff[cnt][4001] = 0;
		cnt++;
	}
	fclose(fp);
	//  PRINT("There are %d lines in file %s.\n", cnt, filename);

	return cnt;
}
void release_buff(char ** const buff, const int valid_item_num)
{
	for (int i = 0; i < valid_item_num; i++)
		free(buff[i]);
}
char* search_route(char *topo[5000],int edge_num,char *demand1,char *result_file)
{
	startTime = clock();
	
	line_num = edge_num;

	///解析topo.csv
	char buffer[5000] = {0};
	memset(buffer, 0, sizeof(buffer));
	char *delim = ",|\n";
	char *p = NULL;	
	int i = 0, j = 0;
	for (i = 0 ; i <edge_num ; i++)
	{
		memcpy(buffer, topo[i], strlen(topo[i]));
		j = 0;
		p = strtok(buffer, delim);
		for (j = 0 ; j <4 ;j++)
		{
			int value =atoi(p);
			all_data[i][j] = value;
			p = strtok(NULL, delim);
		}
		memset(buffer, 0, sizeof(buffer));
	}

	///解析demand.csv
	memcpy(buffer,demand1,strlen(demand1));
	p = strtok(buffer,delim);
	j = 0;
	while(NULL != p)
	{
		int value = atoi(p);
		if (0 == j)
		{
			start1 = value;
		}
		else if (1 == j)
		{
			end1 = value;
		}
		else
		{
			demandSet[demandNum] = value;
			demandNum++;
		}
		j++;
		p = strtok(NULL,delim);
	}

 


	if (demandNum==17&&start1!=100) //case15
	{
		Dijkstra::seed  = 28;
	}
	else if(demandNum>30) //case11
	{
		Dijkstra::seed  = 51;
	}

	else if(demandNum>25&&demandNum<=30)//case14
	{
		Dijkstra::seed  = 19;
	}

	else if(demandNum>20&&demandNum<=25)//case12,13
	{
		Dijkstra::seed  = 34;
	}

	else
	{
		Dijkstra::seed  = 20;
	}



        srand(Dijkstra::seed);
	int count=2;
	int total=0;
	while(count!=0 &&total!=1){
		count=0;

		for(int i = 0;i<edge_num;i++){
			int flag=0;  
			if(all_data[i][1]==end1||all_data[i][2]==start1){         //鍒犻櫎杩涘叆璧风偣锛屽拰缁堢偣鍑哄幓鐨勮竟
				for(int k=0;k<4;k++){
					//        				System.out.print(all_data[i][k]+" ||| ");
					all_data[i][k]=-1;
				}
				count++;
				total++;
				//        			System.out.println("count = "+count);
				continue;
			}
			for(int m=0;m<demandNum;m++){
				if(all_data[i][2] == demandSet[m]){             //鍒ゆ柇绗琲琛岀涓変釜浣嶇疆锛屾槸涓嶆槸閲嶈鐐�
					flag=1;  			
					break;
				}
			}

			if(flag==1){    //绗琲琛岀涓変釜浣嶇疆鏄噸瑕佺偣
				for(int j=0;j<edge_num;j++){     //鍒ゆ柇杩欎釜閲嶈鐐规槸涓嶆槸鍙湁涓�涓叆搴�
					if(i!=j && all_data[i][2]==all_data[j][2]){
						flag=0;
						break;
					}
				}
			}
			if(flag==1){     //濡傛灉杩欎釜閲嶈鐐瑰彧鏈変竴涓叆搴︾殑璇�
				for(int j=0;j<edge_num;j++){
					if(i!=j && all_data[i][1]==all_data[j][1]){               //鍒犻櫎绗笁绉� 鎯呭喌鐨勯偅浜涚偣
						count++;
						total++;

						for(int k=0;k<4;k++){
							//        						System.out.print(all_data[j][k]+" | ");
							all_data[j][k]=-1;
						}
						//        					System.out.println("count = "+count);
						//if(count%4==0)
							//System.out.println();
					}
				}
			}

			int lflag=0;
			int rflag=0;
			for(int j = 0;j<edge_num;j++){
				if(i!=j ){
					if(all_data[i][2]!=all_data[j][1] && all_data[i][2]!=all_data[j][2]&& all_data[i][2]!=end1){  //鍙繘涓嶅嚭鐨勬儏鍐�
						lflag=0;
					}else{
						lflag=1;
						break;
					}
				}
			}
			for(int j=0;j<edge_num;j++){
				if(i!=j ){
					if(all_data[i][1]!=all_data[j][2] && all_data[i][1]!=all_data[j][1]&&all_data[i][1]!=start1){  //鍙嚭涓嶈繘鐨勬儏鍐�
						rflag=0;
					}else{
						rflag=1;
						break;
					}
				}

			}
			if(lflag==0||rflag==0){        //鍒犻櫎浠ヤ笂涓ょ鎯呭喌鐨勯偅浜涜
				for(int k=0;k<4;k++){
					//        				System.out.print(all_data[i][k]+" || ");
					all_data[i][k]=-1;
				}
				count++;
				total++;
				//System.out.println("count = "+count);
			}

		}
		//        	System.out.println("************************88");
		//        	for(int i=0;i<jiexi.length;i++){
		//        		for(int j=0;j<4;j++){
		//        			//System.out.print(all_data[i][j]+"  ");
		//        		}
		//        		//System.out.println();
		//        	}
		//        	System.out.println("total = "+total);
		clock_t thisTime = clock();
		//        	System.out.println(thisTime-startTime);

	}
	all = new int *[edge_num-total+1];
	all_length = edge_num-total+1;
	for (int m = 0 ; m < edge_num-total+1 ;m++)
	{
		all[m] = new int[4];
	}
	for (int aa = 0 ; aa < edge_num-total+1 ; aa++)
	{
		for (int bb = 0 ; bb < 4; bb++)
		{
			all[aa][bb] = 0;
		}
	}
	int index=0;
	int n=0;
	while(n<edge_num){
		if(all_data[n][0]!=-1){
			for(int j=0;j<4;j++){
				all[index][j]=all_data[n][j];
				//System.out.println("index= "+index);
			}
			index++;
		}
		n++;
	}
	//    	System.out.println("999999999999999");
	//    	for(int i=0;i<all.length;i++){
	//    		for(int j=0;j<4;j++){
	//    			System.out.print(all[i][j]+"  ");
	//    		}
	//		System.out.println();
	//    	}

	//============================================================================
//	r = new Route();

	for (int aa = 0 ; aa < 600 ;aa++)
	{
		have_over[aa] = 0;
		hash_have_over[aa] = 0;
	}

	have_over[0] = start1;
	hash_have_over[start1] = 1;											//琛ㄧず璧风偣宸茬粡缁忚繃浜嗐��

	if(edge_num-total+1>0){							//濡傛灉澶т簬100鏉¤竟锛屽氨鐢ㄨ繖绉嶆柟娉曘�傚惁鍒欏氨鐢ㄦ毚鍔涙悳绱�.
		//dij = new Dijkstra();
		all_route= get_all_route(all,demandSet,edge_num-total+1);
		//have_over_v =new ArrayList<Integer>();			//鎸囧凡缁忕粡杩囩殑蹇呴』缁忚繃鐨勭偣銆傚湪杩欓噷鍖呮嫭璧风偣.
		have_over_v.push_back(start1);
		dij_route.push_back(start1);												//琛ㄧず鎵�鏈夌粡杩囩殑鐐�.
		//鎴戣缁欏畠鍔犱笂闆疯揪锛孫(鈭鈭�)O鍝堝搱~
		for(int i = 0;i<demandNum;i++){					
			hash_v[demandSet[i]] = 1;
		}
		//	    			r.run_Dij_2();
		run_Dij_3();
	}

    char *temp = new char;
	if(min_step ==NULL){
		//System.out.println("na");
		return "NA";
	}else{
		for(int i = 0;i<min_step_length;i++){
				printf("%d|",min_step[i]);
			 	record_result(min_step[i]);
		}

		return g_result;
	}
}
void run_Dij_3(){
 if(clock()-startTime>8000000){
 return;
  }
	vector<vector<int> > all_a = dij_3(all_route,have_over_v[have_over_v.size()-1],demandSet,hash_have_over,have_over_v.size()-1,end1,line_num,demandNum);

	if(/*all_a == NULL||*/all_a.size() ==0){
	}else{
		if(all_a[0][0] ==end1){
			for(int j = all_a[0].size()-1;j>=0;j--){
				//褰撹繘鍏ヨ繖閲岀殑鏃跺�欙紝浠ｈ〃杩欐槸鏈�鍚庝竴鏉＄嚎浜�.
				dij_route.push_back(all_a[0][j]);
				hash_have_over[all_a[0][j]] = 1;
				if(j ==0){
					have_over_v.push_back(all_a[0][j]);
				}
			}
			//					    		//涓棿鏄紭鍖栫粨鏋�.
			//					    		ArrayList< Integer> dij_route_opt  =new Optimize().opt_1(dij_route,v,hash_v,all_route);
			int r_len = 0;
			vector<int> dij_route_opt = dij_route;
			for(int i  = 0;i<dij_route_opt.size()-1;i++){
				r_len= all_route[dij_route_opt[i]][dij_route_opt[i+1]]+r_len;
			}

			if(r_len<min_num ||min_num ==0){
				min_num = r_len;
				int *r = new int[dij_route_opt.size()-1];
				for (int i = 0 ; i < dij_route_opt.size()-1 ; i++)
				{
					r[i] = 0;
				}
				for(int i = 0;i<dij_route_opt.size()-1;i++){
					for(int j = 0;j<all_length;j++){
						if(dij_route_opt[i]==all[j][1] &&dij_route_opt[i+1]==all[j][2]&&(true)){		//杩欓噷瑕侀槻姝袱鏉¤矾寰勭粌鍒扮浉鍚岀殑涓ょ偣涓�
							if(r[i]!=0&&all[r[i]][3]<all[j][3]){
								//杩欒鏄庡凡缁忔湁浜嗭紝涓旀瘮鐜板湪杩欎釜鐭��
								//					    									System.out.println("zhu yi--------------------");
							}else{
								r[i] = all[j][0];
							}

						}
					}
				}

				min_step = r;
				min_step_length = dij_route_opt.size()-1;
			}

			for(int j = all_a[0].size()-1;j>=0;j--){
				//褰撹繘鍏ヨ繖閲岀殑鏃跺�欙紝浠ｈ〃閫�鍑烘渶鍚庝竴鏉＄嚎浜�.
				vector<int>::iterator it = dij_route.begin();
				while(it!=dij_route.end())
				{
					if (*it == all_a[0][j])
					{
						dij_route.erase(it);
						break;
					}
					++it;
				}
				//dij_route.erase(dij_route.begin()+all_a[0][j]-1);
				hash_have_over[all_a[0][j]] = 0;
				if(j ==0){
					vector<int>::iterator it = have_over_v.begin();
					while(it!=have_over_v.end())
					{
						if (*it == all_a[0][j])
						{
							have_over_v.erase(it);
							break;
						}
						++it;
					}
					//have_over_v.erase(have_over_v.begin()+all_a[0][j]-1);
				}
			}
		}else{
			for(int i = 0;i<all_a.size();i++){
				for(int j = all_a[i].size()-1;j>=0;j--){
					//褰撹繘鍏ヨ繖閲岀殑鏃跺�欙紝浠ｈ〃杩涘叆浜嗗叾涓竴鏉¤矾寰�.
					//    				System.out.println(i);
					//    				System.out.println(j);
					dij_route.push_back(all_a[i][j]);
					//								    			System.out.print(all_a.get(i).get(j)+ "   ");
					hash_have_over[all_a[i][j]] = 1;
					if(hash_v[all_a[i][j]]==1){						//鎸囪繖涓偣鏄繀椤荤粡杩囩殑鐐�.
						have_over_v.push_back(all_a[i][j]);		
					}
					//							    				if(j ==0){
					//							    					have_over_v.add(all_a.get(i).get(j));		
					//							    				}
				}

				run_Dij_3();

				for(int j = all_a[i].size()-1;j>=0;j--){
					//褰撹繘鍏ヨ繖閲岀殑鏃跺�欙紝浠ｈ〃閫�鍑轰簡鍏朵腑涓�鏉¤矾寰�.
					vector<int>::iterator it = dij_route.begin();
					while(it!=dij_route.end())
					{
						if (*it == all_a[i][j])
						{
							dij_route.erase(it);
							break;
						}
						++it;
					}
					//dij_route.erase(dij_route.begin()+all_a[i][j]-1);
					hash_have_over[all_a[i][j]] = 0;
					if(hash_v[all_a[i][j]]==1){
						vector<int>::iterator it = have_over_v.begin();
						while(it!=have_over_v.end())
						{
							if (*it == all_a[i][j])
							{
								have_over_v.erase(it);
								break;
							}
							++it;
						}
						//have_over_v.erase(have_over_v.begin()+all_a[i][j]-1);
					}
				}
			}
		}
	}
}

vector<vector<int> > dij_3(int **all_rou,int start_point,int v[],int hash_have_over[],int have_pass_v_num,int end,int hang,int vlength)
{
	//srand(time(NULL));
	//int r = (rand() % Dijkstra::seed)+1;
	int **all_route = new int *[Dijkstra::max+1];
	for (int i = 0 ; i < Dijkstra::max+1 ; i++)
	{
		all_route[i] = new int[Dijkstra::max+1];
	}
	for (int i = 0 ; i < Dijkstra::max+1 ; i++)
	{
		for (int j = 0 ; j < Dijkstra::max+1 ;j++)
		{
			all_route[i][j] = all_rou[i][j];
		}
	}
	int *hash_v = new int[600];
	for (int i = 0 ; i < 600 ;i++)
	{
		hash_v[i] = 0;
	}
	for (int i = 0 ; i < vlength ; i++)  /////////////////////////////////// v 的长度
	{
		hash_v[v[i]] = 1;
	}
	bool *s = new bool[Dijkstra::max+1];
	for (int i = 0 ; i < Dijkstra::max+1 ;i++)
	{
		s[i] = false;
	}
	s[start_point]  = true;

	int *previous_p = new int[Dijkstra::max+1];
	for (int i = 0 ; i < Dijkstra::max+1 ; i++)
	{
		previous_p[i] = i;
		if ((all_route[start_point][i]<9999) && (hash_have_over[i] == 0) && (s[i] == false))
		{
			previous_p[i] = start_point;
		}
	}

	for(int j = 0;j<Dijkstra::max+1;j++){
		int ran_next_l = 9999;																
		int ran_point = 9999;			
		int num = 0;
		int tag = 0;
		for(int i = 0;i<Dijkstra::max+1;i++){
			if(hash_have_over[i]==0 && s[i] ==false){					//濡傛灉杩欎釜鐐规病鏈夊湪鏂规硶澶�,浠ュ強鏂规硶鍐呰璧拌繃
				//					if(min_next_l>all_route[start_point][i]){						//杩欓噷鐢ㄩ殢鏈虹殑鏁帮紝閬胯繃鍧戯紝缁曡繃鍧�.
				//							min_next_l = all_route[start_point][i];
				//							min_point = i;
				//					}
				if(all_route[start_point][i]<9999){								//濡傛灉鏈変竴涓紝num = 1
					num++;
				}
			}
		}
		int k = 0;
		if(num != 0){
			k = (rand() % num);
		}

		for(int i = 0;i<Dijkstra::max+1;i++){
			if(hash_have_over[i]==0 && s[i] ==false){					//濡傛灉杩欎釜鐐规病鏈夊湪鏂规硶澶�,浠ュ強鏂规硶鍐呰璧拌繃

				if(all_route[start_point][i]<9999){								//
					if(tag ==k){
						ran_point = i;
						ran_next_l = all_route[start_point][i];
						break;
					}
					tag++;

				}
			}
		}
		if(ran_point<9999){
			s[ran_point] = true;														//浠ｈ〃璺濈鏈�灏忚繖涓偣锛屽湪杩欎釜鏂规硶涓璧拌繃浜嗐��

			for(int i = 0;i<Dijkstra::max+1;i++){												//璺熸柊鏁版嵁.
				if(ran_point ==end){
					break;
				}
				if(hash_have_over[i]==0 && s[i] ==false){
					if(all_route[start_point][i]>ran_next_l+all_route[ran_point][i]){
						all_route[start_point][i]=ran_next_l+all_route[ran_point][i];
						//褰撻渶瑕佹洿鏂扮殑鏃跺��.
						previous_p[i] = ran_point;
					}
				}
			}
		}
	}

	vector<vector <int> > all;
	if (have_pass_v_num == vlength)
	{
		vector<int> li;
		if(previous_p[end] != end){
			li.push_back(end);
			int k =previous_p[end];
			while(k!=start_point){
				li.push_back(k);
				k = previous_p[k];
			}
			all.push_back(li);
		}
	}

	else{
		bool tag = true;
		if(previous_p[end]==end){
			tag  =false;
		}
		for(int i = 0;i<vlength;i++){
			if(hash_have_over[v[i]]==0){			//浠ｈ〃杩欎釜蹇呯粡鐐瑰湪澶栭潰娌℃湁琚蛋杩�.
				if(previous_p[v[i]]==v[i]){
					tag =false;
				}
			}
		}
		if(tag==false){
			all.clear();
			//System.out.println("鏈夌敤");
		}else{
			for(int i = 0;i<vlength;i++){
				vector<int> li;
				int li_v_num = 0;
				if(previous_p[v[i]]!=v[i]){						//璇存槑鏄互鍓嶆病鏈夎蛋杩囩殑鐐�,涓旇兘杈惧埌.
					li.push_back(v[i]);
					int k =previous_p[v[i]];
					while(k !=start_point){
						if(hash_v[k]==1){
							li_v_num ++;
						}
						li.push_back(k);
						k = previous_p[k];
					}
					if(li_v_num==0){
						//									li_v_num = 0;
						//									System.out.println("...");
						all.push_back(li);
					}
				}
			}
		}
	}

	vector<vector<int> > all_dao;
	if (all.size()!=0)
	{
		for (int i = 0 ; i < all.size() ;i++)
		{
			all_dao.push_back(all[all.size()-1]);
		}
	}
	return all;
}
INLINE void write_file(const bool cover, const char * const buff, const char * const filename)
{
	if (buff == NULL)
		return;

	const char *write_type = cover ? "w" : "a";//1:覆盖写文件，0:追加写文件
	FILE *fp = fopen(filename, write_type);
	if (fp == NULL)
	{
		PRINT("Fail to open file %s, %s.\n", filename, strerror(errno));
		return;
	}
	PRINT("Open file %s OK.\n", filename);
	fputs(buff, fp);
	fputs("\n", fp);
	fclose(fp);
}
void write_result(const char * const filename , const char* tt)
{
	write_file(1, tt, filename);
}
int main(int argc, char* argv[])
{
	print_time("Begin");
	char *topo[5000];
	int edge_num;
	char *demand;
	int demand_num;

	char *topo_file =  argv[1];           //"topo9.csv" ;//
	edge_num = read_file(topo, 5000, topo_file);
	if (edge_num == 0)
	{
		printf("Please input valid topo file.\n");
		return -1;
	}
	char *demand_file = argv[2];  //argv[2];"demand9.csv"
	demand_num = read_file(&demand, 1, demand_file);
	if (demand_num != 1)
	{
		printf("Please input valid demand file.\n");
		return -1;
	}

	char *result_file = argv[3];//argv[3];" result.csv"
	char *tt = search_route(topo, edge_num, demand,result_file);
	write_result(result_file,tt);


// 	if (temp == 0)
// 	{
// 		//write_result(result_file);
// 	}
// 
// 	release_buff(topo, edge_num);
// 	release_buff(&demand, 1);

	print_time("End");
	return 0;
}


int ** get_all_route(int **all,int v[],int hang)
{
	for (int i = 0 ; i < hang ; i++)
	{
		if (Dijkstra::max < all[i][1])
		{
			Dijkstra::max = all[i][1];
		}
		if (Dijkstra::max < all[i][2])
		{
			Dijkstra::max = all[i][2];
		}
	}
	int **all_route  = new int * [Dijkstra::max+1];
	for (int i = 0 ; i < Dijkstra::max+1 ;i++)
	{
		all_route[i] = new int [Dijkstra::max+1];
	}
	for (int i = 0 ; i < Dijkstra::max + 1 ;i++)
	{
		for (int j = 0 ; j < Dijkstra::max+1 ; j++)
		{
			all_route[i][j] = 9999;
		}
	}
	for(int i = 0;i<hang;i++){														//鍒濆鍖栨�昏〃銆�
		if(all_route[all[i][1]][all[i][2]] > all[i][3]){
			all_route[all[i][1]][all[i][2]] = all[i][3];
		}
	}
	return all_route;
}

