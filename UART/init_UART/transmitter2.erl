-module(transmitter2).
-export([start/2]).
-export([sendMsg/3]).

-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).
-define(OK, 1).
-define(ERROR, 0).

start(PID, Port) ->
    init_trans_porter(PID, Port).

%%this function is part of the module API. it sent the contant of MSG list (of bytes) to C program
sendMsg(PID, Port, MSG) ->
	[H|T] = MSG,
	sendByte(PID, Port, H, ?START_BYTE),
	sendMSG2(PID, Port, T).
sendMSG2(PID, Port, [H|T]) when T =:= [] -> 
	sendByte(PID, Port, H, ?END_BYTE);
sendMSG2(PID, Port, [H|T]) -> 
	sendByte(PID, Port, H, ?REG_BYTE),
	sendMSG2(PID, Port, T).
	
sendByte(PID, Port, Byte, Type) -> 
	io:format("sending Type:~p , Byte: ~p .~n", [Type, [Byte]]), 
	Port ! {PID, {command, [Type,Byte]}}.
  

%%initialize the process that actiaaly sends & receive data to/from C program
init_trans_porter(PID, Port) ->
    register(transmitter2, self()),
    loop(PID,Port).
    
%%main loop of port procs. sends/receive data to/from C progrm. by receiving data, sends it to receiving pocess for further analyze 
loop(PID, Port) ->
    receive
    check -> io:format("transmitter.erl ok!~n"); 
	  {call, From, Msg} ->
	  	io:format("at transmitter:loop, got Msg to pass:~p~n",[Msg]),
	  	sendMsg(PID, Port, Msg), loop(PID,Port);
	  %	receive
	  	%	{_,{data,[ANS]}} when ANS =:= ?OK -> From!ok;
	  		%{_,{data,[ANS]}} when ANS =:= ?ERROR-> From!error;
	  		%Else -> io:format("at transmitter, waiting for ack - got Else:~p~n",[Else]), From!error
	  	%after 1000 -> From!timeout
	  	%end, loop(Port);
	    
	stop ->
	    Port ! {PID, close},
	    receive
		{Port, closed} ->
		    io:format("C prog closed normaly~n"),
		    exit(normal)
	    end;
	{'EXIT', Port, Reason} ->
	    io:format("c process killed and notifies to erlang port(transmitter).reason is:~p~n",[Reason]),
	    exit(port_terminated)
    end.

