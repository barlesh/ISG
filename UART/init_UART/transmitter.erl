-module(transmitter).
-export([start/0]).
-export([sendMsg/1]).

-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).
-define(OK, 1).
-define(ERROR, 0).

start() ->
    ExtPrg = "/home/ISG/uart/c_transmitter",
    init_trans_porter(ExtPrg).

%%this function is part of the module API. it sent the contant of MSG list (of bytes) to C program
sendMsg(MSG) ->
	[H|T] = MSG,
	sendByte(H, ?START_BYTE),
	sendMSG2(T).
sendMSG2([H|T]) when T =:= [] -> 
	sendByte(H, ?END_BYTE);
sendMSG2([H|T]) -> 
	sendByte(H, ?REG_BYTE),
	sendMSG2(T).
	
sendByte(Byte, Type) -> 
	io:format("sending Type:~p , Byte: ~p .~n", [Type, [Byte]]), 
	t_port ! {self(), {command, [Type,Byte]}}.
  

%%initialize the process that actiaaly sends & receive data to/from C program
init_trans_porter(ExtPrg) ->
    register(transmitter, self()),
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    register(t_port,Port),  
    loop(Port).
    
%%main loop of port procs. sends/receive data to/from C progrm. by receiving data, sends it to receiving pocess for further analyze 
loop(Port) ->
    receive
    check -> io:format("transmitter.erl ok!~n"); 
	  {call, From, Msg} ->
	  	io:format("at transmitter:loop, got Msg to pass:~p~n",[Msg]),
	  	sendMsg(Msg),
	  	receive
	  		{_,{data,[ANS]}} when ANS =:= ?OK -> From!ok;
	  		{_,{data,[ANS]}} when ANS =:= ?ERROR-> From!error;
	  		Else -> io:format("at transmitter, waiting for ack - got Else:~p~n",[Else]), From!error
	  	after 1000 -> From!timeout
	  	end, loop(Port);
	    
	stop ->
	    t_port ! {self(), close},
	    receive
		{Port, closed} ->
		    io:format("C prog closed normaly~n"),
		    exit(normal)
	    end;
	{'EXIT', Port, Reason} ->
	    io:format("c process killed and notifies to erlang port(transmitter).reason is:~p~n",[Reason]),
	    exit(port_terminated)
    end.

