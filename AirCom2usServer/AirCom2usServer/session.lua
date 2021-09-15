myid = -1;
local ENEMY_TYPE = {ENEMY_Plane1=0, ENEMY_Plane2=1, ENEMY_Plane3=2, ENEMY_Boss1=3, ENEMY_Boss2=4 };

enemys0 = {
  ["0"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 1000, x = 0, y = 5},
  ["1"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 2000, x = 0, y = 5 },
  ["2"] = {type = ENEMY_TYPE.ENEMY_Plane3, time = 3000, x = 0, y = 5 }
}

function set_uid(x)
	myid = x;
end

function tablelength(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end

function set_session(session)
	if (session == 0) then
		if (player_y == my_y) then
			API_SendMessage(myid, player, "HELLO");
			API_run_away(myid, player)
		end
	end
end