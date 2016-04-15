/**
 * 实现代码文件
 * 
 * @author XXX
 * @since 2016-3-4
 * @version V1.0
 */
package com.routesearch.route;

import java.util.ArrayList;

public final class Route
{
    /**
     * 你需要完成功能的入口
     * 
     * @author XXX
     * @since 2016-3-4
     * @version V1
     */
	static int start,end,line_num;
	static int all_route[][];
	static ArrayList<Integer> have_over_v;
	static ArrayList<Integer> dij_route;
	static Route r ;
	static Dijkstra dij;
	static int v[];
	static int hash_v[] = new int [600];
	static int all[][];
	static int all_data[][];
	static int min_num =0;				//  最小的路径大小
	static int min_step[];			//  最小的路径路线
	static Integer have_over[] = new Integer [600];		//  已经走过的点
	static int hash_have_over[] = new int [600];					//这里用hash更方便查找,更快啊,没有经过的时候为0，经过了这点就改为1.
	static String ret = null;												//最终返回的结果
	static boolean run_able = true;
	static long startTime;
    public static String searchRoute(String graphContent, String condition)
    {		/**这里是对原始数据进行处理		提取出
    				start,end,v[],all[][]，line_num
     		*/
//    	System.out.println("=============================");
    	startTime = System.currentTimeMillis();
    	condition = condition.trim();
    	String cu[] = condition.split(",");
    	start = Integer.parseInt(cu[0]);									//起点

    	 end = Integer.parseInt(cu[1]);									//终点

    	String su[] =cu[2].split("\\|");
    	v = new int [su.length];											//v为必须经过的点
    	for(int i = 0;i<su.length;i++){
    		v[i] = Integer.parseInt(su[i]);
    	}
    	String jiexi[] = graphContent.split("\n");
    	all_data= new int [jiexi.length][4];									//其中all[][]为那个标准的二维数组.
    	 line_num = jiexi.length;													//line_num为线段的数量，第一个为1
    	for(int i = 0;i<jiexi.length;i++){
    		String tem[] = jiexi[i].split(",");
    		for(int j = 0;j<4;j++){
    			all_data[i][j] = 	Integer.parseInt(tem[j]);	
    		}
    	}
    	if(all_data.length>=0){		//就是这里，
    			Dijkstra.seed = 130;						//12是没问题的，应该是上边的边数量错了
    	}
    	//====================================================================================
    	/*准备好了吗，我要开始剪枝了*/
    	int count=2;
    	int total=0;
    	while(count!=0 &&total!=1){
    		count=0;
    		
        	for(int i = 0;i<jiexi.length;i++){
        		int flag=0;  
        		if(all_data[i][1]==end||all_data[i][2]==start){         //删除进入起点，和终点出去的边
        			for(int k=0;k<4;k++){
//        				System.out.print(all_data[i][k]+" ||| ");
    					all_data[i][k]=-1;
    				}
        			count++;
    				total++;
//        			System.out.println("count = "+count);
        			continue;
        		}
        		for(int m=0;m<su.length;m++){
        			if(all_data[i][2] == v[m]){             //判断第i行第三个位置，是不是重要点
        				flag=1;  			
        				break;
        			}
        		}
        
        		if(flag==1){    //第i行第三个位置是重要点
        			for(int j=0;j<jiexi.length;j++){     //判断这个重要点是不是只有一个入度
        				if(i!=j && all_data[i][2]==all_data[j][2]){
        					flag=0;
        					break;
        				}
        			}
        		}
        		if(flag==1){     //如果这个重要点只有一个入度的话
        			for(int j=0;j<jiexi.length;j++){
        				if(i!=j && all_data[i][1]==all_data[j][1]){               //删除第三种 情况的那些点
        					count++;
        					total++;
        					
        					for(int k=0;k<4;k++){
//        						System.out.print(all_data[j][k]+" | ");
        						all_data[j][k]=-1;
        					}
//        					System.out.println("count = "+count);
        					if(count%4==0)
        						System.out.println();
        				}
        			}
        		}
        		
        		int lflag=0;
        		int rflag=0;
        		for(int j = 0;j<jiexi.length;j++){
        			if(i!=j ){
        				if(all_data[i][2]!=all_data[j][1] && all_data[i][2]!=all_data[j][2]&& all_data[i][2]!=end){  //只进不出的情况
            				lflag=0;
            			}else{
            				lflag=1;
            				break;
            			}
        			}
        		}
        		for(int j=0;j<jiexi.length;j++){
        			if(i!=j ){
        				if(all_data[i][1]!=all_data[j][2] && all_data[i][1]!=all_data[j][1]&&all_data[i][1]!=start){  //只出不进的情况
            				rflag=0;
            			}else{
            				rflag=1;
            				break;
            			}
        			}
        			
        		}
        		if(lflag==0||rflag==0){        //删除以上两种情况的那些行
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
        	long thisTime = System.currentTimeMillis();
//        	System.out.println(thisTime-startTime);
        	
    	}
    	
    	all=new int[all_data.length-total+1][4];
//    	System.out.println("all.length =  "+all.length);
    	int index=0;
    	int n=0;
    	while(n<all_data.length){
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
    	r = new Route();
//    	if(true){															//我要倒着走，谁也拦不住。哇咔咔.
//    		int temp;
//    		temp = start;
//    		start  = end;
//    		end = temp;
//    		for(int i =0;i<all.length;i++){
//    			temp = all[i][1];
//    			all[i][1] = all[i][2];
//    			all[i][2] = temp;
//    		}
//    		for(int i =0;i<all.length;i++){
//    			System.out.print(i+"  ");
//    			System.out.print(all[i][1] +"  ");
//    			System.out.print(all[i][2] +"  ");
//    			System.out.print(all[i][3] +"  ");
//    			System.out.println();
//    		}
//    		System.out.println(start+"  |  "+end);
//    	}
    	
    	
     	have_over[0] = start;
     	hash_have_over[start] = 1;											//表示起点已经经过了。
    	
    	if(all.length>0){							//如果大于100条边，就用这种方法。否则就用暴力搜索.
    		dij_route = new ArrayList<Integer>();
    		dij = new Dijkstra();
			 all_route= dij.get_all_route(all,v);
			 have_over_v =new ArrayList<Integer>();			//指已经经过的必须经过的点。在这里包括起点.
	    	have_over_v.add(start);
	    	dij_route.add(start);												//表示所有经过的点.
	    	//我要给它加上雷达，O(∩_∩)O哈哈~
	    			for(int i = 0;i<v.length;i++){					
	    				hash_v[v[i]] = 1;
	    			}
//	    			r.run_Dij_2();
	    			r.run_Dij_3();
	    			
    	}
    	
    	
    	if(min_step ==null){
    		System.out.println("na");
    		return "NA";
    	}else{
    		for(int i = 0;i<min_step.length;i++){
    			if(ret ==null){
    				ret =Integer.toString(min_step[i])+"|";
    			}else if(i ==min_step.length-1){
    				ret +=Integer.toString(min_step[i]);
    			}else{
    				ret +=Integer.toString(min_step[i])+"|";
    			}
    		}
    		System.out.println(ret);
    		System.out.println(min_num);
    		return ret;
    	}
//    	if(min_step ==null){													//从终点到起点,所以这里要转换一下啊。哇咔咔.
//    		System.out.println("na");
//    		return "NA";
//    	}else{
//    		for(int i = min_step.length-1;i>=0;i--){
//    			if(ret ==null){
//    				ret =Integer.toString(min_step[i])+"|";
//    			}else if(i ==0){
//    				ret +=Integer.toString(min_step[i]);
//    			}else{
//    				ret +=Integer.toString(min_step[i])+"|";
//    			}
//    		}
//    		System.out.println(ret);
//    		System.out.println(min_num);
//    		return ret;
//    	}
    }

    public void run_Dij_3(){
    	if(System.currentTimeMillis()-startTime>9000){
    		return;
    	}
    	
    	ArrayList<ArrayList<Integer>> all_a = dij.dij_3(all_route, have_over_v.get(have_over_v.size()-1),v , hash_have_over,have_over_v.size()-1,end);
    	if(all_a ==null||all_a.size() ==0){
    	}else{
					    		if(all_a.get(0).get(0) ==end){
					    		for(int j = all_a.get(0).size()-1;j>=0;j--){
									//当进入这里的时候，代表这是最后一条线了.
					    			dij_route.add(all_a.get(0).get(j));
									hash_have_over[all_a.get(0).get(j)] = 1;
									if(j ==0){
										have_over_v.add(all_a.get(0).get(j));
									}
								}
//					    		//中间是优化结果.
//					    		ArrayList< Integer> dij_route_opt  =new Optimize().opt_1(dij_route,v,hash_v,all_route);
					    		int r_len = 0;
					    		ArrayList< Integer> dij_route_opt = dij_route;
					    		for(int i  = 0;i<dij_route_opt.size()-1;i++){
					    			r_len= all_route[dij_route_opt.get(i)][dij_route_opt.get(i+1)]+r_len;
					    		}

					    		if(r_len<min_num ||min_num ==0){
					    			min_num = r_len;
					    			int r[] = new int[dij_route_opt.size()-1];
					    			for(int i = 0;i<dij_route_opt.size()-1;i++){
					    				for(int j = 0;j<all.length;j++){
					    						if(dij_route_opt.get(i)==all[j][1] &&dij_route_opt.get(i+1)==all[j][2]&&(true)){		//这里要防止两条路径练到相同的两点上
					    								if(r[i]!=0&&all[r[i]][3]<all[j][3]){
					    									//这说明已经有了，且比现在这个短。
//					    									System.out.println("zhu yi--------------------");
					    								}else{
					    									r[i] = all[j][0];
					    								}
					    								
					    						}
					    				}
					    			}

					    			min_step = r.clone();
					    		}
					    		
					    		for(int j = all_a.get(0).size()-1;j>=0;j--){
									//当进入这里的时候，代表退出最后一条线了.
					    			dij_route.remove(all_a.get(0).get(j));
									hash_have_over[all_a.get(0).get(j)] = 0;
									if(j ==0){
										have_over_v.remove(all_a.get(0).get(j));
									}
								}
							}else{
							    	for(int i = 0;i<all_a.size();i++){
							    			for(int j = all_a.get(i).size()-1;j>=0;j--){
							    				//当进入这里的时候，代表进入了其中一条路径.
							//    				System.out.println(i);
							//    				System.out.println(j);
							    				dij_route.add(all_a.get(i).get(j));
//								    			System.out.print(all_a.get(i).get(j)+ "   ");
							    				hash_have_over[all_a.get(i).get(j)] = 1;
							    				if(hash_v[all_a.get(i).get(j)]==1){						//指这个点是必须经过的点.
							    					have_over_v.add(all_a.get(i).get(j));		
							    				}
//							    				if(j ==0){
//							    					have_over_v.add(all_a.get(i).get(j));		
//							    				}
							    			}
							    			
							    			r.run_Dij_3();

							    			for(int j = all_a.get(i).size()-1;j>=0;j--){
							    				//当进入这里的时候，代表退出了其中一条路径.
							    				dij_route.remove(all_a.get(i).get(j));
							    				hash_have_over[all_a.get(i).get(j)] = 0;
							    				if(hash_v[all_a.get(i).get(j)]==1){
							    					have_over_v.remove(all_a.get(i).get(j));
							    					}
							    			}
							    	}
							}
    	}
    }

}
