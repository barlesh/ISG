-module(uart3).
-export([start/0, stop/0]).
-export([send/2, check/0]).


-define(OK, 1).
-define(ERROR, -1).
-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).
-define(ASK_FOR_INCOMING,254).

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
		erl_port!{call, self(), Msg},
		io:format("uart3:send. Msg sent is:~p~n", [Msg]).

check()-> erl_port!check.
		
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		PORT			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
init_uart_port(User_Pid) ->
	  PID = self(),
	  register(erl_port, PID),
    ExtPrg = "/home/ISG/uart/init_UART3",
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    L = erlang:port_info(Port),
    F = fun({ATOM, VAL}) -> ATOM=:=os_pid end,
    [{os_pid, OS_PID}] = lists:filter(F,L),
    io:format("os pid is:~p~n",[OS_PID]), 
    loop(Port, User_Pid,OS_PID).
    
loop(Port, User_Pid,OS_PID)->
	%io:format("loooooooooping~n"),
	%sendByte(Port, 0, ?ASK_FOR_INCOMING),
	receive
	  	{_,{data, Data}} -> 
	  			io:format("got data:~p~n",[Data]), loop(Port, User_Pid,OS_PID);%, 
	  			%User_Pid!Data, loop(Port, User_Pid);
	  			
		  {call, From, Msg} ->
	  			sendMsg(Port, Msg), loop(Port, User_Pid,OS_PID);
	  			
	  	{Port, closed} ->
		   		io:format("port(erl) closed normaly(no reason)~n"),
		   		close_c_port_program(OS_PID);
		   		
		  {'EXIT', Port, Reason} when Reason =/= normal ->
	    		io:format("error at port(erl) -closed.Reason is:~p~n",[Reason]),
	    		close_c_port_program(OS_PID);
	    		
	    {'EXIT', Port, Reason} -> 
	    		io:format("port(erl) closed. reason is:~p~n", [Reason]),
	    		close_c_port_program(OS_PID);
	    		
	    check -> 
	    		io:format("uart3.erl ok!~n"), loop(Port, User_Pid,OS_PID);
			stop ->
	   	 		Port ! {self(), close},
	   			receive
							{Port, closed} ->
		   					 io:format("port(erl) closed normaly~n"),
		   					 close_c_port_program(OS_PID),
		    					exit(normal)
	    		end;
	    
		 	Else -> io:format("at uart3: got somthing else: ~p~n",[Else])	
	%after 10 -> loop(Port, User_Pid)
	end. 
	
		
close_c_port_program(OS_PID)->
		os:cmd("kill "++integer_to_list(OS_PID)),
		io:format("closed c program for real~n").
	
	
	
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
	%io:format("sending Type:~p , Byte: ~p .~n", [Type, [Byte]]), 
	Port ! {self(), {command, [Type,Byte]}}.  

