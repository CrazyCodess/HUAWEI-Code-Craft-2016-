package com.routesearch.route;

import java.util.ArrayList;

public class Optimize {
	
	     
	     int hash_have_pass[] =new int[600]; 
		public  ArrayList< Integer> opt_1(ArrayList< Integer> dij_route,int v[],int hash_v[],int all_r[][]){
			for(int i =0;i<dij_route.size();i++){
				System.out.print(dij_route.get(i)+"-------");
			}
			System.out.println();
			int all_rou[][]  = new int [all_r.length][all_r.length];
			for(int i =0;i<all_r.length;i++){
				all_rou[i] =all_r[i].clone();
			}
			//all_route千万不能改，改了就爆炸。
			
			ArrayList<Integer> opt_route = new ArrayList<Integer>();
			
			for(int i =0;i<dij_route.size();i++){										//给hash_have_pass初始话.为1代表已经经过了。
				hash_have_pass[dij_route.get(i)] =1;
			}
			int s = 9999;
			int e = 9999;						//一个是前面一个必经点，一个是后面一个必经点.
			for(int i = 0;i<dij_route.size();i++){
				if(i ==0){																//是起点.
						s = dij_route.get(0);
						opt_route.add(s);
				}else{
					if(hash_v[dij_route.get(i)]==1 || (i ==dij_route.size()-1)  ){					//这个是必须经过的点.
						
							e = dij_route.get(i);
							hash_have_pass[e] = 0;
							int m_dij[] = new Optimize().min_dij(s, e,all_rou,hash_have_pass);			//这个为两个必经点之间的最优线路.

								//中间应该是将这小段路线加入到arraylist里面去.
								for(int k = 0;k<m_dij.length;k++){

										opt_route.add(m_dij[k]);
										hash_have_pass[m_dij[k]] = 1;
										
								}
								
							e =9999;
							s = dij_route.get(i);
					}else{																//如果是普通的点.就将它设置为未经过.
							hash_have_pass[dij_route.get(i)] = 0;
					}
				}
			}
			for(int i =0;i<opt_route.size();i++){
				System.out.print(opt_route.get(i)+" ");
			}
			System.out.println();

			return opt_route;
			
		}
		//返回的是中间所有经过的点以及终点。
		public  int[] min_dij(int start_p,int end_p,int all_rou[][],int hash_have_pass[]){
			int all_route[][] = new int [all_rou.length][all_rou.length];
			for(int i =0;i<all_rou.length;i++){
				all_route[i] =all_rou[i].clone();
			}
			boolean s[] = new boolean[all_route.length];							//s代表只有在这个方法中会用到的，这个方法中经过的点。
			for(int i = 0;i<s.length;i++){					//给s赋值，除了起点都为未走过.
				s[i] = false;
			}
			s[start_p] = true;
			
			int previous_p[] = new int[all_route.length];							//在这个方法中，它之前的那个点
			for(int i = 0;i<previous_p.length;i++){
				previous_p[i] = i;											//初始化为它自己.
				if((all_route[start_p][i]<9999) && (hash_have_pass[i] ==0) && (s[i] == false)){
					previous_p[i] = start_p;
				}
			}
			
					for(int j = 0;j<all_route.length;j++){
						int min_next_l = 9999;																
						int min_point = 9999;			
							for(int i = 0;i<all_route.length;i++){
								if(hash_have_pass[i]==0 && s[i] ==false){					//如果这个点没有在方法外,以及方法内被走过
									if(min_next_l>all_route[start_p][i]){						//如果这个点距离start_point点比min_next_l还要小。
											min_next_l = all_route[start_p][i];
											min_point = i;
									}
								}
							}
							if(min_point<9999){
								s[min_point] = true;														//代表距离最小这个点，在这个方法中被走过了。
		
								if(min_point==end_p){
									//如果加进来的点是最后的那个点.那就可以返回了= =
									
									break;
								}else{
										for(int i = 0;i<all_route.length;i++){												//跟新数据.
											if(hash_have_pass[i]==0 && s[i] ==false){
												if(all_route[start_p][i]>min_next_l+all_route[min_point][i]){
													all_route[start_p][i]=min_next_l+all_route[min_point][i];
													//当需要更新的时候.
													previous_p[i] = min_point;
												}
											}
										}
								}
						}
					}
			int ji_shu = 1;																//代表了包括终点在内的中间的点的个数.(不包括起点)
			int num = end_p;							

			while(previous_p[num]!=start_p){
					ji_shu++;
					num = previous_p[num];
			}
			int m_dij[] = new int [ji_shu];
			num = end_p;
			for(int i =ji_shu-1;i>=0;i--){
					m_dij[i] = num;
					num = previous_p[num];
			}
			
			return m_dij;
		}
}
