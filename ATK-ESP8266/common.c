#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� ������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//�û�������

//���Ӷ˿ں�:8086,�������޸�Ϊ�����˿�.
u8 ipbuf[16]="192.168.7.104"; 	 //Զ�� IP
u8 ipbuf2[16]="192.168.4.2";

const u8* portnum="8086";		 //Զ�� �˿ں�

//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
const u8* wifista_ssid="ALIENTEK-FAE";			//·����SSID��
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
const u8* wifista_password="15902020353"; 	//��������

//WIFI APģʽ,ģ���������߲���,�������޸�.
const u8* wifiap_ssid="ATK-ESP0001";			//����SSID��
const u8* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
const u8* wifiap_password="12345678"; 		//�������� 

//ԭ�������Ӳ�������
const u8* yuanziyun_ssid="95319321978172338289";
const u8* yuanziyun_password="12345678";

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4������ģʽ
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STAģʽ ","APģʽ ","AP&STAģʽ "};	//ATK-ESP8266,3������ģʽ,Ĭ��Ϊ·����(ROUTER)ģʽ 
//4�ֹ���ģʽ
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP������","TCP�ͻ���"," UDP ģʽ"};	//ATK-ESP8266,4�ֹ���ģʽ
//5�ּ��ܷ�ʽ
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

#define AT_CMD_RETURN_PRINTF 0



//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		#if AT_CMD_RETURN_PRINTF
		//printf("%s",USART3_RX_BUF);	//���͵�����
		#endif
		
		if(mode)USART3_RX_STA=0;
	} 
}
//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))
				{
					#if AT_CMD_RETURN_PRINTF
					//printf("ack:%s\r\n",(u8*)ack);
					#endif
					break;//�õ���Ч���� 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s",data);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))
				{
					#if !AT_CMD_RETURN_PRINTF
					memset(USART3_RX_BUF,0,sizeof USART3_RX_BUF);
					#endif
					
					break;//�õ���Ч���� 
				}
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//ATK-ESP8266�˳�͸��ģʽ
//����ֵ:0,�˳��ɹ�;
//       1,�˳�ʧ��
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(500);					//�ȴ�500ms
	return atk_8266_send_cmd("AT","OK",20);//�˳�͸���ж�.
}
//��ȡATK-ESP8266ģ���AP+STA����״̬
//����ֵ:0��δ����;1,���ӳɹ�
u8 atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}
//��ȡATK-ESP8266ģ�������״̬
//����ֵ:0,δ����;1,���ӳɹ�.
u8 atk_8266_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//�õ�����״̬	
	return res;
}




//��ȡClient ip��ַ
//ipbuf:ip��ַ���������
void atk_8266_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//��ȡWAN IP��ַʧ��
		{
			ipbuf[0]=0;
			return;
		}		
		p=atk_8266_check_cmd("\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);	
}

//��ȡAP+STA ip��ַ����ָ��λ����ʾ
//ipbuf:ip��ַ���������
void atk_8266_get_ip(void)
{
		u8 *p;
		u8 *p1;
		u8 *p2;
		u8 *ipbuf;
		u8 *buf;
		p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
		p1=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
		p2=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	  ipbuf=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
		buf=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//��ȡWAN IP��ַʧ��
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
			sprintf((char*)buf,"AP IP:%s �˿�:%s",ipbuf,(u8*)portnum);
			printf("AP IP:%s �˿�:%s\r\n",ipbuf,portnum);
			p=(u8*)strstr((const char*)(p2+1),"STAIP,\"");
			p1=(u8*)strstr((const char*)(p+7),"\"");
			*p1=0;
			ipbuf=p+7;
			sprintf((char*)buf,"STA IP:%s �˿�:%s",ipbuf,(u8*)portnum);
			printf("STA IP:%s �˿�:%s",ipbuf,(u8*)portnum);
			myfree(SRAMIN,p);		//�ͷ��ڴ�
			myfree(SRAMIN,p1);		//�ͷ��ڴ�
			myfree(SRAMIN,p2);		//�ͷ��ڴ�
			myfree(SRAMIN,ipbuf);		//�ͷ��ڴ�
			myfree(SRAMIN,buf);		//�ͷ��ڴ�
		}
}


