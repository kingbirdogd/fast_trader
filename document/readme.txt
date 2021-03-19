#### MFH Install

Please apply the config to new supermicro
 

1.login as root

2.add a group real time

#groupadd realtime

3.add security configure file:

#vi /etc/security/limits.d/99-realtime.conf

add this two line to the file

@realtime   -  rtprio     99

@realtime   -  memlock    unlimited

4.add feed_srv to group realtime

#usermod -a -G realtime feed_srv

5.Stop the watch dog

add this line to /etc/sysctl.conf

kernel.nmi_watchdog = 0

and reboot

#### MFH Install


#Create Node User

 node set_user.js user_name=leo password=leo
 
 node set_user.js user_name=eric password=12345678
 
 
 node set_user.js user_name=wendy password=12345678
 
 node set_user.js user_name=kenny password=12345678
 
  node set_user.js user_name=hugo password=12345678
  
  
  node set_user.js user_name=s1algo password=12345678
  
  node set_user.js user_name=s1put password=12345678
  
  
    node set_user.js user_name=jarvis password=p300432
    
    
    
    node set_user.js user_name=M300317 password=68317801
    
    node set_user.js user_name=M315448 password=Ww202012
  
  
  
  #192.168.134.196
  demo user
  	demo001
  		node set_user.js user_name=demo001 password=12345678
  	
  	demo002
  	
  		node set_user.js user_name=demo002 password=12345678
  		
  		
  		
  	#192.168.104.176
  		node set_user.js user_name=demo001 password=12345678
  		
  		
 
 #192.168.104.88
 node set_user.js user_name=demo001 password=12345678  
 
 
  		
  		
  David Task
  #Item 1 : Top Share Library Send Request Queue
  Updated Source:
  	smfh_srv.cpp
  	top_shared_client.hpp
  	top_shared_client.cpp
  	
  #Item 2: Integrate TCP Data to Framework Channel
  e.g: smfh_srv_tcpchannel
  
  MsgType: REUTERS_DATA
  REUTERS_DATA = 0x07
  
  	*ReutersCodeMap
  	*Assign a m_Code in tradeable for each Reuters Symbol
  	*The Data Broadcast to the broadcastQueue
  	
  	
  	  
  
  
  ENABLE_FULLTICK = true
  
  ENABLE_FULLBOOK = true
  
  
  ENABLE_TCPDIRECT




#192.168.134.196 (i9)

account: ft_server    /  123a
source folder: fast_trader
execute script: nohup_start_fast_trader.sh
debug script: nohup_start_fast_trader_debug.sh


compile:

ln -s /home/chief/ocg/libTopTraderFlash.so.1.0.1 libTopTraderFlash.so.1.0.1

ln -s libTopTraderFlash.so.1.0.1 libTopTraderFlash.so.1

ln -s libTopTraderFlash.so.1 libTopTraderFlash.so





make
	>cd fast_trader
	>make clean
	>make

make debug
	>cd fast_trader
	>make clean
	>make CONFIG=debug

enable full tick
	>cd fast_trader
	>make clean
	>make ENABLE_FULLTICK=true


  
Enable ENABLE_TCPDIRECT

ENABLE_TCPDIRECT=true


#TCP CHANNEL
"TcpChannel":
{
	"IP": "192.168.104.116",
	"PORT": 5000,
	"BIND": false
},
	
{"Channel":"TcpChannel"}


ZF_ATTR="rx_ring_max=4096" 







192.168.134.196
login: devuser
pass: 123456

project home: fast_trader


compile:
make clean
make 

debug:
make clean
make CONFIG=debug

FullTick
make clean
make ENABLE_FULLTICK=true


Startup Normal
>>./nohup_start_fast_trader.sh


Startup Debug
>>./nohup_start_fast_trader_debug.sh


add algo user
>>cd fast_trader/gui
>>node set_user.js user_name=leo password=leo


edit config
>>cd fast_trader/config
>>vi config-FT-134.196.json


#CPU ISOLATION (134.197)

2,
3,
6,
7,
8,
9,			
12,
13,
14,
15


[下午5:56, 2021年2月4日] David Uk: /etc/sysctl.conf
[下午5:56, 2021年2月4日] David Uk: 哩個唔得
[晚上6:02, 2021年2月4日] David Uk: nmi_watchdog=0
[晚上6:02, 2021年2月4日] David Uk: 加左哩個
[晚上6:02, 2021年2月4日] David Uk: cat /etc/grub2-efi.cfg
[晚上6:02, 2021年2月4日] David Uk: reboot
[晚上6:02, 2021年2月4日] David Uk: 要咁樣先可以Permanently disable




#謝生
Forticlient VPN:

Gateway: securessl.dbpower.com.hk
User: GWP06
Pass: rmjxZOpu0#2B-TK


HTS票式
http://192.168.14.88:9919



  