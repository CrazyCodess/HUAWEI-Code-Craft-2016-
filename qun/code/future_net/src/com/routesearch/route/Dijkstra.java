package com.routesearch.route;
import java.util.Random;
import java.util.ArrayList;


public class Dijkstra {
	static int max = 0;																			//表示已有的最大的数
	static int seed  = 0;
	public int [][] get_all_route(int all[][],int v[]){

//		System.out.println(all.length);
		for(int i = 0;i<all.length;i++){
			if(max<all[i][1]){
				max = all[i][1];
			}
			if(max<all[i][2]){
				max = all[i][2];
			}
		}
		int all_route[][]  = new int [max+1][max+1];							//表示all_route[][]的横纵坐标为0到max，总共max+1个.
		for(int i = 0;i<max+1;i++){
			for(int j = 0;j<max+1;j++){
				all_route[i][j] = 9999;
			}
		}
		for(int i = 0;i<all.length;i++){														//初始化总表。
			if(all_route[all[i][1]][all[i][2]] > all[i][3]){
				all_route[all[i][1]][all[i][2]] = all[i][3];
			}
		}
//		for(int i = 0;i<max+1;i++){
//			for(int j = 0;j<max+1;j++){
//				System.out.print(all_route[i][j] + "     ");
//			}
//			System.out.println("      ");
//		}

		return all_route;
	}
	/**
	 * 这个方法要求的是忽略普通点后，找寻目前和它直接相连的必须经过的点
	 * （直接相连包括中间可以有普通的点但是不能有必须经过的点以及起点终点）
	 * 2.0版本跟新内容：事实证明，如果找直接相连的点根本跑不完后面，所以这里改为找经过v中最多的最短线，然后返回。
	 * 然后将它们的路径保持下来。
	 * 返回的是这几条路径的第一条路径，然后它的next又指向下一条路径，直到最后为null。
	 * 
	 * */

	//have_pass_v_num指已经通过必须要经过的点的个数
	//v[]是所有必须要经过的点的集合。
	//这个方法只负责找出几条路线，不负责走.
	//当它返回null 的时候，指这条是死路.


public ArrayList<ArrayList<Integer>> dij_3(int all_rou[][],int start_point,int v[],int hash_have_over[],int have_pass_v_num,int end){
	Random r = new Random(seed);
	
	int all_route[][] = new int [all_rou.length][all_rou.length];
	for(int i =0;i<all_rou.length;i++){
		all_route[i] =all_rou[i].clone();
	}
	int hash_v[] = new int [600];
	for(int i =0;i<v.length;i++){
			hash_v[v[i]] =1;
	}
	boolean s[] = new boolean[max+1];							//s代表只有在这个方法中会用到的，这个方法中经过的点。
	for(int i = 0;i<s.length;i++){					//给s赋值，除了起点都为未走过.
		s[i] = false;
	}
	s[start_point] = true;
	
	int previous_p[] = new int[max+1];							//在这个方法中，它之前的那个点
	for(int i = 0;i<previous_p.length;i++){
		previous_p[i] = i;											//初始化为它自己.
		if((all_route[start_point][i]<9999) && (hash_have_over[i] ==0) && (s[i] == false)){
			previous_p[i] = start_point;
		}
	}
			
	for(int j = 0;j<all_route.length;j++){
		int ran_next_l = 9999;																
		int ran_point = 9999;			
		int num = 0;
		int tag = 0;
			for(int i = 0;i<all_route.length;i++){
				if(hash_have_over[i]==0 && s[i] ==false){					//如果这个点没有在方法外,以及方法内被走过
//					if(min_next_l>all_route[start_point][i]){						//这里用随机的数，避过坑，绕过坎.
//							min_next_l = all_route[start_point][i];
//							min_point = i;
//					}
					if(all_route[start_point][i]<9999){								//如果有一个，num = 1
							num++;
					}
				}
			}
			int k = 0;
			if(num != 0){
				k = r.nextInt(num);
			}
			
			for(int i = 0;i<all_route.length;i++){
				if(hash_have_over[i]==0 && s[i] ==false){					//如果这个点没有在方法外,以及方法内被走过

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
				s[ran_point] = true;														//代表距离最小这个点，在这个方法中被走过了。
				
						for(int i = 0;i<all_route.length;i++){												//跟新数据.
							if(ran_point ==end){
								break;
							}
							if(hash_have_over[i]==0 && s[i] ==false){
								if(all_route[start_point][i]>ran_next_l+all_route[ran_point][i]){
									all_route[start_point][i]=ran_next_l+all_route[ran_point][i];
									//当需要更新的时候.
									previous_p[i] = ran_point;
								}
							}
						}
		}
	}

	ArrayList<ArrayList<Integer>> all = new ArrayList<ArrayList<Integer>>();

	if(have_pass_v_num==v.length){
		ArrayList<Integer> li = new ArrayList<Integer>();
			//说明只有最后一点没有走了，还犹豫什么，直接返回到终点的路径
		if(previous_p[end] != end){
			li.add(end);
			int k =previous_p[end];
			while(k!=start_point){
				li.add(k);
				k = previous_p[k];
			}
			all.add(li);
		}
	}else{
		boolean tag = true;
		if(previous_p[end]==end){
			tag  =false;
		}
		for(int i = 0;i<v.length;i++){
			if(hash_have_over[v[i]]==0){			//代表这个必经点在外面没有被走过.
					if(previous_p[v[i]]==v[i]){
							tag =false;
					}
			}
		}
		if(tag==false){
			all = null;
				System.out.println("有用");
		}else{
			for(int i = 0;i<v.length;i++){
				ArrayList<Integer> li = new ArrayList<Integer>();
				int li_v_num = 0;
					if(previous_p[v[i]]!=v[i]){						//说明是以前没有走过的点,且能达到.
						li.add(v[i]);
						int k =previous_p[v[i]];
							while(k !=start_point){
								if(hash_v[k]==1){
									li_v_num ++;
								}
									li.add(k);
									k = previous_p[k];
							}
							if(li_v_num==0){
//									li_v_num = 0;
//									System.out.println("...");
									all.add(li);
							}
					}
			}
		}
		}
		
	ArrayList<ArrayList<Integer>> all_dao = new ArrayList<ArrayList<Integer>>();
	if(all !=null){
		

		for(int i = 0;i<all.size();i++){
			all_dao.add(all.get(all.size()-1));
		}
	}
	return all;
}

}

