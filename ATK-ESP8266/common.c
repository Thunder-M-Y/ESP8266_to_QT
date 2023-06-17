#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 公用驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//用户配置区

//连接端口号:8086,可自行修改为其他端口.
u8 ipbuf[16]="192.168.7.104"; 	 //远端 IP
u8 ipbuf2[16]="192.168.4.2";

const u8* portnum="8086";		 //远端 端口号

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="ALIENTEK-FAE";			//路由器SSID号
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifista_password="15902020353"; 	//连接密码

//WIFI AP模式,模块对外的无线参数,可自行修改.
const u8* wifiap_ssid="ATK-ESP0001";			//对外SSID号
const u8* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifiap_password="12345678"; 		//连接密码 

//原子云连接参数配置
const u8* yuanziyun_ssid="95319321978172338289";
const u8* yuanziyun_password="12345678";

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4个网络模式
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STA模式 ","AP模式 ","AP&STA模式 "};	//ATK-ESP8266,3种网络模式,默认为路由器(ROUTER)模式 
//4种工作模式
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP服务器","TCP客户端"," UDP 模式"};	//ATK-ESP8266,4种工作模式
//5种加密方式
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

#define AT_CMD_RETURN_PRINTF 0



//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		#if AT_CMD_RETURN_PRINTF
		//printf("%s",USART3_RX_BUF);	//发送到串口
		#endif
		
		if(mode)USART3_RX_STA=0;
	} 
}
//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					#if AT_CMD_RETURN_PRINTF
					//printf("ack:%s\r\n",(u8*)ack);
					#endif
					break;//得到有效数据 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					#if !AT_CMD_RETURN_PRINTF
					memset(USART3_RX_BUF,0,sizeof USART3_RX_BUF);
					#endif
					
					break;//得到有效数据 
				}
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(500);					//等待500ms
	return atk_8266_send_cmd("AT","OK",20);//退出透传判断.
}
//获取ATK-ESP8266模块的AP+STA连接状态
//返回值:0，未连接;1,连接成功
u8 atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}
//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_8266_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//得到连接状态	
	return res;
}




//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
		{
			ipbuf[0]=0;
			return;
		}		
		p=atk_8266_check_cmd("\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);	
}

//获取AP+STA ip地址并在指定位置显示
//ipbuf:ip地址输出缓存区
void atk_8266_get_ip(void)
{
		u8 *p;
		u8 *p1;
		u8 *p2;
		u8 *ipbuf;
		u8 *buf;
		p=mymalloc(SRAMIN,32);							//申请32字节内存
		p1=mymalloc(SRAMIN,32);							//申请32字节内存
		p2=mymalloc(SRAMIN,32);							//申请32字节内存
	  ipbuf=mymalloc(SRAMIN,32);							//申请32字节内存
		buf=mymalloc(SRAMIN,32);							//申请32字节内存
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
		{ 
			*ipbuf=0;
		}
		else
		{		
			p=atk_8266_check_cmd("APIP,\"");
			p1=(u8*)strstr((const char*)(p+6),"\"");
			p2=p1;
			*p1=0;
			ipbuf=p+6;
			sprintf((char*)buf,"AP IP:%s 端口:%s",ipbuf,(u8*)portnum);
			printf("AP IP:%s 端口:%s\r\n",ipbuf,portnum);
			p=(u8*)strstr((const char*)(p2+1),"STAIP,\"");
			p1=(u8*)strstr((const char*)(p+7),"\"");
			*p1=0;
			ipbuf=p+7;
			sprintf((char*)buf,"STA IP:%s 端口:%s",ipbuf,(u8*)portnum);
			printf("STA IP:%s 端口:%s",ipbuf,(u8*)portnum);
			myfree(SRAMIN,p);		//释放内存
			myfree(SRAMIN,p1);		//释放内存
			myfree(SRAMIN,p2);		//释放内存
			myfree(SRAMIN,ipbuf);		//释放内存
			myfree(SRAMIN,buf);		//释放内存
		}
}


