-module(uart3).
-export([start/0, stop/0]).
-export([send/2]).


-define(OK, 1).
-define(ERROR, -1).
-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		API			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%start the UART interface. 
start() ->
		PID = self(),
		spawn(fun()->init_uart_port(PID) end).
		
stop() -> io:format("stooping uart connection~n"), erl_port!stop.

send(Channel, Payload) ->
		Msg = "a" ++ "b" ++ Payload,		%%TODO - change "a" to Channel and "b" to 0
		erl_port ! {call, self(), Msg},
		io:format("uart3:send. Msg sent is:~p~n", [Msg]).
		
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		PORT			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
init_uart_port(User_Pid) ->
	  PID = self(),
	  register(erl_port, PID),
    ExtPrg = "/home/ISG/uart/init_UART3",
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    loop(Port, User_Pid).
    
loop(Port, User_Pid)->
	receive
		  {call, From, Msg} ->
	  			io:format("at transmitter:loop, got Msg to pass:~p~n",[Msg]),
	  			sendMsg(Port, Msg), loop(Port, User_Pid);
	  			
	  	{_,{data, Data}} -> 
	  			io:format("got data:~p~n",[Data]), 
	  			User_Pid!Data, loop(Port, User_Pid);
	  			
	  	{Port, closed} ->
		   		io:format("C prog closed normaly(no reason)~n");
		   		
		  {'EXIT', Port, Reason} when Reason =/= normal ->
	    		io:format("error while initiating uart.Reason is:~p~n",[Reason]);
	    		
	    {'EXIT', Port, Reason} -> 
	    		io:format("c program closed. reason is:~p~n", [Reason]);
	    		
	    check -> 
	    		io:format("uart3.erl ok!~n"); 
			stop ->
	   	 		Port ! {self(), close},
	   			receive
							{Port, closed} ->
		   					 io:format("C prog closed normaly~n"),
		    					exit(normal)
	    		end;
	    
		 	Else -> io:format("at uart3: got somthing else: ~p~n",[Else])	
	end. 
	
	
	
	
	%%this function is part of the module API. it sent the contant of MSG list (of bytes) to C program
sendMsg(Port, MSG) ->
	[H|T] = MSG,
	sendByte(Port, H, ?START_BYTE),
	sendMSG2(Port, T).
sendMSG2(Port, [H|T]) when T =:= [] -> 
	sendByte(Port, H, ?END_BYTE);
sendMSG2(Port, [H|T]) -> 
	sendByte(Port, H, ?REG_BYTE),
	sendMSG2(Port, T).
	
sendByte(Port, Byte, Type) -> 
	io:format("sending Type:~p , Byte: ~p .~n", [Type, [Byte]]), 
	Port ! {self(), {command, [Type,Byte]}}.  

