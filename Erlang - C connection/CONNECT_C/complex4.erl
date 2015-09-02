-module(complex4).
-export([start/0, stop/0, init_porter/1]).
-export([sendMsg/1]).

-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).
-define(OK, 1).
-define(ERROR, -1).

start() ->
    ExtPrg = "../connect_to_erlang/port4",
    spawn(fun() -> init_receiver() end),
    spawn(fun() -> init_porter(ExtPrg) end).
     
stop() ->
    complex ! stop,
    receiver!stop.

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
	io:format("sending Type:~p , Byte: ~p .~n", [Type, Byte]), 
	call_port([Type,Byte]).

call_port(Msg) ->
    complex ! {call, Msg}.
  
init_receiver()->
		PID2 = self(),
	  register(receiver, PID2),
	  receive_loop().
	  
receive_loop()->
    receive
	{complex, Result} ->
	    io:format("~p~n",[Result]) , receive_loop();
	stop -> io:format("reciever stopped~n")
    end.

init_porter(ExtPrg) ->
    register(complex, self()),
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    loop(Port).

loop(Port) ->
    receive
	  {call, Msg} ->
	    Port ! {self(), {command, Msg}}, loop(Port);
		{Port, {data, Data}} ->
		    receiver ! {complex, decode(Data)}, loop(Port);
	stop ->
	    Port ! {self(), close},
	    receive
		{Port, closed} ->
		    io:format("C prog closed normaly~n"),
		    exit(normal)
	    end;
	{'EXIT', Port, Reason} ->
	    io:format("c process killed and notifies to erlang port~n"),
	    exit(port_terminated)
    end.

encode({Type, X}) -> [1, X].

decode([Int]) -> Int.