//ATK-ESP8266模块信息显示
//wanip:0,全部更新显示;1,仅更新wanip.
void atk_8266_msg_show(u8 wanip)
{
	u8 *p,*p1,*p2;
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	p1=mymalloc(SRAMIN,32);							//申请32字节内存
	p2=mymalloc(SRAMIN,32);							//申请32字节内存
	POINT_COLOR=BLUE;
	atk_8266_send_cmd("AT+CWMODE=2","OK",20);
	atk_8266_send_cmd("AT+RST","OK",20);
	delay_ms(1000);//延时2s等待模块重启
	delay_ms(1000);//
	
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);    //配置模块AP模式无线参数
	atk_8266_send_cmd(p,"OK",1000);
	if(wanip==0)//全更新
	{
		atk_8266_send_cmd("AT+GMR","OK",20);		//获取固件版本号
		p=atk_8266_check_cmd("SDK version:");
		p1=(u8*)strstr((const char*)(p+12),"(");
		*p1=0;
		printf("--> 固件版本:%s \n",p+12);
		atk_8266_send_cmd("AT+CWMODE?","+CWMODE:",20);	//获取网络模式
		p=atk_8266_check_cmd(":");
		printf("--> 网络模式:%s \n",(u8*)ATK_ESP8266_CWMODE_TBL[*(p+1)-'1']);
			
		atk_8266_send_cmd("AT+CWSAP?","+CWSAP:",20);	//获取wifi配置
		p=atk_8266_check_cmd("\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		p2=p1;
		*p1=0;
		printf("--> SSID号:%s \n",p+1);
		p=(u8*)strstr((const char*)(p2+1),"\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		p2=p1;
		*p1=0;		
		printf("--> 密码:%s \n",p+1);
		
		p=(u8*)strstr((const char*)(p2+1),",");
		p1=(u8*)strstr((const char*)(p+1),",");
		*p1=0;
		printf("--> 通道号:%s \n",p+1);
		
		printf("--> 加密方式:%s \n",(u8*)ATK_ESP8266_ECN_TBL[*(p1+1)-'0']);
	}
	myfree(SRAMIN,p);		//释放内存 
	myfree(SRAMIN,p1);		//释放内存 
	myfree(SRAMIN,p2);		//释放内存 
}







//单独8266 AP模式-TCP服务器模式测试
void ap_tcp_server(void)
{	
	u8 temperature;  	    
		u8 humidity;  
	u8 t=0;
	u8 key;
    u8 *p;
	u8 ipbuf[16];
    u16 rlen=0;
	p=mymalloc(SRAMIN,32);	
    while(atk_8266_send_cmd("AT","OK",20))//检查WIFI模块是否在线
	{
		atk_8266_quit_trans();//退出透传
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	
		printf("--> 未检测到模块!!!\r\n");
		delay_ms(500);
		printf("--> 尝试连接模块...\r\n");
	}
    while(atk_8266_send_cmd("ATE0","OK",20));//关闭回显
    atk_8266_send_cmd("AT+CWMODE=2","OK",50);
    atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时2S等待重启成功
	delay_ms(1000);
	
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);    //配置模块AP模式无线参数
	atk_8266_send_cmd(p,"OK",1000);
	atk_8266_send_cmd("AT+CWSAP?","+CWSAP:",20);	//获取wifi配置
	p=atk_8266_check_cmd("\"");
	printf("--> AP参数 %s\r\n",p);    //AT+CWSAP?\r\n+CWSAP:"ATK-ESP8266","12345678",1,4,4,0

	delay_ms(1000);
	
	//TCP-Server
	printf("-->  ATK-ESP WIFI-AP 测试\r\n");
	printf("-->  正在配置ATK-ESP模块,请稍等...\r\n");
	atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
	sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);    //开启Server模式(0，关闭；1，打开)，端口号为portnum
	atk_8266_send_cmd(p,"OK",50);  
	delay_ms(10);
	
    memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
	atk_8266_get_wanip(p);
	printf("--> 配置完成 [TCP服务器模式]\r\n");
	printf("--> IP:%s PORT:%s\r\n\r\n",p,portnum);
//	printf("--> 请通过按键进行数据发送: \n");
//	printf("--> KEY0:发送数据 \n");
	delay_ms(50);
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
	myfree(SRAMIN,p);		//释放内存 
	
	while(1)
	{	DHT11_Read_Data(&temperature,&humidity);	//读取温湿度值			
		
		key=KEY_Scan(0);
		if(key==KEY0_PRES)			//KEY0发送数据		 
		{ 
				printf("温度：%d°",temperature);
				printf("湿度：%d°\n",humidity);
//			atk_8266_send_cmd("AT+CIPSEND=0,24","OK",200);  //发送指定长度的数据
//			delay_ms(200);
//			atk_8266_send_data("ATK-8266 TCP服务器测试\r\n","OK",100);  //发送指定长度的数据		
//			USART3_RX_STA=0;					

			// 将温湿度数据转换为字符串
				
					
		}

		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{ 
			
			rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;		//添加结束符
			if(strstr(USART3_RX_BUF,"\r\n+IPD")==USART3_RX_BUF)
			{
				p=strstr(USART3_RX_BUF,":");
				printf("接收到数据: %s\r\n",p+1);	//发送到串口 
				if (strcmp(p + 1, "ON") == 0)
    {
       
        LED1 = 0;  
    }
    else if (strcmp(p + 1, "OFF") == 0)
    {
        LED1 = 1;  
    }
					
			}
			else if(strstr(USART3_RX_BUF,"CONNECT")==(USART3_RX_BUF+2))  //0,CONNECT 通道0连接   0,CONNECT FAIL 通道0断开
			{
				key=USART3_RX_BUF[0];
				USART3_RX_STA=0;
				atk_8266_send_cmd("AT+CWLIF","OK",20);
				sscanf(USART3_RX_BUF,"AT+CWLIF\r\n%[^,]",ipbuf);
				printf("--> 设备连接: <通道号 %c IP: %s>\r\n",key,ipbuf);
			}
			else if(strstr(USART3_RX_BUF,"CLOSED")==(USART3_RX_BUF+2))  //0,CONNECT 通道0连接   0,CONNECT FAIL 通道0断开
			{
				printf("--> 通道号 %c 断开连接\r\n",USART3_RX_BUF[0]);
			}

			USART3_RX_STA=0;
		}
		
		if(t%100==0) //5秒发送一次
		{         
					    
			
			u8 data[50]; // 假设数据的最大长度为50个字节
				sprintf((char*)data,"humidity:%d", humidity);

				
				
				atk_8266_send_cmd("AT+CIPSEND=0,11", "OK", 200); // 发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(data, "OK", 100); // 发送实际数据
				USART3_RX_STA = 0;

			t=0;
		}
		
		delay_ms(10);
		t++;
	}
    
}

