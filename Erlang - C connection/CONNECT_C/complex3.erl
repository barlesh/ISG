-module(complex3).
-export([start/0, stop/0, init_porter/1]).
-export([count/1]).

start() ->
    ExtPrg = "../connect_to_erlang/port3",
    spawn(fun() -> init_receiver() end),
    spawn(fun() -> init_porter(ExtPrg) end).
     
stop() ->
    complex ! stop,
    receiver!stop.

count(X) ->
    call_port({count, X}).

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
	    Port ! {self(), {command, encode(Msg)}}, loop(Port);
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

encode({count, X}) -> [1, X].

decode([Int]) -> Int.
