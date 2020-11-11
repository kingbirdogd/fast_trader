#Create Node User

 node set_user.js user_name=leo password=leo
 
 node set_user.js user_name=eric password=12345678
 
 
 node set_user.js user_name=wendy password=12345678
 
 node set_user.js user_name=kenny password=12345678
 
  node set_user.js user_name=hugo password=12345678
  
  
  node set_user.js user_name=s1algo password=12345678
  
  
  
  #192.168.134.196
  demo user
  	demo001
  		node set_user.js user_name=demo001 password=12345678
  	
  	demo002
  	
  		node set_user.js user_name=demo002 password=12345678
  		
  		
  		
  	#192.168.104.176
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



  