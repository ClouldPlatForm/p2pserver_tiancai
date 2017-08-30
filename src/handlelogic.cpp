#include "handlelogic.h"
	
/*************************************************************************
 函数名称: CHandleLogic::CheckIPCINFO
 功能说明: 校验接收数据完整性，正确性
 输入参数: sIPC 解析到的数据		    
 输出参数: 无
 返 回 值: true/false
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CHandleLogic::CheckIPCINFO(SINPUTINFO& sIPC)
{
	string strlog("");
	//int ret = 0;
	
	if(sIPC.strsn.empty()||sIPC.strrandom.empty()||(sIPC.strsn.size() > IPCSNMAX)
		||(sIPC.strrandom.size() > IPCSRANDOMMAX))
	{
		strlog += "check error from SN:"+ 
		sIPC.strsn+" or random:" + sIPC.strrandom + "\n";
		#ifdef _DEBUGMODE
			cout<<strlog.c_str();
		#endif
		WriteLogERROR(g_log.m_rec, strlog.c_str());

		return false;
	}
	if(sIPC.straddr_pri.empty()||sIPC.straddr.empty()||(sIPC.straddr_pri.size() > P2PIPMAX)
		||(sIPC.straddr.size() > P2PIPMAX))
	{
		strlog += "check error from local_IP:" + sIPC.straddr_pri 
		+ " or map_IP:" + sIPC.straddr + "\n";
		#ifdef _DEBUGMODE
			cout<<strlog.c_str();
		#endif
		WriteLogERROR(g_log.m_rec, strlog.c_str());
		return false;
	}
	if((sIPC.mphttp_pri < 0)||(sIPC.mphttp < 0)
		||(sIPC.mphttp_pri > MAPPORTMAX)||(sIPC.mphttp > MAPPORTMAX))
	{
		stringstream tmp_pri, tmp;
		tmp_pri << sIPC.mphttp_pri;
		tmp << sIPC.mphttp;
		strlog += "check error from http local_port:" + tmp_pri.str()
		+ " or map_port:" + tmp.str() + "\n";
		#ifdef _DEBUGMODE
			cout<<strlog.c_str();
		#endif
		WriteLogERROR(g_log.m_rec, strlog.c_str());
		return false;
	}
	if((sIPC.mprtsp_pri < 0)||(sIPC.mprtsp < 0)
		||(sIPC.mprtsp_pri > MAPPORTMAX)||(sIPC.mprtsp > MAPPORTMAX))
	{
		stringstream tmp_pri, tmp;
		tmp_pri << sIPC.mprtsp_pri;
		tmp << sIPC.mprtsp;
		strlog += "check error from http local_port:" + tmp_pri.str()
		+ " or map_port:" + tmp.str() + "\n";			
		#ifdef _DEBUGMODE
			cout<<strlog.c_str();
		#endif		
		WriteLogERROR(g_log.m_rec, strlog.c_str());
		return false;
	}
	if((sIPC.mprtp_pri < 0)||(sIPC.mprtp < 0)
		||(sIPC.mprtp_pri > MAPPORTMAX)||(sIPC.mprtp > MAPPORTMAX))
	{
		stringstream tmp_pri, tmp;
		tmp_pri << sIPC.mprtp_pri;
		tmp << sIPC.mprtp;
		strlog +=  "check error from http local_port:" + tmp_pri.str()
		+ " or map_port:" + tmp.str() + "\n";
		#ifdef _DEBUGMODE
			cout<<strlog.c_str();
		#endif
		WriteLogERROR(g_log.m_rec, strlog.c_str());

		return false;
	}
	if(((sIPC.mphttp_pri&&sIPC.mphttp)||(sIPC.mphttp == sIPC.mphttp_pri))&&
		((sIPC.mprtsp_pri&&sIPC.mprtsp)||(sIPC.mprtsp == sIPC.mprtsp_pri))&&
		((sIPC.mprtp_pri&&sIPC.mprtp)||(sIPC.mprtp == sIPC.mprtp_pri)))//同为0或大于0
	{
		if(!(sIPC.mphttp||sIPC.mprtsp||sIPC.mprtp))//须有一组为真
		{
			strlog +=  "check error : there are not port group number  bigger than 0" ;
			#ifdef _DEBUGMODE
				cout<<strlog.c_str();
			#endif
			WriteLogERROR(g_log.m_rec, strlog.c_str());

			return false;
		}
		//本地端口不能相同
		else if((sIPC.mphttp_pri)&&((sIPC.mphttp_pri == sIPC.mprtsp_pri)||
			(sIPC.mphttp_pri == sIPC.mprtp_pri)))
		{
			strlog +=  "check error : same local port error" ;
			#ifdef _DEBUGMODE
				cout<<strlog.c_str();
			#endif
			WriteLogERROR(g_log.m_rec, strlog.c_str());

			return false;
		}
		else if((sIPC.mprtsp_pri)&&((sIPC.mprtsp_pri == sIPC.mphttp_pri)||
			(sIPC.mprtsp_pri == sIPC.mprtp_pri)))
		{
			strlog +=  "check error : same local port error" ;
			#ifdef _DEBUGMODE
				cout<<strlog.c_str();
			#endif
			WriteLogERROR(g_log.m_rec, strlog.c_str());

			return false;
		}
		else if((sIPC.mprtp_pri)&&((sIPC.mprtp_pri == sIPC.mphttp_pri)||
			(sIPC.mprtp_pri == sIPC.mprtsp_pri)))
		{
			strlog +=  "check error : same local port error" ;
			#ifdef _DEBUGMODE
				cout<<strlog.c_str();
			#endif
			WriteLogERROR(g_log.m_rec, strlog.c_str());

			return false;
		}

	}
	else
	{
		strlog +=  "check error : port group not match" ;
		#ifdef _DEBUGMODE
			cout<<strlog.c_str();
		#endif
		WriteLogERROR(g_log.m_rec, strlog.c_str());

		return false;
	}

	return true;
}
/*************************************************************************
 函数名称: checkID
 功能说明: 数据库连接池query：call checkID操作传入函数，保存call checkID结果
 输入参数: nFiledCount 为结果的列数
 			row 为当前行的结构体	
 			lens 为当前行每一列长度的数组		    
 输出参数: pList “call checkID”结果
 返 回 值: void
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void checkID(int nFiledCount,char ** row, unsigned long *lens,void *pList)
{
	int * tmp = (int *)pList;
	if(nFiledCount > 0)
	{
		//static_cast<int>(*pList) = 1;
		*tmp = atoi(row[0]); 
	}	
}
	
/*************************************************************************
 函数名称: CHandleLogic::RegisterByCallFN
 功能说明: 注册事件处理函数：校验输入，注册入数据库
 输入参数: sIPC 解析到的数据		    
 输出参数: 无
 返 回 值: true/false 成功/失败
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CHandleLogic::RegisterByCallFN(SINPUTINFO& sIPC)
{
	int res = 0,ret = 0;
	string strLog;
	string strSql;
	//	int p2prowflag = 0;
	//string nszSql, nszLog;//替代szSql,szLog
	do
	{
		if(!CheckIPCINFO(sIPC))
		{
			ret = 1;
			break;
		}
  
		WriteLogINFO(g_log.m_rec, "pass CheckIPCINFO");

		
		CDatabaseFactory* pCDFmydb = g_dbpool.GetConnection();
		while(!pCDFmydb) 
		{	
			usleep(1000);
			pCDFmydb = g_dbpool.GetConnection();
		}
		//验证SN与random组合
		{
			int flag = 0;
			//有无IPC初始信息
			strSql = "call checkID('" + sIPC.strsn + "', '" + sIPC.strrandom +"')";
			res = pCDFmydb->m_pDataBase->Query(strSql.c_str(), 
				static_cast<fnDBRowDataPtr>(checkID), static_cast<void*>(&flag));
			if (!res)
			{
				WriteLogERROR(g_log.m_rec, strSql.c_str());
				ret = 2;
				break;
			}
			if(0 == flag)
			{
				WriteLogERROR(g_log.m_rec, strSql.c_str());
				ret = 2;
				break;	
			}		
		}
		//P2P表操作:查询并更新
		{	
			strSql = "call regupnpinfo( '"+ sIPC.strsn + "', '" + sIPC.straddr_pri + "', '" 
 						+  sIPC.straddr + "', 1)";
			res = pCDFmydb->m_pDataBase->Exec(strSql.c_str());
			if(!res) 
			{
				WriteLogERROR(g_log.m_rec, strSql.c_str());
				ret = 2;
				break;
			}   	
		}

		//map表删除旧端口信息，插入新端口信息
		{
			//gettimeofday(&tv1, NULL);			
		
			if((0 != sIPC.mphttp_pri)&&(0 != sIPC.mphttp))
			{
				stringstream sslocal, ssmap;
				ssmap<<sIPC.mphttp;
				sslocal<<sIPC.mphttp_pri;
				strSql ="call regupnpport( '"+ sIPC.strsn +"', "+ sslocal.str() +", "+ssmap.str() 
					+ ", '"+ PTYPEHTTP +"', 'TCP')";
				res = pCDFmydb->m_pDataBase->Exec(strSql.c_str());
				if(!res) 
				{
					WriteLogERROR(g_log.m_rec, strSql.c_str());
					ret = 2;
					break;
				}
				
			}
			
			
			if((0 != sIPC.mprtsp_pri)&&(0 != sIPC.mprtsp))
			{
				stringstream sslocal, ssmap;
				ssmap<<sIPC.mprtsp;
				sslocal<<sIPC.mprtsp_pri;
				strSql ="call regupnpport( '"+ sIPC.strsn +"', "+ sslocal.str() +", "+ssmap.str() 
					+ ", '"+ PTYPERTSP +"', 'TCP')";
				res = pCDFmydb->m_pDataBase->Exec(strSql.c_str());
				if(!res) 
				{
					WriteLogERROR(g_log.m_rec, strSql.c_str());
					ret = 2;
					break;
				}
				
			}
			
			if((0 != sIPC.mprtp_pri)&&(0 != sIPC.mprtp))
			{
				stringstream sslocal, ssmap;
				ssmap<<sIPC.mprtp;
				sslocal<<sIPC.mprtp_pri;
				strSql ="call regupnpport( '"+ sIPC.strsn +"', "+ sslocal.str() +", "+ssmap.str() 
					+ ", '"+ PTYPERTP +"', 'TCP')";
				res = pCDFmydb->m_pDataBase->Exec(strSql.c_str());
				
				if(!res) 
				{
					WriteLogERROR(g_log.m_rec, strSql.c_str());
					ret = 2;
					break;
				}
				
			}
					
			/*gettimeofday(&tv2, NULL);
        	printf("SET NAMES utf8 waste  %u usec\n",
         		(tv2.tv_sec - tv1.tv_sec)*1000000 + tv2.tv_usec - tv1.tv_usec);	*/
		}
		g_dbpool.ReleaseConnection(pCDFmydb);
	
	}while(0);


	if(0 == ret)
	{
		
		return true;
	}	
	else 	
	{
		return false;
	}	
}


/*char * CHandleLogic::getDateTime(void)
{
	static char mt[20] = {0};
	time_t tmp = time(NULL);
  struct tm *tm_ptr = localtime(&tmp);
  
  if(NULL == tm_ptr) return NULL;
  memset(mt, 0, sizeof(mt));
  sprintf(mt,"%04d-%02d-%02d %02d-%02d-%02d", tm_ptr->tm_year + 1900, tm_ptr->tm_mon + 1,
     tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
  return mt;
}*/

