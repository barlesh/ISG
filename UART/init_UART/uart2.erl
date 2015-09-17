-module(uart2).
-export([start/0, stop/0]).
-export([send/2]).

-define(OK, 1).
-define(ERROR, -1).
-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).




    
loop(Port, User_Pid)->
		
		receive			%%wait for c uart init program to send ack about initiation
		
	  {call, From, Msg} ->
	  	io:format("at transmitter:loop, got Msg to pass:~p~n",[Msg]),
	  	sendMsg(Port, Msg), loop(Port, User_Pid);
	  {_,{data, Data}} -> io:format("got data:~p~n",[Data]), User_Pid!Data, loop(Port, User_Pid);
    {Port, closed} ->
		   io:format("C prog closed normaly(no reason)~n");
		{'EXIT', Port, Reason} when Reason =/= normal ->
	    			io:format("error while initiating uart.Reason is:~p~n",[Reason]), exit(-1);
		 {'EXIT', Port, Reason} -> io:format("c program closed. reason is:~p~n", [Reason]);
		 
		 check -> io:format("uart2.erl ok!~n"); 
		 stop ->
	    Port ! {self(), close},
	    receive
					{Port, closed} ->
		   			 io:format("C prog closed normaly~n"),
		    			exit(normal)
	    end;
	    
		 Else -> io:format("at uart2: got somthing else: ~p~n",[Else])
	  end.


%start the UART interface. 
start() ->
		PID = self(),
		spawn(fun()->init_uart_port(PID) end).
    
init_uart_port(User_Pid) ->
	  PID = self(),
	  register(port2, PID),
    ExtPrg = "/home/ISG/uart/init_UART2",
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    loop(Port, User_Pid).





%this process monitor the uart's transmitting & receiving services.
%in case thay crash, it get a massage and try to run them again
monitor_processes(REC_REF, TRAN_REF, CALLER_PID) ->
		receive
			{'DOWN', REC_REF, process, Pid, Reason} when Reason =/= normal -> 
					io:format("Process Pid ~p terminated unexpected. re-spawn it~n",[Pid]),
					REC_PID = spawn(fun() -> receiver:start(CALLER_PID) end), NEW_REC_REF = monitor(process, REC_PID),
					monitor_processes(NEW_REC_REF, TRAN_REF, CALLER_PID);
			{'DOWN', TRAN_REF, process, Pid, Reason} when Reason =/= normal -> 
					io:format("Process Pid ~p terminated unexpected. re-spawn it~n",[Pid]),
					TRAN_PID = spawn(fun() -> transmitter:start() end), NEW_TRAN_REF = monitor(process, TRAN_PID),
					monitor_processes(REC_REF, NEW_TRAN_REF, CALLER_PID);
			stop -> stop
			end.


stop() -> io:format("stooping uart connection~n"), port2!stop.
		%monitor_process ! stop,
    %transmitter2 ! stop,
    %receiver ! stop.

send(Channel, Payload) ->
		Msg = "a" ++ "b" ++ Payload,		%%TODO - change "a" to Channel and "b" to 0
		port2 ! {call, self(), Msg},
		io:format("uart2:send. Msg sent is:~p~n", [Msg]).%,
		%receive 
			%ok -> ok;
			%error -> error
		%end.
		
		
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