//ATK-ESP8266ģ����Ϣ��ʾ
//wanip:0,ȫ��������ʾ;1,������wanip.
void atk_8266_msg_show(u8 wanip)
{
	u8 *p,*p1,*p2;
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	p1=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	p2=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	POINT_COLOR=BLUE;
	atk_8266_send_cmd("AT+CWMODE=2","OK",20);
	atk_8266_send_cmd("AT+RST","OK",20);
	delay_ms(1000);//��ʱ2s�ȴ�ģ������
	delay_ms(1000);//
	
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);    //����ģ��APģʽ���߲���
	atk_8266_send_cmd(p,"OK",1000);
	if(wanip==0)//ȫ����
	{
		atk_8266_send_cmd("AT+GMR","OK",20);		//��ȡ�̼��汾��
		p=atk_8266_check_cmd("SDK version:");
		p1=(u8*)strstr((const char*)(p+12),"(");
		*p1=0;
		printf("--> �̼��汾:%s \n",p+12);
		atk_8266_send_cmd("AT+CWMODE?","+CWMODE:",20);	//��ȡ����ģʽ
		p=atk_8266_check_cmd(":");
		printf("--> ����ģʽ:%s \n",(u8*)ATK_ESP8266_CWMODE_TBL[*(p+1)-'1']);
			
		atk_8266_send_cmd("AT+CWSAP?","+CWSAP:",20);	//��ȡwifi����
		p=atk_8266_check_cmd("\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		p2=p1;
		*p1=0;
		printf("--> SSID��:%s \n",p+1);
		p=(u8*)strstr((const char*)(p2+1),"\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		p2=p1;
		*p1=0;		
		printf("--> ����:%s \n",p+1);
		
		p=(u8*)strstr((const char*)(p2+1),",");
		p1=(u8*)strstr((const char*)(p+1),",");
		*p1=0;
		printf("--> ͨ����:%s \n",p+1);
		
		printf("--> ���ܷ�ʽ:%s \n",(u8*)ATK_ESP8266_ECN_TBL[*(p1+1)-'0']);
	}
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	myfree(SRAMIN,p1);		//�ͷ��ڴ� 
	myfree(SRAMIN,p2);		//�ͷ��ڴ� 
}







//����8266 APģʽ-TCP������ģʽ����
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
    while(atk_8266_send_cmd("AT","OK",20))//���WIFIģ���Ƿ�����
	{
		atk_8266_quit_trans();//�˳�͸��
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //�ر�͸��ģʽ	
		printf("--> δ��⵽ģ��!!!\r\n");
		delay_ms(500);
		printf("--> ��������ģ��...\r\n");
	}
    while(atk_8266_send_cmd("ATE0","OK",20));//�رջ���
    atk_8266_send_cmd("AT+CWMODE=2","OK",50);
    atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	delay_ms(1000);         //��ʱ2S�ȴ������ɹ�
	delay_ms(1000);
	
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);    //����ģ��APģʽ���߲���
	atk_8266_send_cmd(p,"OK",1000);
	atk_8266_send_cmd("AT+CWSAP?","+CWSAP:",20);	//��ȡwifi����
	p=atk_8266_check_cmd("\"");
	printf("--> AP���� %s\r\n",p);    //AT+CWSAP?\r\n+CWSAP:"ATK-ESP8266","12345678",1,4,4,0

	delay_ms(1000);
	
	//TCP-Server
	printf("-->  ATK-ESP WIFI-AP ����\r\n");
	printf("-->  ��������ATK-ESPģ��,���Ե�...\r\n");
	atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
	sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);    //����Serverģʽ(0���رգ�1����)���˿ں�Ϊportnum
	atk_8266_send_cmd(p,"OK",50);  
	delay_ms(10);
	
    memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
	atk_8266_get_wanip(p);
	printf("--> ������� [TCP������ģʽ]\r\n");
	printf("--> IP:%s PORT:%s\r\n\r\n",p,portnum);
//	printf("--> ��ͨ�������������ݷ���: \n");
//	printf("--> KEY0:�������� \n");
	delay_ms(50);
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	
	while(1)
	{	DHT11_Read_Data(&temperature,&humidity);	//��ȡ��ʪ��ֵ			
		
		key=KEY_Scan(0);
		if(key==KEY0_PRES)			//KEY0��������		 
		{ 
				printf("�¶ȣ�%d��",temperature);
				printf("ʪ�ȣ�%d��\n",humidity);
//			atk_8266_send_cmd("AT+CIPSEND=0,24","OK",200);  //����ָ�����ȵ�����
//			delay_ms(200);
//			atk_8266_send_data("ATK-8266 TCP����������\r\n","OK",100);  //����ָ�����ȵ�����		
//			USART3_RX_STA=0;					

			// ����ʪ������ת��Ϊ�ַ���
				
					
		}

		if(USART3_RX_STA&0X8000)		//���յ�һ��������
		{ 
			
			rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
			USART3_RX_BUF[rlen]=0;		//��ӽ�����
			if(strstr(USART3_RX_BUF,"\r\n+IPD")==USART3_RX_BUF)
			{
				p=strstr(USART3_RX_BUF,":");
				printf("���յ�����: %s\r\n",p+1);	//���͵����� 
				if (strcmp(p + 1, "ON") == 0)
    {
       
        LED1 = 0;  
    }
    else if (strcmp(p + 1, "OFF") == 0)
    {
        LED1 = 1;  
    }
					
			}
			else if(strstr(USART3_RX_BUF,"CONNECT")==(USART3_RX_BUF+2))  //0,CONNECT ͨ��0����   0,CONNECT FAIL ͨ��0�Ͽ�
			{
				key=USART3_RX_BUF[0];
				USART3_RX_STA=0;
				atk_8266_send_cmd("AT+CWLIF","OK",20);
				sscanf(USART3_RX_BUF,"AT+CWLIF\r\n%[^,]",ipbuf);
				printf("--> �豸����: <ͨ���� %c IP: %s>\r\n",key,ipbuf);
			}
			else if(strstr(USART3_RX_BUF,"CLOSED")==(USART3_RX_BUF+2))  //0,CONNECT ͨ��0����   0,CONNECT FAIL ͨ��0�Ͽ�
			{
				printf("--> ͨ���� %c �Ͽ�����\r\n",USART3_RX_BUF[0]);
			}

			USART3_RX_STA=0;
		}
		
		if(t%100==0) //5�뷢��һ��
		{         
					    
			
			u8 data[50]; // �������ݵ���󳤶�Ϊ50���ֽ�
				sprintf((char*)data,"humidity:%d", humidity);

				
				
				atk_8266_send_cmd("AT+CIPSEND=0,11", "OK", 200); // ����ָ�����ȵ�����
				delay_ms(200);
				atk_8266_send_data(data, "OK", 100); // ����ʵ������
				USART3_RX_STA = 0;

			t=0;
		}
		
		delay_ms(10);
		t++;
	}
    
}

