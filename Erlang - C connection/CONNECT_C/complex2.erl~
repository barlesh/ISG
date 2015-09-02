-module(complex2).
-export([start/0, stop/0, init/1, sendMsg/1]).

-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).

start() ->
    ExtPrg = "../connect_to_erlang/port2",
    spawn(?MODULE, init, [ExtPrg]).
stop() ->
    complex ! stop.
    
init(ExtPrg) ->
    register(complex, self()),
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    loop(Port).
    
    
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
	io:format("sending Type:~p , Byte: ~p . ", [Type, Byte]), 
	call_port([Type,Byte]).
	
    
call_port(Msg) ->
    complex ! {call, self(), Msg},
    receive 
    	OK -> ok
    end.
    
    
loop(Port) ->
    receive
	{call, Caller, Msg} ->
	    Port ! {self(), {command, Msg}},
	    receive
		{Port, {data, Data}} ->
		    Caller ! {complex, decode(Data)}
		    after(1000) -> io:format("failed~n")
	    end,
	    loop(Port);
	stop ->
	    Port ! {self(), close},
	    receive
		{Port, closed} ->
		    io:format("C prog closed normaly~n"),
		    exit(normal)
	    end;
	{'EXIT', Port, Reason} ->
	    io:format("c process killed and notified to erlang port~n"),
	    exit(port_terminated)
    end.  
    
    
decode([Int]) -> Int. 
