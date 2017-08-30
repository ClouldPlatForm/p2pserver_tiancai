#include "log.h"

/*************************************************************************
 函数名称: CLog::Init
 功能说明: 初始化日志
 输入参数: path 日志文件位置			
 输出参数: 无
 返 回 值: true/false 
 修订记录:
          1.作者: 饶运用,聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CLog::Init(const char * path)
{
	try{
		if(path)
		{
			//输出文件位置，文件最大尺寸5MB，最大5个备份文件
			SharedAppenderPtr  pFileAppender(new RollingFileAppender(path, 5*1024*1024, 5));
			pFileAppender->setName("LoggerName");
			//输出格式：LogLevel，本地时间，原始信息，当前记录器所在的文件名称和行号，换行符
			auto_ptr<Layout> pPatternLayout(new PatternLayout("[%p][%D]-%m[%l]%n"));
			pFileAppender->setLayout(pPatternLayout);

			m_rec = Logger::getInstance("LoggerName");//日志对象
			m_rec.addAppender(pFileAppender);//关联Appender对象
			m_rec.setLogLevel(TRACE_LOG_LEVEL);//先开放全部
		}
		else
		{
			return false;
		}	
	}
	catch (...){
		return false;
	}
	return  true;
}

/*************************************************************************
 函数名称: CLog::ChangeLogLevel
 功能说明: 更改日志记录级别
 输入参数: loglevel 日志级别			
 输出参数: 无
 返 回 值: void 
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void CLog::ChangeLogLevel(int loglevel)
{
	switch(loglevel)
	{
		case TRACE:
			m_rec.setLogLevel(TRACE_LOG_LEVEL);//设置优先级，开放trace信息输出(即ALL_LOG_LEVEL)
			break;
		case DEBUG:
			m_rec.setLogLevel(DEBUG_LOG_LEVEL);//debug-log and above allowed
			break;
		case INFO:
			m_rec.setLogLevel(INFO_LOG_LEVEL);//info-log and above allowed
			break;
		case WARN:
			m_rec.setLogLevel(WARN_LOG_LEVEL);//warn-log and above allowed
			break;
		case ERROR:
			m_rec.setLogLevel(ERROR_LOG_LEVEL);//error-log and above allowed
			break;
		case FATAL:
			m_rec.setLogLevel(FATAL_LOG_LEVEL);//fatal-log and above allowed
			break;
		case OFF:
			m_rec.setLogLevel(OFF_LOG_LEVEL);//fatal-log and above allowed
			break;
		default:
			m_rec.setLogLevel(TRACE_LOG_LEVEL);
			LOG4CPLUS_ERROR(m_rec,"config.xml LOG_LEVEL config ERROR");
			LOG4CPLUS_INFO(m_rec,"program will running and the LOG_LEVEL will be set on ALL_LOG_LEVEL:");
			printf("%s\n", "config.xml LOG_LEVEL config ERROR");
			printf("%s\n", "program will running and the LOG_LEVEL will be set on ALL_LOG_LEVEL:all-log allowed");
			break;
	}
}


/*void CLog::WriteLog(int szlevel, const char* szInfo)
{
	switch(szlevel)
	{
		case TRACE:
			LOG4CPLUS_TRACE(m_pTestLogger,szInfo);
			break;
		case DEBUG:
			LOG4CPLUS_DEBUG(m_pTestLogger,szInfo);
			break;
		case INFO:
			LOG4CPLUS_INFO(m_pTestLogger,szInfo);
			break;
		case WARN:
			LOG4CPLUS_WARN(m_pTestLogger,szInfo);
			break;
		case ERROR:
			LOG4CPLUS_ERROR(m_pTestLogger,szInfo);
			break;
		case FATAL:
			LOG4CPLUS_FATAL(m_pTestLogger,szInfo);
			break;
		default:
			LOG4CPLUS_ERROR(m_pTestLogger,"WriteLog()\'s first parameter LOG_LEVEL ERROR");
			break;
	}
}*/